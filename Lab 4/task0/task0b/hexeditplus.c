#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int debug;

typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
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

void LoadIntoMemory(state *s) {
  printf("Not Implemented\n");
}

void ToggleDisplayMode(state *s) {
  printf("Not Implemented\n");
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

int main(int argc, char **argv) {
  debug = 0;
  state *s = malloc(sizeof(state));
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
    printf("\nChoose action:\n");
    for (int i = 0; i < size - 1; i++) {
      printf("%d)  %s\n", i, menu[i].name);
    }
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
