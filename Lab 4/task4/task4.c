#include <string.h>
#include <stdio.h>

int digit_cnt(char *str)
{
  int counter = 0;
  for (int i = 0; str[i] != 0; i++)
  {
    if (str[i] >= '0' && str[i] <= '9')
      counter++;
  }
  return counter;
}

int main(int argc, char **argv)
{
  int count = digit_cnt(argv[1]);
  printf("we found %d digits in the provided string\n", count);
  return 0;
}