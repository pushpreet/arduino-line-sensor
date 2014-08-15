#include <Arduino.h>

#ifndef _LINE_SENSOR_H_
#define _LINE_SENSOR_H_

#define BLACK_ON_WHITE	0
#define WHITE_ON_BLACK	1
#define AUTO			2

class lineSensor
{
	private:
		unsigned int *sensorValue;
		unsigned int noOfSensors;
		unsigned int *sensorPins;
		unsigned int analog;
		unsigned int *calibratedMin;
		unsigned int *calibratedMax;
		char calibrated;
		int eepromAddress;

		void readCalibrations();
		void writeCalibrations();
		
	public:
		// constructors
		lineSensor();
		
		lineSensor(unsigned int *_sensorPins, unsigned int _noOfSensors, unsigned int _analog, int _eepromAddress = -1);
		
		void initialise( );
		void calibrate(unsigned int _motorLeftA, unsigned int _motorLeftB,
						unsigned int _motorRightA, unsigned int _motorRightB);
		void calibrate();
		void readRawSensors();
		void readCalibratedSensors();
		int readLine(unsigned char _mode = 0);
		
};

#endif