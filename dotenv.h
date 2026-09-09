#ifndef PROXYGPT_DOTENV_H
#define PROXYGPT_DOTENV_H

#include <stddef.h>

typedef struct {
  char *key;
  char *value;
} DotenvEntry;

typedef struct {
  DotenvEntry *entries;
  size_t count;
} Dotenv;

int dotenvReadUserFile(Dotenv *dotenv);
int dotenvApply(const Dotenv *dotenv);
void dotenvFree(Dotenv *dotenv);

#endif
