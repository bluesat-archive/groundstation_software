/*
david here,
like the arduino ide, uses rxtx library (those gnu imports) to communicate
takes in a currently comma seperated file (can be changed)

if youre stilllll using this code and a better version hasnt been made just edit constants for whatever file
 */

import java.io.*;
import gnu.io.*;
import java.lang.String;
import java.lang.Float;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Enumeration;
import java.text.SimpleDateFormat;

//using RXTX 2.1.7 since actual website down a 2.1.7 mirror was available.
public class SimpleClient {

    /*======================================================
    Constants

    for our lil mission in degrees
    arduino hates any degrees with floats so convert into x1000 ints to salvage 3 decimals of precision
    doing this for both azimuth and elevation

    the azimuth data starts off by default at 0/360. it swings forward to 133 and BACK to 227
    the arduino expects azimuth values of +133 and -133 instead.

    the elevation data usually would be from 0 to 90 degrees
    instead the signal to send ranges between 10 to 100 degrees
    ======================================================*/
    // adjusting constant since arduino works best with integers
    private static final int azimuthMultiplier = 1000;
    private static final int azimuthOffset = 0;
    private static final int elevationMultiplier = 1000;
    private static final int elevationOffset = 10;

    // Define the filepath here as "/relative_to_simpleclient"
    // if you want, define the order the columns of data are (if theyre different to below)
    // also the icon each column in the file is seperated with
    //private static final String relativePath = "/luxspace.csv";
    private static final String seperationIcon = ",";
    private static final int dateSlot = 0;
    private static final int azimuthSlot = 1;
    private static final int elevationSlot = 2;


    /*======================================================
    Arduino Website's Interfacing code

    Searching for serial ports
    Connecting to the serial port
    Starting the input output streams
    Adding an event listener to listen for incoming data
    Disconnecting from the serial port
    Sending Data
    Receiving Data
    ======================================================*/
    /*
    // figure out your port name based on os
    private static final String PORT_NAMES[] = {
        "/dev/tty.usbmodem", // Mac OS X
        "/dev/usbdev", // Linux
        "/dev/tty", // Linux
        "/dev/serial", // Linux
        "COM3", // Windows
    };

    // Enumerate system ports and try connecting to Arduino over each
    while (portId == null && portEnum.hasMoreElements()) {
        CommPortIdentifier currPortId =
                (CommPortIdentifier) portEnum.nextElement();
        for (String portName : PORT_NAMES) {
            if ( currPortId.getName().equals(portName)
                    || currPortId.getName().startsWith(portName))
            {
                // Try to connect to the Arduino on this port
                serialPort = (SerialPort)currPortId.open(appName, TIME_OUT);
                portId = currPortId;
                break;
            }
        }
    }

    // set port parameters
    serialPort.setSerialPortParams(
            DATA_RATE, // 9600 baud
            SerialPort.DATABITS_8,
            SerialPort.STOPBITS_1,
            SerialPort.PARITY_NONE);

    // add event listeners
    serialPort.addEventListener(this);
    serialPort.notifyOnDataAvailable(true);
     */


    /**
     * Main function. Parses in angle data into something nice for the arduino
     * Gives the arduino the angles at the time specified for each angle
     */

    public static void main (String args[]) {
        // try or else it wont compile
        try{  
            System.out.println("starting up!!...");
            // java uses absolute file paths, get relative from this 
            //String filePath = new File("").getAbsolutePath();
            //filePath += relativePath;
            //File file = new File(filePath);
            // read file
            FileReader fr = new FileReader("luxspace.csv");
            BufferedReader reader = new BufferedReader(fr);
            // to: contains comma seperated line
            ArrayList<String> date = new ArrayList<String>();
            ArrayList<String> azimuth = new ArrayList<String>();
            ArrayList<String> elevation = new ArrayList<String>();
            String[] location = new String[3];
            while (reader.ready()) {
                location = reader.readLine().split(seperationIcon);
                // debug for splitting strings
                //System.out.println("DEBUG: "+location[dateSlot]+"***"+location[azimuthSlot]+"***"+location[elevationSlot]);
                date.add(location[dateSlot]);
                azimuth.add(location[azimuthSlot]);
                elevation.add(location[elevationSlot]);
            }

            ArrayList<String> newList = new ArrayList<String>();
            // loop, change everything
            for(int x = 0; x < date.size(); x++){
                // string -> float -> int -> string in order to *1000 and remove decimal
                Float bleh = Float.parseFloat(azimuth.get(x));
                bleh *= azimuthMultiplier;                
                int bleh2 = bleh.intValue();
                newList.add(Integer.toString(bleh2));
            }
            azimuth = newList;

            newList = new ArrayList<String>();
            // loop, change everything, more test printing
            for(int x = 0; x < date.size(); x++){
                // string -> float -> int -> string in order to *1000 and remove decimal
                Float bleh = Float.parseFloat(elevation.get(x));
                bleh *= elevationMultiplier;                
                int bleh2 = bleh.intValue();
                newList.add(Integer.toString(bleh2));
            }
            elevation = newList;

            // debug loop
            for(int x = 0; x < date.size(); x++){
                System.out.println("azimuth:   "+azimuth.get(x));
                System.out.println("elevation: "+elevation.get(x));    
            }

            // TIME for SPACE
            /*
            Main loop begins here
            Goin to transmit to arduino here.
             */
            ArduinoTest1 arduLink = new ArduinoTest1();
            /*
java -Djava.library.path=/Users/iuyhcdfs/ArduinoTest1 -cp ./RXTXcomm.jar:./build/classes arduinotest1.ArduinoTest1 
            */
            if (!arduLink.initialize()){
                System.out.println("ground station disconnected: cant find arduino!");
                return;
            }
            Calendar cal = Calendar.getInstance();
            SimpleDateFormat sdf = new SimpleDateFormat("dd/MM/YYYY HH:mm:ss");
            System.out.println("Current Time:  "+sdf.format(cal.getTime())+"!");
            System.out.println("Waiting Until: "+date.get(0)+"!");
            int index = 0;
            // send initial data

            
            arduLink.sendData("["+azimuth.get(index)+","+elevation.get(index)+"]\n");
            while(true){

                // tell arduino where we want to be ALL THE TIME
                //arduLink.sendData("["+azimuth.get(index)+","+elevation.get(index)+"]\n");

                // uncomment this if you want to wait a second each thing
                try {
                    // sleeps one second but is NOT SECOND-SYNCED WITH COMPUTER CLOCK 
                    Thread.sleep(1000);                 
                } catch(InterruptedException ex) {
                    Thread.currentThread().interrupt();
                }

                // always constantly get the time
                System.out.println();
                cal = Calendar.getInstance();
                System.out.println("Last Message:   "+"["+azimuth.get(index)+","+elevation.get(index)+"]");
                System.out.println("Current Moment: "+sdf.format(cal.getTime())+"!");
                System.out.println("Next Update In: "+date.get(index)+"!");
                System.out.println();

                // compare time to the next target.
                // if the time has elapsed, shift index to next date+location!
                if( sdf.format(cal.getTime()) .equals (date.get(index)) ){
                    // NOW THAT WE SENT THE OLD ONE AT THE OLD DATE
                	// BEGIN MOVING TO THE NEXT ONE REGARDLESS OF DUE DATE ASAP
                    index++;
                    arduLink.sendData("["+azimuth.get(index)+","+elevation.get(index)+"]\n");                    
                }

            }

        } 
        // nothing to see here folks
        catch (IOException e){
            e.printStackTrace();
        }

    }

}
