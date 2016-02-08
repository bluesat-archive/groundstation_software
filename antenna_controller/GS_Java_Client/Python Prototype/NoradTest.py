'''
NoradTest.py by Brian Luc
Saturday 17th October 2015
Program used to create Norad Coordinate objects to comply with current design spec of Groundstation

NORAD Two-Line Element Set Format, example in NoradTestCoordinates.txt

AAAAAAAAAAAAAAAAAAAAAAAA (24 characters, is name)
1 NNNNNU NNNNNAAA NNNNN.NNNNNNNN +.NNNNNNNN +NNNNN-N +NNNNN-N N NNNNN
2 NNNNN NNN.NNNN NNN.NNNN NNNNNNN NNN.NNNN NNN.NNNN NN.NNNNNNNNNNNNNN

Line 1
Column 	Description
01 	Line Number of Element Data
03-07 	Satellite Number
08 	Classification (U=Unclassified)
10-11 	International Designator (Last two digits of launch year)
12-14 	International Designator (Launch number of the year)
15-17 	International Designator (Piece of the launch)
19-20 	Epoch Year (Last two digits of year)
21-32 	Epoch (Day of the year and fractional portion of the day)
34-43 	First Time Derivative of the Mean Motion
45-52 	Second Time Derivative of Mean Motion (decimal point assumed)
54-61 	BSTAR drag term (decimal point assumed)
63 	Ephemeris type
65-68 	Element number
69 	Checksum (Modulo 10)
(Letters, blanks, periods, plus signs = 0; minus signs = 1)

Line 2
Column 	Description
01 	Line Number of Element Data
03-07 	Satellite Number
09-16 	Inclination [Degrees]
18-25 	Right Ascension of the Ascending Node [Degrees]
27-33 	Eccentricity (decimal point assumed)
35-42 	Argument of Perigee [Degrees]
44-51 	Mean Anomaly [Degrees]
53-63 	Mean Motion [Revs per day]
64-68 	Revolution number at epoch [Revs]
69 	Checksum (Modulo 10)

THINGS TO DO:
	* Convert Epoch to Python datetime
	* Discuss which fields are relevant and which aren't relevant
	* Culling of the Norad class

THINGS DONE:
	* Reading a file for Norad coordinates, creating an object for them and then storing them into a queue

'''
# Imports
import sys
import Queue

# Global Variables
q = Queue.Queue()

# Classes
# Norad Coordinate Class
class Norad:
	def __init__(self, name, satnum, cfc, id1, id2, id3, epochyear, epoch, td1, td2, dt, ephemeris, element, chck1, satnum2, inc, asc, ecc, peri, ano, motion, revo, chck2):
		self.name = name
		self.satnum = satnum
		# CLassification
		self.cfc = cfc
		# Last 2 digits of launch year
		self.id1 = id1
		# Launch number of the year
		self.id2 = id2
		# Piece of the Launch
		self.id3 = id3
		# epoch: must convert to datetime object		
		self.epochyear = epochyear
		self.epoch = epoch
		# 1st and second time derivative of mean motion
		self.td1 = td1
		self.td2 = td2
		# Bstar drag: Method of modeling aerodynamic drag on a satellite
		self.dt = dt
		# Ephemeris Type: Not sure what this is?
		self.ephemeris = ephemeris
		# Element Number: Also not sure what this is
		self.element = element
		self.chck1 = chck1
		self.satnum2 = satnum2
		# azimuth		
		self.inc = inc
		# longitude (This could also be considered elevation...)
		self.asc = asc
		# eccentricity: deviation of a curve or orbit from circularity
		self.ecc = ecc
		# angle of perigee: angle within the satellite orbit plane that is measured from ascending node to perigee point along with satellite's direction of travel (elevation?)
		self.peri = peri
		# mean anomaly: used to calculate the position of a body in elliptical orbit
		self.ano = ano
		# amount of revolutions each day
		self.motion = motion
		# current number of revolutions at Epoch
		self.revo = revo
		self.chck2 = chck2
	
	# Print Function for Debugging purposes	
	def printCoordinates(self):
		print "Name: " + self.name
		print "Satellite Number: " + self.satnum
		print "Classification: " + self.cfc
		print "ID (Last 2 digits of launch year): " + self.id1
		print "ID (Launch number of the year): " + self.id2
		print "ID (Piece of the Launch): " + self.id3
		print "Epoch Year: " + self.epochyear
		print "Epoch: " + self.epoch
		print "1st Time Derivative of the Mean Motion: " + self.td1
		print "2nd Time Derivative of Mean Motion: " + self.td2
		print "BSTAR drag term: " + self.dt
		print "Ephemeris Type: " + self.ephemeris
		print "Element Number: " + self.element
		print "Checksum: " + self.chck1
		print "Satellite Number: " + self.satnum2
		print "Inclination (Degrees): " + self.inc
		print "Right Ascension of the Ascending Node (Degrees): " + self.asc
		print "Eccentricity: " + self.ecc
		print "Argument of Perigee (Degrees): " + self.peri
		print "Mean Anomaly (Degrees): " + self.ano
		print "Mean Motion (Revs/Day): " + self.motion
		print "Revolution number at Epoch (Revs): " + self.revo
		print "Checksum: " + self.chck2

# Read a file for Coordinates in NORAD format
def readFile(arg):
	fo = open(arg, "r")
	counter = 0
	for line in fo:
		if line == "":
			continue
		if counter == 0:
			name = line
		elif counter == 1:
			satnum = line[2:7].replace(" ", "")
			cfc = line[7].replace(" ", "")
			id1 = line[9:11].replace(" ", "")
			id2 = line[11:14].replace(" ", "")
			id3 = line[14:17].replace(" ", "")
			epochyear = line[18:20].replace(" ", "")
			epoch = line[20:32].replace(" ", "")
			# Conversion of epoch to Datetime to be done here			
			td1 = line[33:43].replace(" ", "")
			td2 = line[44:52].replace(" ", "")
			dt = line[53:61].replace(" ", "")
			ephemeris = line[62].replace(" ", "")
			element = line[64:68].replace(" ", "")
			chck1 = line[68].replace(" ", "")
		else:
			satnum2 = line[2:7].replace(" ", "")
			inc = line[8:16].replace(" ", "")
			asc = line[17:25].replace(" ", "")
			ecc = line[26:33].replace(" ", "")
			peri = line[34:42].replace(" ", "")
			ano = line[43:51].replace(" ", "")
			motion = line[52:63].replace(" ", "")
			revo = line[63:68].replace(" ", "")
			chck2 = line[68].replace(" ", "")
			counter = 0
			n = Norad(name, satnum, cfc, id1, id2, id3, epochyear, epoch, td1, td2, dt, ephemeris, element, chck1, satnum2, inc, asc, ecc, peri, ano, motion, revo, chck2)
			q.put(n)
			continue
		counter += 1

def main():
	readFile(str(sys.argv[1]))
	while not q.empty():
		n = q.get()
		n.printCoordinates()

if __name__ == '__main__':
	main()
