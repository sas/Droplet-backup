#include <stdio.h>
#include <stdlib.h>

#include <usage.h>

#include "help.h"

int cmd_help(int argc, char *argv[])
{
  usage(stdout);
  exit(0);
}
