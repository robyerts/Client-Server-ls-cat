#include <windows.h>
#include <stdio.h>



HANDLE hSem = INVALID_HANDLE_VALUE;
HANDLE hSemNoOverlap = INVALID_HANDLE_VALUE;
LPCWSTR mainPipeServerName = L"\\\\.\\pipe\\MainServerClient";
HANDLE mainPipeServer = INVALID_HANDLE_VALUE;

wchar_t currentDirectory[3000];

#define NR_THREADS 10
#define MAX_DIR_PATH				MAX_PATH //it is 256 on windows

DWORD WINAPI ThreadFunc(LPVOID lpThreadParameter)
{
	while (1)
	{
		WaitForSingleObject(hSem, INFINITE);
		ConnectNamedPipe(mainPipeServer, NULL); //makes the mainPipeServer wait for a client connection
		HANDLE pipeCreated = INVALID_HANDLE_VALUE;
		HANDLE askForDisco = INVALID_HANDLE_VALUE;

		int p = *((int*)lpThreadParameter);

		wchar_t pipeThreadClientName[500] = L"\\\\.\\pipe\\ThreadClient";// size 21
		wchar_t pipeClientThreadName[500] = L"\\\\.\\pipe\\ClientThread"; // size 21
		wchar_t pipeThreadClientName2[500];
		wchar_t pipeClientThreadName2[500];
		wchar_t pipeCreatedName[500] = L"Local\\pipeCreationEvent";//size 23
		wchar_t pipeCreatedName2[500];
		wchar_t askForDiscoName[500] = L"Local\\askForDiscoEvent";//size 22
		wchar_t askForDiscoName2[500];

		swprintf(pipeThreadClientName2, 500, L"%ls%d", pipeThreadClientName, p); // size 22
		swprintf(pipeClientThreadName2, 500, L"%ls%d", pipeClientThreadName, p); //size 22
		swprintf(pipeCreatedName2, 500, L"%ls%d", pipeCreatedName, p);//size 24
		swprintf(askForDiscoName2, 500, L"%ls%d", askForDiscoName, p);//size 23

		pipeCreated = CreateEvent(NULL, FALSE, FALSE, pipeCreatedName2);
		askForDisco = CreateEvent(NULL, FALSE, FALSE, askForDiscoName2);

		int sizee = wcslen(pipeThreadClientName2);
		DWORD bytesWritten = 0;
		WriteFile(mainPipeServer, pipeThreadClientName2, sizee*sizeof(wchar_t), &bytesWritten, NULL);
		if (bytesWritten == 0)
		{
			printf("WriteFile:mainPipeServer, pipeThreadClientName2 failed !!\n");
			Sleep(100000);
			return 0;
		}

		sizee = wcslen(pipeClientThreadName2);
		bytesWritten = 0;
		WriteFile(mainPipeServer, pipeClientThreadName2, sizee*sizeof(wchar_t), &bytesWritten, NULL);
		if (bytesWritten == 0)
		{
			printf("WriteFile:mainPipeServer, pipeClientThreadName2 failed !!\n");
			Sleep(100000);
			return 0;
		}
		sizee = wcslen(pipeCreatedName2);
		bytesWritten = 0;
		WriteFile(mainPipeServer, pipeCreatedName2, sizee*sizeof(wchar_t), &bytesWritten, NULL);
		if (bytesWritten == 0)
		{
			printf("WriteFile:mainPipeServer, pipeCreatedName2 failed !!\n");
			Sleep(100000);
			return 0;
		}
		sizee = wcslen(askForDiscoName2);
		bytesWritten = 0;
		WriteFile(mainPipeServer, askForDiscoName2, sizee*sizeof(wchar_t), &bytesWritten, NULL);
		if (bytesWritten == 0)
		{
			printf("WriteFile:mainPipeServer, askForDiscoName2 failed !!\n");
			Sleep(100000);
			return 0;
		}
		////
		WaitForSingleObject(askForDisco, INFINITE);
		DisconnectNamedPipe(mainPipeServer);
		ReleaseSemaphore(hSemNoOverlap, 1, NULL);
		//////////
		HANDLE pipeThreadClient2 = CreateNamedPipe(pipeThreadClientName2, PIPE_ACCESS_OUTBOUND, PIPE_TYPE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 4096, 4096, NMPWAIT_USE_DEFAULT_WAIT, NULL);
		if (pipeThreadClient2 == INVALID_HANDLE_VALUE)
		{
			printf("CreateNamedPipe--pipeThreadClient2-- failed in server thread with ID %d\n", p);
			Sleep(100000);
			return 0;
		}
		SetEvent(pipeCreated);
		ConnectNamedPipe(pipeThreadClient2, NULL);
		////////
		//DisconnectNamedPipe(mainPipeServer);
		///////

		WaitForSingleObject(pipeCreated, INFINITE);
		HANDLE pipeClientThread2 = CreateFile(pipeClientThreadName2, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (pipeClientThread2 == INVALID_HANDLE_VALUE)
		{
			int err = GetLastError();
			printf("CreateFile--pipeClientThreadName2-- failed in server thread %d \n", err);
			Sleep(100000);
			return 0;
		}
		printf("all connections done !!\n");


		wchar_t lsReturn[50][3000];
		///////////////////////////////////////////////////
		/////////Handling the client commands////////////
		while (1)
		{
			DWORD bytesRead = 0;
			int intt = 0;
			int res = ReadFile(pipeClientThread2, &intt, sizeof(int), &bytesRead, NULL);
			if (res == 0)
			{
				printf("couldn't read the client command from the pipeClientThread2 pipe\n");
			}
			if (intt == 1) {
				wchar_t buffer[3000];
				DWORD bytesRead = 0;
				int sizee = 0;
				int res = ReadFile(pipeClientThread2, &sizee, sizeof(int), &bytesRead, NULL);
				if (res == 0)
				{
					printf("ReadFile(pipeClientThread2, &sizee, sizeof(int), &bytesRead, NULL) failed\n");
				}
				printf("size read from the pipe  is %d\n", sizee);

				bytesRead = 0;
				res = ReadFile(pipeClientThread2, buffer, sizeof(wchar_t)*(sizee), &bytesRead, NULL);;
				if (res == 0)
				{
					printf("ReadFile(pipeClientThread2, buffer, sizeof(wchar_t)*(sizee), &bytesRead, NULL) failed\n");
				}
				buffer[sizee - 1] = NULL;

				WIN32_FIND_DATAW	FindFileData;
				HANDLE			hFind = INVALID_HANDLE_VALUE;
				wchar_t			pFrom[MAX_DIR_PATH], pFF[MAX_DIR_PATH];
				wchar_t			pTo[MAX_DIR_PATH];
				int				iRet = 0;
				size_t			uFrom, uN;
				int dex = 0;

				wcscpy_s(pFF, currentDirectory);
				wcscat_s(pFF, buffer);
				wcscat_s(pFF, L"*");
				do
				{
					if (hFind == INVALID_HANDLE_VALUE)
					{
						hFind = FindFirstFileW(pFF, &FindFileData);
						if (hFind == INVALID_HANDLE_VALUE)
							break;

					}
					else if (FindNextFileW(hFind, &FindFileData) == 0)
					{
						if (GetLastError() != ERROR_NO_MORE_FILES)
							iRet = -1;
						break;
					}

					if (wcscmp(FindFileData.cFileName, L".") == 0 || wcscmp(FindFileData.cFileName, L"..") == 0)
						continue;
					wprintf(TEXT(" file found is %s\n"),
						FindFileData.cFileName);

					wcscpy_s(lsReturn[dex], FindFileData.cFileName);
					dex++;

				} while (iRet == 0);

				for (int i = 0; i < dex; i++)
				{
					int sizee = wcslen(lsReturn[i]);
					DWORD bytesWritten = 0;
					WriteFile(pipeThreadClient2, &sizee, sizeof(int), &bytesWritten, NULL);
					if (bytesWritten == 0)
					{
						printf("WriteFile:pipeThreadClient2, sizee -sizeof(int)  failed !!\n");
						return 0;
					}
					bytesWritten = 0;
					WriteFile(pipeThreadClient2, lsReturn[i], sizee*sizeof(wchar_t), &bytesWritten, NULL);
					if (bytesWritten == 0)
					{
						printf("WriteFile:pipeThreadClient2, lsReturn[i] --sizee*sizeof(wchar_t) !!\n");
						return 0;
					}
				}
				wcscpy_s(buffer, L"Done");
				sizee = wcslen(buffer);
				DWORD bytesWritten = 0;
				WriteFile(pipeThreadClient2, &sizee, sizeof(int), &bytesWritten, NULL);
				if (bytesWritten == 0)
				{
					printf("WriteFile:pipeThreadClient2, sizee -sizeof(int)  failed !!\n");
					return 0;
				}
				bytesWritten = 0;
				WriteFile(pipeThreadClient2, buffer, sizee*sizeof(wchar_t), &bytesWritten, NULL);
				if (bytesWritten == 0)
				{
					printf("WriteFile:pipeThreadClient2, lsReturn[i] --sizee*sizeof(wchar_t) !!\n");
					return 0;
				}
			}
			else if (intt == 2) {
				char bufu[301];
				DWORD bytesRead = 0;
				int sizee = 0;
				int res = ReadFile(pipeClientThread2, &sizee, sizeof(int), &bytesRead, NULL);
				if (res == 0)
				{
					printf(" ReadFile(pipeClientThread2, &sizee, sizeof(int), &bytesRead, NULL); failed\n");
					Sleep(100000000);
					return 0;
				}

				bytesRead = 0;
				res = ReadFile(pipeClientThread2, bufu, (sizee), &bytesRead, NULL);
				if (res == 0)
				{
					printf("ReadFile(pipeClientThread2, bufu, (sizee), &bytesRead, NULL); failed\n");
					Sleep(100000000);
					return 0;
				}
				bufu[sizee - 1] = NULL;
				//////conversion from char to wchar_t
				const size_t cSize = strlen(bufu) + 1;
				size_t nrConverted = 0;
				wchar_t* wc = new wchar_t[cSize];
				mbstowcs_s(&nrConverted, wc, cSize, bufu, cSize);
				//////////////////
				HANDLE pFile = CreateFile(wc, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (pFile == INVALID_HANDLE_VALUE)
				{
					int err = GetLastError();
					printf("CreateFile(wc, GENERIC_READ,- failed in server thread %d \n", err);
					Sleep(100000);
					return 0;
				}

				while (1) {
					DWORD bytesRead = 0;
					res = ReadFile(pFile, bufu, 300, &bytesRead, NULL);
					if (res == 0)
					{
						printf("ReadFile(pFile, bufu, 300, &bytesRead, NULL); failed\n");
						Sleep(100000000);
						return 0;
					}
					DWORD bytesWritten = 0;
					WriteFile(pipeThreadClient2, bufu, bytesRead, &bytesWritten, NULL);
					if (bytesWritten == 0)
					{
						printf("WriteFile(pipeClientThread2, bufu, res, &bytesWritten, NULL);  failed !!\n");
						Sleep(100000000);
						return 0;
					}
					if (bytesRead != 300)
						break;

				}
				CloseHandle(pFile);
			}
			else {
				CloseHandle(pipeCreated);
				CloseHandle(askForDisco);
				CloseHandle(pipeClientThread2);
				CloseHandle(pipeThreadClient2);
				break;
			}
		}
	}
	return 0;
}

int wmain(int argc, wchar_t *argv[])
{
	mainPipeServer = CreateNamedPipe(mainPipeServerName, PIPE_ACCESS_OUTBOUND, PIPE_TYPE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 4096, 4096, NMPWAIT_USE_DEFAULT_WAIT, NULL);
	if (mainPipeServer == INVALID_HANDLE_VALUE)
	{
		printf("CreateNamedPipe--mainPipeServer-- failed in server main\n");
		return 0;
	}
	//ConnectNamedPipe(mainPipeServer, NULL);
	//...command line arguments.....
	int numberOfThreads = wcstol(argv[2], NULL, 10);
	wcscpy_s(currentDirectory, argv[1]);
	//...command line arguments.....
	hSem = CreateSemaphore(NULL, 0, numberOfThreads, L"Global\\SemaphoreName");
	//////
	hSemNoOverlap = CreateSemaphore(NULL, 1, 1, L"Global\\SemaphoreClientOverlap"); // BINARY SEMAPHORE
	//////
	int params[NR_THREADS];

	HANDLE hThreads[NR_THREADS];
	for (int i = 0; i < numberOfThreads; i++)
	{
		params[i] = i+1;
		hThreads[i] = CreateThread(NULL, 0, ThreadFunc, &params[i], 0, NULL);

	}
	for (int i = 0; i < numberOfThreads; i++)
	{
		WaitForSingleObject(hThreads[i],INFINITE);

	}
	return 0;
}