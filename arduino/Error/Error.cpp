#include "Error.h"

#include <stdlib.h>
#include <string.h>

#include "ArduinoController.h"

Error::Error(const char *message) :
    _message(NULL) {
  if (message) {
    size_t messageLength = strlen(message);
    char *newMessage = (char *)malloc(messageLength + 1);
    strcpy(newMessage, message);
    _message = newMessage;
  }
}

Error::~Error() {
  if (_message) {
    free((void *)_message);
  }
}
