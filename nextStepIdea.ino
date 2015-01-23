loop{
	readButtons()
	readPedals()
	sendCode()
	updateLogic()
	updateLCD()
}

void readButtons{				//offload logic, only read sequencially
	for(i=0,i>4,i++){
		if(button().isPressed()){
			activeButton = i
		}

	}
}

void readPedals{
	if(pedalActive == true){		//make a way for by-passing
	}
	else{
	}
}
