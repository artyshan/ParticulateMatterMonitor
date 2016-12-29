

#include <SoftwareSerial.h>

#define DEBUG true

SoftwareSerial A7Serial(8, 9); //RX, TX

/*
 * Default baud rates for serial ports
 */

const int defaultA7BaudRate 	= 115200;
const int defaultDebugBaudRate 	= 9600;

/*
 * Remote addres and port
 */

const String 	remoteAddress	= "data.sparkfun.com";
const int 		remotePort		= 80;

/* 
 * Access Point Name for the Internet
 */

const String AccessPointName	= "internet";

String response = "";


// the setup routine runs once when you press reset:

void setup() 
{   
	
	Serial.begin(defaultDebugBaudRate);

	/*
	
	A7Serial.begin(defaultA7BaudRate);
	
	// Setting A7 to work on 9600 bps
	
	response = sendAtCommand("AT+IPR=9600", 1000);
	
	A7Serial.end();

	*/
	
	A7Serial.begin(9600);
	A7Serial.setTimeout(2000);
	
	delay(1000);
	
	// Disabling command echo
	
	response = sendAtCommand("ATE0", 500);
	if(DEBUG)
	{
		Serial.println("A7 response: " + response);
	}
	
	// Setting errors to verbose mode
	
	response = sendAtCommand("AT+CMEE=2", 500);
	if(DEBUG)
	{
		Serial.println("A7 response: " + response);
	}
	
	// Testing connection with A7 module
	
	response = sendAtCommand("AT", 200);
	if(DEBUG)
	{
		Serial.println("A7 response: " + response);
	}
	
	// Connecting to packet Domain service
	
	response = sendAtCommand("AT+CGATT=1", 1000);
	if(DEBUG)
	{
		Serial.println("A7 response: " + response);
	}
	
	// Defining PDP context
	
	response = sendAtCommand("AT+CGDCONT=1,\"IP\",\"" + AccessPointName + "\"", 1000);
	if(DEBUG)
	{
		Serial.println("A7 response: " + response);
	}
	
	// DEBUG
	
	response = sendAtCommand("AT+CIPSTATUS", 500);
	if(DEBUG)
	{
		Serial.println("A7 response: " + response);
	}
	
	// Activating PDP context
	
	response = sendAtCommand("AT+CGACT=1,1", 1000);
	if(DEBUG)
	{
		Serial.println("A7 response: " + response);
	}
	
	// DEBUG
	
	response = sendAtCommand("AT+CIPSTATUS", 500);
	if(DEBUG)
	{
		Serial.println("A7 response: " + response);
	}
	
	// Begin TCP/IP connection
	
	response = sendAtCommand("AT+CIPSTART=\"TCP\",\"" + remoteAddress + "\"," + remotePort , 8000);
	if(DEBUG)
	{
		Serial.println("A7 response: " + response);
	}
	
	// DEBUG
	
	response = sendAtCommand("AT+CIPSTATUS", 500);
	if(DEBUG)
	{
		Serial.println("A7 response: " + response);
	}
	
	
	/*
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
	*/

	
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

String sendAtCommand(String command, int delayTime)
{
	flushA7Serial();
	
	if(DEBUG)
	{
		Serial.println("Sending AT command: " + command);	
	}
	
	A7Serial.print(command);
	A7Serial.write(13);
	delay(delayTime);
	return A7Serial.readString();
	
}

// the loop routine runs over and over again forever:
void loop() 
{
	
	
}


