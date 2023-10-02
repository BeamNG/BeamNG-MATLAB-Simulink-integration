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

#ifdef _WIN32                               // needed for winsock.
#  include <WinSock2.h>
#  include <Ws2tcpip.h>
#  pragma comment(lib, "Ws2_32.lib")
#else
#endif

/* -------------- INPUT PORTS DEFINES -------------- */
#define NUM_INPUTS                     3  
/* 
MESSAGE ID
CORE
CUSTOM
*/

/* Input Port 0: MESSAGE ID */
#define INPUT_0_DIMS_ND                {1, 1}
#define INPUT_0_NUM_ELEMS              1
#define INPUT_0_COMPLEX                COMPLEX_NO
#define INPUT_0_FEEDTHROUGH            1

/* Input Port 1: CORE */
#define INPUT_1_DIMS_ND                {13, 1}
#define INPUT_1_NUM_ELEMS              13
#define INPUT_1_COMPLEX                COMPLEX_NO
#define INPUT_1_FEEDTHROUGH            1

/* Input Port 2: CUSTOM */
#define INPUT_2_DIMS_ND                {50, 1}
#define INPUT_2_NUM_ELEMS              50
#define INPUT_2_COMPLEX                COMPLEX_NO
#define INPUT_2_FEEDTHROUGH            1

/* -------------- OUTPUT PORTS DEFINES -------------- */
#define NUM_OUTPUTS                    9  
/* 
MESSAGE ID
DRIVER_CONTROLS
BODY_STATE
STATUS
WHEEL_FL
WHEEL_FR
WHEEL_RL
WHEEL_RR
CUSTOM
*/

/* Output Port 0: MESSAGE ID */
#define OUTPUT_0_DIMS_ND               {1, 1}
#define OUTPUT_0_NUM_ELEMS             1
#define OUTPUT_0_COMPLEX               COMPLEX_NO

/* Output Port 1: DRIVER_CONTROLS */
#define OUTPUT_1_DIMS_ND               {10, 1}
#define OUTPUT_1_NUM_ELEMS             10
#define OUTPUT_1_COMPLEX               COMPLEX_NO

/* Output Port 2: BODY_STATE */
#define OUTPUT_2_DIMS_ND               {14, 1}
#define OUTPUT_2_NUM_ELEMS             14
#define OUTPUT_2_COMPLEX               COMPLEX_NO

/* Output Port 3: STATUS */
#define OUTPUT_3_DIMS_ND               {12, 1}
#define OUTPUT_3_NUM_ELEMS             12
#define OUTPUT_3_COMPLEX               COMPLEX_NO

/* Output Port 4: WHEEL_FL */
#define OUTPUT_4_DIMS_ND               {6, 1}
#define OUTPUT_4_NUM_ELEMS             6
#define OUTPUT_4_COMPLEX               COMPLEX_NO

/* Output Port 5: WHEEL_FR */
#define OUTPUT_5_DIMS_ND               {6, 1}
#define OUTPUT_5_NUM_ELEMS             6
#define OUTPUT_5_COMPLEX               COMPLEX_NO

/* Output Port 6: WHEEL_RL */
#define OUTPUT_6_DIMS_ND               {6, 1}
#define OUTPUT_6_NUM_ELEMS             6
#define OUTPUT_6_COMPLEX               COMPLEX_NO

/* Output Port 7: WHEEL_RR */
#define OUTPUT_7_DIMS_ND               {6, 1}
#define OUTPUT_7_NUM_ELEMS             6
#define OUTPUT_7_COMPLEX               COMPLEX_NO

/* Output Port 8: CUSTOM */
#define OUTPUT_8_DIMS_ND               {50, 1}
#define OUTPUT_8_NUM_ELEMS             50
#define OUTPUT_8_COMPLEX               COMPLEX_NO

#define NUM_DISC_STATES                0
#define NUM_CONT_STATES                0

#define MDL_START

