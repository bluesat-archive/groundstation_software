'''
	GSClient.py - Written by Brian Luc - 21st September 2015
	Ground Control Software used to control Sattelite from the ground
	Software ported from Java since better functionality in Python such as:
		* Better implementation of Serial
		* Better implementation of SDR
	Parses in data and sends them to satellite
	TO DO:
		* Server capabilities, perhaps with scheduler
	WOULD BE NICE:
		* Interactive GUI/console which accepts commands and executes stuff
		* Add SDR capabilities
'''

# Modules Imports
import datetime
from datetime import datetime
import time
import sys
import serial
import Queue

# Global variables
q = Queue.Queue()

# Classes
# Coordinate class to store coordinate objects
class Coordinates:
	def __init__(self, time, azi, ele, ran):
		self.UTCTime = time;
		self.Azimuth = azi;
		self.Elevation = ele;
		self.Range = ran;

	def getTime(self):
		return self.UTCTime

	def getAzimuth(self):
		return self.Azimuth

	def getElevation(self):
		return self.Elevation

	def getRange(self):
		return self.Range
	
	# Debug coordinates
	def printCoordinates(self):
		print("Time:\t" + str(self.UTCTime))
		print("Azimuth:\t" + str(self.Azimuth))
		print("Elevation:\t" + str(self.Elevation))
		print("Range:\t" + str(self.Range))  
		print("-------------------------------------------------------------------")

# Functions
# reads the file for coordinate objects and stores them into a queue for processing	
def readFile(arg):
	fo = open(arg, "r")
	split = "\t";
	# reads each line and finds time, azimuth, elevation and range values
	for line in fo:
		a = line.split(split)
		# checks if fields are the right format (cheapest option, but unreliable, reliable would be regexp checks)
		if len(a[0]) > 18:
			time = a[0]
			azi = a[1]
			ele = a[2]
			ran = a[3]
			# Needs security checks so that illegal values cannot be placed inside
			c = Coordinates(time, azi, ele, ran)
			q.put(c)
	fo.close() 

def main():
	if len(sys.argv) < 2:
		print "ERROR: Input Unavailable\t USAGE: python GSClientTest.py [input file]"
	else:
		readFile(str(sys.argv[1]))
		while not q.empty():
			c = q.get()
			if(len(c.getAzimuth())>2):
				azi = c.getAzimuth()
			elif((len(c.getAzimuth())>1)):
				azi = "0"+c.getAzimuth()
			else:
				azi = "00"+c.getAzimuth()
			if(len(c.getElevation())>2):
				ele = c.getElevation()
			elif((len(c.getElevation())>1)):
				azi = "0"+c.getElevation()			
			else:			
				ele = "00"+c.getElevation()
			t = c.getTime()
			tDate = datetime.strptime(t, "%d-%m-%Y %H:%M:%S")
			ser = serial.Serial('/dev/ttyACM0',9600)
			# Continually polls to check if current time is equal to time in coordinate
			while True:		
				currDate = datetime.utcnow()
				currTime = currDate.strftime("%d-%m-%Y %H:%M:%S")
				# If current time is equal to time in coordinate, send coordinates through serial
				if t == currTime:
					
					coor = 	"W"+azi+" "+ezi					
					ser.write(coor)
					print("Sending "+coor)
					break
				# Otherwise sleep for a second before checking again
				else:
					# Checks if time in coordinate is out of date, if so remove coordinate from queue
					if currDate > tDate:
						print("Time is out of date")
						break
					print currTime
					time.sleep(1)
			ser.close()

# Python thing to check for main functions and execute them
if __name__ == '__main__':
	main()
