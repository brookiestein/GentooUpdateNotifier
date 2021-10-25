#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <unistd.h>

#include "notifier.h"

std::string exec(const char* command = "", bool deleteNewLineChar = false);
int run(unsigned long seconds);

int
main(int argc, char* argv[])
{
        unsigned long seconds {1};
        if (argc >= 2) {
                seconds = std::atoi(argv[1]);
                if (argc > 2) {
                        const unsigned minute = 60;
                        const unsigned hour = 3600;
                        const unsigned day = hour * 24;

                        switch (argv[2][0])
                        {
                        case 's': break;
                        case 'm': seconds *= minute; break;
                        case 'h': seconds *= hour; break;
                        case 'd': seconds *= day; break;
                        default:
                                std::cerr << "Time not recognized." << std::endl;
                                return 1;
                        }
                }
        } else {
                /* Well, the 'day' variable does not exit here, but it doesn't hurt
                 * to understand what's going on here. */
                seconds *= (3600 * 24 * 2); /* Verify each two days. */
        }

        return run(seconds);
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
run(unsigned long seconds)
{
        while (true) {
                try {
                        std::string command;
                        std::string output;

#ifdef SYNCHRONIZE
                        if (geteuid() == 0) {
                                command = "emerge --sync";
                                output = exec(command.c_str());
                        } else {
                                std::string message = "Couldn't synchronize because you're not root.";
                                notify(message);
                        }
#endif

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
