#ifndef Controller_h
#define Controller_h

#include "Error.h"
#include "List.h"

class Error;
class SensorList;
class Value;

class Controller {
public:
  Controller();
  virtual ~Controller();

  virtual const char *getName() const = 0;
  virtual void addSensorsToList(SensorList *) {};
  virtual void begin() {};
  virtual void sensorsHasBeenUpdated() {};
  virtual int getValueCount() const = 0;
  virtual const Value *getValueAtIndex(int) const = 0;

  void addError(Error *error);
  void clearNonPersistantErrors();
  const List<Error>::Bucket *getFirstErrorBucket() const { return _errorList.getFirstBucket(); };
  bool nextErrorBucket(const List<Error>::Bucket *&bucket, const Error *&error) const {   return _errorList.nextDataWithBucket(bucket, error); };
  bool removeErrorWithCode(Error::Domain domain, int code);

protected:
  void clearAllErrors();

  List<Error> _errorList;
};

#endif // Controller_h
