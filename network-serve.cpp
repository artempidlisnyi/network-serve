#include <iostream>
#include <winsock2.h>
#include <Windows.h>
#include <iphlpapi.h>
#include <cstring>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

void printTcpConnections() {
    PMIB_TCPTABLE_OWNER_PID pTcpTable;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    // Retrieve table size
    if (GetExtendedTcpTable(NULL, &dwSize, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0) != ERROR_INSUFFICIENT_BUFFER) {
        std::cout << "GetExtendedTcpTable failed to get size." << std::endl;
        return;
    }

    // Allocate memory for the table
    pTcpTable = (PMIB_TCPTABLE_OWNER_PID)malloc(dwSize);
    if (pTcpTable == NULL) {
        std::cout << "Memory allocation failed." << std::endl;
        return;
    }

    // Retrieve TCP table
    if ((dwRetVal = GetExtendedTcpTable(pTcpTable, &dwSize, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0)) != NO_ERROR) {
        std::cout << "GetExtendedTcpTable failed with error: " << dwRetVal << std::endl;
        free(pTcpTable);
        return;
    }

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD cursorPosition = {0, 0};
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    DWORD written;

    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    FillConsoleOutputCharacter(hConsole, ' ', consoleInfo.dwSize.X * consoleInfo.dwSize.Y, cursorPosition, &written);
    FillConsoleOutputAttribute(hConsole, consoleInfo.wAttributes, consoleInfo.dwSize.X * consoleInfo.dwSize.Y, cursorPosition, &written);
    SetConsoleCursorPosition(hConsole, cursorPosition);

    std::cout << "PID\tLocal Address\t\tPort\tRemote Address\t\tRemote Port" << std::endl;
    std::cout << "-------------------------------------------------------------------" << std::endl;

    for (DWORD i = 0; i < pTcpTable->dwNumEntries; i++) {
        MIB_TCPROW_OWNER_PID row = pTcpTable->table[i];
        std::cout << row.dwOwningPid << "\t"
                  << ((row.dwLocalAddr >> 0) & 0xFF) << "."
                  << ((row.dwLocalAddr >> 8) & 0xFF) << "."
                  << ((row.dwLocalAddr >> 16) & 0xFF) << "."
                  << ((row.dwLocalAddr >> 24) & 0xFF) << "\t"
                  << ntohs((u_short)row.dwLocalPort) << "\t"
                  << ((row.dwRemoteAddr >> 0) & 0xFF) << "."
                  << ((row.dwRemoteAddr >> 8) & 0xFF) << "."
                  << ((row.dwRemoteAddr >> 16) & 0xFF) << "."
                  << ((row.dwRemoteAddr >> 24) & 0xFF) << "\t"
                  << ntohs((u_short)row.dwRemotePort) << std::endl;
    }

    free(pTcpTable);
}

int main(int argc, char *argv[]) {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed." << std::endl;
        return 1;
    }

    bool watch = false; // Default refresh rate in milliseconds

    // Check if -w option was passed and store the value in 'watch' variable
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-w") == 0) {
            watch = true; // Convert the next argument to an integer (milliseconds)
            break;
        }
    }
    bool show = true;
      std::cout << watch << std::endl;

    while (show) {
        printTcpConnections();
        if (watch == false) {
            show = false;
        }
        Sleep(2000); // Wait for 2 seconds and refresh output
    }

    WSACleanup(); // Cleanup Winsock
    return 0;
}
