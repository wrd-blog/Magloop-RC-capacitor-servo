#ifndef cap_driver_h
#define cap_driver_h

// Header file for pwmServo-1.ino

// cap_driver-2.01a.h
// PWM servo version
// Last revision 10/31/22 by WrD

#include <EEPROM.h>
#include <LiquidCrystal_I2C_Hangul.h>
#include <TimerOne.h>

#define hi 1
#define lo 0

LiquidCrystal_I2C_Hangul lcd(0x27,16,2);      // instantiate the LCD library

// Define the pins:
int capServoPin = 3;
int upButton = 8;           // Pushbutton to run the capacitor toward higher capacitance
int dnButton = 7;           // Pushbutton to run the capacitor toward lower capacitance
int homeButton = 2;         // Pushbutton to force homing of the capacitor
int savePosButton = 1;      // Pushbutton to save the current capacitor position to EEPROM
int pwrSwitch = 10;         // output to drive the base of the servo power switch transistor
int spd = A0;               // analog input from speed pot
int setPosButton = 12;		// Allows setting the servo position using the speed pot
int slowSlewButton = 9;		// Slows the slew rate for fine positioning


// variables
float servoPos = 0;
int speed = 0;
bool runUp = false;
bool runDn = false;
bool setPos = false;
bool savePos = false;
int shaftPos = 1;           // EEPROM address of the saved servo position
String posStr = "0";        // String version of servoPos for display
int min = 500;
int max = 2400;
int newPos = 0;
float scale = 0;
bool ready = false;
bool dispPos = false;
volatile bool tFlag = false;
bool slowSlew = false;

// Misc function declarations

void incrementCap(void){
	if(servoPos < max){
		if(slowSlew){
			servoPos += 1;					
		}
	else {
		servoPos += scale;
		}
	}
}

void decrementCap(void){
	if(servoPos > min){
		if(slowSlew){
			servoPos -= 1;					
		}
		else {
		servoPos -= scale;
		}
   }
}

// EEPROM functions

int memRead(word loc){
  word retval = 0;
  retval = EEPROM.read(loc);
  delay(50);
  retval += EEPROM.read(loc+1) << 8;
  delay(50);  
  Serial.println("Retrieved servoPos = "+String(retval));
  return retval;
}

void memWrt(int loc, int data){
  byte lowData = data;
  byte highData = data >> 8;
  EEPROM.write(loc, lowData);
  delay(50);
  EEPROM.write(loc+1, highData);
  delay(50);
}

// function to rotate the cap to the low capacitance 'home' limit
void homeCap(void){
    servoPos = min;
}

// Displays the selected position while the setPos button is pressed
void displayNewPos(String selPos){        
  lcd.setCursor(0,1);
    lcd.print("                   ");
    lcd.setCursor(5,1);
    lcd.print("Select Pos");
    lcd.setCursor(0,1);
    lcd.print("    ");
    lcd.setCursor(0,1);
    lcd.print(selPos);
    ready = false;
    dispPos = true;
    }

void pulse(int pos){           // generates servo pulse
  noInterrupts();
  digitalWrite(capServoPin, HIGH);
  delayMicroseconds(int(servoPos));
  digitalWrite(capServoPin, LOW);
  interrupts();
  tFlag = false;
}

void setTFlag(void){           // Timer1 controls the pulse spacing of approx 50Hz
  tFlag = true;                // tFlag == true allows writeServo() to generate a pulse
}

#endif
	
