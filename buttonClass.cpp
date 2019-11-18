unsigned char readButton(unsigned char _button) {
	if(!debounceTime) {
		debounceTime = 20;
		// for all buttons
		
		if(_button != statePrev) { // if a change occured
			statePrev = _button;

			if(_button)	return RISING; 
			else			return FALLING; }

		else {							// if there is no change
		
			if(_button)	return PRESSED; 
			else			return RELEASED; } } }


