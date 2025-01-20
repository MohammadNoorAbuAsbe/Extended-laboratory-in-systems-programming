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
#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0
#define HISTLEN 5
typedef struct process
{
	cmdLine *cmd;		  /* the parsed command line*/
	pid_t pid;			  /* the process id that is running the command*/
	int status;			  /* status of the process: RUNNING/SUSPENDED/TERMINATED */
	struct process *next; /* next process in chain */
} process;
char *commands[HISTLEN];
int listfull = 0;
int addedCom = 0;
process *Processes;
int debug;
void printProcessList(process **process_list);
void printProcess(process *process, int index);
void pipeCom(cmdLine *cmd);
char *statusStr(int status)
{
	if (status == -1)
		return "TERMINATED";
	else if (status == 0)
		return "SUSPENDED";
	else if (status == 1)
		return "RUNNING";
	else
	{
		return "ERROR: Invalid Status Value";
	}
}
void addProcess(process **process_list, cmdLine *cmd, pid_t pid)
{
	process *toAdd = malloc(sizeof(struct process));
	toAdd->cmd = cmd;
	toAdd->next = NULL;
	toAdd->pid = pid;
	toAdd->status = RUNNING;
	if (*process_list == NULL)
	{
		*process_list = toAdd;
		// Processes=process_list;
		if (debug == 1)
		{
			printProcessList(&Processes);
			printf("Process Added\n");
		}
	}
	else
	{
		process *currProc = *process_list;
		while (currProc->next != NULL)
		{
			currProc = currProc->next;
		}
		currProc->next = toAdd;
		if (debug == 1)
			if (debug == 1)
			{
				printProcessList(&Processes);
				printf("Process Added\n");
			}
	}
}
void freeProcess(process *curProcess)
{
	if (curProcess != NULL)
	{
		freeProcess(curProcess->next);
		freeCmdLines(curProcess->cmd);
		free(curProcess);
	}
}
void freeList(process **processes)
{
	if (*processes != NULL)
		freeProcess(*processes);
}
process *deleteTerminated(process *process_list)
{
	if (process_list != NULL)
	{
		if (process_list->status == TERMINATED)
		{
			// if(debug==1){
			//	printf("deleting: ");
			//	printProcess(process_list,0);
			// }
			process *tmp = process_list->next;
			freeCmdLines(process_list->cmd);
			process_list->cmd = NULL;
			process_list = NULL;
			free(process_list);
			process_list = tmp;
			return deleteTerminated(process_list);
		}
		else
		{
			if (process_list->next != NULL)
				deleteTerminated(process_list->next);
			return process_list;
		}
	}
	else
		return NULL;
}
void updateProcesses()
{
	if (Processes != NULL)
	{
		process *currProc = Processes;
		while (currProc != NULL)
		{
			int status;
			int updateStatus = RUNNING;
						int isChanged = waitpid(currProc->pid, &status, WNOHANG | WUNTRACED);
			if (isChanged != 0)
			{
				if (WIFEXITED(status) || WIFSIGNALED(status))
					updateStatus = TERMINATED;
				else if (WIFSTOPPED(status))
					updateStatus = SUSPENDED;
				else if (WIFCONTINUED(status))
					updateStatus = RUNNING;
			}
			currProc->status = updateStatus;
			currProc = currProc->next;
		}
	}
}

void printProcess(process *currProcess, int index)
{
	if (currProcess != NULL && currProcess->cmd != NULL)
	{
		printf("\n%d) ", index);
		printf("%d ", currProcess->pid);
		printf("%s ", statusStr(currProcess->status));

		for (int i = 0; i < currProcess->cmd->argCount; i++)
		{
			printf("%s  ", currProcess->cmd->arguments[i]);
		}
	}
}

void printProcessList(process **process_list)
{
	updateProcesses();
	if (*process_list != NULL)
	{
		process *currProc = *process_list;
		int index = 0;
		while (currProc != NULL)
		{
			printProcess(currProc, index);
			index++;
			currProc = currProc->next;
		}
		printf("\n");
		*process_list = deleteTerminated(*process_list);
		//*process_list=deleteTerminated(*process_list);
	}
	else
		printf("No Processes\n");
}

// last is first in his
void printWholeHist()
{
	if (!listfull)
	{
		int j = 1;
		for (int i = addedCom - 1; i >= 0; i--)
		{
			printf("%d) %s", j, commands[i]);
			j++;
		}
	}
	else
	{ // go backwards from addedcom
		int j = 1;
		for (int i = addedCom - 1; i >= 0; i--)
		{
			printf("%d) %s", j, commands[i]);
			j++;
		}
		for (int i = HISTLEN - 1; i >= addedCom; i--)
		{
			printf("%d) %s", j, commands[i]);
			j++;
		}
	}
}
void printHist(int index)
{
	if (!listfull)
	{
		if (index >= addedCom)
		{
			printf("Invalid index\n");
		}
		else
		{
			printf("%s", commands[addedCom - 1 - index]);
		}
	}
	else
	{
		// now listfull 4 - 1 - 0 = 3 ... 0
		int c = addedCom - 1 - index;
		if (c < 0)
		{
			c = HISTLEN + c;
		}
		printf("%s", commands[c]);
	}
}
void addCommand(char *comm)
{
	char *toAdd = strdup(comm);
	if (!listfull)
	{
		commands[addedCom] = toAdd;
		addedCom++;
	}
	else
	{
		addedCom = addedCom % HISTLEN;
		commands[addedCom] = toAdd;
		addedCom++;
	}
	if (addedCom == HISTLEN)
		listfull = 1;
}

