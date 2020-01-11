#ifndef List_h
#define List_h

#include <Arduino.h>

#include "Debug.h"

template <class T>
class List {
protected:
  struct _Bucket {
    T *data;
    struct _Bucket *previousBucket;
    struct _Bucket *nextBucket;
  };

public:
  typedef struct _Bucket Bucket;
  enum class WhenDataRemoved {
    NoFree,
    Free,
  };

  List(WhenDataRemoved whenDataRemoved) : _whenDataRemoved(whenDataRemoved){
    _firstBucket = NULL;
    _lastBucket = NULL;
  };
  ~List() {
    removeAllData();
  };
  
  bool hasMoreThanOne() const {
    return _firstBucket && _firstBucket->nextBucket;
  }

  void testList() const {
    bool failed = false;
    Serial.print("test this: ");
    Serial.println((unsigned int)this, HEX);
    if (!_firstBucket) {
      if (_lastBucket) {
        Serial.print("First bucket: null, last bucket: ");
        Serial.println((unsigned int)_lastBucket, HEX);
        failed = true;
      }
    } else if (!_lastBucket) {
      Serial.print("_firstBucket: ");
      Serial.print((unsigned int)_firstBucket, HEX);
      Serial.print(", last bucket: null");
      failed = true;
    } else if (_firstBucket->previousBucket) {
      Serial.print("_firstBucket->previousBucket: ");
      Serial.println((unsigned int)_firstBucket->previousBucket, HEX);
      failed = true;
    } else if (_lastBucket->nextBucket) {
      Serial.print("_lastBucket->nextBucket: ");
      Serial.println((unsigned int)_lastBucket->nextBucket, HEX);
      failed = true;
    } else {
      Bucket *previousBucket = _firstBucket;
      Bucket *currentBucket = _firstBucket->nextBucket;
      while (currentBucket) {
        if (previousBucket != currentBucket->previousBucket) {
          Serial.println("previousBucket != currentBucket->previousBucket");
          failed = true;
        }
        if (previousBucket->nextBucket != currentBucket) {
          Serial.println("previousBucket->nextBucket != currentBucket");
          failed = true;
        }
        previousBucket = previousBucket->nextBucket;
        currentBucket = currentBucket->nextBucket;
      }
    }
    if (failed) {
      Serial.println("=== Failed ===");
    } else {
      Serial.println("=== Good ===");
    }
  }

  void printInfo() const {
    Serial.print("this: ");
    Serial.println((unsigned int)this, HEX);
    Serial.print("_firstBucket: ");
    Serial.println((unsigned int)_firstBucket, HEX);
    Serial.print("_firstBucket->data: ");
    Serial.println((unsigned int)_firstBucket->data, HEX);
    Serial.print("_firstBucket->nextBucket: ");
    Serial.println((unsigned int)_firstBucket->nextBucket, HEX);
    Serial.print("_lastBucket: ");
    Serial.println((unsigned int)_lastBucket, HEX);
  }

  void printAllInfo() const {
    Serial.print("this: ");
    Serial.println((unsigned int)this, HEX);
    Serial.print("_firstBucket: ");
    Serial.println((unsigned int)_firstBucket, HEX);
    Serial.print("_lastBucket: ");
    Serial.println((unsigned int)_lastBucket, HEX);
    Bucket *bucket = _firstBucket;
    while (bucket) {
      Serial.print("  bucket: ");
      Serial.println((unsigned int)bucket, HEX);
      Serial.print("  bucket->nextBucket: ");
      Serial.println((unsigned int)bucket->nextBucket, HEX);
      Serial.print("  bucket->previousBucket: ");
      Serial.println((unsigned int)bucket->previousBucket, HEX);
      Serial.println("  ");
      Serial.flush();
      bucket = bucket->nextBucket;
    }
    Serial.println("-- done --");
  }

  void addData(T *data) {
    Bucket *bucket = (Bucket *)malloc(sizeof(*bucket));
    bucket->data = data;
    bucket->previousBucket = NULL;
    bucket->nextBucket = NULL;
    if (!_firstBucket) {
      _firstBucket = bucket;
      _lastBucket = bucket;
    } else {
      _lastBucket->nextBucket = bucket;
      bucket->previousBucket = _lastBucket;
      _lastBucket = bucket;
    }
  };

  bool removeDataAtIndex(size_t index) {
    Bucket *bucket = bucketAtIndex(index);
    if (!bucket) {
      return false;
    }
    removeBucket(bucket);
    return true;
  };

  bool removeData(T *data) {
    Bucket *bucket = bucketForData(data);
    if (!bucket) {
      return false;
    }
    removeBucket(bucket);
    return true;
  };

  void removeAllData() {
    while (_firstBucket) {
      removeBucket(_firstBucket);
    }
  };

  T *dataAtIndex(size_t index) const {
    Bucket *bucket = bucketAtIndex(index);
    if (!bucket) {
      return NULL;
    }
    return bucket->data;
  };

  size_t indexForData(const T*data) const {
    Bucket *bucket = _firstBucket;
    size_t index = 0;
    while (bucket) {
      if (*(bucket->data) == *data) {
        return index;
      }
      bucket = bucket->nextBucket;
      ++index;
    }
    return (size_t)-1;
  }

  const Bucket *firstBucket() const {
    return _firstBucket;
  };
  
  bool currentDataWithBucket(const Bucket *bucket, T *&data) const {
    if (!bucket) {
      data = NULL;
      return false;
    }
    data = bucket->data;
    return true;
  }
  
  bool currentDataWithBucket(const Bucket *bucket, const T *&data) const {
    if (!bucket) {
      data = NULL;
      return false;
    }
    data = bucket->data;
    return true;
  }

  bool nextDataWithBucket(const Bucket *&bucket, T *&data) const {
    bool result = currentDataWithBucket(bucket, data);
    if (bucket) {
      bucket = bucket->nextBucket;
    }
    return result;
  };

  bool nextDataWithBucket(const Bucket *&bucket, const T *&data) const {
    bool result = currentDataWithBucket(bucket, data);
    if (bucket) {
      bucket = bucket->nextBucket;
    }
    return result;
  };

  bool removeCurrentDataWithNextBucket(const Bucket *&bucket) {
    if (!bucket) {
      return false;
    }
    const Bucket *bucketToRemove = bucket;
    bucket = bucket->nextBucket;
    removeBucket(bucketToRemove);
    return bucket != NULL;
  }

  Bucket *bucketAtIndex(size_t index) const {
    Bucket *result = _firstBucket;
    while (result && index > 0) {
      result = result->nextBucket;
      index--;
    }
    return result;
  };

  Bucket *bucketForData(T *data) const {
    Bucket *result = _firstBucket;
    while (result && result->data != data) {
      result = result->nextBucket;
    }
    return result;
  };

  void removeBucket(const Bucket *bucket) {
    if (bucket == _firstBucket) {
      _firstBucket = bucket->nextBucket;
    } else {
      bucket->previousBucket->nextBucket = bucket->nextBucket;
    }
    if (bucket == _lastBucket) {
      _lastBucket = bucket->previousBucket;
    } else {
      bucket->nextBucket->previousBucket = bucket->previousBucket;
    }
    if (_whenDataRemoved == WhenDataRemoved::Free && bucket->data) {
      delete bucket->data;
    }
    free((void *)bucket);
  };

protected:
  WhenDataRemoved _whenDataRemoved;
  Bucket *_firstBucket;
  Bucket *_lastBucket;
};

#endif // List_h
