/*
ISC License

Copyright 2022-2024 BeamNG

Permission to use, copy, modify, and/or distribute this software for
any purpose with or without fee is hereby granted, provided that the
above copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
OF THIS SOFTWARE.
*/

#define CSV_FIELD_SECTION         0
#define CSV_FIELD_SUBSECTION      1
#define CSV_FIELD_PARAMETER_NAME  2
#define CSV_FIELD_PARAMETER_VALUE 3
#define CSV_FIELD_TYPE            4
#define CSV_FIELD_DESCRIPTION     5
#define CSV_FIELD_UNITS           6
#define CSV_FIELD_NOTES           7

#define SOCK_BUFF_COUNT           256

#define MDL_START

#define S_FUNCTION_NAME           beamng_sfunction
#define S_FUNCTION_LEVEL          2

#define NPARAMS                   1
#define CSV_FILE                  mxArrayToString(ssGetSFcnParam(S, 0))

#include <simstruc.h>

#include "beamng_csv_parser.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef _WIN32
#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#elif __linux__
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#endif

typedef struct
{
    char* addr;
    u_short port;

#ifdef _WIN32
    SOCKET socket;
#elif __linux__
    long socket;
#endif

    char* buffer;
    long num_elements;
    long buffer_stride;
} BNG_CONN;

static struct
{
    csv_table* csv;
    BNG_CONN egress;
    BNG_CONN ingress;
} global;

static void conn_open()
{
#ifdef _WIN32

    WSADATA wsad;
    if (WSAStartup(MAKEWORD(2, 2), &wsad) != NO_ERROR) {
        printf("WSAStartup failed");
        return;
    }

    // Set up a socket to send data to Lua
    global.egress.socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (global.egress.socket == INVALID_SOCKET) {
        printf("egress socket init failed with error %i\n", WSAGetLastError());
        return;
    }

    // Set up a socket to receive data from Lua
    global.ingress.socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (global.ingress.socket == INVALID_SOCKET) {
        printf("ingress socket init failed with error %i\n", WSAGetLastError());
        return;
    }

    // Set egress port as non-blocking
    u_long mode = 1;
    if(ioctlsocket(global.egress.socket, FIONBIO, &mode) != NO_ERROR) {
        printf("egress ioctlsocket failed with error: %i\n", WSAGetLastError());
        closesocket(global.egress.socket);
        return;
    }

    long egress_buff_size = global.egress.buffer_stride * SOCK_BUFF_COUNT;
    if (setsockopt(global.egress.socket, SOL_SOCKET, SO_RCVBUF,
                   (char*)&egress_buff_size, sizeof(egress_buff_size)))
        printf("error setting egress buffer size");

    long ingress_buff_size = global.ingress.buffer_stride * SOCK_BUFF_COUNT;
    if (setsockopt(global.ingress.socket, SOL_SOCKET, SO_RCVBUF,
                   (char*)&ingress_buff_size, sizeof(ingress_buff_size)))
        printf("error setting ingress buffer size");

    struct timeval tv = {.tv_sec = 3000, .tv_usec = 0 };
    if (setsockopt(global.ingress.socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv))) {
        printf("error setting ingress socket timeout");
        closesocket(global.ingress.socket);
        return;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(global.ingress.port);
    addr.sin_addr.s_addr = inet_addr(global.ingress.addr);
    if (bind(global.ingress.socket, (SOCKADDR*)&addr, sizeof(addr))) {
        printf("bind failed with error %i\n", WSAGetLastError());
        closesocket(global.ingress.socket);
        return;
    }

#elif __linux__

    // Set up a socket to send data to Lua
    global.egress.socket = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    if (global.egress.socket == -1) {
        printf("egress socket init failed");
        return;
    }

    // Set up a socket to receive data from Lua
    global.ingress.socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (global.ingress.socket == -1) {
        printf("ingress socket init failed");
        return;
    }

    long egress_buff_size = global.egress.buffer_stride * SOCK_BUFF_COUNT;
    if (setsockopt(global.egress.socket, SOL_SOCKET, SO_RCVBUF,
                   &egress_buff_size, sizeof(egress_buff_size)))
        printf("error setting egress buffer size");

    long ingress_buff_size = global.ingress.buffer_stride * SOCK_BUFF_COUNT;
    if (setsockopt(global.ingress.socket, SOL_SOCKET, SO_RCVBUF,
                   &ingress_buff_size, sizeof(ingress_buff_size)))
        printf("error setting ingress buffer size");

    struct timeval tv = {.tv_sec = 3000, .tv_usec = 0 };
    if (setsockopt(global.ingress.socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv))) {
        printf("error setting ingress socket timeout");
        close(global.ingress.socket);
        return;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(global.ingress.port);
    inet_pton(AF_INET, global.ingress.addr, &addr.sin_addr);
    if (bind(global.ingress.socket, (struct sockaddr*)&addr, sizeof(addr))) {
        printf("bind failed\n");
        close(global.ingress.socket);
        return;
    }

#endif
}

