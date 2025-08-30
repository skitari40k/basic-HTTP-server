#include "get_ext.h"

const char *get_ext(char *filename)
{
  const char *dot = strrchr(filename, '.');
  if(!dot || dot == filename)
  {
    return "";
  }
  return dot + 1;
}
