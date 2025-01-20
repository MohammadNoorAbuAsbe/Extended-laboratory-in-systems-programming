#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_CHUNK_SIZE 10000
typedef enum { False, True } boolean;

FILE *file;
char *fileName;
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

struct fun_desc {
    char *name;
    link* (*fun)(link*);
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


void neutralize_virus(char *fileName, int signatureOffset) {
    FILE *fp = fopen(fileName, "r+b");
    fseek(fp, signatureOffset, SEEK_SET);
    fputc(0xC3, fp); // Replace first byte of virus with the RET instruction (0xC3)
    fclose(fp);
}


link* FixFile(link *virus_list)
{

    int size = 1 << 10;
    char* createdBuffer = malloc(size);
    if (createdBuffer == NULL) {
        perror("Failed to allocate memory for buffer");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    int fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    fread(createdBuffer, fsize, 1, file);
    fclose(file);
    createdBuffer[fsize] = 0;

    if (fsize < size) {
        size = fsize;
    }


    while (virus_list != NULL)
	{
		for (int i = 0; i < (int)(size - virus_list->vir->SigSize); i++)
		{
			if (memcmp(virus_list->vir->sig, createdBuffer + i, virus_list->vir->SigSize) == 0)
			{
                if(virus_list->vir->SigSize != 0)
                {
                    printf("virus has been detected:\n");
				    printf("the starting byte is: %d\n", i);
				    printf("the virus name is: %s\n", virus_list->vir->virusName);
				    printf("size of the virus signature is: %d\n\n\n", virus_list->vir->SigSize);
                    neutralize_virus(fileName,i);
                }
				//break;
			}
		}
		virus_list = virus_list->nextVirus;
	}
    free(createdBuffer);
    return virus_list;
}

link* LoadingSignatures(link* listOfViruses)
{
    char inputFileName[256];
    printf("\nLoading Signatures\n");
    printf("(WAITING INPUT FROM USER) input file name is: ");
    char createdBuffer[256];
    fgets(createdBuffer, 256, stdin);
    sscanf(createdBuffer, "%s", inputFileName);
    FILE* input = fopen(inputFileName, "rb");
    if(input != NULL){
        // check the magic number
        char magic[5];
        fread(magic, 4, 1, input);
        magic[4] = '\0';
        // if (strcmp(magic, "VIRL") != 0) {
        //     fprintf(stderr, "Error: invalid signature file format\n");
        //     fclose(input);
        //     exit(0);
        // }
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
        exit(1);
    }  
}

link* PrintSignatures(link* listOfViruses)
{
    if (listOfViruses!=NULL)
    {
        printf("\nPrint signatures\n");
        list_print(listOfViruses, stdout);
    }
    return listOfViruses;
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
				//break;
			}
		}
		virus_list = virus_list->nextVirus;
	}
}

link* detect(link* virus_list){
    int size = 1 << 10;
    char* createdBuffer = malloc(size);
    if (createdBuffer == NULL) {
        perror("Failed to allocate memory for buffer");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    int fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    fread(createdBuffer, fsize, 1, file);
    fclose(file);
    createdBuffer[fsize] = 0;

    if (fsize < size) {
        size = fsize;
    }

    detect_virus(createdBuffer, size, virus_list);

    free(createdBuffer);
    return virus_list;
}




link* quit(struct link *list){
  list_free(list);
  printf("/////////////////// QUITTING ///////////////\n\n");
  exit(0);
  return list;
}

int main(int argc, char **argv)
{
    link* listOfViruses = NULL;
    char createdBuffer[256];
    int choosen = -1;
    boolean signaturesLoaded = False; 
    file=fopen(argv[1],"r+");
    fileName = argv[1];
    struct fun_desc menu[] = {
        { "Load signatures", LoadingSignatures },
        { "Print signatures", PrintSignatures },
        { "Detect viruses", detect },
        { "Fix file", FixFile },
        { "Quit", quit },
        { NULL, NULL }
    };
    while(1){
        printf("(WAITING INPUT FROM USER) Choose from 1-5: \n");
        for (int i = 0; menu[i].name != NULL; i++)
        {
            printf("%d-%s\n", i + 1, menu[i].name);
        }
        fgets(createdBuffer, sizeof(createdBuffer), stdin);
        sscanf(createdBuffer, "%d", &choosen);
        if (choosen < 1 || choosen > 5)
        {
            printf("Invalid choice. Try again.\n");
            continue;
        }
        if (choosen == 5)
        {
            if(signaturesLoaded == True){
                quit(listOfViruses);
            }
            break;
        }
        listOfViruses = menu[choosen - 1].fun(listOfViruses);
        if (choosen == 1 && listOfViruses != NULL)
        {
            signaturesLoaded = True;
        }
        choosen = -1;
    } 
    return 0;
}