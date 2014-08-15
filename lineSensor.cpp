#include <lineSensor.h>
#include <Arduino.h>
#include <EEPROM.h>

//This function will write a 2 byte integer to the eeprom at the specified address and address + 1
void EEPROMWriteInt(int p_address, int p_value)
{
	byte lowByte = ((p_value >> 0) & 0xFF);
	byte highByte = ((p_value >> 8) & 0xFF);

	EEPROM.write(p_address, lowByte);
	EEPROM.write(p_address + 1, highByte);
}

//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
unsigned int EEPROMReadInt(int p_address)
{
	byte lowByte = EEPROM.read(p_address);
	byte highByte = EEPROM.read(p_address + 1);

	return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}

lineSensor::lineSensor()
{
	noOfSensors = 0;
	*sensorPins = NULL;
	*sensorValue = NULL;
	*calibratedMin = NULL;
	*calibratedMax = NULL;
	analog = 0;
	calibrated = 0;
}

lineSensor::lineSensor(unsigned int *_sensorPins, unsigned int _noOfSensors, unsigned int _analog, int _eepromAddress)
{
	sensorPins = (unsigned int*)malloc(sizeof(unsigned int) * _noOfSensors);
	calibratedMin = (unsigned int*)malloc(sizeof(unsigned int) * _noOfSensors);
	calibratedMax = (unsigned int*)malloc(sizeof(unsigned int) * _noOfSensors);
	sensorValue = (unsigned int*)malloc(sizeof(unsigned int) * _noOfSensors);

	for (unsigned int i = 0; i < _noOfSensors; i++)
	{
		sensorPins[i] = _sensorPins[i];
	}

	noOfSensors = _noOfSensors;
	analog = _analog;
	calibrated = 0;
	eepromAddress = _eepromAddress;

	for (unsigned int i = 0; i < _noOfSensors; i++)
	{
		calibratedMin[i] = 1023;
		calibratedMax[i] = 0;
	}
}

void lineSensor::initialise()
{
	for (unsigned int i = 0; i < noOfSensors; i++)
	{
		pinMode(sensorPins[i], INPUT);
	}
}

void lineSensor::readRawSensors()
{
	for (unsigned int i = 0; i < noOfSensors; i++)
	{
		if (analog)
		{
			sensorValue[i] = analogRead(sensorPins[i]);
			// Serial.print(sensorValue[i]);
			// Serial.print("     ");
		}
		else
		{
			sensorValue[i] = digitalRead(sensorPins[i]);
		}
	}

	// Serial.println();
}

void lineSensor::readCalibratedSensors()
{
	readRawSensors();

	readCalibrations();

	if (calibrated)
	{
		for (unsigned int i = 0; i < noOfSensors; i++)
		{
			if (sensorValue[i] < calibratedMin[i])
				sensorValue[i] = calibratedMin[i];
			else if (sensorValue[i] > calibratedMax[i])
				sensorValue[i] = calibratedMax[i];

			sensorValue[i] = map(sensorValue[i], calibratedMin[i], calibratedMax[i], 0, 100);
			Serial.print(sensorValue[i]);
			Serial.print("     ");
		}

		Serial.println();
	}
	else
	{
		for (unsigned int i = 0; i < noOfSensors; i++)
		{
			sensorValue[i] = map(sensorValue[i], 0, 1023, 0, 100);
		}
	}

}

void lineSensor::readCalibrations()
{
	if (eepromAddress != -1)
	{
		calibrated = EEPROM.read(eepromAddress + (2 * ((2 * noOfSensors) + 1)));

		if (calibrated)
			for (unsigned int i = 0; i < noOfSensors; i++)
			{
				calibratedMin[i] = EEPROMReadInt(eepromAddress + (2*i));
				calibratedMax[i] = EEPROMReadInt(eepromAddress + (2 * (noOfSensors + i + 1)));
			}
	}
}

void lineSensor::writeCalibrations()
{
	calibrated = 1;

	if (eepromAddress != -1)
	{
		for (unsigned int i = 0; i < noOfSensors; i++)
		{
			EEPROMWriteInt((eepromAddress + (2*i)), calibratedMin[i]);
			EEPROMWriteInt((eepromAddress + (2*(noOfSensors + i + 1))), calibratedMax[i]);
		}

		EEPROM.write((eepromAddress + (2*((2 * noOfSensors) + 1))), calibrated);
	}
}

