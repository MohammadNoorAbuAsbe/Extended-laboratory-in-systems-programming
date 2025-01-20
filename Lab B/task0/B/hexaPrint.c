#include <stdio.h>
#include <sys/stat.h>


void printHex(char buffer[],int length)
{
    for (int i = 0; i < length; i++)
    {
        printf("%02X ", buffer[i]&0xff);
    }
}


int main(int argc, char **argv)
{
    
    struct stat st;
    stat(argv[1], &st);
    char charToRead[st.st_size];
    FILE* input = fopen(argv[1], "r");
    fread(charToRead,1,st.st_size,input);
    fclose(input);
    printHex(charToRead, st.st_size);
    return 0;
}