static long sock_recv(SimStruct* S)
{
    long rval = recvfrom(global.ingress.socket, global.ingress.buffer, global.ingress.buffer_stride, 0, NULL, NULL);

#ifdef _WIN32

    if (rval == SOCKET_ERROR && WSAGetLastError() == WSAETIMEDOUT) {
        ssSetStopRequested(S, 1);
        return 0;
    }

#elif __linux__

    if (rval == -1 && errno == ETIMEDOUT) {
        ssSetStopRequested(S, 1);
        return 0;
    }

#endif

    return 1;
}

static void sock_send(char* data, const long size)
{
#if _WIN32

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(global.egress.port);
    addr.sin_addr.s_addr = inet_addr(global.egress.addr);

    long rval = 0;
    do {
        rval = sendto(global.egress.socket, data, size, 0, (SOCKADDR*)&addr, sizeof(addr));
    } while(rval == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK);

#elif __linux__

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(global.egress.port);
    inet_pton(AF_INET, global.egress.addr, &addr.sin_addr);

    long rval = 0;
    do {
        rval = sendto(global.egress.socket, data, size, 0, (struct sockaddr*)&addr, sizeof(addr));
    } while(rval == -1 && errno == EWOULDBLOCK);

#endif
}

static void bng_free_resources() {
    free_csv_table(&global.csv);
    free(global.egress.buffer);
    free(global.ingress.buffer);

#ifdef _WIN32
    closesocket(global.egress.socket);
    closesocket(global.ingress.socket);
    WSACleanup();
#elif __linux__
    close(global.egress.socket);
    close(global.ingress.socket);
#endif

    memset(&global, 0, sizeof(global));
}

static void init(SimStruct* S)
{
    if (global.csv) {
        bng_free_resources();
    }
    global.csv = csv_decode(CSV_FILE, ',');
    if (!global.csv) {
        return;
    }

    global.egress.num_elements = csv_count_field_entries(global.csv, CSV_FIELD_SECTION, "signalFrom");
    global.egress.buffer_stride = (global.egress.num_elements + 1) * (sizeof(double)); // +1 for msg id
    global.egress.buffer = malloc(global.egress.buffer_stride);
    memset(global.egress.buffer, 0, global.egress.buffer_stride);

    global.ingress.num_elements = csv_count_field_entries(global.csv, CSV_FIELD_SECTION, "signalTo");
    global.ingress.buffer_stride = (global.ingress.num_elements + 1) * (sizeof(double)); // +1 for msg id
    global.ingress.buffer = malloc(global.ingress.buffer_stride);
    memset(global.ingress.buffer, 0, global.ingress.buffer_stride);

    long lineID = csv_get_next_line_ID(global.csv, 0, CSV_FIELD_SECTION, "connection");
    while (lineID > -1) {
        csv_table* line = csv_get_line(global.csv, lineID);
        if (csv_check_field_value(line, CSV_FIELD_SUBSECTION, "otherUDP")) {
            if (csv_check_field_value(line, CSV_FIELD_PARAMETER_NAME, "IP"))
                global.ingress.addr = (char *)csv_get_item(line, CSV_FIELD_PARAMETER_VALUE);
            else if (csv_check_field_value(line, CSV_FIELD_PARAMETER_NAME, "port"))
                global.ingress.port = (u_short)csv_get_item_as_double(line, CSV_FIELD_PARAMETER_VALUE);
        }
        else if (csv_check_field_value(line, CSV_FIELD_SUBSECTION, "beamngUDP")) {
            if (csv_check_field_value(line, CSV_FIELD_PARAMETER_NAME, "IP"))
                global.egress.addr = (char *)csv_get_item(line, CSV_FIELD_PARAMETER_VALUE);
            else if (csv_check_field_value(line, CSV_FIELD_PARAMETER_NAME, "port"))
                global.egress.port = (u_short)csv_get_item_as_double(line, CSV_FIELD_PARAMETER_VALUE);
        }

        lineID = csv_get_next_line_ID(global.csv, lineID + 1, CSV_FIELD_SECTION, "connection");
    }
}

