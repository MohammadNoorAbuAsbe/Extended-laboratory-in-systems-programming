#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <elf.h>
#include <sys/mman.h>
#include <unistd.h>

int debug;
int Currentfd1, Currentfd2;  // Separate file descriptors for two ELF files
void *mapPointer1, *mapPointer2;  // Separate map pointers for two ELF files
char filename1[2048];
char filename2[2048];

void ToggleDebugMode();
void ExamineELFFile();
void PrintSectionNames();
void PrintSymbols();
void CheckFilesForMerge();
void MergeELFFiles();
void Quit();
void map(void (*f)())
{
  f();
}
struct fun_desc
{
  char *name;
  void (*fun)();
};

int main(int argc, char **argv)
{
  debug = 0;
  Currentfd1 = -1;
  Currentfd2 = -1;
  mapPointer1 = NULL;
  mapPointer2 = NULL;

  struct fun_desc menu[] = {
      {"Toggle Debug Mode", ToggleDebugMode},
      {"Examine ELF File", ExamineELFFile},
      {"Print Section Names", PrintSectionNames},
      {"Print Symbols", PrintSymbols},
      {"Check Files for Merge", CheckFilesForMerge},
      {"Merge ELF Files", MergeELFFiles},
      {"Quit", Quit},
  };

  int size = sizeof(menu) / sizeof(menu[0]);

  while (1)
  {
    for (int i = 0; i < size; i++)
    {
      printf("%d)  %s\n", i, menu[i].name);
    }
    
    printf("Option: ");
    int inNum;
    char in;
    fflush(stdin);
    in = fgetc(stdin);
    
    while (in < '0' || in > '9')
      in = fgetc(stdin);
    
    inNum = in - '0';
    char tmp[20];
    fgets(tmp, 20, stdin);
    
    if (inNum >= 0 && inNum < size)
    {
      printf("Within bounds\n");
      fflush(stdin);
      map(menu[inNum].fun);
      printf("DONE.\n");
    }
    else
    {
      printf("Not Within bounds\n");
      return 0;
    }
  }
  
  return 0;
}

void ToggleDebugMode()
{
  if (debug == 0)
  {
    printf("Debug mode on\n");
    debug = 1;
  }
  else if (debug == 1)
  {
    printf("Debug mode off\n");
    debug = 0;
  }
  else
  {
    printf("Invalid debug value: debug=%d\nTerminating...", debug);
    exit(1);
  }
}

void ExamineELFFile()
{
  char filename[2048];
  int newFD;
  FILE *filePointer;
  
  printf("Input elf file name: ");
  fscanf(stdin, "%s", filename);
  
  if ((filePointer = fopen(filename, "r")) == NULL)
  {
    fprintf(stderr, "Error: Can not open file\n");
    return;
  }
  
  newFD = fileno(filePointer);
  fseek(filePointer, 0, SEEK_END);
  long size = ftell(filePointer);
  fseek(filePointer, 0, SEEK_SET);
  
  void *newMapPointer = mmap(0, size, PROT_READ, MAP_PRIVATE, newFD, 0);
  
  if (newMapPointer == MAP_FAILED)
  {
    fprintf(stderr, "Error: mapping error\n");
    fclose(filePointer);
    return;
  }
  
  if (Currentfd1 == -1)
  {
    Currentfd1 = newFD;
    mapPointer1 = newMapPointer;
    strcpy(filename1, filename);  // Save the filename to global variable
  }
  else if (Currentfd2 == -1)
  {
    Currentfd2 = newFD;
    mapPointer2 = newMapPointer;
    strcpy(filename2, filename);  // Save the filename to global variable
  }
  else
  {
    fprintf(stderr, "Error: Cannot examine more than two ELF files simultaneously\n");
    fclose(filePointer);
    munmap(newMapPointer, size);
    return;
  }
  
  Elf32_Ehdr *header = (Elf32_Ehdr *)newMapPointer; // structure in elf file with all header values
  
  if (header->e_ident[0] != 0x7F || header->e_ident[1] != 'E' ||
      header->e_ident[2] != 'L' || header->e_ident[3] != 'F')
  {
    fprintf(stderr, "ERROR: file not elf file");
    close(Currentfd1);
    close(Currentfd2);
    Currentfd1 = -1;
    Currentfd2 = -1;
    mapPointer1 = NULL;
    mapPointer2 = NULL;
  }
  else
  {
    char *type;
    
    if (header->e_ident[5] == 1)
      type = "2's complement, little endian";
    else if (header->e_ident[5] == 2)
      type = "2's complement, big endian";
    else
      type = "Invalid data type";
    
    printf("Magic Numbers: %x %x %x\n",
           header->e_ident[0], header->e_ident[1], header->e_ident[2]);
    printf("Data encoding scheme: %s\n", type);
    printf("Enty point: %x\n", header->e_entry);
    printf("Section header table offset: %d\n", header->e_shoff);
    printf("Number of section header entries: %d\n", header->e_shnum);
    printf("Size of each section header entry: %d\n", header->e_shentsize);
    printf("Program header table offset: %d\n", header->e_phoff);
    printf("Number of program header entries: %d\n", header->e_phnum);
    printf("Size of each program header entry: %d\n", header->e_phentsize);
  }
}

