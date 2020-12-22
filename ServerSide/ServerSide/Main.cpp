#define _WINSOCK_DEPRECATED_NO_WARNINGS 

#include <winsock2.h>
#include <iostream> 
#include <string>

#pragma comment(lib,"Ws2_32.lib")

using namespace std;

void main()
{
	WSADATA wsaData;

	const int maxStrLength = 15;

	// NOTE: Try to initialize WinSock library
	int initializeOutput = WSAStartup(MAKEWORD(2, 2), &wsaData);

	// NOTE: Intialize errors handling
	if (initializeOutput != 0) {
		printf("WSAStartup failed with error: %d\n", initializeOutput);
		return;
	}

	// NOTE: Define socket with UDP protocol
	SOCKET connectSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (connectSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	// Note: Socket params
	struct sockaddr_in ad;
	ad.sin_family = AF_INET;
	ad.sin_port = htons(1024);
	ad.sin_addr.s_addr = 0;
	int size = sizeof(ad);

	// NOTE: Bind socket params to socket descriptor
	bind(connectSocket, (struct sockaddr*)&ad, sizeof(ad));

	const int requestBufferLen = 1024;
	char requestBuffer[requestBufferLen];

	const int responseLen = 1124;
	char responseBuffer[responseLen];

	// NOTE: Listening for new messages
	while (1)
	{
		printf("Waiting for data...\n");
		fflush(stdout);

		//NOTE: Clear the buffer by filling null, it might have previously received data
		memset(requestBuffer, '\0', requestBufferLen);
		memset(responseBuffer, '\0', responseLen);

		// NOTE: Try to receive some data
		int request = recvfrom(connectSocket, requestBuffer, requestBufferLen, 0, (sockaddr*)&ad, &size);

		// NOTE: Handle error
		if (request == SOCKET_ERROR) {
			int iError = WSAGetLastError();
			if (iError == WSAEWOULDBLOCK)
				printf("recv failed with error : WSAEWOULDBLOCK\n");
			else
				printf("recv failed with error code: %ld\n", iError);

			continue;
		}
		requestBuffer[request] = '\0';

		//NOTE: Print details of the request
		printf("Received packet from %s:%d\n", inet_ntoa(ad.sin_addr), ntohs(ad.sin_port));
		printf("Data: %s\n", requestBuffer);

		// NOTE: Process request
		int digitsRemoved = 0;
		if (strlen(requestBuffer) > maxStrLength) {
			string result;
			for (int i = 0; i < strlen(requestBuffer); i++)
			{
				if (isdigit(requestBuffer[i])) {
					digitsRemoved++;
				} else {
					result += requestBuffer[i];
				}
			}

			strcpy_s(responseBuffer, result.c_str());
		} else {
			strcpy_s(responseBuffer, requestBuffer);
		}
		
		// NOTE: Prepare response
		sprintf_s(responseBuffer, "%s%s%d", responseBuffer, ", digits removed: ", digitsRemoved);

		// NOTE: Send response
		int responseSendingStatus = sendto(connectSocket, responseBuffer, responseLen, 0, (sockaddr*)&ad, size);
		if (responseSendingStatus == SOCKET_ERROR) {
			printf("Sendto failed with error code : %ld \n", WSAGetLastError());
		}
		else {
			printf("Response: %s\n\n", responseBuffer);
		}

	}

	// NOTE: Close socket
	closesocket(connectSocket);

	// NOTE: Dispose WinSock lib
	WSACleanup();
}