#include "List.h"

#include <Arduino.h>

/*
template <class T>
List<T>::List(WhenDataRemoved whenDataRemoved) : _whenDataRemoved(whenDataRemoved){
  _firstBucket = NULL;
  _lastBucket = NULL;
}

template <class T>
List<T>::~List() {
  removeAllData();
}

template <class T>
void List<T>::addData(T *data) {
  Bucket *bucket = malloc(sizeof(Bucket));
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
}

template <class T>
bool List<T>::removeDataAtIndex(int index) {
  Bucket *bucket = bucketAtIndex(index);
  if (!bucket) {
    return false;
  }
  removeBucket(bucket);
  return true;
}

template <class T>
bool List<T>::removeData(T *data) {
  Bucket *bucket = bucketForData(data);
  if (!bucket) {
    return false;
  }
  removeBucket(bucket);
  return true;
}

template <class T>
void List<T>::removeAllData() {
  while (_firstBucket) {
    removeBucket(_firstBucket);
  }
}

template <class T>
T *List<T>::dataAtIndex(int index) {
  Bucket *bucket = bucketAtIndex(index);
  if (!bucket) {
    return NULL;
  }
  return bucket->data;
}

template <class T>
bool List<T>::enumerateData(typename List<T>::Bucket *&bucket, T *&data) {
  if (!bucket) {
    bucket = _firstBucket;
  }
  bool hasData = false;
  if (bucket) {
    data = bucket->data;
    hasData = true;
    bucket = bucket->nextBucket;
  } else {
    data = NULL;
  }
  return hasData;
}

template <class T>
typename List<T>::Bucket *List<T>::bucketAtIndex(int index) {
  Bucket *result = _firstBucket;
  while (result && index > 0) {
    result = result->nextBucket;
    index--;
  }
  return result;
}

template <class T>
typename List<T>::Bucket *List<T>::bucketForData(T *data) {
  Bucket *result = _firstBucket;
  while (result && result->data != data) {
    result = result->nextBucket;
  }
  return result;
}

template <class T>
void List<T>::removeBucket(List<T>::Bucket *bucket) {
  if (bucket == _firstBucket) {
    _firstBucket = bucket->nextBucket;
  } else {
    bucket->previousBucket->nextBucket = bucket->nextBucket;
  }
  if (bucket == _lastBucket) {
    _lastBucket = bucket->previousBucket;
  } else {
    _lastBucket->nextBucket->previousBucket = bucket->previousBucket;
  }
  if (_whenDataRemoved == WhenDataRemoved::Free && bucket->data) {
    delete bucket->data;
  }
}
*/
