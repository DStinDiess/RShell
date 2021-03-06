#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <fcntl.h>

#include "../header/pipe.h"

Pipe::Pipe() : Connector(0, 0) {}

Pipe::Pipe(Base* left, Base* right) : Connector(left, right) {}

bool Pipe::execute() {
    int pipefd[2];
    pid_t cpid1, cpid2;
    int statVal1;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return false;
    }

    cpid1 = fork();
    if (cpid1 < 0) {
        perror("fork");
        return false;
    }

    if (cpid1 == 0) {
        cpid2 = fork();
        if (cpid2 == 0) {
            if (dup2(pipefd[0], 0) == -1) {
                perror("dup2");
                exit(EXIT_FAILURE);
                return false;
            }
            if (close(pipefd[1]) == -1) {
                perror("close");
                exit(EXIT_FAILURE);
                return false;
            }
            if (!right->execute()) {
                exit(EXIT_FAILURE);
                return false;
            }
            exit(EXIT_SUCCESS);
        } else if (cpid2 < 0) {
            perror("fork");
            return false;
        } else {
            if (dup2(pipefd[1], 1) == -1) {
                perror("dup2");
                exit(EXIT_FAILURE);
                return false;
            }
            if (close(pipefd[1]) == -1) {
                perror("close");
                exit(EXIT_FAILURE);
                return false;
            }
            if (!left->execute()) {
                exit(EXIT_FAILURE);
                return false;
            }
            exit(EXIT_SUCCESS);
        }
    }

    if (close(pipefd[0]) == -1) {
        perror("close");
        return false;
    }
    if (close(pipefd[1]) == -1) {
        perror("close");
        return false;
    }
    do {
        waitpid(cpid1, &statVal1, 0);
    } while (!WIFEXITED(statVal1));

    return !WEXITSTATUS(statVal1);
}

int Pipe::precedence() {
    return 1;
}
