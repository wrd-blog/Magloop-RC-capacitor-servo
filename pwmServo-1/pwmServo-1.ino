// Variable capacitor driver for the magloop antenna
// This version uses a PWM, 180 degree model servo to drive the capacitor
// instead of a stepper motor
// Last revision 10/226/2022 by WrD

//
// Timer1 is used to trigger an interrupt-driven servo pulse start. The interrupt
// occurs aproximately every 20mS (50Hz rate). The servo pulse width which sets
// the servo position varies from 500 to 2400 microseconds and is stored in the
// servoPos variable. When the servo position is displayed on the LCD, servoPos
// microseconds is first converted to 0 to 180 degrees.
//
// One degree of capacitor shaft rotation is equal to 10.55 microseconds of servo
// pulse width. This factor is stored in a variable called "scale" and is equal to
// (max-min)/180.0. This var must be a float or the degrees display will 
// show an accumulating error due to rounding of the var.
//
// The speed knob controls the slew rate of the servo when the "increment" or
// "decrement" button is pressed.
//
// The "setPosButton" pushbutton in this revsion of the controller is used to select a
// servoPos (using the speed pot) to set the capacitor to any arbitrary position.
// press and hold this "set position" button while rotating the speed pot until the display
// shows the desired capacitor position (degrees). When the Set Position button is released, the 
// servo will move the capacitor to the selected angular position.
//
// The savePosButton saves the current value of servoPos to addresses 1 and 2 of the
// microcontroller's internal EEPROM. If power to the controller is interrupted, the
// servo (and the servo position display) will go to the last saved servo position.
//
// When the "home" button is pressed, servoPos is set to the minimum setting, 500uS
// which corresponds to the 0 degree (minimum capacitance) capacitor position. Also,
// the value of the variable "min" (500) is saved to EEPROM as the saved servo position.
//
// The optional "slowSlew" pushbutton, when pressed, changes the servo position display
// from degrees to the corresponding microsecond pulse width. When the increment or
// decrement button is pressed while holding the slowSlew button down, the servo position
// will change by microseconds instead of degrees. This allows for fine tuning of the
// capacitor shaft angular position.
// 
// Added a slowSlew pushbutton to slow the increment/decrement from increments of 1 degree
// to increments of 1 uS.
//
// The servo used for testing this SW was an MG996R 180 degree model with metal gears.

#include "pwmServo-1.h"		// most of the includes are in this header file


void setup() {
  Timer1.initialize(20000);				// Set timer one for 20mS spaced interrupts (50Hz pulse rate)
  Timer1.attachInterrupt(setTFlag);		// interrupt sets tFlag to true to trigger generation
										// of the next servo positioning pulse
  
  //Serial.begin(9600);  // for debug only

  pinMode(upButton, INPUT_PULLUP);
  pinMode(dnButton, INPUT_PULLUP);
  pinMode(savePosButton, INPUT_PULLUP);
  pinMode(homeButton, INPUT_PULLUP);
  pinMode(setPosButton, INPUT_PULLUP);
  pinMode(pwrSwitch, OUTPUT);
  pinMode(capServoPin, OUTPUT);         // PWM output to servo
  pinMode(slowSlewButton, INPUT_PULLUP);

// LCD display setup
lcd.init(); 
lcd.backlight();

// startup messages
lcd.setCursor(0,0);
lcd.print("Servo Controller");
lcd.setCursor(0,1);
lcd.print("SW Rev 1.0");
delay(1000);
lcd.setCursor(0,1);
lcd.print("WrD 11/04/2022");
delay(1000);
lcd.setCursor(0,1);
lcd.print("               ");

servoPos = memRead(shaftPos);	// Retrieved servo position in microseconds

if(servoPos > min){  
  lcd.setCursor(4,1);
  lcd.print("           ");
  lcd.setCursor(4,1);
  lcd.print("load preset"); 
  delay(2000) ;
} 

scale = (max-min)/180.0;		// scale must be a float
ready = false;
dispPos = true;

} // END SETUP CODE


void loop() {
  runUp = false;
  runDn = false;
  savePos = false;
  setPos = false;
  slowSlew = false;
  

  if(tFlag){
	  pulse(servoPos);
  }
  
  if(servoPos > max){
    servoPos = max;
  }

  if(servoPos < min){
    servoPos = min;
  }
  
	if(digitalRead(slowSlewButton) == LOW){
		slowSlew = true;
	}

	if(digitalRead(upButton) == LOW){
		runUp = true;
	}
	
	if(digitalRead(dnButton) == LOW){
		runDn = true; 
	}

  if(digitalRead(homeButton) == LOW){
    lcd.setCursor(0,1);
    lcd.print("                   ");
    lcd.setCursor(5,1);
    lcd.print("Run to home");
    homeCap();
    lcd.setCursor(0,1);
    lcd.print("    ");
    ready = false; 
    dispPos = true;   
    savePos = true;
  }

  while(digitalRead(setPosButton) == LOW){   // Select an arbitrary capacitor shaft position
    servoPos = int(map((analogRead(spd)),0,1023,min,max));
    posStr = String(int((servoPos-min)/scale));
    Serial.println("posStr = "+String(posStr));
    displayNewPos(posStr);
    memWrt(shaftPos, int(servoPos));
    delay(250);
    dispPos = true;
  }
 
  if(digitalRead(savePosButton) == LOW){
      savePos = true;
    }

  if(!runUp && !runDn && !setPos && !ready){  // if neither run button nor dispSpeed is pressed show "Ready"
    lcd.setCursor(4,1);
    lcd.print("            ");
    lcd.setCursor(5,1);
    lcd.print("Ready     ");
    delay(20);
    ready = true;
  }

	speed = map((analogRead(spd)),0,1023,500,0);  // translate pot setting to motor speed mS delay integer

	if(slowSlew) {
		posStr = String(int(servoPos));
	}
	else {
		posStr = String(int((servoPos-min)/scale));  // string representation of servoPos in deg for display
	}

  if(servoPos == 999){             // clear third LCD digit on down count
    lcd.setCursor(3,1);
    lcd.print("  ");
  }

  if(servoPos == 99){             // clear third LCD digit on down count
    lcd.setCursor(2,1);
    lcd.print(" ");
  }

  if(servoPos == 9){             // clear third LCD digit on down count
    lcd.setCursor(1,1);
    lcd.print("   ");
  }


  if(dispPos){
    lcd.setCursor(0,1);           // display the shaft position
    lcd.print("    ");          
    lcd.setCursor(0,1); 
    lcd.print(posStr);
    dispPos = false;			// Toggeling this var reduces display flashing 
  }

  if(savePos){                  // save the current servo position to EEPROM 
    memWrt(shaftPos, int(servoPos));
    lcd.setCursor(4,1);
    lcd.print("            ");
    lcd.setCursor(4,1);
    lcd.print("Save pos");
    delay(1000);
    ready = false;
  }

  // rotate cap 1 deg toward high capacitance
  if(runUp){
    incrementCap();
    delay(speed);
    dispPos = true;
    }

  // rotate cap 1 deg toward low capacitance
  if(runDn){
    decrementCap();
    delay(speed);
    dispPos = true;
    }

  
}   // END MAIN LOOP


