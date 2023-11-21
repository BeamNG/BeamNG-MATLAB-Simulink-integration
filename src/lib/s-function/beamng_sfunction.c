/*
ISC License

Copyright 2022 BeamNG

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

#define S_FUNCTION_LEVEL               2
#define S_FUNCTION_NAME                beamng_sfunction

/* Include */
#include "simstruc.h"
#include "string.h"
#include "time.h"

#ifdef _WIN32
# include <WinSock2.h>
# include <Ws2tcpip.h>
# pragma comment(lib, "Ws2_32.lib")
#elif __linux__
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <errno.h>
# include <unistd.h>
#endif

#define IN_DIM              1
#define OUT_DIM             1
#define NUM_DISC_STATES     0
#define NUM_CONT_STATES     0
#define MDL_START

// CSV
#define CSV_PATH            ssGetSFcnParam(S, 4)
#define MAX_CSV_LINE_SIZE   1024
#define MAX_CSV_FIELDS      9

/* -------------- SOCKET DEFINES -------------- */
#define NPARAMS        5
#define IN_UDPPORT     (u_short) mxGetScalar(ssGetSFcnParam(S, 0))
#define OUT_UDPPORT    (u_short) mxGetScalar(ssGetSFcnParam(S, 1))
#define IN_UDPADDR     ssGetSFcnParam(S, 2)
#define OUT_UDPADDR    ssGetSFcnParam(S, 3)
#define BUF_SIZE       1024*8

static int maxId = 0;
static bool uniqueId = true;
static struct timeval tv;
static bool connectionStarted;

static int num_inputs = 0;
static int num_outputs = 0;

#ifdef _WIN32
// globals for storing the socket state
static SOCKET socketIn;
static SOCKET socketOut;

static int windowsSocketSetup(u_short inUdpPort, const mxArray* inUdpAddr)
{
    // this function is called a the beginning of the simulation
    WSADATA wsaData;
    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (res != NO_ERROR) {
        printf("WSAStartup failed with error ");
        return 1;
    }

    // Set up a socket to receive data from Lua, on port 64890.
    socketIn = INVALID_SOCKET;
    socketIn = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socketIn == INVALID_SOCKET) {
        printf("UDP in socket failed with error %d\n", WSAGetLastError());
        return 1;
    }

    tv.tv_sec = 1; //milliseconds
    tv.tv_usec = 0;
    if (setsockopt(socketIn, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv)) < 0)
        printf("Error setting socket timeout");

    struct sockaddr_in serverAddrIn;
    serverAddrIn.sin_family = AF_INET;
    serverAddrIn.sin_port = htons(inUdpPort);
    serverAddrIn.sin_addr.s_addr = inet_addr(mxArrayToString(inUdpAddr));
    if (bind(socketIn, (SOCKADDR*) & serverAddrIn, sizeof (serverAddrIn))) {
        printf("bind failed with error %d\n", WSAGetLastError());
        return 1;
    }

    // Set up a socket to send data to Lua, on port 64891.
    socketOut = INVALID_SOCKET;
    socketOut = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socketOut == INVALID_SOCKET) {
        printf("UDP out socket failed with error %d\n", WSAGetLastError());
        return 1;
    }

    return 0;
}


static void windowsReceive(char* recvData, SimStruct* S)
{
    struct sockaddr_in incomingAddress;
    int incomingAddrSize = sizeof(incomingAddress);
    int recvLength = recvfrom(socketIn, recvData, BUF_SIZE, 0, (SOCKADDR *) &incomingAddress, &incomingAddrSize);

    if (WSAGetLastError() == WSAETIMEDOUT) {
        if (connectionStarted) {
            ssSetStopRequested(S, 1);
            printf("Simulation stopped due to disconnection");
            connectionStarted = false;
        }
    } else {
        if (!connectionStarted) {
            tv.tv_sec = 3000; //milliseconds
            if (setsockopt(socketIn, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv)) < 0)
                printf("Error setting socket timeout");

            printf("Connection opened at time: %f\n", ssGetT(S));
            connectionStarted = true;
        }
    }
}


static void windowsSend(char* sendData, const int sizeOfInputData, u_short outUdpPort, const mxArray* outUdpAddr)
{
    struct sockaddr_in serverAddrOut;
    serverAddrOut.sin_family = AF_INET;
    serverAddrOut.sin_port = htons(outUdpPort);
    serverAddrOut.sin_addr.s_addr = inet_addr(mxArrayToString(outUdpAddr));
    sendto(socketOut, sendData, sizeOfInputData, 0, (SOCKADDR *) &serverAddrOut, sizeof(serverAddrOut));
}