static void mdlStart(SimStruct* S)
{
    if (!global.csv) {
        printf("Cannot open file '%s'. Please check if it exists.\n", CSV_FILE);
        return;
    }
    conn_open();
}

static void mdlTerminate(SimStruct* S)
{
    bng_free_resources();
}

static void mdlInitializeSampleTimes(SimStruct* S)
{
    ssSetSampleTime(S, 0, CONTINUOUS_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0.0);
}

static void mdlInitializeSizes(SimStruct* S)
{
    init(S);

    ssSetNumSFcnParams(S, NPARAMS);
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S))
        return;

    if (!global.csv) {
        printf("Cannot open file '%s'. Please check if it exists.\n", CSV_FILE);
        return;
    }

    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);
    ssSetArrayLayoutForCodeGen(S, SS_COLUMN_MAJOR);
    ssSetOperatingPointCompliance(S, USE_DEFAULT_OPERATING_POINT);

    if (!ssSetNumInputPorts(S, global.egress.num_elements))
        return;
    for (long i = 0; i < global.egress.num_elements; i++) {
        ssSetInputPortWidth(S, i, 1);
        ssSetInputPortDataType(S, i, SS_DOUBLE);
        ssSetInputPortComplexSignal(S, i, COMPLEX_NO);

        ssSetInputPortDirectFeedThrough(S, i, 1);
        ssSetInputPortRequiredContiguous(S, i, 1);
    }

    if (!ssSetNumOutputPorts(S, global.ingress.num_elements))
        return;
    for (long i = 0; i < global.ingress.num_elements; i++) {
        ssSetOutputPortWidth(S, i, 1);
        ssSetOutputPortDataType(S, i, SS_DOUBLE);
        ssSetOutputPortComplexSignal(S, i, COMPLEX_NO);
    }

    ssSetNumRWork(S, 0);
    ssSetNumIWork(S, 0);
    ssSetNumPWork(S, 0);
    ssSetNumModes(S, 0);
    ssSetNumNonsampledZCs(S, 0);

    ssSetSimulinkVersionGeneratedIn(S, "10.6");
    ssSetNumSampleTimes(S, 1);
    ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE);
}

static void mdlOutputs(SimStruct* S, int_T tid)
{
    if (!sock_recv(S)) {
        printf("Simulation stopped due to network timeout");
        return;
    }

    if (!global.csv) {
        printf("Cannot open file '%s'. Please check if it exists.\n", CSV_FILE);
        return;
    }

    double msgID = 0.0;
    memcpy(&msgID, &global.ingress.buffer[0], sizeof(msgID));

    for(long i = 0; i < global.ingress.num_elements; ++i) {
        const void* port = ssGetOutputPortSignal(S, i);
        memcpy((void *)port, &global.ingress.buffer[(i + 1) * sizeof(double)], sizeof(double));
    }

    memcpy(global.egress.buffer, &msgID, sizeof(msgID));
    for (long i = 0; i < global.egress.num_elements; ++i) {
        const void* port = ssGetInputPortSignal(S, i);
        memcpy(&global.egress.buffer[(i + 1) * sizeof(double)], port, sizeof(double));
    }

    sock_send(global.egress.buffer, global.egress.buffer_stride);
}

#ifdef MATLAB_MEX_FILE                 /* Is this file being compiled as a MEX-file? */
#include "simulink.c"                  /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"                   /* Code generation registration function */
#endif
