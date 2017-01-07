/*
 * ------------------------- PMS7003 related functions ---------------------------
 */

/*
 * Flushes PMS7003Serial
 */

void PMS7003_flush()
{
	while(PMS7003Serial.available())
	{
		PMS7003Serial.read();
	}
}

/*
 * Initializes connection with PMS7003
 */

void PMS7003_init()
{
	PMS7003Serial.begin(PMS7003_defaultBaudRate);
	
	// Setting PMS7003 to passive mode
	
	PMS7003Serial.write(PMS7003_COMMAND_PASSIVE, 7);
}

/*
 * Reads two bytes from PMS7003Serial as uint16_t value
 */

uint16_t PMS7003_readUint16()
{	
	uint8_t highByte 	= PMS7003Serial.read();
	uint8_t lowByte		= PMS7003Serial.read();
	
	return ((highByte << 8) | lowByte);
}

/*
 * Reads a single frame of data from PMS7003Serial
 * Reads bytes from serial until the frame header (0x42, 0x4d) is found
 * If no frame header is found and there is no byte to read, the function returns with nothing
 * Assumed that the only valid frame length is 28
 * Returns 
 */

int PMS7003_readFrame(struct PMS7003_frameStruct * PMS7003_frameStructPointer)
{
	uint8_t frameHeader [2] = {0, 0};
		
	// Searching for the frame header
	
	while(frameHeader[1] != 77 && PMS7003Serial.available())
	{
		while(frameHeader[0] != 66 && PMS7003Serial.available())
		{
			frameHeader[0] = PMS7003Serial.read();
		}
		frameHeader[1] = PMS7003Serial.read();
	}
	
	// If there are no bytes in the serial buffer, this means that no frame 
	// has been found and the function returns with nothing
	
	if(!PMS7003Serial.available())
	{
		return PMS7003_FRAME_NOTFOUND;
	}
	
	// Checking the frame length
	// Assuming the only valid length is 28 
	
	uint16_t frameLength = PMS7003_readUint16();
	
	if(frameLength != 28)
	{
		return PMS7003_FRAME_INVALID;
	}
	
	// Else the values are read to structure

	PMS7003_frameStructPointer->PM1_0_CF1_concentration 	= PMS7003_readUint16();
	PMS7003_frameStructPointer->PM2_5_CF1_concentration 	= PMS7003_readUint16();
	PMS7003_frameStructPointer->PM10_0_CF1_concentration 	= PMS7003_readUint16();
	
	PMS7003_frameStructPointer->PM1_0_atmospheric_concentration 	= PMS7003_readUint16();
	PMS7003_frameStructPointer->PM2_5_atmospheric_concentration 	= PMS7003_readUint16();
	PMS7003_frameStructPointer->PM10_0_atmospheric_concentration 	= PMS7003_readUint16();
	
	PMS7003_frameStructPointer->beyond_0_3_number 	= PMS7003_readUint16();
	PMS7003_frameStructPointer->beyond_0_5_number 	= PMS7003_readUint16();
	PMS7003_frameStructPointer->beyond_1_0_number 	= PMS7003_readUint16();
	PMS7003_frameStructPointer->beyond_2_5_number 	= PMS7003_readUint16();
	PMS7003_frameStructPointer->beyond_5_0_number 	= PMS7003_readUint16();
	PMS7003_frameStructPointer->beyond_10_0_number 	= PMS7003_readUint16();

	PMS7003_frameStructPointer->reserved = PMS7003_readUint16();
	PMS7003_frameStructPointer->checkSum = PMS7003_readUint16();

	
	// Checking validity with checkSum
	
	return PMS7003_checkFrame(PMS7003_frameStructPointer);
}

/*
 * Checks validity of PMS7003 frame 
 * Adds up bytes of all fields except for check sum
 * For more details see Appendix A of PMS7003 datasheet
 */

int PMS7003_checkFrame(struct PMS7003_frameStruct * PMS7003_frameStructPointer)
{
	
	uint16_t sum = 0x42 + 0x4d + 28 +
		sumOfBytes(PMS7003_frameStructPointer->PM1_0_CF1_concentration) + 
		sumOfBytes(PMS7003_frameStructPointer->PM2_5_CF1_concentration) + 
		sumOfBytes(PMS7003_frameStructPointer->PM10_0_CF1_concentration) + 
		sumOfBytes(PMS7003_frameStructPointer->PM1_0_atmospheric_concentration) +
		sumOfBytes(PMS7003_frameStructPointer->PM2_5_atmospheric_concentration) +
		sumOfBytes(PMS7003_frameStructPointer->PM10_0_atmospheric_concentration) +
		sumOfBytes(PMS7003_frameStructPointer->beyond_0_3_number) +
		sumOfBytes(PMS7003_frameStructPointer->beyond_0_5_number) +  
		sumOfBytes(PMS7003_frameStructPointer->beyond_1_0_number) +
		sumOfBytes(PMS7003_frameStructPointer->beyond_2_5_number) +
		sumOfBytes(PMS7003_frameStructPointer->beyond_5_0_number) +
		sumOfBytes(PMS7003_frameStructPointer->beyond_10_0_number) +
		sumOfBytes(PMS7003_frameStructPointer->reserved);
	
	if(sum == PMS7003_frameStructPointer->checkSum)
	{
		return PMS7003_FRAME_OK;	
	}
	else
	{
		return PMS7003_FRAME_INVALID;	
	}
}

/*
 * Function returning sum of bytes of two-byte value
 */

uint16_t sumOfBytes(uint16_t data)
{
	return (data & 0xFF) + (data >> 8);	
}