void PrintSectionNames()
{
  if (Currentfd1 == -1 && Currentfd2 == -1)
  {
    fprintf(stderr, "No ELF files currently mapped.\n");
    return;
  }
  
  if (Currentfd1 != -1)
  {
    Elf32_Ehdr *elfHdr1 = (Elf32_Ehdr *)mapPointer1;
    Elf32_Shdr *strTable1 = mapPointer1 + (elfHdr1->e_shoff) + ((elfHdr1->e_shstrndx) * (elfHdr1->e_shentsize));
    printf("\nFile: %s\n", filename1);
    printf("Index \t Section name \t Section address \t Section offset \t Section size \t Section type\n");
    for (size_t i = 0; i < elfHdr1->e_shnum; i++)
    {
      Elf32_Shdr *section1 = mapPointer1 + (elfHdr1->e_shoff) + (i * elfHdr1->e_shentsize);
      char *name1 = mapPointer1 + strTable1->sh_offset + section1->sh_name;
      printf("[%2d] \t %-14s \t %#09x \t %06d \t\t %06d \t %-15d\n",
             i, name1, section1->sh_addr, section1->sh_offset, section1->sh_size, section1->sh_type);

      if (debug)
      {
        printf("Debug Mode: sh_name offset = %d\n", section1->sh_name);
        printf("Debug Mode: shstrndx = %d\n", elfHdr1->e_shstrndx);
        printf("--------------------------------------\n");
      }
    }
    printf("\n");
  }
  
  if (Currentfd2 != -1)
  {
    Elf32_Ehdr *elfHdr2 = (Elf32_Ehdr *)mapPointer2;
    Elf32_Shdr *strTable2 = mapPointer2 + (elfHdr2->e_shoff) + ((elfHdr2->e_shstrndx) * (elfHdr2->e_shentsize));
    printf("File: %s\n", filename2);
    printf("Index \t Section name \t Section address \t Section offset \t Section size \t Section type\n");
    for (size_t i = 0; i < elfHdr2->e_shnum; i++)
    {
      Elf32_Shdr *section2 = mapPointer2 + (elfHdr2->e_shoff) + (i * elfHdr2->e_shentsize);
      char *name2 = mapPointer2 + strTable2->sh_offset + section2->sh_name;
      printf("[%2d] \t %-14s \t %#09x \t %06d \t\t %06d \t %-15d\n",
             i, name2, section2->sh_addr, section2->sh_offset, section2->sh_size, section2->sh_type);

      if (debug)
      {
        printf("Debug Mode: sh_name offset = %d\n", section2->sh_name);
        printf("Debug Mode: shstrndx = %d\n", elfHdr2->e_shstrndx);
        printf("--------------------------------------\n");
      }
    }
    printf("\n");
  }
}


void PrintSymbols()
{
  fprintf(stdout, "\nNot implemented yet\n");
}

void CheckFilesForMerge()
{
  fprintf(stdout, "\nNot implemented yet\n");
}

void MergeELFFiles()
{
  fprintf(stdout, "\nNot implemented yet\n");
}

void Quit()
{
  printf("Quitting...\n");
  exit(0);
}