/* -------------- SOCKET DEFINES -------------- */
#define NPARAMS        4
#define IN_UDPPORT     (u_short) mxGetScalar(ssGetSFcnParam(S, 0))
#define OUT_UDPPORT    (u_short) mxGetScalar(ssGetSFcnParam(S, 1))
#define IN_UDPADDR     ssGetSFcnParam(S, 2)
#define OUT_UDPADDR    ssGetSFcnParam(S, 3)
#define BUF_SIZE       (OUTPUT_0_NUM_ELEMS + OUTPUT_1_NUM_ELEMS + \
                        OUTPUT_2_NUM_ELEMS + OUTPUT_3_NUM_ELEMS + \
                        OUTPUT_4_NUM_ELEMS + OUTPUT_5_NUM_ELEMS + \
                        OUTPUT_6_NUM_ELEMS + OUTPUT_7_NUM_ELEMS + \
                        OUTPUT_8_NUM_ELEMS) * 8

// globals for storing the socket state
static SOCKET socketIn;    
static SOCKET socketOut;

static int maxId = 0;
static bool uniqueId = true;
static struct timeval tv;
static bool connectionStarted;

static void mdlInitializeSizes(SimStruct *S) {
    // this function is called when the model is compiled
    DECL_AND_INIT_DIMSINFO(inputDimsInfo);
    DECL_AND_INIT_DIMSINFO(outputDimsInfo);
    ssSetNumSFcnParams(S, NPARAMS);
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) return;
  
    ssSetArrayLayoutForCodeGen(S, SS_COLUMN_MAJOR);
    ssSetOperatingPointCompliance(S, USE_DEFAULT_OPERATING_POINT);
    ssSetNumContStates(S, NUM_CONT_STATES);
    ssSetNumDiscStates(S, NUM_DISC_STATES);
    if (!ssSetNumInputPorts(S, NUM_INPUTS))
      return;

    ssAllowSignalsWithMoreThan2D(S);
    inputDimsInfo.numDims = 2;
  
    /* Input Port  0: MESSAGE ID */
    inputDimsInfo.width = INPUT_0_NUM_ELEMS;
    int_T in0Dims[] = INPUT_0_DIMS_ND;
    inputDimsInfo.dims = in0Dims;
    ssSetInputPortDimensionInfo(S, 0, &inputDimsInfo);
    ssSetInputPortDataType(S, 0, SS_DOUBLE);
    ssSetInputPortComplexSignal(S, 0, INPUT_0_COMPLEX);
    ssSetInputPortDirectFeedThrough(S, 0, INPUT_0_FEEDTHROUGH);
    ssSetInputPortRequiredContiguous(S, 0, 1); /* direct input signal access */

    /* Input Port  1: CORE */
    inputDimsInfo.width = INPUT_1_NUM_ELEMS;
    int_T in1Dims[] = INPUT_1_DIMS_ND;
    inputDimsInfo.dims = in1Dims;
    ssSetInputPortDimensionInfo(S, 1, &inputDimsInfo);
    ssSetInputPortDataType(S, 1, SS_DOUBLE);
    ssSetInputPortComplexSignal(S, 1, INPUT_1_COMPLEX);
    ssSetInputPortDirectFeedThrough(S, 1, INPUT_1_FEEDTHROUGH);
    ssSetInputPortRequiredContiguous(S, 1, 1); /* direct input signal access */

    /* Input Port  0: CUSTOM */
    inputDimsInfo.width = INPUT_2_NUM_ELEMS;
    int_T in2Dims[] = INPUT_2_DIMS_ND;
    inputDimsInfo.dims = in2Dims;
    ssSetInputPortDimensionInfo(S, 2, &inputDimsInfo);
    ssSetInputPortDataType(S, 2, SS_DOUBLE);
    ssSetInputPortComplexSignal(S, 2, INPUT_2_COMPLEX);
    ssSetInputPortDirectFeedThrough(S, 2, INPUT_2_FEEDTHROUGH);
    ssSetInputPortRequiredContiguous(S, 2, 1); /* direct input signal access */

    if (!ssSetNumOutputPorts(S, NUM_OUTPUTS))
      return;
  
    /* Output Port 0: MESSAGE ID */
    ssSetOutputPortWidth(S, 0, OUTPUT_0_NUM_ELEMS);
    ssSetOutputPortDataType(S, 0, SS_DOUBLE);
    ssSetOutputPortComplexSignal(S, 0, OUTPUT_0_COMPLEX);
    ssSetNumPWork(S, 0);
    ssSetNumRWork(S, 0);
    ssSetNumIWork(S, 0);
    ssSetNumModes(S, 0);
    ssSetNumNonsampledZCs(S, 0);

    /* Output Port 1: DRIVER_CONTROLS */
    ssSetOutputPortWidth(S, 1, OUTPUT_1_NUM_ELEMS);
    ssSetOutputPortDataType(S, 1, SS_DOUBLE);
    ssSetOutputPortComplexSignal(S, 1, OUTPUT_1_COMPLEX);
    ssSetNumPWork(S, 1);
    ssSetNumRWork(S, 1);
    ssSetNumIWork(S, 1);
    ssSetNumModes(S, 1);
    ssSetNumNonsampledZCs(S, 1);

    /* Output Port 2: BODY_STATE */
    ssSetOutputPortWidth(S, 2, OUTPUT_2_NUM_ELEMS);
    ssSetOutputPortDataType(S, 2, SS_DOUBLE);
    ssSetOutputPortComplexSignal(S, 2, OUTPUT_2_COMPLEX);
    ssSetNumPWork(S, 2);
    ssSetNumRWork(S, 2);
    ssSetNumIWork(S, 2);
    ssSetNumModes(S, 2);
    ssSetNumNonsampledZCs(S, 2);

    /* Output Port 3: STATUS */
    ssSetOutputPortWidth(S, 3, OUTPUT_3_NUM_ELEMS);
    ssSetOutputPortDataType(S, 3, SS_DOUBLE);
    ssSetOutputPortComplexSignal(S, 3, OUTPUT_3_COMPLEX);
    ssSetNumPWork(S, 3);
    ssSetNumRWork(S, 3);
    ssSetNumIWork(S, 3);
    ssSetNumModes(S, 3);
    ssSetNumNonsampledZCs(S, 3);

    /* Output Port 4: WHEEL_FL */
    ssSetOutputPortWidth(S, 4, OUTPUT_4_NUM_ELEMS);
    ssSetOutputPortDataType(S, 4, SS_DOUBLE);
    ssSetOutputPortComplexSignal(S, 4, OUTPUT_4_COMPLEX);
    ssSetNumPWork(S, 4);
    ssSetNumRWork(S, 4);
    ssSetNumIWork(S, 4);
    ssSetNumModes(S, 4);
    ssSetNumNonsampledZCs(S, 4);

    /* Output Port 5: WHEEL_FR */
    ssSetOutputPortWidth(S, 5, OUTPUT_5_NUM_ELEMS);
    ssSetOutputPortDataType(S, 5, SS_DOUBLE);
    ssSetOutputPortComplexSignal(S, 5, OUTPUT_5_COMPLEX);
    ssSetNumPWork(S, 5);
    ssSetNumRWork(S, 5);
    ssSetNumIWork(S, 5);
    ssSetNumModes(S, 5);
    ssSetNumNonsampledZCs(S, 5);

    /* Output Port 6: WHEEL_RL */
    ssSetOutputPortWidth(S, 6, OUTPUT_6_NUM_ELEMS);
    ssSetOutputPortDataType(S, 6, SS_DOUBLE);
    ssSetOutputPortComplexSignal(S, 6, OUTPUT_6_COMPLEX);
    ssSetNumPWork(S, 6);
    ssSetNumRWork(S, 6);
    ssSetNumIWork(S, 6);
    ssSetNumModes(S, 6);
    ssSetNumNonsampledZCs(S, 6);

    /* Output Port 7: WHEEL_RR */
    ssSetOutputPortWidth(S, 7, OUTPUT_7_NUM_ELEMS);
    ssSetOutputPortDataType(S, 7, SS_DOUBLE);
    ssSetOutputPortComplexSignal(S, 7, OUTPUT_7_COMPLEX);
    ssSetNumPWork(S, 7);
    ssSetNumRWork(S, 7);
    ssSetNumIWork(S, 7);
    ssSetNumModes(S, 7);
    ssSetNumNonsampledZCs(S, 7);

    /* Output Port 8: CUSTOM */
    ssSetOutputPortWidth(S, 8, OUTPUT_8_NUM_ELEMS);
    ssSetOutputPortDataType(S, 8, SS_DOUBLE);
    ssSetOutputPortComplexSignal(S, 8, OUTPUT_8_COMPLEX);
    ssSetNumPWork(S, 8);
    ssSetNumRWork(S, 8);
    ssSetNumIWork(S, 8);
    ssSetNumModes(S, 8);
    ssSetNumNonsampledZCs(S, 8);

    ssSetSimulinkVersionGeneratedIn(S, "10.6");
    ssSetNumSampleTimes(S, 1);
    ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE); 
}


