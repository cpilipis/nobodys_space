#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "frozen.h"



int main()
{
  char *str;
  str = json_fread("example2.json");

  int a;
  char *c;
  void *my_data = NULL;
  json_scanf(str, strlen(str), "{\n\tplace : {name : %Q, rating : %d}}", &c, &a);

  printf("Rating is %d\n", a);
  printf("Name is %s\n", c);
  return 0;
}
