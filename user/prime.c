#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "stdbool.h"
#include "stddef.h"

int
main(int argc, char *argv[])
{
    int pid;
    bool main = true;
    int childPipe[2];
    int parentPipe[2];
    int buffer[2];
    int number = -1;

    printf("creating initial child pipe\n");
    if (pipe(childPipe) == -1) {
        printf("Pipe error\n");
        exit(1);
    }

    printf("starting while\n");
    while (true) {
        printf("top of while\n");
        if (main) {
            printf("main in while about to fork\n");
            main = false;
            pid = fork();

            if (pid == -1) {
                printf("Fork error\n");
                exit(1);
            } else if (pid > 0) {
                printf("parent proces in main while about to exit\n");
                main = true;
                close(childPipe[0]);
                break;
            } else {
                printf("child proces in main while closing write child pipe and copying it to parent\n");
                close(childPipe[1]);
                parentPipe[0] = childPipe[0];
                parentPipe[1] = 0;
            }
        }

        printf("reading parent pipe\n");
        read(parentPipe[0], &buffer, sizeof(number));
        printf("buffer number is %d\n", number);

        if (buffer[0] == -1) {
            printf("buffer is -1\n");
            if (pid > 0) {
                printf("writing -1 to children and waiting\n");
                write(childPipe[1], &number, sizeof(number));
                wait(NULL);
                close(childPipe[1]);
            }
            printf("stoping after buffer -1\n");
            close(parentPipe[0]);
            exit(0);
        } else if (buffer[0] % number != 0) {
            printf("buffer number is not divisible\n");
            if (pid == 0) {
                printf("creating child pipe and forking\n");
                if (pipe(childPipe) == -1) {
                    printf("Pipe error\n");
                    exit(1);
                }

                pid = fork();

                if (pid == -1) {
                    printf("Fork error\n");
                    exit(1);
                } else if (pid > 0) {
                    printf("parent process writing the number to the child pipe\n");
                    close(childPipe[0]);
                    write(childPipe[1], &number, sizeof(number));
                } else {
                    printf("child process moving pipes and about to print the prime\n");
                    close(childPipe[1]);
                    parentPipe[0] = childPipe[0];
                    parentPipe[1] = 0;
                    number = buffer[0];
                    printf("prime: %d\n", number);
                }
            } else {
                printf("already have a child sending the number onward\n");
                write(childPipe[1], &number, sizeof(number));
            }
        }
    }

    printf("outside of while loop\n");

    if (main) {
        printf("main outside of while loop sending numbers\n");
        for (number = 2; number <= 280; number += 1) {
            write(childPipe[1], &number, sizeof(number));
            printf("wrote %d\n", number);
        }
        printf("writing last -1 to stop all processes\n");
        number = -1;
        write(childPipe[1], &number, sizeof(number));
        wait(NULL);
    } else {
        close(parentPipe[0]);
    }

    printf("exiting\n");

    close(childPipe[1]);

    exit(0);
}