static void mdlStart(SimStruct *S) {
    // this function is called a the beginning of the simulation
    WSADATA wsaData;
    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    
    if (res != NO_ERROR) {
        printf("WSAStartup failed with error ");
        return;
    }

    // Set up a socket to receive data from Lua, on port 64890.
    socketIn = INVALID_SOCKET;
    socketIn = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socketIn == INVALID_SOCKET) {
        printf("UDP in socket failed with error %d\n", WSAGetLastError());
        return ;
    }

    tv.tv_sec = 1; //milliseconds
    tv.tv_usec = 0;
    if (setsockopt(socketIn, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv)) < 0) {
        printf("Error setting socket timeout");
    }

    struct sockaddr_in serverAddrIn;
    serverAddrIn.sin_family = AF_INET;
    serverAddrIn.sin_port = htons(IN_UDPPORT);
    serverAddrIn.sin_addr.s_addr = inet_addr(mxArrayToString(IN_UDPADDR));
    if (bind(socketIn, (SOCKADDR*) & serverAddrIn, sizeof (serverAddrIn))) {
        printf("bind failed with error %d\n", WSAGetLastError());
        return ;
    }
    
    // Set up a socket to send data to Lua, on port 64891.
    socketOut = INVALID_SOCKET;
    socketOut = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socketOut == INVALID_SOCKET) {
        printf("UDP out socket failed with error %d\n", WSAGetLastError());
        return ;
    }

    connectionStarted = false;
}


