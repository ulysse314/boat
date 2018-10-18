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
  Value(Type type);
  ~Value();

  void setNull(); 
  void setString(const char *value);
  void setInteger(int32_t value);
  void setDouble(double value);
  void setBoolean(bool value);
  
  Type getType() const { return _type; };
  bool isNull() const { return _null; };
  const char *getString() const;
  int32_t getInteger() const;
  double getDouble() const;
  bool getBoolean() const;
  
protected:
  Type _type;
  bool _null;
  const char *_string;
  int32_t _integer;
  double _double;
  bool _boolean;
};

#endif // Value_h