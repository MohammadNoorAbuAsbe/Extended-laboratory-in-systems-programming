#include "util.h"

#define SYS_WRITE 4
#define STDOUT 1
#define SYS_OPEN 5
#define O_RDWR 2
#define SYS_SEEK 19
#define SEEK_SET 0
#define SHIRA_OFFSET 0x291

extern int system_call();

int main(int argc, char *argv[], char *envp[])
{
    char *outputBuffer;
    int i = 0;
    while(i < argc)
    {
        outputBuffer = argv[i];
        unsigned int countOfBytes = strlen(outputBuffer); // 1 char = 1 byte                                  
        system_call(SYS_WRITE,STDOUT,outputBuffer,countOfBytes);
        outputBuffer = "\n";
        countOfBytes = strlen(outputBuffer);                                    
        system_call(SYS_WRITE,STDOUT,outputBuffer,countOfBytes);
        i = i+1;
    }
    return 0;
}