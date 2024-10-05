#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "stdbool.h"

int
main(int argc, char *argv[])
{
    int pid;
    bool main = true;
    int childPipe[2];
    int parentPipe[2];
    int buffer[2];
    int number = -1;

    if (pipe(childPipe) == -1) {
        printf("Pipe error\n");
        exit(1);
    }

    while (true) {
        if (main) {
            main = false;
            pid = fork();

            if (pid == -1) {
                printf("Fork error\n");
                exit(1);
            } else if (pid > 0) {
                main = true;
                close(childPipe[0]);
                break;
            } else {
                close(childPipe[1]);
                parentPipe[0] = childPipe[0];
                parentPipe[1] = 0;
            }
        }

        read(parentPipe[0], buffer, sizeof(number) + 1);

        if (buffer[0] == -1) {
            if (pid > 0) {
                write(childPipe[1], number, sizeof(number) + 1);
                wait();
                close(childPipe[1]);
            }
            close(parentPipe[0]);
            exit(0);
        } else if (buffer[0] % number != 0) {
            if (pid == 0) {
                if (pipe(childPipe) == -1) {
                    printf("Pipe error\n");
                    exit(1);
                }

                pid = fork();

                if (pid == -1) {
                    printf("Fork error\n");
                    exit(1);
                } else if (pid > 0) {
                    close(childPipe[0]);
                    write(childPipe[1], number, sizeof(number) + 1);
                } else {
                    close(childPipe[1]);
                    parentPipe[0] = childPipe[0];
                    parentPipe[1] = 0;
                    number = buffer[0];
                    printf("prime: %d\n", number);
                }
            } else {
                write(childPipe[1], number, sizeof(number) + 1);
            }
        }
    }

    if (main) {
        for (number = 2; number <= 280; number += 1) {
            write(childPipe[1], number, sizeof(number) + 1);
        }
        write(childPipe[1], -1, sizeof(number) + 1);
        wait();
    } else {
        close(parentPipe[0]);
    }

    close(childPipe[1]);

    exit(0);
}
