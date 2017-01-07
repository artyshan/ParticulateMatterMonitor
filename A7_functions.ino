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
 * Initializes connection with A7
 */

int A7_init()
{
	String echo 	= "";
	String response = "";
	int tryCounter 	= 0;
	
	
  	digitalWrite(3, HIGH);
  	delay(5000);
  	digitalWrite(3, LOW);
  	delay(500);
	
	A7Serial.begin(A7_defaultBaudRate);
	
	// Setting A7 to work on 9600 bps
	
	response = A7_sendCommand("AT+IPR=9600", 5000);
	
	A7Serial.end();
	
	A7Serial.begin(9600);
	A7Serial.setTimeout(2000);
	
	delay(1000);
	
	// Testing connection with A7 with AT command
	
	A7_flush();
	
	while(response != "OK" && tryCounter < 5)
	{
		A7_sendCommand("AT", 500);
		
		echo		= A7_getLine();
		response 	= A7_getLine();
		
		delay(2000);
		
		tryCounter ++;
	}
	
	if(response == "OK")
	{
		return A7_STATE_OK;
	}
	else
	{
		return A7_STATE_NOTOK;
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

/*
 * Connects to IP network 
 * PDPid - ID of a PDP context definition used to connect to network (range from 1 to 7)
 * accessPointName - APN of a network
 */

void A7_connectToNetwork(uint8_t PDPid, String accessPointName)
{
	String response = "";
	
	// Connecting to packet domain service
		
	while(response != "OK")
	{
		A7_sendCommand("AT+CGATT=1", 1000);
		response = A7_getLine();
		if(DEBUG)
		{
			Serial.println("A7 response: ");
			Serial.println(response);
			while(A7Serial.available())
			{
				Serial.println(A7_getLine());
			}
		}
	}
	
	
	// Defining PDP context
	
	response = "";
	
	while(response != "OK")
	{
		A7_sendCommand("AT+CGDCONT=" + String(PDPid) + ",\"IP\",\"" + accessPointName + "\"", 1000);
		response = A7_getLine();
		if(DEBUG)
		{
			Serial.println("A7 response: ");
			Serial.println(response);
			while(A7Serial.available())
			{
				Serial.println(A7_getLine());
			}
		}
	}
		
	// DEBUG
	
	A7_sendCommand("AT+CIPSTATUS", 500);
	if(DEBUG)
	{
		Serial.println("A7 response: ");
		while(A7Serial.available())
		{
			Serial.println(A7_getLine());
		}
	}
	
	// Activating PDP context
	
	response = "";
	while(response != "OK")
	{
		A7_sendCommand("AT+CGACT=1," + PDPid, 1000);
		response = A7_getLine();
		if(DEBUG)
		{
			Serial.println("A7 response: ");
			Serial.println(response);
			while(A7Serial.available())
			{
				Serial.println(A7_getLine());
			}
		}
	}
		
	// DEBUG
	
	response = A7_sendCommand("AT+CIPSTATUS", 500);
	if(DEBUG)
	{
		Serial.println("A7 response: ");
		while(A7Serial.available())
		{
			Serial.println(A7_getLine());
		}
	}
	
	
	
}

/*
 * Begins a TCP/IP connection with given address on a given port
 * remoteAddres - address of a remote end
 * port - port of a remote end
 */

void A7_beginConnection(String remoteAddress, int port)
{
	// Begin TCP/IP connection
	
	response = "";
	while(response != "OK" && response != "CONNECT OK")
	{
		A7_sendCommand("AT+CIPSTART=\"TCP\",\"" + remoteAddress + "\"," + remotePort , 6000);
		response = A7_getLine();
		if(DEBUG)
		{
			Serial.println("A7 response: ");
			Serial.println(response);
			while(A7Serial.available())
			{
				Serial.println(A7_getLine());
			}
		}
	}
}

/*
 * Gets a single line from A7Serial
 * Skips special characters at the beginnig (<CR>, <LF>)
 */

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