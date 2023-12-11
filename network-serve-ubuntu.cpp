#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

struct Args
{
    bool watch = false;     // Watch for changes (default: false)
    int number = -1;        // Number of records to display (default: -1, all)
    bool establish = false; // Filter for established connections (default: false)
    std::string filter;
};

Args parse_args(int argc, char *argv[])
{
    Args args; // Initialize arguments structure

    // Iterate through arguments
    for (int i = 1; i < argc; ++i)
    {
        std::string arg(argv[i]); // Get current argument

        // Check for watch flag
        if (arg == "-w")
        {
            args.watch = true;
            continue;
        }

        // Check for number argument
        if (arg == "-n")
        {
            if (i + 1 < argc)
            {
                args.number = std::stoi(argv[i + 1]);
            }
            else
            {
                std::cerr << "Missing argument for -n option." << std::endl;
                return args;
            }
            continue;
        }

        // Check for establish flag
        if (arg == "-e")
        {
            args.establish = true;
            continue;
        }

        if (arg == "-f")
        {
            if (i + 1 < argc)
            {
                args.filter = argv[i + 1];
                ++i; // Skip the next argument after -f
            }
            else
            {
                std::cerr << "Missing argument for -f option." << std::endl;
                return args;
            }
            continue;
        }

        // Handle unrecognized arguments
        std::cerr << "Unrecognized argument: " << arg << std::endl;
    }

    return args;
}

void printTcpConnections(int n, bool established, std::string filter)
{
    // Execute netstat command to get TCP connections information
    std::string command = "netstat -t -n -p --numeric-ports";
    if(!filter.empty()) {
        command += "| grep " + filter;
    }
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        std::cerr << "popen() failed!" << std::endl;
        return;
    }

    char buffer[128];
    std::cout << "PID\tLocal Address\t\tPort\tRemote Address\t\tRemote Port" << std::endl;
    std::cout << "-------------------------------------------------------------------" << std::endl;

    int i = 0;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        if ((buffer, "ESTABLISHED") != nullptr || established)
        {
            std::cout << buffer;
            if (i > n && n != -1)
                break;
            i++;
        }
    }

    pclose(pipe);
}

int main(int argc, char *argv[])
{
    bool watch = false; // Default refresh rate in seconds

    Args args = parse_args(argc, argv);
    std::cout << "args - " << args.number;
    bool show = true;

    while (show)
    {
        printTcpConnections(args.number, args.establish, args.filter);
        if (!args.watch)
        {
            show = false;
        }
        else
        {
            sleep(2); // Wait for 2 seconds and refresh output
            system("clear");
        }
    }

    return 0;
}
