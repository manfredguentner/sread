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

#define BUFFER_SIZE 10

void usage() {
  printf("Usage: -t timeout\n");
}

int 
main(int argc, char **argv) {
  int op;
  opterr = 0;
  double timeout;
 
  while ((op = getopt (argc, argv, "ht:")) != -1)
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

  struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

  fd_set set;
  struct timeval tv;
  char c;

  FD_ZERO(&set);
  FD_SET(STDIN_FILENO, &set);
  tv.tv_sec = (int)timeout;
  tv.tv_usec = (int)((timeout - (int)timeout) * 1000000);

  int ret = select(STDIN_FILENO + 1, &set, NULL, NULL, &tv);
  if (ret > 0) {
    read(STDIN_FILENO, &c, 1);
    printf("%c\n", c);
  } else {
    printf("");
  }

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, ~O_NONBLOCK);

  return 0;
}
