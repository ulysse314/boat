#ifndef StringUtils_h
#define StringUtils_h

#include <stdlib.h>

inline const char *createStringCopy(const char *string) {
  if (!string) return NULL;
  size_t length = strlen(string) + 1;
  char *stringCopy = (char *)malloc(length);
  strncpy(stringCopy, string, length);
  return stringCopy;
}

#endif // StringUtils_h
