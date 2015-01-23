#include <LiquidCrystal.h>
#include <MIDI.h>
#include <midi_Defs.h>
#include <midi_Message.h>
#include <midi_Namespace.h>
#include <midi_Settings.h>
#include <Button.h>


/*
 JAMES WOODY
 12JAN2015
 
 First try at MIDI floorboard will have 4 buttons, debounced via software.  Buttons will trigger actions on Laptop
 recording software (mobius) and will also control the POD by trigging scripts in mobius
 
 At this point, the below MIDI mappings are in use.  For next phase, I'll be handling the button
 logic in it's own function
 
 FLOORBOARD                         MIDI                  MAPPING
 button5, long press                 PC42                 Mobius - reset         
 button5, short press                PC40                 Mobius - Start/Stop Loop
 button6, short press                PC41                 Mobius - Next Track
 button7, long press                 n/a                  Enter Pedal Target Mode (PTM)
 button7, short press                n/a                  Cancel PTM, set's pedalActive to false
 button8, short/long press           PC 0-37              POD via Mobius - Changes presets on POD
 //button7  volume to mobius         PC43                 Mobius master volume
*/


//create 4 buttons on pin 2,3,11,12
//will activate pull-up resistor in "SETUP"
Button button5 = Button(2); //pin 2, button 5, "STOP/START LOOP-RESET", mobius, MIDI channel 1
Button button6 = Button(3); //pin 3, button 6, "ADVANCE TRACK", mobius, MIDI channel 1
Button button7 = Button(11); //pin 11, button 7, "pedalTargetMode"
Button button8 = Button(12); //pin 12, button 8, "preset change", pod, MIDI channel 1


//create an instance of MIDI - also need to call it in "SETUP"
MIDI_CREATE_DEFAULT_INSTANCE();


//create variables for POD channel
int currentPodPreset = -1;  //set to '-1' so first button calls manual.  incrementing and roll over handled in long/short press loops


/*  Delay variables  */
boolean needDelay = false;      //Flag for defaultLoopDelay, reset at end of main loop()
int defaultLoopDelay = 200;    //Oscope shows button stays low about 180ms for short press
int longPressDelay = 350;        //Try this, if need more go up to 300 _> now at 350
int resetButtonDelay = 400;    //This is for the long press on button5, to prevent numerous reads of button

/* New variables for handling button 7 and pedal targeting   */
#define pedalActiveLED 1     //pinMode is in setup()          USING D1 for built in wah LED
boolean pedalActive = false;
boolean pedalTargetPODvolume = false;
boolean pedalTargetPODeq = false;
boolean pedalTargetMobiusVolume = false;

/* Working with pedal #1 */
#define pedalOnePin A1
int pedalOneValue = 0;                //will pass this to pedal target
//analogReference(EXTERNAL); //need to set up a 2.5VDC signal for the AREF pin
//right now working on mapping to 0-127


/*  Getting display up and running.  Haven't set up A0 yet      */
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);



void setup(){

  //Ini the LCD display
  lcd.begin(16, 2);  //LCD begins 16 char x 2 rows
  lcd.setCursor(0,0);  //positions the cursor in first coloum first row
  lcd.print("FuGuE Beta"); //Write to the display
  lcd.setCursor(0,1);
  lcd.print("Woody Rules"); //Write to the display

  //Ini the "pedalActiveLED" led
  pinMode(1, OUTPUT);

  //Ini the Analog In for pedal1
  pinMode(A1, INPUT);

  //Ini the built-in LED for testing
  pinMode(13, OUTPUT);

  //Ini the 4 buttons with pullup resistors
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);

  //Launch MIDI and listen to all channels
  MIDI.begin();


}//end setup


