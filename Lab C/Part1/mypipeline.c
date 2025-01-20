#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define READ_END 0
#define WRITE_END 1


int main(void) {
    int pipe_fd[2];
    int pid1, pid2;
    char *ls_args[] = {"ls", "-l", NULL};
    char *tail_args[] = {"tail", "-n", "2", NULL};
    // create pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // fork first child process
    fprintf(stderr, "(parent_process>forking...)\n");
    pid1 = fork();

    if (pid1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "(parent_process>created process with id: %d)\n", pid1);

    if (pid1 == 0) {  // child1 process
        close(STDOUT_FILENO);  // close standard output
        dup(pipe_fd[WRITE_END]);  // duplicate write-end of pipe
        close(pipe_fd[WRITE_END]);  // close duplicated file descriptor
        fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe...)\n");
        fprintf(stderr, "(child1>going to execute cmd: %s %s %s)\n", ls_args[0], ls_args[1], ls_args[2]);
        execvp(ls_args[0], ls_args);  // execute "ls -l"
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    // parent process
    fprintf(stderr, "(parent_process>closing the write end of the pipe...)\n");
    close(pipe_fd[WRITE_END]);  // close write-end of pipe

    // fork second child process
    fprintf(stderr, "(parent_process>forking...)\n");
    pid2 = fork();
    
    if (pid2 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "(parent_process>created process with id: %d)\n", pid2);

    if (pid2 == 0) {  // child2 process
        close(STDIN_FILENO);  // close standard input
        fprintf(stderr, "(child2>redirecting stdin to the read end of the pipe...)\n");
        dup(pipe_fd[READ_END]);  // duplicate read-end of pipe
        close(pipe_fd[READ_END]);  // close duplicated file descriptor

        fprintf(stderr, "(child2>going to execute cmd: %s %s %s)\n", tail_args[0], tail_args[1], tail_args[2]);
        execvp(tail_args[0], tail_args);  // execute "tail -n 2"
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "(parent_process>closing the read end of the pipe...)\n");
    close(pipe_fd[READ_END]);  // close read-end of pipe


    fprintf(stderr, "(parent_process>waiting for child processes to terminate...)\n");
    waitpid(pid1, NULL, 0);  // wait for child1 process to terminate
    waitpid(pid2, NULL, 0);  // wait for child2 process to terminate

    fprintf(stderr, "(parent_process>exiting...)\n");
    return 0;
}

