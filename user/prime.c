#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
primes(int num, int parentPipe[2]) {
  close(parentPipe[1]);
  fprintf(1, "prime: %d\n");
  int childPipe[2];

  if (pipe(childPipe) == -1) {
    fprintf(2, "Pipe error\n");
    exit(1);
  }

  int buffer[2];
  int pid = 0;
  do {
    read(parentPipe[0], buffer, sizeof(buffer) + 1);
    if (buffer[0] % num != 0) {
      if (forked) {
        write(childPipe[1], buffer[0], sizeof(buffer[0]) + 1);
      }
    }
  } while (buffer[0] < 100 && buffer[0] != -1)
  return 0;
}

int
main(int argc, char *argv[])
{
  int pid = 0;
  for (int i = 2; i <= 280; i += 1) {

  }

  int pipeP[2];
  int pipeC[2];
  int pid;
  char buffer[2];

  if (pipe(pipeP) == -1 || pipe(pipeC) == -1) {
    fprintf(2, "Pipe error\n");
    exit(1);
  }

  pid = fork();

  if (pid == -1) {
    fprintf(2, "Fork error\n");
    exit(1);
  }

  if (pid > 0) { // Parent
    close(pipeP[0]);
    close(pipeC[1]);

    write(pipeP[1], "x", 2);
    close(pipeP[1]);

    wait()

    read(pipeC[0], buffer, 2);
    if (buffer == "y\0") {
        fprintf(1, "<%d>: recieved pong", pid);
    }
    close(pipeC[0]);
  } else { // Child
    close(pipeP[1]);
    close(pipeC[0]);

    sleep(1);

    read(pipeP[0], buffer, 2);
    if (buffer == "x\0") {
        fprintf(1, "<%d>: recieved ping", pid);
    }
    close(pipeP[0]);

    write(pipeC[1], "y", 2);
    close(pipeC[1]);
  }

  exit(0);
}
