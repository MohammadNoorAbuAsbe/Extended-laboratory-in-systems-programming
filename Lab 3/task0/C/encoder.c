#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void Debugger(int Debug, char charToRead, char ecnodedChar)
{
    if (Debug == 1)
    {
        if (charToRead == 10)                           //avoid LineFeed 
        {
            fprintf(stderr, "\n");
        }
        else                                            //form Hex Table
        {
            fprintf(stderr, "%02X", charToRead);
            fprintf(stderr, " ");
            fprintf(stderr, "%02X", ecnodedChar);
            fprintf(stderr, "\n");
        }
    }
}

void DefaultNoIncoding(int Debug, FILE *inputType, FILE *outputType)
{
    do
    {
        char charToRead = fgetc(inputType);             //read one char at a time        
        if (feof(inputType))                            //EOF checking
        {
            fclose(inputType);
            fclose(outputType);
            break;
        }
        Debugger(Debug, charToRead, charToRead);                    //works if Debug is enabled
        fputc(charToRead, outputType);       
    }  while(1);
}

void KeyEncription(int Debug, int sign, int key[], int size, FILE *inputType ,FILE *outputType)
{
    int i = 0;
    do
    {
        char charToRead = fgetc(inputType);                 //read one char at a time
        char tempIncode = charToRead;
        if (feof(inputType))                                //EOF checking
        {
            fclose(inputType);
            fclose(outputType);
            break;
        }
        if (charToRead == '\n')
        {
            Debugger(Debug, charToRead, charToRead);
            fputc(charToRead, outputType);
        }
        else
        {
            if (tempIncode >= 65 && tempIncode <= 90)
            {
                tempIncode += key[i] * sign;
                if (tempIncode < 65 || tempIncode > 90)
                {
                    tempIncode = tempIncode + (-90 + 64) * sign;
                }
                
            }
            else if (tempIncode >= 97 && tempIncode <= 122)
            {
                tempIncode += key[i] * sign;
                if (tempIncode < 97 || tempIncode > 122)
                {
                    tempIncode = tempIncode + (-122 + 96) * sign;
                }
            }
            else if (tempIncode >= 48 && tempIncode <= 57)
            {
                tempIncode += key[i] * sign;
                if (tempIncode < 48 || tempIncode > 57)
                {
                    tempIncode = tempIncode + (-57 + 47) * sign;
                }
            }
            Debugger(Debug, charToRead, tempIncode);
            fputc(tempIncode, outputType);
        }
        i = i + 1;
        if (i >= size)                              //cycle
        {
            i = 0;
        }
    }  while(1);
}

FILE* InputTypeOpener(int fileInput, char inputFileName[])
{
    FILE *inputType;
    if (fileInput == 0)
    {
        inputType = stdin;
    }
    else
    { 
        inputType = fopen(inputFileName, "r");
        if(inputType == NULL)
        {
            fprintf(stderr, "ERORR: Can't open the given file at %s", inputFileName); 
            exit(1);             
        }
    }
    return inputType;
}

FILE* OutputTypeOpener(int fileOutput, char outputFileName[])
{
    FILE *outputType;
    if (fileOutput == 0)
    {
        outputType = stdout;
    }
    else
    { 
        outputType = fopen(outputFileName, "w");
        if(outputType == NULL)
        {
            fprintf(stderr, "ERORR: Can't open the given file at %s", outputFileName); 
            exit(1);             
        }
    }
    return outputType;
}

int main(int argc, char *argv[])
{
    /////////////////////////// OPTIONS ///////////////////////////////
    int Debug = 0;                                      //Debug is OFF
    int sign = 0;                                       //encryption Side
    int fileInput = 0;                                  //standard input
    int fileOutput = 0;                                 //standard OUTPUT
    ///////////////////////////////////////////////////////////////////

    char inputFileName[256]= "./";                      // i did this because of stack smashing error IF I DONT SET IT TO(256)
    char outputFileName[256]= "./";                     // i did this because of stack smashing error IF I DONT SET IT TO(256)
    int opt = 0;                                        //where to get the key from if needed
    FILE *inputType = stdin;
    FILE *outputType = stdout;

    /////////////////////////// OPTIONS CHECKER ////////////////////////
    for (int option = 1; option < argc; option++)
    {
        
        if (strncmp(argv[option], "+D", 2) == 0)
        {
            Debug = 1;                                   //Debug is ON 
            for (int option = 1; option < argc; option++)
            {
                fprintf(stderr, "%s \n", argv[option]); 
            } 
        }
        else if (strncmp(argv[option], "-D", 2) == 0)
        {
            Debug = 0;                                   //Debug is OFF 
        }
        else if (strncmp(argv[option], "+e", 2) == 0)
        {
            sign = 1;                                    //positive Encription
            opt = option;
        }
        else if (strncmp(argv[option], "-e", 2) == 0)
        {
            sign = -1;                                   //negative Encription
            opt = option;
        }
        else if (strncmp(argv[option], "-i", 2) == 0)
        {
            fileInput = 1;                               //file Input ON
            strcat(inputFileName, argv[option] + 2);
        }
        else if (strncmp(argv[option], "-o", 2) == 0)
        {
            fileOutput = 1;                               //file Output ON
            strcat(outputFileName, argv[option] + 2);
        }
        
    }
    ///////////////////////////////////////////////////////////////////////
    

    inputType = InputTypeOpener(fileInput, inputFileName);
    outputType = OutputTypeOpener(fileOutput, outputFileName);
    

    if (sign == 0)
    {
        DefaultNoIncoding(Debug, inputType, outputType);    //Default 
    }
    else
    {
        argv[opt] += 2;                                //move the pointer
        int size = 0;
        int i = 0;
        while (argv[opt][i] != 0)
        {
            size++;
            i++;
        }
        int key[size];
        for (int i = 0; i < size; i++)
        {   
            key[i] = argv[opt][i] - 48;                   //convert char to int
        }   
        KeyEncription(Debug, sign, key, size, inputType, outputType);
    }
}