#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

int main(int argc, char** argv) {
  int pipefd[2];
  pid_t pid;
  char* msg = "hello";
  if (pipe(pipefd)==-1){
      perror("Pipe error: ");
      exit(1);
  }
  if (!(pid = fork())){ //child actions    
  write(pipefd[1],msg, strlen(msg));
	close(pipefd[0]);
  }
  else {
	  char buffer[10];
	  for(int i=0;i<10;i++)
	    buffer[i]=0; //init buffer with null terminators
    read(pipefd[0], buffer, strlen(msg));
    printf("message from child: %s\n", buffer);
	  close(pipefd[1]);
  }
  return 0;
}