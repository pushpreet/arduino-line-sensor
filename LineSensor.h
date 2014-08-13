#include <Arduino.h>

#ifdef _MOTOR_H_
#include <Motor.h>
#endif

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
		void calibrate(motor _motorLeft, motor _motorRight);
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

void lineSensor::calibrate( motor _motorLeft, motor _motorRight )
{
	calibrated = 1;
	
	_motorLeft.setSpeed(-64);
	_motorRight.setSpeed(64);

	for (unsigned int samples = 0; samples < 10; samples++)
	{
		readRawSensors();
		
		for (unsigned int i = 0; i < noOfSensors; i++)
		{
			if (sensorValue[i] < calibratedMin[i])
				calibratedMin[i] = sensorValue[i];

			if (sensorValue[i] > calibratedMax[i])
				calibratedMax[i] = sensorValue[i];
		}
	}

	_motorLeft.stop();
	_motorRight.stop();

	delay(100);

	_motorLeft.setSpeed(64);
	_motorRight.setSpeed(-64);

	for (unsigned int samples = 0; samples < 10; samples++)
	{
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

void lineSensor::calibrate( unsigned int _motorLeftA, unsigned int _motorLeftB,
							unsigned int _motorRightA, unsigned int _motorRightB )
{
	calibrated = 1;

}


void lineSensor::calibrate( )
{
	calibrated = 1;
}

int lineSensor::readLine()
{

}

#endif