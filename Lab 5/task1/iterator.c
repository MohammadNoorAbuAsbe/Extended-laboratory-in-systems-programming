#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>
#include <sys/mman.h>
#include <unistd.h>

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg);
void PrintHeader(Elf32_Phdr *header, int arg);
void PrintHeaderFull(Elf32_Phdr *header, int arg);
void *ExamineELFFile(char *fileName);

int main(int argc, char **argv)
{
  // Examine the ELF file and get the starting address of the mapped file
  void *map_start = ExamineELFFile(argv[1]);

  // If the examination was successful, iterate over each program header and print it
  if (map_start != NULL)
  {
    foreach_phdr(map_start, *PrintHeaderFull, 0);
  }
}

// Function to get the string representation of a program header type
char *getType(int type)
{
  switch (type)
  {
    // Different program header types
    case SHT_NULL:
      return "NULL";
    case PT_LOAD:
      return "LOAD";
    case PT_DYNAMIC:
      return "DYNAMIC";
    case PT_INTERP:
      return "INTERP";
    case PT_NOTE:
      return "NOTE";
    case PT_SHLIB:
      return "SHLIB";
    case PT_PHDR:
      return "PHDR";
    case PT_TLS:
      return "TLS";
    case PT_NUM:
      return "NUM";
    case PT_LOOS:
      return "LOOS";
    case PT_GNU_EH_FRAME:
      return "GNU_EH_FRAME";
    case PT_GNU_STACK:
      return "GNU_STACK";
    case PT_LOSUNW:
      return "LOSUNW/SUNWBSS/HIOS";
    case PT_SUNWSTACK:
      return "SUNWSTACK";
    case PT_HISUNW:
      return "HISUNW";
    case PT_LOPROC:
      return "LOPROC";
    case PT_GNU_RELRO:
      return "GNU_RELRO";
    case PT_HIPROC:
      return "HIPROC";
  }
  return "Invalid type";
}

// Function to get the string representation of program header flags
char *getFlag(int flag)
{
  switch (flag)
  {
    // Different flag combinations (R = Read, W = Write, X = Execute)
    case 0:
      return "NONE";
    case 1:
      return "X";
    case 2:
      return "W";
    case 3:
      return "X W";
    case 4:
      return "R";
    case 5:
      return "R X";
    case 6:
      return "R W";
    case 7:
      return "R W X";
  }
  return "Invalid flag";
}

// Function to print basic information about a program header
void PrintHeader(Elf32_Phdr *header, int arg)
{
  printf("Program header number %d at address %x\n", arg, header->p_paddr);
}

// Function to print detailed information about a program header
void PrintHeaderFull(Elf32_Phdr *header, int arg)
{
  printf("%-6s  0x%06x  0x%06x  0x%06x  0x%06x  0x%06x  %-5s  0x%06x\n",
         getType(header->p_type), header->p_offset, header->p_vaddr, header->p_paddr, header->p_filesz,
         header->p_memsz, getFlag(header->p_flags), header->p_align);
}

// Function to iterate over each program header and invoke a given function
int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg)
{
  printf("Type    Offset    VirtAddr   PhysAddr   FileSiz   MemSiz    Flg    Align\n");

  // Get the ELF header from the mapped file
  Elf32_Ehdr *header = (Elf32_Ehdr *)map_start;

  // Iterate over each program header and invoke the provided function
  for (int i = 0; i < header->e_phnum; i++)
  {
    Elf32_Phdr *progHeader = map_start + (header->e_phoff) + (i * header->e_phentsize);
    func(progHeader, arg);
  }

  return 0;
}

// Function to examine the ELF file and return the starting address of the mapped file
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

  // Get the file descriptor of the opened file
  newFD = filePointer->_fileno;

  // Determine the size of the file
  fseek(filePointer, 0, SEEK_END);
  long size = ftell(filePointer);
  fseek(filePointer, 0, SEEK_SET);

  // Map the file into memory
  mapPointer = mmap(0, size, PROT_READ, MAP_PRIVATE, newFD, 0);

  // Handle mapping errors
  if (mapPointer == MAP_FAILED)
  {
    mapPointer = NULL;
    fprintf(stderr, "Error: mapping error\n");
    return NULL;
  }

  return mapPointer;
}