#include <Arduino.h>

#ifndef _LINE_SENSOR_H_
#define _LINE_SENSOR_H_

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
		
		// private functions
		void readRawSensors( );
		void calibrate( unsigned int _motorLeftA, unsigned int _motorLeftB,
						unsigned int _motorRightA, unsigned int _motorRightB );
		void calibrate();

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
			sensorValue = (unsigned int*) malloc( sizeof(unsigned int) * _noOfSensors );
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
		
		// public functions
		void initialise( );
		int readLine( );
		
};

void lineSensor::initialise()
{
	for (unsigned int i = 0; i < noOfSensors; i++)
	{
		pinMode(sensorPins[i], OUTPUT);
	}
}

void lineSensor::readRawSensors()
{
	for (unsigned int i = 0; i < noOfSensors; i++)
	{
		if (analog)
		{
			sensorValue[i] = analogRead(sensorPins[i]);
		}
		else
		{
			sensorValue[i] = digitalRead(sensorPins[i]);
		}
	}
}

void lineSensor::calibrate(unsigned int _motorLeftA, unsigned int _motorLeftB,
	unsigned int _motorRightA, unsigned int _motorRightB)
{
	calibrated = 1;

	// turn left motor backward
	digitalWrite(_motorLeftA, LOW);
	analogWrite(_motorLeftB, 64);

	// turn right motor forward
	analogWrite(_motorRightA, 64);
	digitalWrite(_motorRightB, LOW);

	for (unsigned int samples = 0; samples < 10; samples++)
	{
		delay(10);
		readRawSensors();

		for (unsigned int i = 0; i < noOfSensors; i++)
		{
			if (sensorValue[i] < calibratedMin[i])
				calibratedMin[i] = sensorValue[i];

			if (sensorValue[i] > calibratedMax[i])
				calibratedMax[i] = sensorValue[i];
		}
	}

	delay(100);

	// turn left motor forward
	analogWrite(_motorLeftA, 64);
	digitalWrite(_motorLeftB, LOW);

	// turn right motor backward
	digitalWrite(_motorRightA, LOW);
	analogWrite(_motorRightB, 64);

	for (unsigned int samples = 0; samples < 20; samples++)
	{
		delay(10);
		readRawSensors();

		for (unsigned int i = 0; i < noOfSensors; i++)
		{
			if (sensorValue[i] < calibratedMin[i])
				calibratedMin[i] = sensorValue[i];

			if (sensorValue[i] > calibratedMax[i])
				calibratedMax[i] = sensorValue[i];
		}
	}

	delay(100);

	// turn left motor backward
	digitalWrite(_motorLeftA, LOW);
	analogWrite(_motorLeftB, 64);

	// turn right motor forward
	analogWrite(_motorRightA, 64);
	digitalWrite(_motorRightB, LOW);

	for (unsigned int samples = 0; samples < 10; samples++)
	{
		delay(10);
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

void lineSensor::calibrate( )
{
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

int lineSensor::readLine()
{

}

#endif