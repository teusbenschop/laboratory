/*
Copyright (©) 2021-2026 Teus Benschop.

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "linux.h"

#include <csignal>
#include <initializer_list>
#include <iostream>

namespace linux {

namespace signals {
void demo()
{
    return;
    sigset_t blocked_signals;

    bool keep_going{true};

    // Sending a Linux signal can be handled in various ways.
    // 1. It gets handled right away.
    // 2. The process ignores it, nothing happens.
    // 3. The kernel keeps that signal pending,
    //    and delivers it to the process when the process asks for it.

    // A set of signals that the kernel will not deliver to the process asynchronically.
    // The kernel will block those signals and deliver them when asked to do so.
    const auto sync_signals = {SIGINT, SIGUSR1, SIGUSR2, SIGHUP, SIGTERM};

    // A set of ignored signals.
    const auto ignored_signals = {SIGALRM, SIGCHLD, SIGPIPE};

    // A set of signals processed asynchronically, that is, in real time.
    // Commented out for macOS.
    const std::initializer_list<int> async_signals = {/*SIGRTMIN, SIGRTMAX*/};

    // Initialize empty set of signals.
    sigemptyset(&blocked_signals);
    // Add the blocked signals to the set.
    for (const int signal : sync_signals)
        sigaddset(&blocked_signals, signal);
    // Inform kernel to block the list of signals.
    pthread_sigmask(SIG_BLOCK, &blocked_signals, nullptr);

    // Ignore the given signals.
    // const struct sigaction sa_ignore = {SIG_IGN, {}, {}, {}};
    // for (const int signal : ignored_signals)
    //     sigaction(signal, &sa_ignore, nullptr);

    // Raise the synchronized signals.
    // Notice that the kernel will not deliver them to the process right away.
    for (const int signal : sync_signals)
    {
        std::cout << "Raise synchronized signal " << signal << " " << strsignal(signal) << std::endl;
        std::raise(signal);
    }

    // Raise the ignored signals -> nothing will happen.
    for (const int signal : ignored_signals)
    {
        std::cout << "Raise ignored signal " << signal << " " << strsignal(signal) << std::endl;
        std::raise(signal);
    }

    // A real time handler for the asynchronically delivered signals.
    // const struct sigaction sa_async = {
    //     [](const int signal)
    //     {
    //         std::cout << "Processing real time signal " << signal << " " << strsignal(signal) << std::endl;
    //     },
    //     {}, {}, {}
    // };
    // for (const int s : async_signals)
    // {
    //     sigaction(s, &sa_async, nullptr);
    // }

    // Raise the real time signals.
    for (const int signal : async_signals)
    {
        std::cout << "Raise real time signal " << signal << " " << strsignal(signal) << std::endl;
        std::raise(signal);
    }

    // Start processing raised signals in a controlled manner.
    // It will get the lowest pending signal first.
    while (keep_going)
    {
        const timespec timespec{.tv_sec = 1, .tv_nsec = 0};
        const int signal = -1;
        //sigtimedwait(&blocked_signals, nullptr, &timespec);
        //sigwait(&blocked_signals, &signal);
        if (signal == -1)
        {
            switch (errno)
            {
            case EINVAL:
                std::cout << "Invalid timespec" << std::endl;
                break;
            case EAGAIN:
                std::cout << "Timeout waiting for signal" << std::endl;
                keep_going = false;
                break;
            case EINTR:
            default:
                std::cout <<
                    "The signal handler was interrupted by an asynchronous signal handled somewhere else "
                    << strerror(errno) << std::endl;
            }
        }
        else
        {
            std::cout << "Processing synchronized signal " << signal << " " << strsignal(signal) << std::endl;
        }
    }
}
}


void demo()
{
    signals::demo();
}
}

