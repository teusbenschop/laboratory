/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2022.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

#include <sys/un.h>
#include <sys/socket.h>
#include <ctype.h>

// Maximum size of messages exchanged between client and server.
#define BUF_SIZE 1024

// The Unix domain socket.
#define SV_SOCK_PATH "/tmp/ud_ucase"
