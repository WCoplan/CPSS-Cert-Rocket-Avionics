/*
  Smoother.cpp
  Created by Ryan Westcott - January 2021
*/

#include "Arduino.h"
#include "Smoother.h"

Smoother::Smoother(int bufferSize){
  _bufferSize = bufferSize;
}

void Smoother::init(){
  if(_bufferSize > MAX_SIZE){
    Serial.println("SMOOTHING LIB ERROR");
  }
  
  for(int i = 0; i < _bufferSize; i++){
    _buffer[i] = 0.0;
  }
}


void Smoother::update(float newVal){
  _totalVal -= _buffer[_readIndex];
  _buffer[_readIndex] = newVal;
  _totalVal += _buffer[_readIndex];

  _readIndex += 1;
  if(_readIndex >= _bufferSize){
    _readIndex = 0;
  }

  _loopCount += 1;

  if(_loopCount < _bufferSize){
    _avgVal = _totalVal / _loopCount;
  } else {
    _avgVal = _totalVal / _bufferSize;
  }
}

float Smoother::getVal(){
  return _avgVal;
}

bool Smoother::isReady(){
  return _loopCount >= _bufferSize;
}
