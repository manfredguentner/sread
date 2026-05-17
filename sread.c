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
  char c1;
  char c2;
  char c3;
  char c4;

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

  /* read key and try to parse it if ESC sequence */
    ssize_t rb = 0;
    int i = 0;
    int isparsed = 0;
    char keycode[20];
    while (1) { 
      rb = read(td, &c, 1);
      /* not ESC: print printable or hex */
      if (c != 27) {
        if (c >= 32 && c <= 126) {
          keycode[0] = c;
          keycode[1] = '\0'; 
        }
        break;
      }

      /* before we ove on we have to do a 200ms wait
         to see if it is ONLY the ESC key */

      FD_ZERO(&set);
      FD_SET(td, &set);
      tv.tv_sec = 0;
      tv.tv_usec = 200000; /* 200 ms */
      /* if select times out it is the Esc key */
      if (select(td + 1, &set, NULL, NULL, &tv) <= 0) {
        strcpy(keycode,"ESC");
        isparsed = 1;
        break;
      } else {
        rb = read(td, &c1, 1);
      }
      /* An now we go on a long journey to hopefully get all
         105/104 ISO keyboard keys. I dont have mucht to test.. */

      if (c1 == '[') {
        rb = read(td, &c2, 1); 
        switch (c2) {
          case 'A': strcpy(keycode,"KEY_UP"); 
                    isparsed = 1;
                    break;
          case 'B': strcpy(keycode,"KEY_DOWN"); 
                    isparsed = 1;
                    break;
          case 'C': strcpy(keycode,"KEY_RIGHT"); 
                    isparsed = 1;
                    break;
          case 'D': strcpy(keycode,"KEY_LEFT"); 
                    isparsed = 1;
                    break;
          case 'H': strcpy(keycode,"KEY_HOME"); 
                    isparsed = 1;
                    break;
          case 'F': strcpy(keycode,"KEY_END"); 
                    isparsed = 1;
                    break;
          case '1': strcpy(keycode,"KEY_HOME"); 
                    read(td, &c3, 1);
                    /* for two digit codes we have to read on */
                    if (c3 >= '0' && c3 <= '9') {
                      switch (c3) {
                        case '5': strcpy(keycode,"KEY_F5");
                                  read(td, &c4, 1);
                                  break;
                        case '7': strcpy(keycode,"KEY_F6");
                                  read(td, &c4, 1);
                                  break;
                        case '8': strcpy(keycode,"KEY_F7");
                                  read(td, &c4, 1);
                                  break;
                        case '9': strcpy(keycode,"KEY_F8");
                                  read(td, &c4, 1);
                                  break;
                      }
                    }
                    isparsed = 1;
                    break;
          case '2': strcpy(keycode,"KEY_INSERT"); 
                    read(td, &c3, 1);
                    /* the twenties codes */
                    if (c3 >= '0' && c3 <= '9') {
                      switch (c3) {
                        case '0': strcpy(keycode,"KEY_F9");
                                  read(td, &c4, 1);
                                  break;  
                        case '1': strcpy(keycode,"KEY_F10");
                                  read(td, &c4, 1);
                                  break;  
                        case '3': strcpy(keycode,"KEY_11");
                                  read(td, &c4, 1);
                                  break;  
                        case '4': strcpy(keycode,"KEY_F12");
                                  read(td, &c4, 1);
                                  break;  
                      }
                    }
                    isparsed = 1;
                    break;
          case '3': strcpy(keycode,"KEY_DELETE"); 
                    read(td, &c3, 1);
                    isparsed = 1;
                    break;
          case '4': strcpy(keycode,"KEY_END"); 
                    read(td, &c3, 1);
                    isparsed = 1;
                    break;
          case '5': strcpy(keycode,"KEY_PGUP"); 
                    read(td, &c3, 1);
                    isparsed = 1;
                    break;
          case '6': strcpy(keycode,"KEY_PGDN"); 
                    read(td, &c3, 1);
                    isparsed = 1;
                    break;
        }  
        if (isparsed == 1) {
          break;
        }
      /* There is not only [. There is also O */
      } else if (c1 == 'O') {
        rb = read(td, &c2, 1);
        switch (c2) {
          case 'A': strcpy(keycode,"KEY_UP"); 
                    isparsed = 1;
                    break;
          case 'B': strcpy(keycode,"KEY_DOWN");
                    isparsed = 1;
                    break;
          case 'C': strcpy(keycode,"KEY_RIGHT");
                    isparsed = 1;
                    break;
          case 'D': strcpy(keycode,"KEY_LEFT");
                    isparsed = 1;
                    break;
          case 'H': strcpy(keycode,"KEY_HOME");
                    isparsed = 1;
                    break;
          case 'F': strcpy(keycode,"KEY_END");
                    isparsed = 1;
                    break;
          case 'P': strcpy(keycode,"KEY_F1");
                    isparsed = 1;
                    break;
          case 'Q': strcpy(keycode,"KEY_F2");
                    isparsed = 1;
                    break;
          case 'R': strcpy(keycode,"KEY_F3");
                    isparsed = 1;
                    break;
          case 'S': strcpy(keycode,"KEY_F4");
                    isparsed = 1;
                    break;
        }
      }
      if (isparsed == 1) {
        break;
      }
    }

    /* if key clear line and activate cursor and set terminal back */ 
    write(td, "\033[2K\r",5);
    write(td, "\033[?25h",6);
    if ( usetty == 1) {
      /* only reset if tty */
      tcsetattr(td, TCSANOW, &oldt);
    }
    /* Print keycode */
    printf("%s\n", keycode);
  } else {
    /* if timeout clear line and activate cursor and set terminal back */
    write(td, "\033[2K\r",5);
    write(td, "\033[?25h",6);
    if (usetty == 1) {
      /* only reset if tty */
      tcsetattr(td, TCSANOW, &oldt);
    }
    /* print no keycode */
    puts("");
  }

  /* close tty if used */
  if ( usetty == 1) {
    close(td);
  }
  
  /* Thats all folks */
  return 0;
}
