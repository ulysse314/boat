#ifndef Controller_h
#define Controller_h

#include "Error.h"
#include "List.h"

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
  virtual void setValue(int) {};

  const List<Value>::Bucket *getFirstValueBucket() const { return _valueList.getFirstBucket(); };
  bool nextValueBucket(const List<Value>::Bucket *&bucket, const Value *&value) const {   return _valueList.nextDataWithBucket(bucket, value); };

  bool addError(Error *error);
  void clearNonPersistantErrors();
  const List<Error>::Bucket *getFirstErrorBucket() const { return _errorList.getFirstBucket(); };
  bool nextErrorBucket(const List<Error>::Bucket *&bucket, const Error *&error) const {   return _errorList.nextDataWithBucket(bucket, error); };
  bool removeErrorWithCode(Error::Domain domain, int code);

protected:
  void addValue(Value *value);

  List<Value> _valueList;
  List<Error> _errorList;
};

#endif // Controller_h
