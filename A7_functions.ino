/*
 * ------------------------- A7 related functions ---------------------------
 */

void A7_flush()
{
	while(A7Serial.available())
	{
		A7Serial.read();
	}
}

/*
 * Sending single AT command to A7 module
 * Command given as a parameter shall not be ended with any special character
 * <CR> character is appended to command
 * Returns a string containing full response from A7 module (with all special characters such as CR and LF)
 * A7Serial buffer is flushed each time before sending a command
 */

String A7_sendCommand(String command, int delayTime)
{
	A7_flush();
	
	if(DEBUG)
	{
		Serial.println("Sending AT command: " + command);	
	}
	
	A7Serial.print(command);
	A7Serial.write(13);
	delay(delayTime);
	//return A7Serial.readString();
	return "";
	
}

void A7_connectToNetwork()
{
	
}

String A7_getLine()
{
	String result = String("");
	char currentCharacter = 0;
		
	// Skipping special characters at the beginning 
		
	while(A7Serial.available())
	{
		currentCharacter = A7Serial.read();
		if(currentCharacter == 10 || currentCharacter == 13)
		{
			continue;	
		}
		else
		{
			result += currentCharacter;
			break;
		}
	}
		
	// Reading characters until <CR>
		
	while(A7Serial.available())
	{
		currentCharacter = A7Serial.read();
		if(currentCharacter == 13)
		{
			break;
		}
		else
		{
			result += currentCharacter;	
		}
	}
	
	return result;	
}