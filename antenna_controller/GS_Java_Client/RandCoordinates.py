# RandCoordinates.py written by Brian Luc
# July 18th 2015
# Script to create random coordinates

# Modules
import random
from datetime import datetime
from datetime import timedelta
import sys

# Enquire the amount of entries to be generated
if len(sys.argv) == 1:
	noEntries = raw_input("Please enter amount of entries: ")
else:
	noEntries = str(sys.argv[1])

# Create file
fo = open("Coordinates.txt", "w")

fo.write("UTC Time\tAzimuth (degrees)\tElevation (degrees)\tRange (km)\n")

fo.close()

fo = open("Coordinates.txt", "a")
# While loop -> Generate time (greater than current time), Random Elevation, Random Azimuth, Random Range
#	     -> Then Append to file
currDate = datetime.utcnow()
for i in range(0,int(noEntries)):
	currDate = currDate + timedelta(seconds=30)
	time = currDate.strftime("%d-%m-%Y %H:%M:%S")
	azimuth = random.uniform(-178.0,178)
	elevation = random.uniform(0,178)
	r = random.uniform(0,1024)
	fo.write(time + "\t" + repr(azimuth) + "\t" + repr(elevation) + "\t" + repr(r) + "\n")

# Finalise file operations
fo.close()
