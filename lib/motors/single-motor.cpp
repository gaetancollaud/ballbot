#include "single-motor.h"

SingleMotor::SingleMotor(structMotorConfig* config, structPid* pidValue)
  :pinLeft(config->pinLeft), pinRight(config->pinRight), rotaryA(config->rotaryA), rotaryB(config->rotaryB),
  pidSpeed(&currentSpeed, &outputSpeed, &targetSpeed, pidValue){
  this->encoderValue = 0;
  this->lastEncoderValue=0;
  this->targetSpeed = 0;
  this->maskRotaryA = g_APinDescription[rotaryA].ulPin;
  this->maskRotaryB = g_APinDescription[rotaryB].ulPin;
  pidSpeed.setLimit(MAX_DC_MOTOR_VALUE);
}

void SingleMotor::init(){
  pinMode(this->pinLeft, OUTPUT);
  pinMode(this->pinRight, OUTPUT);
  pinMode(this->rotaryA, INPUT_PULLUP);
  pinMode(this->rotaryB, INPUT_PULLUP);

  analogWrite(this->pinLeft, LOW);
  analogWrite(this->pinRight, LOW);

}

void SingleMotor::setSpeed(double speed){
  this->targetSpeed = speed;
}

void SingleMotor::loop(unsigned long nowMs, double dtS) {
  if (nowMs >= this->nextTime) {
  	this->nextTime = nowMs + REGULATION_DELAY_MS;

    long ev = this->encoderValue;
    this->currentSpeed = ev-this->lastEncoderValue;
    this->lastEncoderValue = ev;

    long ec = this->encoderCount;
    long ecDiff = ec-this->lastEncoderCount;
    this->lastEncoderCount = ec;

    this->pidSpeed.loop(nowMs, REGULATION_DELAY_S);
    this->writeOutputSpeed();

    // MOTOR_DEBUG("encoder: ");
    // MOTOR_DEBUG(ev);
    // MOTOR_DEBUG("count: ");
    // MOTOR_DEBUG(ecDiff);
    // MOTOR_DEBUG("current: ");
    // MOTOR_DEBUG(this->currentSpeed);
    // MOTOR_DEBUG("\ttarget: ");
    // MOTOR_DEBUG(this->targetSpeed);
    // MOTOR_DEBUG("\toutput: ");
    // MOTOR_DEBUG(this->outputSpeed);
    // MOTOR_DEBUGLN();
  }

  // MOTOR_DEBUG(this->encoderValue);
  // MOTOR_DEBUG((int16_t)digitalRead(this->rotaryA));
  // MOTOR_DEBUG((int16_t)digitalRead(this->rotaryB));
  // MOTOR_DEBUGln();
}

void SingleMotor::updateEncoder(uint32_t mask){
  encoderCount++;

  int MSB = !!(mask & maskRotaryA); //MSB = most significant bit
  int LSB = !!(mask & maskRotaryB); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  lastEncoded = encoded; //store this value for next time

  // if (digitalReadDirect(rotaryA) == digitalReadDirect(rotaryB)) {
  //   encoderValue++;
  // } else {
  //   encoderValue--;
  // }
}

void SingleMotor::writeOutputSpeed(){
  int speed = this->outputSpeed;
  int16_t l=0, r=0;
  if(speed>=0){
    if(speed!=0){
      speed += MOTOR_MIN_SPEED;
    }
    r = speed;
    l = LOW;
  }else{
    speed -= MOTOR_MIN_SPEED;
    l = -speed;
    r = LOW;
  }
  analogWrite(this->pinLeft, l);
  analogWrite(this->pinRight, r);
}
