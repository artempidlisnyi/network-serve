# Nerwork serve - TCP Connection Monitor

## Description

This application is designed to retrieve and display active TCP connections on a Windows system. It uses the Windows API to gather information about TCP connections and presents the details including Process IDs (PIDs), local and remote addresses, and ports in the console.

## Usage

### Compilation

To compile the application, use the following command:

```bash
g++ network-serve.cpp -o network-serve.exe -liphlpapi -lws2_32 -Wcpp
```

### Run program
```bash
./network-serve.exe
```
