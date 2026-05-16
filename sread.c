/* Copyright (c) 2026, Manfred Güntner
    SPDX-License-Identifier: BSD-2-Clause */

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h> 
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <ctype.h>
#include <getopt.h>


void usage() {
  printf("Usage: -t timeout\n");
}

int 
main(int argc, char **argv) {
  int op;
  opterr = 0;
  double timeout = 0.000;
  char prompt[1024];
  int promptlen = 0;
 
  while ((op = getopt (argc, argv, "ht:p:")) != -1)
  {
    switch (op)
      {
      case 'h':
        usage();
        return 0;
      case 't':
        /* cvalue = optarg; */
        timeout = strtod(optarg, NULL);
        break;
      case 'p':
        promptlen = snprintf(prompt, 1024, "%s", optarg);
        break;    
      case '?':
        if (optopt == 't')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }
  }
  if (!timeout) {
    usage();
    return 0;
  }

  if (timeout < 0) {
    printf("negative timeout. Cant do timetravel.. \n");
    return 1;
  }

  /* open TTY for the prompt */
  int td = -1;
  td = open("/dev/tty", O_RDWR | O_NOCTTY);
  if (td == -1) {
    perror("open tty");
    return 1;
  }

  /* tty fd exists, so use it. If tty fails fallback to stdin */
  int r;
  int usetty = 0;
  struct termios oldt, newt;
  r = tcgetattr(td, &oldt);
  if (r == -1) {
    close(td);
    td = STDIN_FILENO;
  } else {
    usetty = 1;
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    r = tcsetattr(td, TCSANOW, &newt);
    if (r == -1) {
      usetty = 0;
      td = STDIN_FILENO;
    }
  }

  fd_set set;
  struct timeval tv;
  char c;

  FD_ZERO(&set);
  FD_SET(td, &set);
  tv.tv_sec = (int)timeout;
  tv.tv_usec = (int)((timeout - (int)timeout) * 1000000);

  /* while in raw mode lowlevel write ist required */
  write(td, "\033[?25l",6);
  write(td, "\033[7m",4);
  write(td, prompt, promptlen);
  write(td, "\r",1);
  write(td, "\033[0m",4);

  int ret = select(td + 1, &set, NULL, NULL, &tv);
  if (ret > 0) {
    read(td, &c, 1); 
    /* if key clear line and activate cursor and set terminal back */ 
    write(td, "\033[2K\r",5);
    write(td, "\033[?25h",6);
    if ( usetty == 1) {
      /* only reset if tty */
      tcsetattr(td, TCSANOW, &oldt);
    }
    printf("%c\n", c);
  } else {
    /* if timeout clear line and activate cursor and set terminal back */
    write(td, "\033[2K\r",5);
    write(td, "\033[?25h",6);
    if (usetty == 1) {
      /* only reset if tty */
      tcsetattr(td, TCSANOW, &oldt);
    }
    puts("");
  }

  /* close tty if used */
  if ( usetty == 1) {
    close(td);
  }
  
  /* Thats all folks */
  return 0;
}
