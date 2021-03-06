void get_function_block_by_id(int id, int pot_value){
  /*
  motor_assignment[0]= indication what variables there are
   2 = driving forward/backwards
   3 = turning left/right
   4 = speed
   5 = delay
   6 = loop a
   7 = loop b
   motor_assignment[0]= function
   motor_assignment[1]= direction left motor (1 = forward, 0 = backwards)  ||  the speed we should drive this direction || the ammount of time to repeat
   motor_assignment[2]= direction right motor (1 = forward, 0 = backwards)
   motor_assignment[3]= the ammount of time to drive this direction
   motor_assignment[4]= global speed value:  the speed for this block <-- we need this here because we change the speed setting globaly
   
   */
  int speed_setting = 0;
  if(local_speed_set == global_speed_robot){
    speed_setting = global_speed_robot;
  }
  else{
    speed_setting = local_speed_set;
  }
  int current_level  = robot_drive_pattern_counter;

  robot_drive_pattern[current_level][0] = round(floor((id/10)));
  robot_drive_pattern[current_level][6] = id;
  // Serial.print("inside function block id = ");
  //Serial.println(robot_drive_pattern[current_level][6]);
  switch  (robot_drive_pattern[current_level][0]){
  case speed_function:
    {
      global_speed_robot = round(map(pot_value, begin_knob, end_knob, minimum_speed, maximum_speed));//  speed block needs to be set global still
      robot_drive_pattern[current_level][0] = 0;// if last block is an speed block be sure it is zero
      robot_drive_pattern_counter --;// overwrite this part of the pattern      
      break;
    }
  case driving_forwards_backwards:
    {
      //drive forward/backward

      if(pot_value < middle_knob){

        //drive forward
        int time_to_drive_forward = round(map(pot_value,begin_knob,middle_knob,max_time,min_time));
        //Serial.print("There was a forward block that should drive ");
        // Serial.print(time_to_drive_forward);
        // Serial.println(" Miliseconds");
        // both motors should go forward
        robot_drive_pattern[current_level][1] = 1;
        robot_drive_pattern[current_level][2] = 1;
        robot_drive_pattern[current_level][3] = time_to_drive_forward;
        robot_drive_pattern[current_level][4] = speed_setting;// left motor
        robot_drive_pattern[current_level][5] = speed_setting;// right motor
      }
      else{

        //drive backward
        // both motors should go backward
        int time_to_drive_backward = round(map(pot_value,middle_knob,end_knob,min_time,max_time));
        //  Serial.print("There was a backward block that should drive ");
        //  Serial.print(time_to_drive_backward);
        //  Serial.println(" Miliseconds");
        robot_drive_pattern[current_level][1] = 0;
        robot_drive_pattern[current_level][2] = 0;
        robot_drive_pattern[current_level][3] = time_to_drive_backward;
        robot_drive_pattern[current_level][4] = speed_setting;// left motor
        robot_drive_pattern[current_level][5] = speed_setting;// right motor
      }
      break;
    }
  case driving_left_rigt:
    {
      if(pot_value < middle_knob){
        // drive left
        int time_to_drive_left = round(map(pot_value,begin_knob,middle_knob,max_time_turning,min_time_turning));
        robot_drive_pattern[current_level][1] = 1;// left wheel backwards
        robot_drive_pattern[current_level][2] = 0;// right wheel forwards
        robot_drive_pattern[current_level][3] = time_to_drive_left;
        robot_drive_pattern[current_level][4] = speed_setting;// left motor
        robot_drive_pattern[current_level][5] = speed_setting;// right motor

      }
      else{
        // drive right
        int time_to_drive_right = round(map(pot_value,middle_knob,end_knob,min_time_turning,max_time_turning));
        robot_drive_pattern[current_level][1] = 0;//left wheel forwards
        robot_drive_pattern[current_level][2] = 1;// right wheel backwards
        robot_drive_pattern[current_level][3] = time_to_drive_right;
        robot_drive_pattern[current_level][4] = speed_setting;// left motor
        robot_drive_pattern[current_level][5] = speed_setting;// right motor
      }

      break;
    }
  case delay_function:
    {
      int delay_time = round(map(pot_value,begin_knob,end_knob,min_time,max_time));
      robot_drive_pattern[current_level][3] = delay_time;
      robot_drive_pattern[current_level][4] = 0;// left motor set motor speed to 0 so the motor will actualy do nothing
      robot_drive_pattern[current_level][5] = 0;// right motor

      // delay
      break;
    }
  case loop_a:
    {
      // loop A:
      if (pot_value == 0){
        // we have a loop_a block without a nr, check if we are the last block in the loop or not
        for(int i  = current_level+1; i < rows; i++ ){
          int current_id = id_array [i][0];
          if(current_id != 0){
            // as long as there are blocks check if there is another loop_a block
            int current_id_section = round(floor((current_id/10)));

            if(current_id_section == loop_a){
              // we where not the last block in the loop so reverse the potvalues
              pot_value = round(map(pot_array [i][0],-90,end_knob,0,8)) ;
              // store id value in driving pattern for light up both loop blocks
              robot_drive_pattern[current_level][1] = pot_value;
              robot_drive_pattern[current_level][2] = current_id;
              pot_array[i][0] = 0;
              break;
            }
          }
          else{
            robot_drive_pattern[current_level][1] = 0;// 0 on this place means end of loop

            // begin of loop a was already in the code so save it as a 0
            break;
          }

        }
        // this for loop searches down for the next value, if there is nothing to find its the closing part of the repeat
      }
      else if (pot_value != 0){
        int repeat_x_times = round(map(pot_value,-90,end_knob,0,8));
        // store id value in driving pattern for light up both loop blocks
        for(int i  = current_level+1; i < rows; i++ ){
          int current_id = id_array [i][0];
          if(current_id != 0){
            int current_id_section = round(floor((current_id/10)));
            if(current_id_section == loop_a){
              robot_drive_pattern[current_level][2] = current_id;
              break;
            }
          }
          else{
          //  Serial.println("Error no other loop block found");
            break;
          }
        }

        robot_drive_pattern[current_level][1] = repeat_x_times;
      }

      break;
    }
  case loop_b:
    {
      // loop B:
      if (pot_value == 0){
        // we have a loop_a block without a nr, check if we are the last block in the loop or not
        for(int i  = current_level+1; i < rows; i++ ){
          int current_id = id_array [i][0];
          if(current_id != 0){
            // as long as there are blocks check if there is another loop_a block
            int current_id_section = round(floor((current_id/10)));
            if(current_id_section == loop_b){
              
              pot_value = round(map(pot_array [i][0],-80,end_knob,1,9)) ;
              robot_drive_pattern[current_level][1] = pot_value;
              robot_drive_pattern[current_level][2] = current_id;
              pot_array[i][0] = 0;
              break;
            }
          }
          else{
            robot_drive_pattern[current_level][1] = 0;// 0 on this place means end of loop
            // begin of loop a was already in the code so save it as a 0
            break;
          }
        }
        // this for loop searches down for the next value, if there is nothing to find its the closing part of the repeat
      }
      else if (pot_value != 0){
        int repeat_x_times = round(map(pot_value,-80,end_knob,1,9));
        // store id value in driving pattern for light up both loop blocks
        for(int i  = current_level+1; i < rows; i++ ){
          int current_id = id_array [i][0];
          if(current_id != 0){
            int current_id_section = round(floor((current_id/10)));
            if(current_id_section == loop_b){
              robot_drive_pattern[current_level][2] = current_id;
              break;
            }
          }
          else{
         //  Serial.println("Error no other loop block found");
            break;
          }
        }
        robot_drive_pattern[current_level][1] = repeat_x_times;
      }

      break;

    }
  default:
    {
      //   Serial.println("error in get function block by id");
      break;
    }
  }
  robot_drive_pattern_counter++;// next level will be saved on new line

}