#elif __linux__
static int socketIn;
static int socketOut;

static int linuxSocketSetup(u_short inUdpPort, const mxArray* inUdpAddr)
{
    // Set up a socket to receive data from Lua, on port 64890.
    socketIn = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketIn < 0) {
        printf("UDP in socket failed");
        return 1;
    }

    tv.tv_sec = 0;
    tv.tv_usec = 1000;
    if (setsockopt(socketIn, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv)) < 0) {
        printf("Error setting socket timeout");
        close(socketIn);
        return 1;
    }

    struct sockaddr_in serverAddrIn;
    serverAddrIn.sin_family = AF_INET;
    serverAddrIn.sin_port = htons(inUdpPort);
    inet_pton(AF_INET, mxArrayToString(inUdpAddr), &(serverAddrIn.sin_addr));

    if (bind(socketIn, (struct sockaddr*) &serverAddrIn, sizeof(serverAddrIn)) < 0) {
        printf("Bind failed");
        close(socketIn);
        return 1;
    }

    // Set up a socket to send data to Lua, on port 64891.
    socketOut = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketOut < 0) {
        perror("UDP out socket failed");
        return 1;
    }

    return 0;
}


static void linuxReceive(char* recvData, SimStruct* S)
{
    struct sockaddr_in incomingAddress;
    socklen_t incomingAddrSize = sizeof(incomingAddress);
    int recvLength = recvfrom(socketIn, recvData, BUF_SIZE, 0, (struct sockaddr *) &incomingAddress, &incomingAddrSize);

    if (recvLength < 0) {
        if (connectionStarted) {
            ssSetStopRequested(S, 1);
            printf("Simulation stopped due to disconnection");
            connectionStarted = false;
        }
    } else {
        if (!connectionStarted) {
            tv.tv_sec = 3;
            tv.tv_usec = 0;
            if (setsockopt(socketIn, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv)) < 0)
                printf("Error setting socket timeout");

            printf("Connection opened at time: %f\n", ssGetT(S));
            connectionStarted = true;
        }
    }
}


static void linuxSend(const char* sendData, const int sizeOfInputData, const u_short outUdpPort, const mxArray* outUdpAddr)
{
    struct sockaddr_in serverAddrOut;
    //memset(&serverAddrOut, 0, sizeof(serverAddrOut));
    serverAddrOut.sin_family = AF_INET;
    serverAddrOut.sin_port = htons(outUdpPort);
    inet_pton(AF_INET, mxArrayToString(outUdpAddr), &serverAddrOut.sin_addr);
    sendto(socketOut, sendData, sizeOfInputData, 0, (struct sockaddr *) &serverAddrOut, sizeof(serverAddrOut));
}
#endif


static void mdlInitializeSizes(SimStruct *S)
{
    // this function is called when the model is compiled
    DECL_AND_INIT_DIMSINFO(inputDimsInfo);
    DECL_AND_INIT_DIMSINFO(outputDimsInfo);
    ssSetNumSFcnParams(S, NPARAMS);
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) return;

    ssSetArrayLayoutForCodeGen(S, SS_COLUMN_MAJOR);
    ssSetOperatingPointCompliance(S, USE_DEFAULT_OPERATING_POINT);
    ssSetNumContStates(S, NUM_CONT_STATES);
    ssSetNumDiscStates(S, NUM_DISC_STATES);

    FILE* file = fopen(mxArrayToString(CSV_PATH), "r");
    if (!file) {
        printf("Error opening file\n");
        return;
    }

    num_inputs = 0;
    num_outputs = 0;

    char line[MAX_CSV_LINE_SIZE];

    // parse csv
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character
        line[strcspn(line, "\n")] = '\0';

        // Tokenize the line based on commas
        char *token;
        char *fields[MAX_CSV_FIELDS];
        int fieldCount = 0;

        token = strtok(line, ",");
        while (token != NULL && fieldCount < MAX_CSV_FIELDS) {
            fields[fieldCount++] = token;
            token = strtok(NULL, ",");
        }

        if (strcmp(fields[3], "BeamNG_to_Simulink") == 0) {
            num_outputs += 1;
        } else if (strcmp(fields[3], "Simulink_to_BeamNG") == 0) {
            num_inputs += 1;
        }
    }

    // input ports
    if (!ssSetNumInputPorts(S, num_inputs))
      return;

    ssAllowSignalsWithMoreThan2D(S);
    inputDimsInfo.numDims = 2;
    inputDimsInfo.width = IN_DIM;
    int_T inDims[] = {IN_DIM, 1};
    inputDimsInfo.dims = inDims;

    for (int i = 0; i < num_inputs; i++) {
        ssSetInputPortDimensionInfo(S, i, &inputDimsInfo);
        ssSetInputPortDataType(S, i, SS_DOUBLE);
        ssSetInputPortComplexSignal(S, i, COMPLEX_NO);
        ssSetInputPortDirectFeedThrough(S, i, 1);
        ssSetInputPortRequiredContiguous(S, i, 1); /* direct input signal access */
    }

    // output ports
    if (!ssSetNumOutputPorts(S, num_outputs))
      return;

    for (int i = 0; i < num_outputs; i++) {
        ssSetOutputPortWidth(S, i, OUT_DIM);
        ssSetOutputPortDataType(S, i, SS_DOUBLE);
        ssSetOutputPortComplexSignal(S, i, COMPLEX_NO);
        ssSetNumPWork(S, i);
        ssSetNumRWork(S, i);
        ssSetNumIWork(S, i);
        ssSetNumModes(S, i);
        ssSetNumNonsampledZCs(S, i);
    }

    ssSetSimulinkVersionGeneratedIn(S, "10.6");
    ssSetNumSampleTimes(S, 1);
    ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE);
}


