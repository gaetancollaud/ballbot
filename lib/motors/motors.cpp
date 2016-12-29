#include "motors.h"

Motors::Motors(structMotorConfig configs[], structPid* pid){
  for(int i=0; i<3; i++){
    this->motors[i] = new SingleMotor(configs[i], pid);
  }
}

void Motors::init(){
  for(int i=0; i<3; i++){
    this->motors[i]->init();
  }
}
