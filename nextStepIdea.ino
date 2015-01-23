/*    This will be the working code for next phase.  Code should be compilable each time saved.
       01/23/2015        BareMinimum, modified w/ functions to handle basic things
*/

void setup() {
}

void loop() {
	readButtons();
	readPedals();
	sendCode();
	updateLogic();
	updateLCD();
}

void readButtons() {				//offload logic, only read sequencially
/*	for(i=0,i>4,i++){
		if(button().isPressed()){
			activeButton = i
		}

	} */
}

void readPedals() {
        boolean pedalActive;
        
  	if(pedalActive == true){		//make a way for by-passing
	}
	else{
	}
}

void sendCode() {
}

void updateLogic() {
}

void updateLCD() {
}
