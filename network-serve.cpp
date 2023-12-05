#include <iostream>
#include <winsock2.h>
#include <Windows.h>
#include <iphlpapi.h>
#include <winsock2.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

int main() {
    // Code to retrieve TCP connections

    PMIB_TCPTABLE_OWNER_PID pTcpTable;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed." << std::endl;
        return 1;
    }

    // Retrieve table size
    if (GetExtendedTcpTable(NULL, &dwSize, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0) != ERROR_INSUFFICIENT_BUFFER) {
        std::cout << "GetExtendedTcpTable failed to get size." << std::endl;
        WSACleanup();
        return 1;
    }

    // Allocate memory for the table
    pTcpTable = (PMIB_TCPTABLE_OWNER_PID)malloc(dwSize);
    if (pTcpTable == NULL) {
        std::cout << "Memory allocation failed." << std::endl;
        WSACleanup();
        return 1;
    }

    // Retrieve TCP table
    if ((dwRetVal = GetExtendedTcpTable(pTcpTable, &dwSize, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0)) != NO_ERROR) {
        std::cout << "GetExtendedTcpTable failed with error: " << dwRetVal << std::endl;
        free(pTcpTable);
        WSACleanup();
        return 1;
    }

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
    WSACleanup();
    return 0;
}
