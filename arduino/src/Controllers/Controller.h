#ifndef Controller_h
#define Controller_h

#include "../Errors/Error.h"
#include "../Utils/List.h"

class SensorList;
class Value;

class Controller {
public:
  Controller();
  virtual ~Controller();

  virtual const char *name() const = 0;
  virtual void addSensorsToList(SensorList *) {};
  virtual void begin() {};
  virtual void sensorsHasBeenUpdated() {};

  const List<Value>::Bucket *firstValueBucket() const { return _valueList.firstBucket(); };
  bool nextValueBucket(const List<Value>::Bucket *&bucket, const Value *&value) const {   return _valueList.nextDataWithBucket(bucket, value); };

  bool addError(Error *error);
  bool removeError(Error *error);
  void clearNonPersistantErrors();
  const List<Error>::Bucket *firstErrorBucket() const { return _errorList.firstBucket(); };
  bool nextErrorBucket(const List<Error>::Bucket *&bucket, const Error *&error) const {   return _errorList.nextDataWithBucket(bucket, error); };
  bool removeErrorWithCode(Error::Domain domain, int code);

protected:
  void addValue(Value *value);

  List<Value> _valueList;
  List<Error> _errorList;
};

#endif // Controller_h
