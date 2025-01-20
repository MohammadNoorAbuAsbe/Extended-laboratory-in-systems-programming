#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>
#include <sys/mman.h>
#include <unistd.h>

extern int startup(int argc, char **argv, void (*start)());
int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg);
void PrintHeader(Elf32_Phdr *header, int arg);
void PrintHeaderFull(Elf32_Phdr *header, int arg);
void load_phdr(Elf32_Phdr *phdr, int fd);
void *ExamineELFFile(char *fileName);
int currFD;

int main(int argc, char **argv)
{
  void *map_start = ExamineELFFile(argv[1]);

  if (map_start != NULL)
  {
    // Iterate over each program header and load them
    foreach_phdr(map_start, *load_phdr, currFD);
    // Call the startup function from the loaded ELF file
    startup(argc - 1, argv + 1, (void *)((Elf32_Ehdr *)map_start)->e_entry);
  }
  else
  {
    fprintf(stderr, "Error: Failed to examine ELF file\n");
  }

  close(currFD);
}

char *getType(int type)
{
  switch (type)
  {
  case PT_NULL:
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

char *getFlag(int flag)
{
  switch (flag)
  {
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

void PrintHeader(Elf32_Phdr *header, int arg)
{
  printf("Program header number %d at address %x\n", arg, header->p_paddr);
}

void PrintHeaderFull(Elf32_Phdr *header, int arg)
{
  printf("%-6s  0x%06x  0x%06x  0x%06x  0x%06x  0x%06x  %-5s  0x%06x\n",
         getType(header->p_type), header->p_offset, header->p_vaddr, header->p_paddr, header->p_filesz,
         header->p_memsz, getFlag(header->p_flags), header->p_align);
}

void load_phdr(Elf32_Phdr *phdr, int fd)
{
  if ((int)phdr->p_type == PT_LOAD)
  {
    void *mapPointer;
    // Calculate virtual and offset addresses
    Elf32_Addr vaddr = phdr->p_vaddr & 0xfffff000;
    Elf32_Off offset = phdr->p_offset & 0xfffff000;
    Elf32_Addr padding = phdr->p_vaddr & 0xfff;
    int r = 0, w = 0, x = 0;
    // Set flags based on the p_flags value
    if (phdr->p_flags & (1 << 0))
      r = 0x1;
    if (phdr->p_flags & (1 << 1))
      w = 0x2;
    if (phdr->p_flags & (1 << 2))
      x = 0x4;
    // Map the memory region with the specified flags
    mapPointer = mmap((void *)vaddr, phdr->p_memsz + padding, r | w | x, MAP_PRIVATE | MAP_FIXED, fd, offset);
    if (mapPointer == NULL)
      fprintf(stderr, "ERROR: map failed for program header in address %x", phdr->p_paddr);
  }
}

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg)
{
  Elf32_Ehdr *header = (Elf32_Ehdr *)map_start;
  // Iterate over each program header and call the provided function
  for (int i = 0; i < header->e_phnum; i++)
  {
    Elf32_Phdr *progHeader = map_start + (header->e_phoff) + (i * header->e_phentsize);
    func(progHeader, arg);
  }
  return 0;
}

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
  newFD = fileno(filePointer);
  fseek(filePointer, 0, SEEK_END);
  long size = ftell(filePointer);
  fseek(filePointer, 0, SEEK_SET);
  // Map the ELF file into memory
  mapPointer = mmap(0, size, PROT_READ, MAP_PRIVATE, newFD, 0);
  if (mapPointer == MAP_FAILED)
  {
    mapPointer = NULL;
    fprintf(stderr, "Error: mapping error\n");
    return NULL;
  }
  currFD = newFD;
  return mapPointer;
}
