void drive( boolean direction_left_forward, boolean direction_rigt_forward, int speed_left, int speed_right){
  if(speed_right !=0){
    speed_right +=20;// speed adjustment to let it drive straight
  }
  if(direction_left_forward){
    analogWrite(AIA, speed_left);
    analogWrite(AIB, 0);
  }
  else{
    analogWrite(AIA, 0);
    analogWrite(AIB, speed_left);
  }

  if(direction_rigt_forward){
    analogWrite(BIA, speed_right);
    analogWrite(BIB, 0);
  }
  else{
    analogWrite(BIA, 0);
    analogWrite(BIB, speed_right);
  }

}

// TODO: when start button is pressed again stop the code




