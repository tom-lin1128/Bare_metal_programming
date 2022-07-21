#include "util.h"
void *memset(void *s, int c, int n)
{
  char *p = s;
  for (int i = 0; i < n; i++)
  {
    p[i] = c;
  }
  return s;
}