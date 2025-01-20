#include <stdlib.h>
#include <stdio.h>
#include <string.h>
 
struct fun_desc {
char *name;
char (*fun)(char);
}; 


char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  for (int i = 0; i < array_length; i++)
  {
    mapped_array[i] = f(array[i]);
  }
  return mapped_array;
}
 
char my_get(char c)
{
  return fgetc(stdin);
}

char cprt(char c)
{
  if (0x20 <= c && c <= 0x7E)
  {
    printf("%c\n", c);
  }
  else
  {
    printf("%c",'.');
  }
  
  return c;
}


char encrypt(char c)
{
  if (0x20 <= c && c <= 0x7E)
  {
    return c+2;
  }
  else
  {
    return c;
  }
  
}


char decrypt(char c)
{
  if (0x20 <= c && c <= 0x7E)
  {
    return c-2;
  }
  else
  {
    return c;
  }
}


char xprt(char c)
{
  printf("%x\n",c);
  return c;
}



int main(int argc, char **argv){

  int base_len = 5;
  
  char *carray = malloc(base_len);

  struct fun_desc menu[] = 
  {
    { "Get string", my_get },
    { "Print string", cprt },
    { "Encrypt", encrypt },
    { "Decrypt", decrypt },
    { "Print Hex", xprt },
    { NULL, NULL } 
  };

  int bound = sizeof(menu)/sizeof(struct fun_desc);

  while (1)
  {
    for (int i = 1; i < bound; i++)
    {
      printf("%d) %s\n", i-1, menu[i-1].name);
    }
    printf("Select operation from the following menu:");
    fflush(stdin);
    char buf[5];
    if (fgets(buf, sizeof(buf), stdin)!=NULL)
    {
      int inputNum = atoi(buf);
      if ( 0 <= inputNum && inputNum < bound-1)
      {
        printf("Within bounds\n\n");
        carray = map(carray, base_len, menu[inputNum].fun);
        printf("\nDone.\n\n");
      }
      else
      {
        printf("Not within bounds\n\n");
        free(carray);
        exit(0);
      }
    }
    else
    {
      printf("\nEOF! Quitting! \n\n");
      free(carray);
      exit(0);
    }
  }
}