#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum { False, True } boolean;


typedef struct virus {
unsigned short SigSize;
char virusName[16];
unsigned char* sig;
} virus;

typedef struct link link;
 
struct link {
    link *nextVirus;
    virus *vir;
};





void printHex(char buffer[],int length)
{
    for (int i = 0; i < length; i++)
    {
        printf("%02X ", buffer[i]&0xff);
    }
}




virus* readVirus(FILE* input)
{
    virus* newVirus = (virus*)malloc(sizeof(virus));
    fread(&newVirus->SigSize, 2, 1, input);
    fread(newVirus->virusName, 16, 1, input); 
    newVirus->sig = (unsigned char*)malloc(newVirus->SigSize);
    fread(newVirus->sig, newVirus->SigSize, 1, input);    
    return newVirus;
}

void printVirus(virus* virus, FILE* output)
{
    
    fprintf(output, "Virus name: %s", virus->virusName);
	fprintf(output, "\nVirus size: %d", virus->SigSize);
	fprintf(output, "\nsignature:\n");
    printHex(virus->sig, virus->SigSize);
    fprintf(output, "\n");
}

void list_print(link *virus_list, FILE* output){

    if(virus_list->nextVirus == NULL){
        printVirus(virus_list->vir, output);
        fprintf(output, "\n");
    }
    else{
        while(virus_list->nextVirus != NULL){
            printVirus(virus_list->vir, output);
            fprintf(output, "\n");
            virus_list = virus_list->nextVirus;
        }
    }
    
}

link* list_append(link* virus_list, virus* data)
{
    link* newLink = (link*)malloc(sizeof(link));
    newLink->vir = data;
    newLink->nextVirus = NULL;
    if(virus_list == NULL)
    {
        return newLink;
    }
    else
    {
        link* nextlink = virus_list;
        while(nextlink->nextVirus != NULL)
        {
            nextlink = nextlink->nextVirus;
        }
        nextlink->nextVirus = newLink;
        return virus_list; 
    }
}

void list_free(link *virus_list){
    if(virus_list->nextVirus != NULL)
    {
        list_free(virus_list->nextVirus);
    }
    free(virus_list->vir->sig);
    free(virus_list->vir);
    free(virus_list);
}

void kill_virus(FILE *fileName, int signitureOffset, int signitureSize)
{ 
    fseek(fileName, 0,SEEK_SET);
  fseek(fileName, signitureOffset,SEEK_SET);
  char* newChar = malloc(signitureSize); 
  for(int i = 0; i < signitureSize; i++)
  {
    newChar[i] = 0x90;
    printf("new char\n");
  }
  fwrite(newChar, 1, signitureSize, fileName);
  printf("hey wow");
}


void FixFile(FILE* needsFixing)
{
    int VirSize;
    char size[4];
    int location;
    char offSet[4];
    printf("(WAITING INPUT FROM USER) Please enter the offset: ");
    fgets(offSet,16,stdin);
    location=atoi((char*)offSet);
    printf("(WAITING INPUT FROM USER) Please enter the size of the virus: ");
    fgets(size,16,stdin);
    VirSize=atoi((char*)size);
    fflush(stdin);
    kill_virus(needsFixing,location,VirSize);
}

link* LoadingSignatures(char* createdBuffer, FILE* input, link* listOfViruses)
{
    char inputFileName[256];
    printf("\nLoading Signatures\n");
    printf("(WAITING INPUT FROM USER) input file name is: ");
    fgets(createdBuffer, 256, stdin);
    sscanf(createdBuffer, "%s", inputFileName);
    input = fopen(inputFileName, "rb");
    if(input != NULL){
        unsigned char magic[4];
        if (fread(magic, 1, 4, input) != 4) {
        if (feof(input)) 
            fputs("File is less than 4 bytes\n", stderr);
        else
            fputs("Error reading the file\n", stderr);
        return 1;
        }   
        puts("The magic number of your file is:");
        printf("%02hhx%02hhx%02hhx%02hhx\n", magic[0], magic[1], magic[2], magic[3]);
        fseek(input,4,SEEK_SET);
        while(!feof(input)){
            listOfViruses = list_append(listOfViruses, readVirus(input));
        }
        fclose(input);
        return listOfViruses;
    }
    else
    {
        printf("\nCant find the requested file\n");
        fclose(input);
        exit(1);
    }  
}

void PrintSignatures(boolean signaturesLoaded, link* listOfViruses)
{
    printf("\nPrint signatures\n");
    if(signaturesLoaded == True)
    {
        list_print(listOfViruses, stdout);
    }  
    else{
        printf("\nERORR: Signatures are not Loaded\n");
    }
}

void detect_virus(char *buffer, unsigned int size, link *virus_list)
{
	while (virus_list != NULL)
	{
		for (int i = 0; i < (int)(size - virus_list->vir->SigSize); i++)
		{
			if (memcmp(virus_list->vir->sig, buffer + i, virus_list->vir->SigSize) == 0)
			{
                if(virus_list->vir->SigSize != 0)
                {
                    printf("virus has been detected:\n");
				    printf("the starting byte is: %d\n", i);
				    printf("the virus name is: %s\n", virus_list->vir->virusName);
				    printf("size of the virus signature is: %d\n\n\n", virus_list->vir->SigSize);
                }
				break;
			}
		}
		virus_list = virus_list->nextVirus;
	}
}

void quit(FILE *stream,struct link *list){
  list_free(list);
  printf("/////////////////// QUITTING ///////////////\n\n");
  fclose(stream);
  exit(0);
}

int main(int argc, char **argv)
{
    link* listOfViruses = NULL;
    char createdBuffer[256];
    int choosen = -1;
    FILE *input;
    boolean signaturesLoaded = False; 
    FILE *file=fopen(argv[1],"r+");
    while(1){
        printf("(WAITING INPUT FROM USER) Choose from 1-5: ");
        fgets(createdBuffer, sizeof(createdBuffer), stdin);
        sscanf(createdBuffer, "%d", &choosen);
        if (choosen == 5)
        {
            if(signaturesLoaded == True){
                quit(file,listOfViruses);
            }
            break;
        }
        if(choosen == 1){
            listOfViruses = LoadingSignatures(createdBuffer, input, listOfViruses);
            if (listOfViruses != NULL)
            {
                signaturesLoaded = True;
            }
        }
        else if (choosen == 2)
        {
            PrintSignatures(signaturesLoaded, listOfViruses);
        }
        else if (choosen == 3)
        {
            int size = 1 << 10;
	        fseek(file, 0, SEEK_END);
	        int fsize = ftell(file);
	        fseek(file, 0, SEEK_SET);

	        fread(createdBuffer, fsize, 1, file);
	        fclose(file);
	        createdBuffer[fsize] = 0;
			if (fsize < size)
			{
				size = fsize;
			}
			detect_virus(createdBuffer, size, listOfViruses);
        }
        else if (choosen == 4)
        {
            FixFile(file);
        }
        choosen = -1;
    } 

}