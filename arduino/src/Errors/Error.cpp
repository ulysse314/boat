#include "Error.h"

#include <stdlib.h>
#include <string.h>

#include "../Controllers/ArduinoController.h"
#include "../Utils/StringUtils.h"

Error::Error(const char *message) :
    _message(createStringCopy(message)) {
}

Error::~Error() {
  if (_message) {
    free((void *)_message);
  }
}
