#include <SoftwareSerial.h>

#define DEBUG true

SoftwareSerial A7Serial(8, 9); //RX, TX

String response = "";

int defaultA7BaudRate 		= 115200;
int defaultDebugBaudRate 	= 9600;


// the setup routine runs once when you press reset:

void setup() 
{   
	pinMode(13, OUTPUT);
	
	Serial.begin(defaultDebugBaudRate);
	A7Serial.begin(defaultA7BaudRate);
	
	// Setting A7 to work on 9600 bps
	
	response = sendAtCommand("AT+IPR=9600");
	
	if(DEBUG)
	{
		Serial.println("A7 response: " + response);
	}
	
	A7Serial.end();
	A7Serial.begin(9600);
	A7Serial.setTimeout(2000);
	
	delay(1000);
	
	// Disabling command echo
	
	response = sendAtCommand("ATE0");
	if(DEBUG)
	{
		Serial.println("A7 response: " + response);
	}
	
	// Testing connection with A7 module
	
	response = sendAtCommand("AT");
	if(DEBUG)
	{
		Serial.println("A7 response: " + response);
	}
	
	String test = String('\r' + '\n' + "OK" + '\r' + '\n');
	
	Serial.println("test: ");
	
	for(int i = 0; i < test.length(); i++)
	{
		Serial.println(test.charAt(i), DEC);
	}
	
	Serial.println("response: ");
	
	for(int i = 0; i < test.length(); i++)
	{
		Serial.println(test.charAt(i), DEC);
	}
	
	if(response == String('\r' + '\n' + "OK" + '\r' + '\n'))
	{
		digitalWrite(13, HIGH);
	}
	

	
}


void setA7BaudRate(int baudRate)
{
	sendAtCommand(String("AT+IPR=" + baudRate));
	delay(200);
	A7Serial.end();
}

void flushA7Serial()
{
	if(DEBUG)
	{
		Serial.println("Flushing A7Serial buffer...");	
	}
	
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

String sendAtCommand(String command)
{
	flushA7Serial();
	
	if(DEBUG)
	{
		Serial.println("Sending AT command: " + command);	
	}
	
	A7Serial.print(command);
	A7Serial.write(13);
	delay(500);
	return A7Serial.readString();
	
}

// the loop routine runs over and over again forever:
void loop() 
{
	
	
}