void lineSensor::calibrate(unsigned int _motorLeftA, unsigned int _motorLeftB,
							unsigned int _motorRightA, unsigned int _motorRightB)
{
	if (!analog)
		return;

	// turn left motor backward
	analogWrite(_motorLeftA, 127);
	digitalWrite(_motorLeftB, HIGH);

	// turn right motor forward
	analogWrite(_motorRightA, 127);
	digitalWrite(_motorRightB, LOW);

	for (unsigned int samples = 0; samples < 10; samples++)
	{
		delay(30);
		readRawSensors();

		for (unsigned int i = 0; i < noOfSensors; i++)
		{
			if (sensorValue[i] < calibratedMin[i])
				calibratedMin[i] = sensorValue[i];

			if (sensorValue[i] > calibratedMax[i])
				calibratedMax[i] = sensorValue[i];
		}
	}

	// stop both motors
	digitalWrite(_motorLeftA, LOW);
	digitalWrite(_motorLeftB, LOW);
	digitalWrite(_motorRightA, LOW);
	digitalWrite(_motorRightB, LOW);

	delay(1000);

	// turn left motor forward
	analogWrite(_motorLeftA, 127);
	digitalWrite(_motorLeftB, LOW);

	// turn right motor backward
	analogWrite(_motorRightA, 127);
	digitalWrite(_motorRightB, HIGH);

	for (unsigned int samples = 0; samples < 17; samples++)
	{
		delay(30);
		readRawSensors();

		for (unsigned int i = 0; i < noOfSensors; i++)
		{
			if (sensorValue[i] < calibratedMin[i])
				calibratedMin[i] = sensorValue[i];

			if (sensorValue[i] > calibratedMax[i])
				calibratedMax[i] = sensorValue[i];
		}
	}

	// stop both motors
	digitalWrite(_motorLeftA, LOW);
	digitalWrite(_motorLeftB, LOW);
	digitalWrite(_motorRightA, LOW);
	digitalWrite(_motorRightB, LOW);

	delay(1000);

	// turn left motor backward
	analogWrite(_motorLeftA, 127);
	digitalWrite(_motorLeftB, HIGH);

	// turn right motor forward
	analogWrite(_motorRightA, 127);
	digitalWrite(_motorRightB, LOW);

	for (unsigned int samples = 0; samples < 10; samples++)
	{
		delay(30);
		readRawSensors();

		for (unsigned int i = 0; i < noOfSensors; i++)
		{
			if (sensorValue[i] < calibratedMin[i])
				calibratedMin[i] = sensorValue[i];

			if (sensorValue[i] > calibratedMax[i])
				calibratedMax[i] = sensorValue[i];
		}
	}

	// stop both motors
	digitalWrite(_motorLeftA, LOW);
	digitalWrite(_motorLeftB, LOW);
	digitalWrite(_motorRightA, LOW);
	digitalWrite(_motorRightB, LOW);

	delay(1000);

	writeCalibrations();
}

void lineSensor::calibrate()
{
	if (!analog)
		return;

	for (unsigned int samples = 0; samples < 30; samples++)
	{
		delay(100);
		readRawSensors();

		for (unsigned int i = 0; i < noOfSensors; i++)
		{
			if (sensorValue[i] < calibratedMin[i])
				calibratedMin[i] = sensorValue[i];

			if (sensorValue[i] > calibratedMax[i])
				calibratedMax[i] = sensorValue[i];
		}
	}

	writeCalibrations();
}

int lineSensor::readLine(unsigned char _mode)
{
	unsigned char onLine = 0;
	static int lastValue = 0;
	float average = 0, sum = 0;
	unsigned int value;

	readCalibratedSensors();

	for (int i = 0; i < noOfSensors; i++)
	{
		value = sensorValue[i];

		if (_mode == AUTO)
		{
			if ((sensorValue[0] < 25) && (sensorValue[noOfSensors - 1] < 25))
				value = 100 - value;
		}

		else if (_mode == WHITE_ON_BLACK)
			value = 100 - value;

		if (value < 60)
			onLine = 1;

		average += value * (i - ((noOfSensors - 1.00) / 2)) * 100;
		sum += value;
	}

	if (!onLine)
	{
		if (lastValue < (noOfSensors - 1) * 100 / 2)
			return -((noOfSensors - 1) * 100 / 2);

		else
			return ((noOfSensors - 1) * 100 / 2);
	}

	lastValue = (average / sum);

	return lastValue;
}
