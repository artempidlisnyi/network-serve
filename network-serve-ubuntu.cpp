#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

struct Args
{
    bool watch = false;
    int number = -1;
    std::string filter;
};

Args parse_args(int argc, char *argv[])
{
    Args args;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg(argv[i]);

        if (arg == "-w")
        {
            args.watch = true;
            continue;
        }

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

        if (arg == "-f")
        {
            if (i + 1 < argc)
            {
                args.filter = argv[i + 1];
                ++i;
            }
            else
            {
                std::cerr << "Missing argument for -f option." << std::endl;
                return args;
            }
            continue;
        }

        std::cerr << "Unrecognized argument: " << arg << std::endl;
    }

    return args;
}

void printTcpConnections(int n, std::string filter)
{

    std::string command = "netstat -t -n -p --numeric-ports";
    if (!filter.empty())
    {
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

        std::cout << buffer;
        if (i > n && n != -1)
            break;
        i++;
    }

    pclose(pipe);
}

int main(int argc, char *argv[])
{
    bool watch = false;

    Args args = parse_args(argc, argv);
    std::cout << "args - " << args.number;
    bool show = true;

    while (show)
    {
        printTcpConnections(args.number, args.filter);
        if (!args.watch)
        {
            show = false;
        }
        else
        {
            sleep(2);
            system("clear");
        }
    }

    return 0;
}
