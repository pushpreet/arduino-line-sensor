#include <Arduino.h>

#ifndef _LINE_SENSOR_H_
#define _LINE_SENSOR_H_

#define WHITE 1
#define BLACK 0

class lineSensor
{
	private:
		static unsigned int *sensorValue;
		unsigned int noOfSensors;
		unsigned int *sensorPins;
		unsigned int analog;
		unsigned int *calibratedMin;
		unsigned int *calibratedMax;
		char calibrated;
		
	public:
		// constructors
		lineSensor( )
		{
			noOfSensors = 0;
			*sensorPins = NULL;
			*sensorValue = NULL;
			*calibratedMin = NULL;
			*calibratedMax = NULL;
			analog = 0;
			calibrated = 0;
		}
		
		lineSensor( unsigned int *_sensorPins, unsigned int _noOfSensors, unsigned int _analog )
		{
			sensorPins = (unsigned int*) malloc( sizeof(unsigned int) * _noOfSensors );
			calibratedMin = (unsigned int*)malloc(sizeof(unsigned int) * _noOfSensors);
			calibratedMax = (unsigned int*)malloc(sizeof(unsigned int) * _noOfSensors);

			for( unsigned int i = 0 ; i < _noOfSensors ; i++ )
			{
				sensorPins[i] = _sensorPins[i];
			}
			
			noOfSensors = _noOfSensors;
			analog = _analog;
			calibrated = 0;

			for (unsigned int i = 0; i < noOfSensors; i++)
			{
				calibratedMin[i] = 1023;
				calibratedMax[i] = 0;
			}

		}
		
		void initialise( );
		void calibrate(unsigned int _motorLeftA, unsigned int _motorLeftB,
						unsigned int _motorRightA, unsigned int _motorRightB);
		void calibrate();
		void readRawSensors();
		void readCalibratedSensors();
		int readLine(unsigned char lineColor = BLACK);
		
};

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

void lineSensor::calibrate(unsigned int _motorLeftA, unsigned int _motorLeftB,
							unsigned int _motorRightA, unsigned int _motorRightB)
{
	if (!analog)
		return;

	calibrated = 1;

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
}

void lineSensor::calibrate( )
{
	if (!analog)
		return;

	calibrated = 1;

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
}

int lineSensor::readLine(unsigned char lineColor)
{
	unsigned char onLine = 0;
	static int lastValue = 0;
	float average = 0, sum = 0;
	unsigned int value;

	readCalibratedSensors();

	for (int i = 0; i < noOfSensors; i++)
	{
		value = sensorValue[i];

		if (lineColor == WHITE)
			value = 100 - value;
		
		if (value < 60)
			onLine = 1;

		average += value * (i - ((noOfSensors - 1.00)/2)) * 100;
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

#endif