void loop(){

  if(button5.isPressed()){


    digitalWrite(13,HIGH);
    MIDI.sendProgramChange(40,1);      //reference to script for start/stop & reset on Mobius
    needDelay = true;

  }
  else{
    digitalWrite(13,LOW);


  }//end of 5 total

  if(button6.isPressed()){


    digitalWrite(13,HIGH);
    MIDI.sendProgramChange(41,1);    //reference to script for next track on Mobius
    needDelay = true;


  }//end of 6 total
  else{
    digitalWrite(13,LOW);


  }

  if(button7.isPressed()){

        digitalWrite(13,HIGH);
        delay(longPressDelay);              //wait, then re-read  //changed longPressDelay to 350, might change back to 250 and add another new delay here
    
        if(button7.isPressed()){            //if true, we are in long press "pedal targeting mode"
                                      pedalActive = true;
                                      lcd.clear();
                                      lcd.print("PTM");
                                      digitalWrite(pedalActiveLED, HIGH);
                                      boolean pedalTargetPODvolume = false;         //clear booleans for fresh start
                                      boolean pedalTargetPODeq = false;                //clear booleans for fresh start
                                      boolean pedalTargetMobiusVolume = false;    //clear booleans for fresh start
                                
                                      while(pedalActive == true){
                                
                                        if(button5.isPressed()){                            //the code here simply picks our target
                                          pedalTargetMobiusVolume = true;
                                          pedalTargetPODvolume = false;
                                          pedalTargetPODeq = false;
                                          lcd.clear();
                                          lcd.print("Mobius Volume");
                                          delay(defaultLoopDelay); //testing
                                
                                        }
                                        if(button6.isPressed()){
                                          pedalTargetMobiusVolume = false;
                                          pedalTargetPODvolume = false;
                                          pedalTargetPODeq = true;
                                          lcd.clear();
                                          lcd.print("POD EQ");
                                          delay(defaultLoopDelay);  //testing        
                                        }
                                        if(button8.isPressed()){
                                          pedalTargetMobiusVolume = false;
                                          pedalTargetPODvolume = true;
                                          pedalTargetPODeq = false;
                                          lcd.clear();
                                          lcd.print("POD Volume");
                                          delay(defaultLoopDelay); //testing
                                        }
                                       
                                        delay(longPressDelay);
                                        if(button7.isPressed()){
                                          delay(resetButtonDelay);
                                          lcd.setCursor(0,1);
                                          lcd.print("leaving PTM");
                                          delay(500); //testing
                                          break;
                                        }                            
                                      }//end while
                                
                                      //here we are in long-press, but not in pedal target mode, here
                                      //we need to read the pedal and send the output to the targeted device
                                      //might need another while() loop here
                                      //analogeRead on A1
                                      //call funciton to send out MIDI command
                                      //might need to break out like pedal target mode
                                      //set pedalActive to false
                                      //might turn off led
                                      //break out of while
                                
                                
                                      while(pedalActive == true){
                                        lcd.clear();
                                        lcd.print("Mobius or Pod");
                                        delay(500);
                                
                                        //put if statement to assign target
                                        //TESTING
                                
                                        if(pedalTargetMobiusVolume == true){
                                          lcd.clear();
                                          lcd.print("Mobius");
                                          pedalOneValue = constrain(map(analogRead(pedalOnePin), 0, 89, 0, 127), 0, 127);    //need map and constrain
                                          lcd.print(pedalOneValue);
                                          MIDI.sendControlChange(43,pedalOneValue,1);  //prob need a function here
                                          delay(500);
                                
                                        }
                                        else  if(pedalTargetPODvolume == true){
                                          lcd.clear();
                                          lcd.print("Pod");
                                          pedalOneValue = constrain(map(analogRead(pedalOnePin), 0, 89, 0, 127), 0, 127);    //need map and constrain
                                          lcd.print(pedalOneValue);
                                          MIDI.sendControlChange(7,pedalOneValue,1);
                                          delay(500);
                                        }
                                         
                                          if(button7.isPressed()){
                                          pedalActive = false;
                                          delay(resetButtonDelay);  //not sure if this is the right delay to use or not
                                          lcd.clear();
                                          lcd.print("leave pedal");
                                          delay(700);
                                          break;
                                        }//kicks us out to long press? I could put code here to update registers (if i create them first)
                                      }//end while
                                
                                
                                      //still in long press, but not targeting
                                      //might need another break here
                                
        }//end long press
        else{                                                                          //short press start
          pedalActive = false;                                                         //we had a short press of button 7, clear pedal flag
          digitalWrite(pedalActiveLED, LOW);                                //and turn off LED                                                       
        }
    
    
    
      }
      else{                                                //we haven't pushed button 7
        digitalWrite(13,LOW);
    

  } //end of button 7 total

  /*  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   here is the old example button
  
   if(button8.isPressed()){
   digitalWrite(13,HIGH);
   MIDI.sendProgramChange(1,1);      //in Mobius do POD preset change
   //  Serial.print('Button 8 was pressed');
   }
   else{
   digitalWrite(13,LOW);
   }
   */  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////


  /*    START OF TRIAL
   the idea is to have button8 increment each time button is pressed, overflowing to zero each time
   each successive MIDI command will be bound as a trigger in Mobius to send the appropriate MIDI
   command to the POD.  Currently runs from Manual, 1A-2D, and Tuner
  
   Right now, "longPressDelay" handles the debouncing on button8, other buttons use "needDelay" flag
  
   */
  if(button8.isPressed()){
    digitalWrite(13,HIGH);
    delay(longPressDelay);

    if(button8.isPressed()){  //long pressed
      currentPodPreset = currentPodPreset - 1; //decrement due to long press
      if(currentPodPreset == -1){        //wrap back around
        currentPodPreset = 9;
      }
      //put case statement here for long press                   
      switch (currentPodPreset) {
      case 1:
        MIDI.sendProgramChange(0,1);
        break;
      case 2:
        MIDI.sendProgramChange(1,1);
        break;
      case 3:
        MIDI.sendProgramChange(2,1);
        break;
      case 4:
        MIDI.sendProgramChange(3,1);
        break;
      case 5:
        MIDI.sendProgramChange(4,1);
        break;
      case 6:
        MIDI.sendProgramChange(5,1);
        break;
      case 7:
        MIDI.sendProgramChange(6,1);
        break;
      case 8:
        MIDI.sendProgramChange(7,1);
        break;
      case 9:
        MIDI.sendProgramChange(37,1);
        break;
      }  //end of case statement for long press
      delay(longPressDelay); //delay so reversing doesn't go too fast            //this isn't working quite right


    }//end of long press
    else{                                                                                                            //short pressed
      currentPodPreset = currentPodPreset + 1; //increment counter
      if(currentPodPreset == 10){  //wrap back around
        currentPodPreset = 0;
      }

      switch (currentPodPreset) {
      case 1:
        MIDI.sendProgramChange(0,1);
        break;
      case 2:
        MIDI.sendProgramChange(1,1);
        break;
      case 3:
        MIDI.sendProgramChange(2,1);
        break;
      case 4:
        MIDI.sendProgramChange(3,1);
        break;
      case 5:
        MIDI.sendProgramChange(4,1);
        break;
      case 6:
        MIDI.sendProgramChange(5,1);
        break;
      case 7:
        MIDI.sendProgramChange(6,1);
        break;
      case 8:
        MIDI.sendProgramChange(7,1);
        break;
      case 9:
        MIDI.sendProgramChange(37,1);
        break;
      }  //end of casestatement for short press

    }//end of short press  



  }//end button 8 actions
  else{
    digitalWrite(13,LOW);

  }//end button 8 total

  /* This would be button 9, will run each time through loop
   here we use needDelay to signal a wait time of defaultLoopDelay
   and we reset the needDelay flag
  /*   We also re-read button5 for long press action
   */
  if(needDelay == true){
    delay(defaultLoopDelay);
    needDelay = false;
    if(button5.isPressed()){
      MIDI.sendProgramChange(42,1);      //map to reset mobius
      delay(resetButtonDelay);          //delay to reduce reset bouncing
    }//end of reset loop inside reset loop
  }//end of reset loop (button9 total/only)


}
