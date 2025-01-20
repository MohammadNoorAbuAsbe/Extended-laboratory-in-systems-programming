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
#define O_RWX 777

extern int system_call();
typedef struct ent {
    int inode;
    int offset;
    short len;
    char buf[];
} ent;

int main (int argc , char* argv[], char* envp[])
{
    int fileDesc, dirLen, i = 0;
    char buffer[8192];
    ent* file;

    /* opens the current directory in read-only mode with the append flag */
    fileDesc = system_call(SYS_OPEN, ".", O_RDONLY, O_APPEND);
    if (fileDesc < 0)
        return 0x55;

    /* reads the directory entries into the buffer using the getdents system call */
    dirLen = system_call(Sys_getDent, fileDesc, buffer, 8192);
    if (dirLen < 0)
        return 0x55;

    /* iterates over each directory entry in the buffer */
    while (i < dirLen) {
        /* sets the file pointer to the current directory entry */
        file = (ent*)(buffer+i);

        /* checks if the directory entry is not "." or ".." */
        if (strcmp(file->buf, ".") != 0 && strcmp(file->buf, "..") != 0) {
            /* writes the name of the directory entry to the standard output file descriptor followed by a newline character */
            system_call(SYS_WRITE, STDOUT, file->buf, strlen(file->buf), 1);
            system_call(SYS_WRITE, STDOUT, "\n", 1, 1);
        }

        /* increments i by the length of the current directory entry */
        i = i + file->len;
    }

    return 0;
}