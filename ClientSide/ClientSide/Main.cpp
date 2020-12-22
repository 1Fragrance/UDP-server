#define _WINSOCK_DEPRECATED_NO_WARNINGS 

#include <winsock2.h>
#include <iostream> 

#pragma comment(lib,"Ws2_32.lib")

using namespace std;

void main()
{
	WSADATA wsaData;
	struct sockaddr_in ad;

	// NOTE: Try to initialize WinSock library
	int initializeOutput = WSAStartup(MAKEWORD(2, 2), &wsaData);

	// NOTE: Intialize errors handling
	if (initializeOutput != 0) {
		printf("WSAStartup failed with error: %d\n", initializeOutput);
		exit(EXIT_FAILURE);
	}

	// NOTE: Define socket with UDP protocol
	SOCKET connectSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (connectSocket < 0) {
		printf("socket creation failed");
		exit(EXIT_FAILURE);
	}

	// Note: Socket params
	ad.sin_family = AF_INET;
	ad.sin_port = htons(1024);
	ad.sin_addr.s_addr = inet_addr("127.0.0.1");
	int size = sizeof(ad);

	// NOTE: UI
	cout << "This program will remove digits from your input if the length of typed string exceeds 15 characters" << endl;

	const int bufferLen = 1125;
	char buffer[bufferLen];
	while (1)
	{
		fflush(stdout);

		//NOTE: Clear the buffer by filling null, it might have previously received data
		memset(buffer, '\0', bufferLen);

		cout << "\nPlease, enter the string:" << endl;
		cin.getline(buffer, bufferLen, '\n');

		// NOTE: Send request
		int sendingStatus = sendto(connectSocket, buffer, strlen(buffer), 0, (sockaddr*)&ad, size);
		if(sendingStatus == SOCKET_ERROR) {
			printf("Sendto failed with error code : %ld \n", WSAGetLastError());
			continue;
		}

		// NOTE: Receive response
		int response = recvfrom(connectSocket, buffer, bufferLen, 0, (sockaddr*)&ad, &size);
		if (response == SOCKET_ERROR) {
			int iError = WSAGetLastError();
			if (iError == WSAEWOULDBLOCK)
				printf("recv failed with error : WSAEWOULDBLOCK\n");
			else
				printf("recv failed with error code: %ld\n", iError);

			continue;
		}

		buffer[response] = '\0';

		cout << endl << "Server response : " << buffer << endl;
	}

	// NOTE: Close socket
	closesocket(connectSocket);

	// NOTE: Dispose WinSock lib
	WSACleanup();
}