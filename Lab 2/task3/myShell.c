#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>
#include "LineParser.h"
#include <linux/limits.h>
#include <sys/wait.h>
#include <fcntl.h>


#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

int debug;
void pipeCom(cmdLine* cmd);

void pipeCom(cmdLine* cmd){
	int pipefd[2];
  int pid1,pid2;
  if (pipe(pipefd)==-1){
      perror("Pipe error: ");
      exit(1);
  }
  pid1 = fork();
  
  if (!pid1){ //child1 actions    
  	fclose(stdout);
  	dup2(pipefd[1],1);
  	close(pipefd[1]);
  	execvp(cmd->arguments[0],cmd->arguments);
  	perror("ERROR:"); //if exevp fails
  	_exit(1);
  }
  else {
    close(pipefd[1]);
    pid2 = fork();
	if (!pid2){ //child2 actions
    	fclose(stdin);
      dup2(pipefd[0],0);
      close(pipefd[0]);
      execvp(cmd->next->arguments[0],cmd->next->arguments);
      perror("ERROR:"); //if exevp fails
      _exit(1); 
    }
    else{
      close(pipefd[0]);
      waitpid(pid1,NULL,0);
      waitpid(pid2,NULL,0);
    }
  }
}
void execute(cmdLine *pCmdLine);
int main(int argc, char **argv)
{
	debug=0;
	for(int i=1;i<argc;i++){
		if(strcmp(argv[i],"-d")==0){
			printf("Debug Mode Active\n");
			debug=1;
		}
	}
	printf("Starting the program\n");
	while(1){
		char *buf=malloc(2048);
		char currDir[PATH_MAX];
		if(getcwd(currDir,PATH_MAX)==NULL){
			return 1;
		}
		printf("%s: ",currDir);
		fgets(buf,2048,stdin);
		cmdLine* command = parseCmdLines(buf);
		free(buf);
		execute(command);
	}
	return 0;
}
void execute(cmdLine *pCmdLine){
	int Pid;
	int special=0;
	if(strcmp("quit",pCmdLine->arguments[0])==0){
		freeCmdLines(pCmdLine);
		exit(0);
	}
	else if(strncmp("cd",pCmdLine->arguments[0],2)==0){
		if(chdir(pCmdLine->arguments[1])<0)
      		perror("CD Error");
			special=1;
			freeCmdLines(pCmdLine);
		}
	else if(strncmp("kill",pCmdLine->arguments[0],4)==0){
		special=1;
    	int pid = atoi(pCmdLine->arguments[1]);
    	if(kill(pid,SIGINT)==-1)
    	 	perror("Kill Error");
    	else
    	  printf("Sending SIGINT to %d\n",pid);
		}
	else if(strncmp("suspend",pCmdLine->arguments[0],7)==0){
		special=1;
		int pid = atoi(pCmdLine->arguments[1]);
		if(kill(pid,SIGTSTP)==-1)
    	 	perror("Suspend Error");
    	else
    	  printf("Sending SIGSTP to %d\n",pid);
	}
	else if(strncmp("wake",pCmdLine->arguments[0],4)==0){
		special=1;
		int pid = atoi(pCmdLine->arguments[1]);
		if(kill(pid,SIGCONT)==-1)
    	 	perror("Wake Error");
    	else
    	  printf("Sending SIGCONT to %d\n",pid);
	}
	else if(!(Pid=fork())){ 
		if(pCmdLine->inputRedirect){
        	close(0);
        	if(!open(pCmdLine->inputRedirect, 0777)){
          		perror("Invalid input: ");
          		_exit(1);
        	}
      }
      if(pCmdLine->outputRedirect){
        close(1);
        if(!open(pCmdLine-> outputRedirect,0777)){
          perror("Invalid output: ");
          _exit(1);
        }
	  }
	  if(pCmdLine->next==NULL){
		execvp(pCmdLine->arguments[0],pCmdLine->arguments);				
        perror("Invalid Command");
        _exit(1);
	  }
	  else {
		pipeCom(pCmdLine);
	  }
	}
	if(special==0){
	if(pCmdLine->blocking==1){  //if & in command
		if(debug==1)
			printf("blocking...\n");
		waitpid(Pid,NULL,0);
	}
	if(debug==1){
		fprintf(stderr,"Pid: %d, Command: %s\n",Pid,pCmdLine->arguments[0]);
	}
	}

}