static void mdlStart(SimStruct *S)
{
    #ifdef _WIN32
    if (windowsSocketSetup(IN_UDPPORT, IN_UDPADDR))
        return;
    #elif __linux__
    if (linuxSocketSetup(IN_UDPPORT, IN_UDPADDR))
        return;
    #endif

    connectionStarted = false;
}


static void mdlInitializeSampleTimes(SimStruct *S)
{
    // set the saple time of the s-function to be equal to the sample time of the simulink solver
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0.0);
}


static void mdlOutputs(SimStruct *S, int_T tid)
{
    // this function is called at each iteration
    real_T *y = (real_T *) ssGetOutputPortRealSignal(S, 0);

    char recvData[BUF_SIZE];
    double recvArray[BUF_SIZE / sizeof(double)];

    bool invalid = true;

    clock_t t1, t2;
    double elapsedTime;

    int recvReadPos = 0;

    t1 = clock();

    while (invalid) {
        #ifdef _WIN32
        windowsReceive(recvData, S);
        #elif __linux__
        linuxReceive(recvData, S);
        #endif

        memcpy(y, &recvData[recvReadPos], sizeof(double));

        if (y[0] > maxId) {
            invalid = false;
            uniqueId = true;
            maxId = y[0];
        } else if (y[0] == maxId && uniqueId) {
            invalid = false;
            uniqueId = false;
        } else {
            t2 = clock() - t1;
            elapsedTime = ((double) t2) / CLOCKS_PER_SEC;

            if (elapsedTime > 2) {
                invalid = false;
                uniqueId = true;
                maxId = 0;
            }
        }
    }

    for (int i = 0; i < num_outputs; i++) {
        real_T *y = (real_T *) ssGetOutputPortRealSignal(S, i);
        memcpy(y, &recvData[recvReadPos], sizeof(double));
        recvReadPos += sizeof(double);
    }

    const int sizeOfInputData = num_inputs * sizeof(double);
    char sendData[sizeOfInputData];

    int sendCopyPos = 0;
    for (int i = 0; i < num_inputs; i++) {
        real_T *u = (real_T *) ssGetInputPortRealSignal(S, i);
        memcpy(&sendData[sendCopyPos], u, sizeof(double));
        sendCopyPos += sizeof(double);
    }

    #ifdef _WIN32
    windowsSend(sendData, sizeOfInputData, OUT_UDPPORT, OUT_UDPADDR);
    #elif __linux__
    linuxSend(sendData, sizeOfInputData, OUT_UDPPORT, OUT_UDPADDR);
    #endif
}


static void mdlTerminate(SimStruct *S)
{
    // this function is called at the end of the simulation
    #ifdef _WIN32
    closesocket(socketIn);
    closesocket(socketOut);
    WSACleanup();
    #elif __linux__
    close(socketIn);
    close(socketOut);
    #endif
}


#ifdef MATLAB_MEX_FILE                 /* Is this file being compiled as a MEX-file? */
#include "simulink.c"                  /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"                   /* Code generation registration function */
#endif