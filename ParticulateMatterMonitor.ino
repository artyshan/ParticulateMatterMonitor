/*
 * Dominik Gruszkowski
 * 06.01.2017
 *
 * System for air quality monitoring on Arduino platform
 * 
 * Sensor used: 		Plantower PMS7003
 * GPRS module used:	AiThinker A7
 * Controller used:		Arduino Pro Mini 3.3 V / 8 Mhz
 *
 * PMS7003 related functions are in PMS7003_functions.ino
 * A7 related functions are in A7_functions.ino
 *
 * PMS7003 datasheet referred in this code is PMS7003 datasheet V2.2 by Zhou Yong, released on 2016-04-07 
 * A7 manual referred in this code is A6/A7/A6C/A20 AT Command Set V1.02 by Ai Thinker Technology Co.Ltd
 */

#include <SoftwareSerial.h>

#define DEBUG true

/*
 * --------------------------------------------------------------------------------------------------------
 *
 * PMS7003 related definitions - cannot be exported to other file since
 * Arduino IDE starts from loading this one
 * For more details regarding PMS7003 commands see Appendix B of PMS7003 datasheet
 * For mode details regarding PMS7003 frame structure see Appendix A of PMS7003 datasheet
 */

const int PMS7003_defaultBaudRate 	= 9600;

const int PMS7003_FRAME_OK 			= 0;
const int PMS7003_FRAME_INVALID 	= 1;
const int PMS7003_FRAME_NOTFOUND 	= 2;

const int PMS7003_STATE_OK			= 0;
const int PMS7003_STATE_NOTOK		= 1;

const uint8_t PMS7003_COMMAND_READ 		[7] = {0x42, 0x4d, 0xe2, 0x00, 0x00, 0x01, 0x71};
const uint8_t PMS7003_COMMAND_PASSIVE 	[7] = {0x42, 0x4d, 0xe1, 0x00, 0x00, 0x01, 0x70};

/*
 * PMS7003 frame structure
 */

struct PMS7003_frameStruct {

	// Concentration of particle matter (PM1.0, PM2.5, PM10) 
	// The unit is micrograms per cubic meter 
	// CF=1, for factory environment 
	
    uint16_t PM1_0_CF1_concentration;
    uint16_t PM2_5_CF1_concentration;
    uint16_t PM10_0_CF1_concentration;
	
	// Concentration of particle matter (PM1.0, PM2.5, PM10) 
	// The unit is micrograms per cubic meter 
	// For atmospheric environment 
	
    uint16_t PM1_0_atmospheric_concentration;
    uint16_t PM2_5_atmospheric_concentration;
    uint16_t PM10_0_atmospheric_concentration;
	
	// Numbers of particles with diameter beyond 0.3, 0.5, 1.0, 2.5, 5.0, 10.0 micrometers
	// The unit is number of particles per 0.1 L of air
	
    uint16_t beyond_0_3_number;
    uint16_t beyond_0_5_number;
    uint16_t beyond_1_0_number;
	uint16_t beyond_2_5_number;
	uint16_t beyond_5_0_number;
	uint16_t beyond_10_0_number;

	// Reserved bytes
	
    uint16_t  reserved;	

	// Checksum
	
    uint16_t checkSum;
};

struct PMS7003_frameStruct * framePointer;

/*
 * --------------------------------------------------------------------------------------------------------
 *
 * A7 related definitions - cannot be exported to other file since
 * Arduino IDE starts from loading this one
 * 
 */

const int A7_defaultBaudRate 	= 115200;

const int A7_STATE_NOTOK	= 0;
const int A7_STATE_OK		= 1;

/*
 * Software Serials for A7 and PMS7003
 */

SoftwareSerial PMS7003Serial(6, 7); //RX, TX
SoftwareSerial A7Serial(8, 9); 		//RX, TX


/*
 * Default baud rates for serial ports
 */


const int defaultDebugBaudRate 		= 9600;
;

/*
 * Remote address and port
 */

const String 	remoteAddress	= "data.sparkfun.com";
const int 		remotePort		= 80;

/* 
 * Access Point Name for the Internet
 */

const String AccessPointName	= "internet";

String response = "";

int A7state = A7_STATE_NOTOK;




/*
 * --------------------------------- SETUP and LOOP routines -------------------------------------
 */


/*
 * The setup() routine - called one after booting or resetting Arduino
 * Opens all needed serial ports
 * Connects to the given server via GPRS
 */

void setup() 
{   
	
	Serial.begin(defaultDebugBaudRate);
	pinMode(3, OUTPUT);
	
	
	framePointer = (struct PMS7003_frameStruct*)malloc(sizeof(struct PMS7003_frameStruct));
	
	PMS7003_init();
	
	while(A7state != A7_STATE_OK)
	{
		A7state = A7_init();
	}
	
		
	// Disabling command echo
	
	while(response != "OK")
	{
		A7_sendCommand("ATE0", 500);
		response = A7_getLine();
		if(DEBUG)
		{
			Serial.println("A7 response: ");
			Serial.println(response);
		}
	}
	
	
	
	/*
	// Setting errors to verbose mode
	
	A7_sendCommand("AT+CMEE=2", 500);
	if(DEBUG)
	{
		Serial.println("A7 response: " + A7_getLine());
	}
	*/
	
	A7_connectToNetwork(1, AccessPointName);
	
	
	
	
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
	
	/*
	A7_sendCommand("AT+CIPSEND", 1000);
	if(DEBUG)
	{
		Serial.println("A7 response: " + response);
	}
	*/


	
}



/*
 * The loop() routine - goes over and over 
 */

void loop() 
{
	PMS7003_flush();
	
	PMS7003Serial.write(PMS7003_COMMAND_READ, 7);
	
	delay(200);
	
	if(PMS7003Serial.available())
	{
		Serial.println("-------------------------");
		
		int errorCode = PMS7003_readFrame(framePointer);
		
		if(errorCode == PMS7003_FRAME_OK)
		{
			Serial.print("PM 1.0 atm:		");
			Serial.print(framePointer->PM1_0_atmospheric_concentration);
			Serial.println(" ug/m^3");
			Serial.print("PM 2.5 atm:		");
			Serial.print(framePointer->PM2_5_atmospheric_concentration);
			Serial.println(" ug/m^3");
			Serial.print("PM 10.0 atm:		");
			Serial.print(framePointer->PM10_0_atmospheric_concentration);
			Serial.println(" ug/m^3");
		}
		else
		{
			Serial.println("Frame invalid or missing...");
		}
	}
	
	
	delay(500);
	
}


