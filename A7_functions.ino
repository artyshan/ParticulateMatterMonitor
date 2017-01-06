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