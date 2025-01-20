#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"
int debug;

typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
  int displayMode; // 0=int !0=hex
} state;

struct fun_desc {
  char *name;
  void (*fun)(state *s);
};

int validSize(int size) {
  if (size < 1 || size > 4 || size == 3)
    return 0;
  else
    return 1;
}

void ToggleDebugMode(state *s) {
  if (debug == 0) {
    printf("Debug flag now on\n");
    debug = 1;
  } else if (debug == 1) {
    printf("Debug flag now off\n");
    debug = 0;
  } else {
    printf("Invalid debug value: debug=%d\nTerminating...", debug);
    exit(1);
  }
}

void SetFileName(state *s) {
  printf("Enter new file name:\n");
  char name[100];
  fgets(name, sizeof(name), stdin);
  name[strcspn(name, "\n")] = '\0'; // Remove trailing newline character
  strncpy(s->file_name, name, sizeof(s->file_name) - 1);
  if (debug) {
    fprintf(stderr, "Debug: file name set to %s\n", name);
  }
}

void SetUnitSize(state *s) {
  printf("Enter new size:\n");
  int newSize = 0;
  char input[100];
  fgets(input, sizeof(input), stdin);
  sscanf(input, "%d", &newSize);

  if (!validSize(newSize)) {
    printf("Invalid unit size\n");
  } else {
    s->unit_size = newSize;
    if (debug) {
      fprintf(stderr, "Debug: set size to %d\n", newSize);
    }
  }
}

void LoadIntoMemory(state* s) {
  if (strcmp(s->file_name, "") == 0) {
    printf("Error: file name empty\n");
    return;
  } else {
    FILE* file;
    if (!(file = fopen(s->file_name, "r+"))) {
      printf("Error: cannot open file\n");
    } else {
      printf("Please enter <location> <length>\n");

      // Read user input as a string
      char input[100];
      fgets(input, sizeof(input), stdin);

      int location = 0, length = 0;

      // Parse the input string using sscanf
      if (sscanf(input, "%x %d", &location, &length) != 2) {
        printf("Error: invalid input\n");
        fclose(file);
        return;
      }

      if (s->debug_mode) {
        fprintf(stderr, "file name: %s\n", s->file_name);
        fprintf(stderr, "location: %d\n", location);
        fprintf(stderr, "length: %d\n", length);
      }

      fseek(file, location, SEEK_SET);
      fread(s->mem_buf, length, s->unit_size, file);
      s->mem_count = length * s->unit_size;
      fclose(file);
      fprintf(stdout, "\nLoaded %d units into memory\n", length);
    }
  }
}

void ToggleDisplayMode(state* s){
   if(s->displayMode==0){
    printf("Display flag now on, hexadecimal representation\n");
    s->displayMode=1;
  }
  else if(s->displayMode==1){
    printf("Display flag now off, decimal representation\n");
    s->displayMode=0;
  }
  else{
    printf("Invalid display mode value: mode=%d\nTerminating...",s->displayMode);
    exit(1);
  }

}

void MemoryDisplay(state *s) {
  printf("Not Implemented\n");
}

void SaveIntoFile(state *s) {
  printf("Not Implemented\n");
}

void MemoryModify(state *s) {
  printf("Not Implemented\n");
}

void Quit(state *s) {
  printf("quitting...\n");
  exit(0);
}

void map(state *s, void (*f)(state *)){
  f(s);
}

char* unit_to_format(int unit,int mode) {
  char* formats[4];
  if(mode)
      {formats[0]="%#hhx\n"; formats[1]="%#hx\n"; formats[2]="No such unit"; formats[3]="%#x\n";}
  else
    {formats[0]="%#hhd\n"; formats[1]="%#hd\n"; formats[2]="No such unit"; formats[3]="%#d\n";}
    return formats[unit-1];
}

/* Prints the buffer to screen by converting it to text with printf */
void print_units(FILE* output, state *s, int count) {
  char *buffer=s->file_name;
    char* end =  buffer + s->unit_size*count;
    while (buffer < end) {
        //print stuff
        int var = *((int*)(buffer));
        fprintf(output, unit_to_format(s->unit_size,s->displayMode), var);
        buffer += s->unit_size;
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {
  debug = 0;
  state *s = malloc(sizeof(state));
  s->unit_size = 1; // Set unit_size to 1
  s->displayMode=0;
  struct fun_desc menu[] =
      {
          {"Toggle Debug Mode", ToggleDebugMode},
          {"Set File Name", SetFileName},
          {"Set Unit Size", SetUnitSize},
          {"Load Into Memory", LoadIntoMemory},
          {"Toggle Display Mode", ToggleDisplayMode},
          {"Memory Display", MemoryDisplay},
          {"Save Into File", SaveIntoFile},
          {"Memory Modify", MemoryModify},
          {"Quit", Quit},
          {NULL, NULL}};
  int size = sizeof(menu) / sizeof(menu[0]);
  while (1) {
    if (debug) {
      printf("\nfile name: %s\nunit size: %d\nmem count: %d\n", s->file_name, s->unit_size, s->mem_count);
    }
    printf(ANSI_COLOR_GREEN "\nChoose action:\n");
    for (int i = 0; i < size - 1; i++) {
      printf("%d)  %s\n", i, menu[i].name);
    }
    printf(ANSI_COLOR_RESET);
    int inNum;
    char in[20];
    fgets(in, sizeof(in), stdin);
    sscanf(in, "%d", &inNum);
    if (inNum >= 0 && inNum < size - 1) {
      printf("\nWithin bounds\n");
      map(s, menu[inNum].fun);
      printf("\nDONE.\n");
    } else {
      printf("\nNot Within bounds\n");
      return 0;
    }
  }
  free(s);
  return 0;
}
