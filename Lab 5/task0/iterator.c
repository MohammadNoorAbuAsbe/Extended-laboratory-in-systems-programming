#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>
#include <sys/mman.h>
#include <unistd.h>


int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg);
void PrintHeader(Elf32_Phdr *header, int arg);
void *ExamineELFFile(char *fileName);

int main(int argc, char **argv)
{
  void *map_start = ExamineELFFile(argv[1]); // Examine the ELF file and get the mapped memory address
  if (map_start != NULL)
  {
    foreach_phdr(map_start, *PrintHeader, 0); // Iterate over program headers and print their information
  }
}

// Function to print the program header information
void PrintHeader(Elf32_Phdr *header, int arg)
{
  printf("Program header number %d at address %x\n", arg, header->p_paddr);
}

// Function to iterate over program headers
int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg)
{
  Elf32_Ehdr *header = (Elf32_Ehdr *)map_start; // Get the ELF header from the mapped memory
  for (int i = 0; i < header->e_phnum; i++)
  {
    /* Calculate the address the program header by adding the base address of the mapped memory (map_start) 
    to the offset of the program headers (header->e_phoff) and the size of each program header (i * header->e_phentsize).*/
    Elf32_Phdr *progHeader = map_start + (header->e_phoff) + (i * header->e_phentsize);                                                                         
    func(progHeader, i + 1); // Call the provided function with the program header and its number
  }
  return 0;
}

// Function to examine the ELF file and return the mapped memory address
void *ExamineELFFile(char *fileName)
{
  int newFD;
  FILE *filePointer;
  void *mapPointer;
  if ((filePointer = fopen(fileName, "r")) < 0)
  {
    fprintf(stderr, "Error: Can not open file\n");
    return NULL;
  }
  newFD = filePointer->_fileno; // Get the file descriptor of the opened file
  fseek(filePointer, 0, SEEK_END); // Move the file pointer to the end of the file
  long size = ftell(filePointer); // Get the size of the file
  fseek(filePointer, 0, SEEK_SET); // Move the file pointer back to the beginning of the file
  mapPointer = mmap(0, size, PROT_READ, MAP_PRIVATE, newFD, 0); // Map the file into memory
  if (mapPointer == MAP_FAILED) // Check if the mapping failed
  {
    mapPointer = NULL;
    fprintf(stderr, "Error: mapping error\n");
    return NULL;
  }
  return mapPointer; // Return the mapped memory address
}