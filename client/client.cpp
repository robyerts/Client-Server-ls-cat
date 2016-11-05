#include <windows.h>
#include <stdio.h>
#include <cstdio>
#include <tchar.h>



HANDLE pipeCreated = NULL;
HANDLE askForDisco = NULL;

LPCWSTR mainPipeServerName = L"\\\\.\\pipe\\MainServerClient";

int wmain(int argc, wchar_t *argv[])
{
	HANDLE hSem = OpenSemaphore(SEMAPHORE_MODIFY_STATE, FALSE, L"Global\\SemaphoreName");
	if (hSem== NULL)
	{
		printf("couldn't open created semaphore\n");
		Sleep(100000);
		return 0;
	}
	HANDLE hSemNoOverlap = OpenSemaphore(SEMAPHORE_MODIFY_STATE, FALSE, L"Global\\SemaphoreClientOverlap");
	if (hSemNoOverlap == NULL)
	{
		printf("couldn't open created semaphore\n");
		Sleep(100000);
		return 0;
	}
	
	
	//////
	WaitForSingleObject(hSemNoOverlap, INFINITE); // to have one client accessing the mainPipeServer at a time
									// for some reason i can't have muplie instances opened of the mainPipeServer
	//////
	printf("releasing object from already existing semaphore\n");
	ReleaseSemaphore(hSem, 1, NULL);// letting the thread run
	Sleep(500); // so that the thread can run ConnectNamedPipe(mainPipeServer, NULL); function;

	HANDLE mainPipeServer = CreateFile(mainPipeServerName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (mainPipeServer == INVALID_HANDLE_VALUE)
	{
		int err = GetLastError();
		printf("CreateFile:mainPipeServerName, GENERIC_READ- failed in client %d \n",err);
		Sleep(100000);
		return 0;
	}
	
	//////////////////////

	wchar_t pipeThreadClientName2[500] ; //22 fixed size - every thread adds its number at the end of the name
	wchar_t pipeClientThreadName2[500] ; // 22 fized size- every thread adds its number at the end of the name  // see ThreadFunc in server for details
	wchar_t pipeCreatedName2[500]; // 24 fixed size - every thread adds its number at the end of the name 
	wchar_t askForDiscoName2[500]; // 24 fixed size - every thread adds its number at the end of the name 
	DWORD bytesRead = 0;

	int res = ReadFile(mainPipeServer, pipeThreadClientName2, 22*sizeof(wchar_t), &bytesRead, NULL);
	if (res == 0)
	{
		printf("ReadFile:mainPipeServer, pipeThreadClientName2 failed in client\n");
		Sleep(1000000);
		return 0;
	}
	pipeThreadClientName2[22] = NULL;
	bytesRead = 0;
	res = ReadFile(mainPipeServer, pipeClientThreadName2, 22*sizeof(wchar_t), &bytesRead, NULL);
	if (res == 0)
	{
		printf("ReadFile:mainPipeServer, pipeClientThreadName2 failed in Client\n");
		Sleep(1000000);
		return 0;
	}
	pipeClientThreadName2[22] = NULL;
	bytesRead = 0;
	res = ReadFile(mainPipeServer, pipeCreatedName2, 24 * sizeof(wchar_t), &bytesRead, NULL);
	if (res == 0)
	{
		printf("ReadFile:mainPipeServer, pipeCreatedName2 failed in Client\n");
		Sleep(1000000);
		return 0;
	}
	pipeCreatedName2[24] = NULL;
	bytesRead = 0;
	res = ReadFile(mainPipeServer, askForDiscoName2, 23 * sizeof(wchar_t), &bytesRead, NULL);
	if (res == 0)
	{
		printf("ReadFile:mainPipeServer, askForDiscoName2 failed in Client\n");
		Sleep(1000000);
		return 0;
	}
	askForDiscoName2[23] = NULL;
	askForDisco = OpenEvent(EVENT_ALL_ACCESS, FALSE, askForDiscoName2);
	///////
	CloseHandle(mainPipeServer); // we need to free it so that another thread can have access to it
	SetEvent(askForDisco);    
	pipeCreated = OpenEvent(EVENT_ALL_ACCESS, FALSE, pipeCreatedName2);
	//////////
	WaitForSingleObject(pipeCreated, INFINITE);
	HANDLE pipeThreadClient2 = CreateFile(pipeThreadClientName2, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (pipeThreadClient2 == INVALID_HANDLE_VALUE)
	{
		int err = GetLastError();
		printf("CreateFile--pipeThreadClientName2-- failed in client %d \n", err);
		Sleep(1000000);
		return 0;
	}
	HANDLE pipeClientThread2 = CreateNamedPipe(pipeClientThreadName2, PIPE_ACCESS_OUTBOUND, PIPE_TYPE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 4096, 4096, NMPWAIT_USE_DEFAULT_WAIT, NULL);
	if (pipeClientThread2 == INVALID_HANDLE_VALUE)
	{
		printf("CreateNamedPipe--pipeClientThread2-- failed in client \n");
		return 0;
	}
	SetEvent(pipeCreated);
	ConnectNamedPipe(pipeClientThread2, NULL);
	printf("all connections done !!\n");
	///////////
	//////////////
	////////////
	while (1)
	{
		int command = 0;
		printf(" client, enter a command\n");
		scanf_s("%d", &command);
		char inter;
		scanf_s("%c", &inter);
		DWORD bytesWritten = 0;
		WriteFile(pipeClientThread2, &command, sizeof(int), &bytesWritten, NULL);
		if (bytesWritten == 0)
		{
			printf("error while sending the command to the server !!\n");
			return 0;
		}
		if (command == 1) // ls
		{
			char bufu[301];
			wprintf(L"enter directory path\n ");
			fgets(bufu, 300, stdin);

				/// conversion from char to wchar_t
				const size_t cSize = strlen(bufu) + 1;
				size_t nrConverted = 0;
				wchar_t* wc = new wchar_t[cSize];
				mbstowcs_s(&nrConverted,wc,cSize, bufu, cSize);
				wprintf(L"Check conversion result :%ls\n", wc);
				//////
				
			int sizee = wcslen(wc);
			DWORD bytesWritten = 0;
			WriteFile(pipeClientThread2, &sizee, sizeof(int), &bytesWritten, NULL);
			if (bytesWritten == 0)
			{
				printf("WriteFile(pipeClientThread2, &sizee, sizeof(int), &bytesWritten, NULL)  failed in server !!\n");
				return 0;
			}
			bytesWritten = 0;
			WriteFile(pipeClientThread2, wc, sizee*sizeof(wchar_t), &bytesWritten, NULL);
			if (bytesWritten == 0)
			{
				printf("WriteFile(pipeClientThread2, buffer, sizee*sizeof(wchar_t), &bytesWritten, NULL) failed in server!!\n");
				return 0;
			}
			wchar_t buffer[3000] = L"lol";
			while (wcscmp(buffer, L"Done") != 0)
			{
				DWORD bytesRead = 0;
				int sizee = 0;
				int res = ReadFile(pipeThreadClient2, &sizee, sizeof(int), &bytesRead, NULL);
				if (res == 0)
				{
					printf("couldn't read the size of the next file/directory from the pipeThreadClient2 pipe\n");
				}
				printf("size of the next file/directory name is : %d\n", sizee);

				bytesRead = 0;
				res = ReadFile(pipeThreadClient2, buffer, sizeof(wchar_t)*(sizee), &bytesRead, NULL);
				if (res == 0)
				{
					printf("couldn't read the buffer from the  pipeThreadClient2 pipe\n");
				}
				wprintf(L"bytes read: %d\n", bytesRead);
				buffer[sizee] = NULL;
				if((sizee==4)&&(wcscmp(buffer,L"Done")==0))
					wprintf(L"Done was read\n");
				
				else
					wprintf(L"file/directory: %ls\n", buffer);
			}
		}
		else if (command ==2){
			char bufu[301];
			wprintf(L"enter file  path\n ");
			fgets(bufu, 300, stdin);

			int sizee = strlen(bufu);
			DWORD bytesWritten = 0;
			WriteFile(pipeClientThread2, &sizee, sizeof(int), &bytesWritten, NULL);
			if (bytesWritten == 0)
			{
				printf("WriteFile(pipeClientThread2, &sizee, sizeof(int), &bytesWritten, NULL) failed in client !!\n");
				return 0;
			}
			bytesWritten = 0;
			WriteFile(pipeClientThread2, bufu, sizee, &bytesWritten, NULL);
			if (bytesWritten == 0)
			{
				printf("WriteFile(pipeThreadClient2, bufu, sizee, &bytesWritten, NULL)failed in client !!\n");
				return 0;
			}

			while (1) {
				strcpy_s(bufu , " ");
				DWORD bytesRead = 0;
				res = ReadFile(pipeThreadClient2, bufu, 300, &bytesRead, NULL);
				if (res == 0)
				{
					printf("ReadFile(pipeThreadClient2, bufu, 300, &bytesRead, NULL) failed in client\n");
					Sleep(100000000);
					return 0;
				}
				bufu[bytesRead] = NULL;
				printf("%s", bufu);
				if (bytesRead != 300)
					break;

			}printf("\n");
		}
		else {
			break;
		}
	}


	CloseHandle(hSem);
	CloseHandle(pipeClientThread2);
	CloseHandle(pipeThreadClient2);
	CloseHandle(pipeCreated);
	
	return 0;
}