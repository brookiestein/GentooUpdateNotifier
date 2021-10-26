#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <getopt.h>
#include <unistd.h>

#include "notifier.h"

int usage(const std::string name);
std::string exec(const char* command = "", bool deleteNewLineChar = false);
int run(long seconds, bool also_layman);

int
main(int argc, char* argv[])
{
        long seconds {1};
        char specifier {'s'};
        bool also_layman {};

        const struct option large_options[] = {
                { "help",         no_argument,          0,      'h' },
                { "layman",       no_argument,          0,      'l' },
                { "specifier",    required_argument,    0,      's' },
                { "time-to-wait", required_argument,    0,      't' },
                { NULL, 0, NULL, 0 }
        };

        int option;
        while ((option = getopt_long(argc, argv, "hls:t:", large_options, NULL)) >= 0) {
                switch (option)
                {
                case 'h': return usage(argv[0]);
                case 'l': also_layman = true; break;
                case 's': specifier = optarg[0]; break;
                case 't': seconds = std::atoi(optarg); break;
                default: break;
                }
        }

        if (geteuid() != 0) {
                std::cerr << "You need to run me as user root." << std::endl;
                return 1;
        } else if (argc == 1) {
                seconds = 3600 * 48; /* 2 days. */
        }

        if (specifier != 's') {
                /* Seconds in either minutes, hours, or days. */
                const unsigned minutes = 60;
                const unsigned hours = 3600;
                const unsigned days = hours * 24;
                switch (specifier)
                {
                case 'm': seconds *= minutes; break;
                case 'h': seconds *= hours; break;
                case 'd': seconds *= days; break;
                default:
                        std::cerr << "Unknown time specifier." << std::endl;
                        return 1;
                }
        }

        return run(seconds, also_layman);
}

int
usage(const std::string name)
{
        std::cout << name << " usage: " << "\n\n";
        std::cout << "-h | --help" << "\t\t\t" << "Show this help." << '\n';
        std::cout << "-l | --layman" << "\t\t\t" << "Synchronize layman's repositories too." << '\n';
        std::cout << "-s | --specifier [s, m, h, d]" << '\t' << "Time to wait specifier." << '\n';
        std::cout << "-t | --time-to-wait <time>" << "\t" << "Time to wait per se." << '\n';
        std::cout << '\n' << "Note: In -s option, specifier means:" << '\n';
        std::cout << "s = seconds, m = minutes, h = hours, d = days." << '\n';
        std::cout << "If none is specified, seconds is default one." << std::endl;
        return 0;
}

/* Credits: https://stackoverflow.com/users/854871/gregpaton08 */
std::string
exec(const char* command, bool deleteNewLineChar)
{
        std::array<char, 128> buffer;
        std::string output;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command, "r"), pclose);

        if (!pipe)
                throw std::runtime_error("An error occurred while running the command.");

        while (std::fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
                output += buffer.data();

        if (deleteNewLineChar)
                output = output.substr(0, output.size() - 1);

        return output;
}

int
run(long seconds, bool also_layman)
{
        while (true) {
                try {
                        std::string command;
                        std::string output;

                        command = "emerge --sync";
                        exec(command.c_str());

                        if (also_layman) {
                                command = "layman --sync-all";
                                exec(command.c_str());
                        }

                        /* -puDN means: emerge --pretend --update --deep --newuse */
                        /* This will give us the number of packages that would be merged after synchronizing.
                        * That doesn't necesarily mean that that is the number of updates because there could
                        * be new packages if they are needed. */
                        command = "emerge -puDN --with-bdeps=y @world | grep Total | awk '{print $2}'";
                        output = exec(command.c_str(), true);

                        std::string message = "There";
                        if (output == "1")
                                message += " is an update ";
                        else
                                message += " are " + output + " updates ";
                        message += "available.";
                        notify(message);
                } catch (const std::runtime_error& e) {
                        notify(e.what());
                        return 1;
                }

                std::this_thread::sleep_for(std::chrono::seconds(seconds));
        }
}
