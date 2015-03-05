groundstation_software
======================

Software to run the BLUEsat Groundstation:

Antenna Controller
- Given a data table, points an antenna to a given azimuth/elevation at a given time
- Uses java to read the data table & send to an Arduino at given times
- Uses an arduino to control the antenna actuators


APRS Translator
- Given an APRS packet (in ASCII), 'translates' it into lat/long coordinates, elevation, horizontal speed, vertical speed and course (horizontal direction)
- User needs to run an SDR receiver to obtain APRS packets
- Once the packet is recieved, copy into the translator, and relevant data will be printed
