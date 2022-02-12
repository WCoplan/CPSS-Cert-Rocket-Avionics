#ifndef Smoother_h
#define Smoother_h

#define MAX_SIZE 2000

#include "Arduino.h"

class Smoother {
  public:
    Smoother(int bufferSize);
    void init();
    void update(float newVal);
    float getVal();
    bool isReady();
  private:
    int _bufferSize;
    float _buffer[MAX_SIZE];
    int _readIndex;
    float _totalVal;
    float _avgVal;
    long _loopCount;
};

#endif
