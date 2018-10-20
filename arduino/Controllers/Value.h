#ifndef Value_h
#define Value_h

#include <Arduino.h>

class Value {
public:
  enum class Type {
    String,
    Integer,
    Double,
    Boolean
  };
  Value(Type type, const char *name);
  ~Value();

  void setNull(); 
  void setString(const char *value);
  void setInteger(int32_t value);
  void setDouble(double value);
  void setBoolean(bool value);
  
  Type getType() const { return _type; };
  const char *getName() const { return _name; };
  bool isNull() const { return _null; };
  const char *getString() const;
  int32_t getInteger() const;
  double getDouble() const;
  bool getBoolean() const;

  bool operator==(const Value &a) const {
    return strcmp(_name, a.getName()) == 0;
  }

protected:
  Type _type;
  const char *_name;
  bool _null;
  const char *_string;
  int32_t _integer;
  double _double;
  bool _boolean;
};

#endif // Value_h