void execute(cmdLine *pCmdLine);
int main(int argc, char **argv)
{
	Processes = NULL;
	debug = 0;
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-d") == 0)
		{
			printf("Debug Mode Active\n");
			debug = 1;
		}
	}
	for (int i = 0; i < HISTLEN; i++)
		commands[i] = 0;
	printf("Starting the program\n");
	while (1)
	{
		fflush(stdin);
		char *buf = malloc(2048);
		char currDir[PATH_MAX];
		if (getcwd(currDir, PATH_MAX) == NULL)
		{
			return 1;
		}
		printf("%s: ", currDir);
		fgets(buf, 2048, stdin);
		if (strcmp(buf, "") != 0 && strcmp(buf, "\n") != 0 && buf != NULL)
		{
			cmdLine *command = parseCmdLines(buf);

			if (buf[0] != '!')
				addCommand(buf);
			free(buf);
			execute(command);
		}
		else
		{
			free(buf);
		}
	}
	return 0;
}
void pipeCom(cmdLine *cmd)
{
	int pipefd[2];
	int pid1, pid2;
	if (pipe(pipefd) == -1)
	{
		perror("Pipe error: ");
		exit(1);
	}
	pid1 = fork();

	if (!pid1)
	{ // child1 actions
		fclose(stdout);
		dup2(pipefd[1], 1);
		close(pipefd[1]);
		execvp(cmd->arguments[0], cmd->arguments);
		perror("ERROR:"); // if exevp fails
		_exit(1);
	}
	else
	{
		close(pipefd[1]);
		pid2 = fork();

		if (!pid2)
		{ // child2 actions
			fclose(stdin);
			dup2(pipefd[0], 0);
			close(pipefd[0]);
			execvp(cmd->next->arguments[0], cmd->next->arguments);
			perror("ERROR:"); // if exevp fails
			_exit(1);
		}
		else
		{
			close(pipefd[0]);
			waitpid(pid1, NULL, 0);
			waitpid(pid2, NULL, 0);
		}
	}
}
void execute(cmdLine *pCmdLine)
{
	int Pid;
	int special = 0;
	if (strcmp("quit", pCmdLine->arguments[0]) == 0)
	{
		freeList(&Processes);
		// free(pCmdLine->arguments[0]);
		freeCmdLines(pCmdLine);
		for (int i = 0; i < HISTLEN; i++)
		{
			if (commands[i] != NULL)
				free(commands[i]);
		}
		exit(0);
	}
	else if (strncmp("cd", pCmdLine->arguments[0], 2) == 0)
	{
		if (chdir(pCmdLine->arguments[1]) < 0)
			perror("CD Error");
		special = 1;
		freeCmdLines(pCmdLine);
	}
	else if (strcmp("procs", pCmdLine->arguments[0]) == 0)
	{
		printProcessList(&Processes);
		freeCmdLines(pCmdLine);
		special = 1;
	}
	else if (strncmp("kill", pCmdLine->arguments[0], 4) == 0)
	{
		special = 1;
		int pid = atoi(pCmdLine->arguments[1]);
		if (kill(pid, SIGINT) == -1)
			perror("Kill Error");
		else
			printf("Sending SIGINT to %d\n", pid);
	}
	else if (strncmp("suspend", pCmdLine->arguments[0], 7) == 0)
	{
		special = 1;
		int pid = atoi(pCmdLine->arguments[1]);
		if (kill(pid, SIGTSTP) == -1)
			perror("Suspend Error");
		else
			printf("Sending SIGSTP to %d\n", pid);
	}
	else if (strncmp("wake", pCmdLine->arguments[0], 4) == 0)
	{
		special = 1;
		int pid = atoi(pCmdLine->arguments[1]);
		if (kill(pid, SIGCONT) == -1)
			perror("Wake Error");
		else
			printf("Sending SIGCONT to %d\n", pid);
	}
	else if (strcmp("history", pCmdLine->arguments[0]) == 0)
	{
		printWholeHist();
		special = 1;
		freeCmdLines(pCmdLine);
	}
	else if (strcmp("!!", pCmdLine->arguments[0]) == 0)
	{
		printHist(0);
		special = 1;
		freeCmdLines(pCmdLine);
	}
	else if (strncmp("!", pCmdLine->arguments[0], 1) == 0)
	{
		int i = atoi(pCmdLine->arguments[0] + 1);
		if (i < 1 || i > HISTLEN)
			printf("Invalid index\n");
		else
		{
			printHist(i - 1);
		}
		special = 1;
		freeCmdLines(pCmdLine);
	}
	else if (!(Pid = fork()))
	{
		if (pCmdLine->inputRedirect)
		{
			close(0);
			if (!open(pCmdLine->inputRedirect, O_RDONLY, 0777))
			{
				perror("Invalid input: ");
				_exit(1);
			}
		}
		if (pCmdLine->outputRedirect)
		{
			close(1);
			if (!open(pCmdLine->outputRedirect, O_WRONLY | O_CREAT, 0777))
			{
				perror("Invalid output: ");
				_exit(1);
			}
		}
		if (pCmdLine->next == NULL)
		{
			execvp(pCmdLine->arguments[0], pCmdLine->arguments);
			perror("Invalid Command");
			_exit(1);
		}
		else
		{
			pipeCom(pCmdLine);
		}
	}

	if (special == 0)
	{
		addProcess(&Processes, pCmdLine, Pid);
		if (pCmdLine->blocking == 1)
		{ // if & in command
			if (debug == 1)
				printf("blocking...\n");
			waitpid(Pid, NULL, 0);
		}
		if (debug == 1)
		{
			fprintf(stderr, "Pid: %d, Command: %s\n", Pid, pCmdLine->arguments[0]);
		}
	}
}
