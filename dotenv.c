#include "dotenv.h"

#include <ctype.h>
#include <errno.h>
#include <pwd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char *const DOTENV_FILE_NAME = "/.proxygpt";

static char *copyBytes(const char *source, size_t length) {
  if (length > SIZE_MAX - 1) {
    errno = EOVERFLOW;
    return NULL;
  }

  char *copy = malloc(length + 1);
  if (copy == NULL) {
    return NULL;
  }

  memcpy(copy, source, length);
  copy[length] = '\0';
  return copy;
}

static char *getUserDotenvPath(void) {
  struct passwd *user = getpwuid(getuid());
  if (user == NULL || user->pw_dir == NULL || user->pw_dir[0] == '\0') {
    errno = ENOENT;
    return NULL;
  }

  size_t homeLength = strlen(user->pw_dir);
  size_t fileNameLength = strlen(DOTENV_FILE_NAME);
  if (homeLength > SIZE_MAX - fileNameLength - 1) {
    errno = EOVERFLOW;
    return NULL;
  }

  char *path = malloc(homeLength + fileNameLength + 1);
  if (path == NULL) {
    return NULL;
  }

  memcpy(path, user->pw_dir, homeLength);
  memcpy(path + homeLength, DOTENV_FILE_NAME, fileNameLength + 1);
  return path;
}

static int isAsciiLetter(char character) {
  return (character >= 'A' && character <= 'Z') ||
         (character >= 'a' && character <= 'z');
}

static int isAsciiDigit(char character) {
  return character >= '0' && character <= '9';
}

static int isValidKey(const char *key, size_t length) {
  if (length == 0 || (!isAsciiLetter(key[0]) && key[0] != '_')) {
    return 0;
  }

  for (size_t index = 1; index < length; ++index) {
    if (!isAsciiLetter(key[index]) && !isAsciiDigit(key[index]) &&
        key[index] != '_') {
      return 0;
    }
  }

  return 1;
}

static int containsWhitespace(const char *value, size_t length) {
  for (size_t index = 0; index < length; ++index) {
    if (isspace((unsigned char)value[index])) {
      return 1;
    }
  }

  return 0;
}

static int parseLine(const char *line, size_t lineLength, char **key,
                     char **value) {
  while (lineLength > 0 &&
         (line[lineLength - 1] == '\n' || line[lineLength - 1] == '\r')) {
    --lineLength;
  }

  const char *separator = memchr(line, '=', lineLength);
  if (separator == NULL) {
    return 0;
  }

  size_t keyLength = (size_t)(separator - line);
  size_t valueLength = lineLength - keyLength - 1;
  const char *valueStart = separator + 1;

  if (!isValidKey(line, keyLength) || valueLength == 0 ||
      containsWhitespace(line, keyLength) ||
      containsWhitespace(valueStart, valueLength) ||
      memchr(line, '\0', lineLength) != NULL) {
    return 0;
  }

  *key = copyBytes(line, keyLength);
  if (*key == NULL) {
    return -1;
  }

  *value = copyBytes(valueStart, valueLength);
  if (*value == NULL) {
    free(*key);
    *key = NULL;
    return -1;
  }

  return 1;
}

static int appendEntry(Dotenv *dotenv, char *key, char *value) {
  if (dotenv->count >= (SIZE_MAX / sizeof(DotenvEntry)) - 1) {
    errno = EOVERFLOW;
    return -1;
  }

  DotenvEntry *entries =
      realloc(dotenv->entries, (dotenv->count + 1) * sizeof(DotenvEntry));
  if (entries == NULL) {
    return -1;
  }

  dotenv->entries = entries;
  dotenv->entries[dotenv->count].key = key;
  dotenv->entries[dotenv->count].value = value;
  ++dotenv->count;
  return 0;
}

int dotenvReadUserFile(Dotenv *dotenv) {
  if (dotenv == NULL) {
    errno = EINVAL;
    return -1;
  }

  dotenv->entries = NULL;
  dotenv->count = 0;

  char *path = getUserDotenvPath();
  if (path == NULL) {
    return -1;
  }

  FILE *file = fopen(path, "r");
  if (file == NULL) {
    int error = errno;
    free(path);
    if (error == ENOENT) {
      return 0;
    }

    errno = error;
    return -1;
  }

  char *line = NULL;
  size_t lineCapacity = 0;
  int result = 0;
  ssize_t lineLength;

  while ((lineLength = getline(&line, &lineCapacity, file)) != -1) {
    char *key = NULL;
    char *value = NULL;
    int parseResult = parseLine(line, (size_t)lineLength, &key, &value);

    if (parseResult < 0) {
      result = -1;
      break;
    }
    if (parseResult == 0) {
      continue;
    }

    if (appendEntry(dotenv, key, value) == -1) {
      free(key);
      free(value);
      result = -1;
      break;
    }
  }

  if (result == 0 && ferror(file)) {
    if (errno == 0) {
      errno = EIO;
    }
    result = -1;
  }

  int closeResult = fclose(file);
  if (result == 0 && closeResult != 0) {
    result = -1;
  }

  free(line);
  free(path);

  if (result == -1) {
    dotenvFree(dotenv);
  }

  return result;
}

int dotenvApply(const Dotenv *dotenv) {
  if (dotenv == NULL) {
    errno = EINVAL;
    return -1;
  }

  for (size_t index = 0; index < dotenv->count; ++index) {
    if (setenv(dotenv->entries[index].key, dotenv->entries[index].value, 1) ==
        -1) {
      return -1;
    }
  }

  return 0;
}

void dotenvFree(Dotenv *dotenv) {
  if (dotenv == NULL) {
    return;
  }

  for (size_t index = 0; index < dotenv->count; ++index) {
    free(dotenv->entries[index].key);
    free(dotenv->entries[index].value);
  }

  free(dotenv->entries);
  dotenv->entries = NULL;
  dotenv->count = 0;
}
