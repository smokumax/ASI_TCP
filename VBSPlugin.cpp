#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "VBSPlugin.h"
#include <stdio.h>
//#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>

#include <string>
#include <thread>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

// UDP

WSADATA wsaData;
int iResult;

SOCKET ListenSocket = INVALID_SOCKET;
SOCKET ClientSocket = INVALID_SOCKET;

struct addrinfo* result = NULL;
struct addrinfo hints;

int iSendResult;
char recvbuf[DEFAULT_BUFLEN] = "BRAK";
int recvbuflen = DEFAULT_BUFLEN;

// UDP END

using namespace std;
char r[200] = "BRAK";
char rc[200];


int i = 0;
bool UNLOAD = false;

bool ACTION = false;





void TCPthread()
{

    do {

        bool clientON = true;

        // Initialize Winsock
        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            //printf("WSAStartup failed with error: %d\n", iResult);
            //return 1;
            clientON = false;
        }

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        // Resolve the server address and port
        iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
        if (iResult != 0) {
            //printf("getaddrinfo failed with error: %d\n", iResult);
            WSACleanup();
            //return 1;
            clientON = false;
        }

        // Create a SOCKET for connecting to server
        ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (ListenSocket == INVALID_SOCKET) {
            //printf("socket failed with error: %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            //return 1;
            clientON = false;
        }

        // Setup the TCP listening socket
        iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            //printf("bind failed with error: %d\n", WSAGetLastError());
            freeaddrinfo(result);
            closesocket(ListenSocket);
            WSACleanup();
            //return 1;
            clientON = false;
        }

        freeaddrinfo(result);

        iResult = listen(ListenSocket, SOMAXCONN);
        if (iResult == SOCKET_ERROR) {
            //printf("listen failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            //return 1;
            clientON = false;
        }

        // Accept a client socket
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            //printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            //return 1;
            clientON = false;
        }

        // No longer need server socket
        closesocket(ListenSocket);

        // Receive until the peer shuts down the connection

        do {

            iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);

            ACTION = true;



            /*
            if (iResult > 0) {
                //printf("Bytes received: %d\n", iResult);

              
                // Echo the buffer back to the sender
                iSendResult = send(ClientSocket, r, 200, 0);
                if (iSendResult == SOCKET_ERROR) {
                    //printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(ClientSocket);
                    WSACleanup();
                    //return 1;
                    clientON = false;
                }


                //printf("Bytes sent: %d\n", iSendResult);
                if (UNLOAD) {
                    return ; 
                }

            }
            else if (iResult != 0)
            {
                //printf("recv failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                //return 1;
                clientON = false;
            }
*/



            if (UNLOAD) {
                return ;
            }
        } while (clientON);

        closesocket(ClientSocket);

        WSACleanup();

    } while (1);


}

thread t1(TCPthread);


string convertToString(char* a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
    
}



// Command function declaration
typedef int (WINAPI * ExecuteCommandType)(const char *command, char *result, int resultLength);

// Command function definition
ExecuteCommandType ExecuteCommand = NULL;
const char scfipt[] = "getPos(allMissionObjects 'BISIM_IG_BLU_NAVY_47MLB_GRY_X'";

// Function that will register the ExecuteCommand function of the engine
VBSPLUGIN_EXPORT void WINAPI RegisterCommandFnc(void *executeCommandFnc)
{
  ExecuteCommand = (ExecuteCommandType)executeCommandFnc;
}



// This function will be executed every simulation step (every frame) and took a part in the simulation procedure.
// We can be sure in this function the ExecuteCommand registering was already done.
// deltaT is time in seconds since the last simulation step

VBSPLUGIN_EXPORT void WINAPI OnSimulationStep(float deltaT)
{
    
    
    if (ACTION)
    {   
        for (int i = 0; i < 200; i++)
        {
            r[i] = 0;
        }

        ExecuteCommand(recvbuf, r, 200);
        for (int i = 0; i < DEFAULT_BUFLEN; i++)
        {
            recvbuf[i] = 0;
        }

        send(ClientSocket, r, 200, 0);
        ACTION = false;
        //ACTION_RES = true;
    }
    
    /*
    if (i == 0)
    {
        ExecuteCommand("AAA = 999;", NULL, 0);

        // t1.join();
    }

    i++;


    if (i == 11)
    {
        char command1[1024];
        char command2[1024];
        char command3[1024] = "'DUPA'";
        char r[200];

        sprintf(command1, "D=%i;", i);
        ExecuteCommand(command1, NULL, 0);

        ExecuteCommand("getPos (allMissionObjects 'BISIM_IG_BLU_NAVY_47MLB_GRY_X' select 0);", r, 200);

        sprintf_s(command2, "titleText ['%s','plain down'];", recvbuf);

        //sprintf_s(command2, "titleText ['TCP plgin lanched /n neptunsdudo.eu','plain top'];", r);
        ExecuteCommand(command2, NULL, 0);
        
        //iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        //send(ClientSocket, "r", 200, 0);
        //iSendResult = send(ClientSocket, r, 200, 0);
        
        i = 1;

        // UDP
        //send(ClientSocket, recvbuf, iResult, 0);
    }
    */
}


VBSPLUGIN_EXPORT void WINAPI OnUnload()
{
    
    ExecuteCommand("yyy=22",NULL, 0);
    UNLOAD = true;
    t1.detach();
}


// This function will be executed every time the script in the engine calls the script function "pluginFunction"
// We can be sure in this function the ExecuteCommand registering was already done.
// Note that the plugin takes responsibility for allocating and deleting the returned string
VBSPLUGIN_EXPORT const char* WINAPI PluginFunction(const char *input) 
{
  //{ Sample code:
  static const char result[]="[1.0, 3.75]";
  return result;
  //!}
}

// DllMain
BOOL WINAPI DllMain(HINSTANCE hDll, DWORD fdwReason, LPVOID lpvReserved)
{
   
   switch(fdwReason)
   {
      case DLL_PROCESS_ATTACH:
         OutputDebugString("Called DllMain with DLL_PROCESS_ATTACH\n");
         break;
      case DLL_PROCESS_DETACH:
         OutputDebugString("Called DllMain with DLL_PROCESS_DETACH\n");
         break;
      case DLL_THREAD_ATTACH:
         OutputDebugString("Called DllMain with DLL_THREAD_ATTACH\n");
         break;
      case DLL_THREAD_DETACH:
         OutputDebugString("Called DllMain with DLL_THREAD_DETACH\n");
         break;
   }

   return TRUE;
   t1.join();
}