static void mdlInitializeSampleTimes(SimStruct *S) {
    // set the saple time of the s-function to be equal to the sample time of the simulink solver
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0.0);
}


static void mdlOutputs(SimStruct *S, int_T tid) {
    // this function is called at each iteration
    const real_T *u0 = (real_T *) ssGetInputPortRealSignal(S, 0);
    const real_T *u1 = (real_T *) ssGetInputPortRealSignal(S, 1);
    const real_T *u2 = (real_T *) ssGetInputPortRealSignal(S, 2);

    real_T *y0 = (real_T *) ssGetOutputPortRealSignal(S, 0);
    real_T *y1 = (real_T *) ssGetOutputPortRealSignal(S, 1);
    real_T *y2 = (real_T *) ssGetOutputPortRealSignal(S, 2);
    real_T *y3 = (real_T *) ssGetOutputPortRealSignal(S, 3);
    real_T *y4 = (real_T *) ssGetOutputPortRealSignal(S, 4);
    real_T *y5 = (real_T *) ssGetOutputPortRealSignal(S, 5);
    real_T *y6 = (real_T *) ssGetOutputPortRealSignal(S, 6);
    real_T *y7 = (real_T *) ssGetOutputPortRealSignal(S, 7);
    real_T *y8 = (real_T *) ssGetOutputPortRealSignal(S, 8);
    
    const int y0Size = (int) ssGetOutputPortWidth(S, 0) * sizeof(double);
    const int y1Size = (int) ssGetOutputPortWidth(S, 1) * sizeof(double);
    const int y2Size = (int) ssGetOutputPortWidth(S, 2) * sizeof(double);
    const int y3Size = (int) ssGetOutputPortWidth(S, 3) * sizeof(double);
    const int y4Size = (int) ssGetOutputPortWidth(S, 4) * sizeof(double);
    const int y5Size = (int) ssGetOutputPortWidth(S, 5) * sizeof(double);
    const int y6Size = (int) ssGetOutputPortWidth(S, 6) * sizeof(double);
    const int y7Size = (int) ssGetOutputPortWidth(S, 7) * sizeof(double);
    const int y8Size = (int) ssGetOutputPortWidth(S, 8) * sizeof(double);

    char recvData[BUF_SIZE];
    double recvArray[BUF_SIZE / sizeof(double)];

    bool invalid = true;

    clock_t t1, t2;
    double elapsedTime;

    int recvReadPos = 0;

    t1 = clock();
    
    while (invalid) {
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
                if (setsockopt(socketIn, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv)) < 0) {
                    printf("Error setting socket timeout");
                }
                printf("Connection opened at time: %f\n", ssGetT(S));
                connectionStarted = true;
            }
        }

        memcpy(y0, &recvData[recvReadPos], y0Size);

        if (y0[0] > maxId) {
            invalid = false;
            uniqueId = true;
            maxId = y0[0];
        } else if (y0[0] == maxId && uniqueId) {
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

    recvReadPos += y0Size;
    memcpy(y1, &recvData[recvReadPos], y1Size);
    recvReadPos += y1Size;
    memcpy(y2, &recvData[recvReadPos], y2Size);
    recvReadPos += y2Size;
    memcpy(y3, &recvData[recvReadPos], y3Size);
    recvReadPos += y3Size;
    memcpy(y4, &recvData[recvReadPos], y4Size);
    recvReadPos += y4Size;
    memcpy(y5, &recvData[recvReadPos], y5Size);
    recvReadPos += y5Size;
    memcpy(y6, &recvData[recvReadPos], y6Size);
    recvReadPos += y6Size;
    memcpy(y7, &recvData[recvReadPos], y7Size);
    recvReadPos += y7Size;
    memcpy(y8, &recvData[recvReadPos], y8Size);

    const int u0Size = (int) ssGetInputPortWidth(S, 0) * sizeof(double);
    const int u1Size = (int) ssGetInputPortWidth(S, 1) * sizeof(double);
    const int u2Size = (int) ssGetInputPortWidth(S, 2) * sizeof(double);
    const int sizeOfInputData = u0Size + u1Size + u2Size;

    char sendData[sizeOfInputData];

    memcpy(&sendData[0], u0, u0Size);
    memcpy(&sendData[u0Size], u1, u1Size);
    memcpy(&sendData[u0Size + u1Size], u2, u2Size);

    struct sockaddr_in serverAddrOut;
    serverAddrOut.sin_family = AF_INET;
    serverAddrOut.sin_port = htons(OUT_UDPPORT);
    serverAddrOut.sin_addr.s_addr = inet_addr(mxArrayToString(OUT_UDPADDR));
    sendto(socketOut, sendData, sizeOfInputData, 0, (SOCKADDR *) &serverAddrOut, sizeof(serverAddrOut));
}


static void mdlTerminate(SimStruct *S){
    // this function is called at the end of the simulation
    closesocket(socketIn);
    closesocket(socketOut);
    WSACleanup();
}


#ifdef MATLAB_MEX_FILE                 /* Is this file being compiled as a MEX-file? */
#include "simulink.c"                  /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"                   /* Code generation registration function */
#endif