#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <elf.h>
#include <sys/mman.h>
#include <unistd.h>

int debug;
int Currentfd1, Currentfd2;      // Separate file descriptors for two ELF files
void *mapPointer1, *mapPointer2; // Separate map pointers for two ELF files
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
    strcpy(filename1, filename); // Save the filename to global variable
  }
  else if (Currentfd2 == -1)
  {
    Currentfd2 = newFD;
    mapPointer2 = newMapPointer;
    strcpy(filename2, filename); // Save the filename to global variable
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

char *getType(int type)
{
  switch (type)
  {
  case SHT_NULL:
    return "NULL";
  case SHT_PROGBITS:
    return "PROGBITS";
  case SHT_SYMTAB:
    return "SYMTAB";
  case SHT_STRTAB:
    return "STRTAB";
  case SHT_RELA:
    return "RELA";
  case SHT_HASH:
    return "HASH";
  case SHT_DYNAMIC:
    return "DYNAMIC";
  case SHT_NOTE:
    return "NOTE";
  case SHT_NOBITS:
    return "NOBITS";
  case SHT_REL:
    return "REL";
  case SHT_SHLIB:
    return "SHLIB";
  case SHT_DYNSYM:
    return "DYNSYM";
  }
  return "Invalid type";
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
  if (Currentfd1 == -1 && Currentfd2 == -1)
  {
    fprintf(stderr, "No ELF files currently mapped.\n");
    return;
  }

  if (Currentfd1 != -1)
  {
    Elf32_Ehdr *elfHdr1 = (Elf32_Ehdr *)mapPointer1;
    Elf32_Shdr *secStrTable1 = mapPointer1 + (elfHdr1->e_shoff) + ((elfHdr1->e_shstrndx) * (elfHdr1->e_shentsize));
    Elf32_Shdr *symTable1 = NULL;
    Elf32_Shdr *strTable1;

    // Find symbol table and string table
    for (size_t i = 0; i < elfHdr1->e_shnum; i++)
    {
      Elf32_Shdr *section1 = mapPointer1 + (elfHdr1->e_shoff) + (i * elfHdr1->e_shentsize);
      char *name1 = mapPointer1 + secStrTable1->sh_offset + section1->sh_name;

      if (strcmp(name1, ".symtab") == 0)
        symTable1 = section1;
      else if (strcmp(name1, ".strtab") == 0)
        strTable1 = section1;
    }

    if (symTable1 == NULL)
    {
      printf("No symbol table detected in %s\n", filename1);
    }
    else
    {
      printf("File: %s\n", filename1);

      int size = symTable1->sh_size / sizeof(Elf32_Sym);

      if (debug)
      {
        printf("Debug Mode: Symbol table size: %d\n", size);
        printf("Debug Mode: Symbol table address: %p\n", symTable1);
        printf("Debug Mode: Symbol table offset: %d\n", symTable1->sh_offset);
        printf("Debug Mode: Symbol table entry size: %d\n", symTable1->sh_entsize);
        printf("--------------------------------------\n");
      }

      printf("Index \t Value \t\t Section Index \t Section Name \t\t Symbol Name\n");

      for (int i = 0; i < size; i++)
      {
        Elf32_Sym *symbol = mapPointer1 + symTable1->sh_offset + (i * sizeof(Elf32_Sym));
        char *symName = mapPointer1 + strTable1->sh_offset + symbol->st_name;
        int index = symbol->st_shndx;
        char *sectName;

        if (index == 0)
        {
          sectName = "UND";
        }
        else if (index == 65521)
        {
          sectName = "ABS";
        }
        else
        {
          Elf32_Shdr *section = mapPointer1 + (elfHdr1->e_shoff) + (index * elfHdr1->e_shentsize);
          sectName = mapPointer1 + secStrTable1->sh_offset + section->sh_name;
        }

        printf("[%2d] \t %08x \t %d \t\t %s \t\t %s\n", i, symbol->st_value, index, sectName, symName);
      }
    }
    printf("\n");
  }

  if (Currentfd2 != -1)
  {
    Elf32_Ehdr *elfHdr2 = (Elf32_Ehdr *)mapPointer2;
    Elf32_Shdr *secStrTable2 = mapPointer2 + (elfHdr2->e_shoff) + ((elfHdr2->e_shstrndx) * (elfHdr2->e_shentsize));
    Elf32_Shdr *symTable2 = NULL;
    Elf32_Shdr *strTable2;

    // Find symbol table and string table
    for (size_t i = 0; i < elfHdr2->e_shnum; i++)
    {
      Elf32_Shdr *section2 = mapPointer2 + (elfHdr2->e_shoff) + (i * elfHdr2->e_shentsize);
      char *name2 = mapPointer2 + secStrTable2->sh_offset + section2->sh_name;
      if (strcmp(name2, ".symtab") == 0)
        symTable2 = section2;
      else if (strcmp(name2, ".strtab") == 0)
        strTable2 = section2;
    }

    if (symTable2 == NULL)
    {
      printf("No symbol table detected in %s\n", filename2);
    }
    else
    {
      printf("File: %s\n", filename2);

      int size = symTable2->sh_size / sizeof(Elf32_Sym);

      if (debug)
      {
        printf("Debug Mode: Symbol table size: %d\n", size);
        printf("Debug Mode: Symbol table address: %p\n", symTable2);
        printf("Debug Mode: Symbol table offset: %d\n", symTable2->sh_offset);
        printf("Debug Mode: Symbol table entry size: %d\n", symTable2->sh_entsize);
        printf("--------------------------------------\n");
      }
      printf("Index \t Value \t\t Section Index \t Section Name \t\t Symbol Name\n");

      for (int i = 0; i < size; i++)
      {
        Elf32_Sym *symbol = mapPointer2 + symTable2->sh_offset + (i * sizeof(Elf32_Sym));
        char *symName = mapPointer2 + strTable2->sh_offset + symbol->st_name;
        int index = symbol->st_shndx;
        char *sectName;

        if (index == 0)
        {
          sectName = "UND";
        }
        else if (index == 65521)
        {
          sectName = "ABS";
        }
        else
        {
          Elf32_Shdr *section = mapPointer2 + (elfHdr2->e_shoff) + (index * elfHdr2->e_shentsize);
          sectName = mapPointer2 + secStrTable2->sh_offset + section->sh_name;
        }

        printf("[%2d] \t %08x \t %d \t\t %s \t\t %s\n", i, symbol->st_value, index, sectName, symName);
      }
    }
    printf("\n");
  }
}

void CheckFilesForMerge()
{
  if (Currentfd1 == -1 || Currentfd2 == -1)
  {
    fprintf(stderr, "Error: Two ELF files must be opened and mapped.\n");
    return;
  }

  Elf32_Ehdr *elfHdr1 = (Elf32_Ehdr *)mapPointer1;
  Elf32_Ehdr *elfHdr2 = (Elf32_Ehdr *)mapPointer2;

  // Check if the ELF file types are compatible for merging
  if (elfHdr1->e_type != elfHdr2->e_type || elfHdr1->e_machine != elfHdr2->e_machine || elfHdr1->e_ident[EI_CLASS] != elfHdr2->e_ident[EI_CLASS])
  {
    fprintf(stderr, "Error: Incompatible ELF file types for merging.\n");
    return;
  }

  // Check if both files have exactly one symbol table
  int symTableCount1 = 0;
  int symTableCount2 = 0;

  for (size_t i = 0; i < elfHdr1->e_shnum; i++)
  {
    Elf32_Shdr *section1 = mapPointer1 + (elfHdr1->e_shoff) + (i * elfHdr1->e_shentsize);

    if (section1->sh_type == SHT_SYMTAB)
      symTableCount1++;
  }

  for (size_t i = 0; i < elfHdr2->e_shnum; i++)
  {
    Elf32_Shdr *section2 = mapPointer2 + (elfHdr2->e_shoff) + (i * elfHdr2->e_shentsize);

    if (section2->sh_type == SHT_SYMTAB)
      symTableCount2++;
  }

  if (symTableCount1 != 1 || symTableCount2 != 1)
  {
    printf("Feature not supported: Each file must contain exactly one symbol table.\n");
    return;
  }

  Elf32_Shdr *secStrTable1 = mapPointer1 + (elfHdr1->e_shoff) + ((elfHdr1->e_shstrndx) * (elfHdr1->e_shentsize));
  Elf32_Shdr *symTable1 = NULL;
  Elf32_Shdr *strTable1 = NULL;

  // Find symbol table and string table for file 1
  for (size_t i = 0; i < elfHdr1->e_shnum; i++)
  {
    Elf32_Shdr *section1 = mapPointer1 + (elfHdr1->e_shoff) + (i * elfHdr1->e_shentsize);
    char *name1 = mapPointer1 + secStrTable1->sh_offset + section1->sh_name;

    if (strcmp(name1, ".symtab") == 0)
      symTable1 = section1;
    else if (strcmp(name1, ".strtab") == 0)
      strTable1 = section1;
  }

  Elf32_Shdr *secStrTable2 = mapPointer2 + (elfHdr2->e_shoff) + ((elfHdr2->e_shstrndx) * (elfHdr2->e_shentsize));
  Elf32_Shdr *symTable2 = NULL;
  Elf32_Shdr *strTable2 = NULL;

  // Find symbol table and string table for file 2
  for (size_t i = 0; i < elfHdr2->e_shnum; i++)
  {
    Elf32_Shdr *section2 = mapPointer2 + (elfHdr2->e_shoff) + (i * elfHdr2->e_shentsize);
    char *name2 = mapPointer2 + secStrTable2->sh_offset + section2->sh_name;

    if (strcmp(name2, ".symtab") == 0)
      symTable2 = section2;
    else if (strcmp(name2, ".strtab") == 0)
      strTable2 = section2;
  }

  if (symTable1 == NULL || strTable1 == NULL || symTable2 == NULL || strTable2 == NULL)
  {
    printf("Error: Symbol table or string table not found in one or both ELF files.\n");
    return;
  }

  // Iterate over symbol table entries in file 1
  int symTableSize = symTable1->sh_size / sizeof(Elf32_Sym);
  Elf32_Sym *symbolEntries = (Elf32_Sym *)(mapPointer1 + symTable1->sh_offset);

  for (int i = 1; i < symTableSize; i++) // Start from 1 to skip the dummy null symbol
  {
    Elf32_Sym *symbol = &symbolEntries[i];

    if (symbol->st_shndx == SHN_UNDEF)
    {
      char *symbolName = (char *)(mapPointer1 + strTable1->sh_offset + symbol->st_name);

      // Search for the symbol in the symbol table of file 2
      int symTableSize2 = symTable2->sh_size / sizeof(Elf32_Sym);
      Elf32_Sym *symbolEntries2 = (Elf32_Sym *)(mapPointer2 + symTable2->sh_offset);
      int foundInSymTable2 = 0;

      for (int j = 1; j < symTableSize2; j++) // Start from 1 to skip the dummy null symbol
      {
        Elf32_Sym *symbol2 = &symbolEntries2[j];
        char *symbolName2 = (char *)(mapPointer2 + strTable2->sh_offset + symbol2->st_name);

        if (strcmp(symbolName, symbolName2) == 0)
        {
          foundInSymTable2 = 1;
          if (symbol2->st_shndx == SHN_UNDEF)
          {
            foundInSymTable2 = 2;
          }
          break;
        }
      }

      if (foundInSymTable2 == 0 || foundInSymTable2 == 2)
      {
        printf("Symbol %s undefined\n", symbolName);
      }
    }
    else
    {
      char *symbolName = (char *)(mapPointer1 + strTable1->sh_offset + symbol->st_name);

      // Search for the symbol in the symbol table of file 2
      int symTableSize2 = symTable2->sh_size / sizeof(Elf32_Sym);
      Elf32_Sym *symbolEntries2 = (Elf32_Sym *)(mapPointer2 + symTable2->sh_offset);
      int foundInSymTable2 = 0;

      for (int j = 1; j < symTableSize2; j++) // Start from 1 to skip the dummy null symbol
      {
        Elf32_Sym *symbol2 = &symbolEntries2[j];
        char *symbolName2 = (char *)(mapPointer2 + strTable2->sh_offset + symbol2->st_name);

        if (strcmp(symbolName, symbolName2) == 0)
        {
          foundInSymTable2 = 1;
          if (symbol2->st_shndx != SHN_UNDEF)
          {
            foundInSymTable2 = 0;
          }
          break;
        }
      }

      if (foundInSymTable2 == 0)
      {
        printf("Symbol %s multiply defined\n", symbolName);
      }
    }
  }

  // Iterate over symbol table entries in file 2
  int symTableSize2 = symTable2->sh_size / sizeof(Elf32_Sym);
  Elf32_Sym *symbolEntries2 = (Elf32_Sym *)(mapPointer2 + symTable2->sh_offset);

  for (int i = 1; i < symTableSize2; i++) // Start from 1 to skip the dummy null symbol
  {
    Elf32_Sym *symbol2 = &symbolEntries2[i];

    if (symbol2->st_shndx == SHN_UNDEF)
    {
      char *symbolName2 = (char *)(mapPointer2 + strTable2->sh_offset + symbol2->st_name);

      // Search for the symbol in the symbol table of file 1
      int symTableSize1 = symTable1->sh_size / sizeof(Elf32_Sym);
      Elf32_Sym *symbolEntries1 = (Elf32_Sym *)(mapPointer1 + symTable1->sh_offset);
      int foundInSymTable1 = 0;

      for (int j = 1; j < symTableSize1; j++) // Start from 1 to skip the dummy null symbol
      {
        Elf32_Sym *symbol1 = &symbolEntries1[j];
        char *symbolName1 = (char *)(mapPointer1 + strTable1->sh_offset + symbol1->st_name);

        if (strcmp(symbolName2, symbolName1) == 0)
        {
          foundInSymTable1 = 1;
          if (symbol1->st_shndx == SHN_UNDEF)
          {
            foundInSymTable1 = 2;
          }
          break;
        }
      }

      if (foundInSymTable1 == 0 || foundInSymTable1 == 2)
      {
        printf("Symbol %s undefined\n", symbolName2);
      }
    }
    else
    {
      char *symbolName2 = (char *)(mapPointer2 + strTable2->sh_offset + symbol2->st_name);

      // Search for the symbol in the symbol table of file 1
      int symTableSize1 = symTable1->sh_size / sizeof(Elf32_Sym);
      Elf32_Sym *symbolEntries1 = (Elf32_Sym *)(mapPointer1 + symTable1->sh_offset);
      int foundInSymTable1 = 0;

      for (int j = 1; j < symTableSize1; j++) // Start from 1 to skip the dummy null symbol
      {
        Elf32_Sym *symbol1 = &symbolEntries1[j];
        char *symbolName1 = (char *)(mapPointer1 + strTable1->sh_offset + symbol1->st_name);

        if (strcmp(symbolName2, symbolName1) == 0)
        {
          foundInSymTable1 = 1;
          if (symbol1->st_shndx != SHN_UNDEF)
          {
            foundInSymTable1 = 0;
          }
          break;
        }
      }

      if (foundInSymTable1 == 0)
      {
        printf("Symbol %s multiply defined\n", symbolName2);
      }
    }
  }
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
