#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "frozen.h"



int main()
{
  char *str;
  str = json_fread("example3.json");

  int i, value, len = strlen(str);
  struct json_token t;

  int numbers[10];

  for (i = 0; json_scanf_array_elem(str, len, ".things", i, &t) > 0; i++) {
    // t.type == JSON_TYPE_OBJECT
    json_scanf(t.ptr, t.len, "%d", &numbers[i]);  // value is 123, then 345
    printf("%d\n", numbers[i]);
  }
  return 0;
}
