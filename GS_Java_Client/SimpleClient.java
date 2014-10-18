/*
david here,
like the arduino ide, uses rxtx library (those gnu imports) to communicate
takes in a currently comma seperated file (can be changed)

if youre using this code and a better version hasnt been made just edit constants for whatever file
 */

import java.io.*;

import java.lang.String;
import java.lang.Float;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.Enumeration;

import java.text.SimpleDateFormat;

import gnu.io.CommPortIdentifier; 
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent; 
import gnu.io.SerialPortEventListener; 

//using RXTX 2.1.7 since actual website down a 2.1.7 mirror was available.
public class SimpleClient implements SerialPortEventListener {

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
    private static final int elevationMultiplier = 1000;

    // Define the filepath here as "/relative_to_simpleclient"
    // if you want, define the order the columns of data are (if theyre different to below)
    // also the icon each column in the file is seperated with
    private static final String relativePath = "/luxspace.csv";
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

    // bunch of ardu code, main is at bottom
    SerialPort serialPort;
    /** The port we're normally going to use. */
    private static final String PORT_NAMES[] = { 
        "/dev/tty.usbserial-A9007UX1", // Mac OS X
        "/dev/ttyACM0", // Raspberry Pi
        "/dev/ttyUSB0", // Linux
        "COM3", // Windows
    };

    /**
     * A BufferedReader which will be fed by a InputStreamReader 
     * converting the bytes into characters 
     * making the displayed results codepage independent
     */
    private BufferedReader input;
    /** The output stream to the port */
    private OutputStream output;
    /** Milliseconds to block while waiting for port open */
    private static final int TIME_OUT = 2000;
    /** Default bits per second for COM port. */
    private static final int DATA_RATE = 9600;

    public void initialize() {
        // the next line is for Raspberry Pi and 
        // gets us into the while loop and was suggested here was suggested http://www.raspberrypi.org/phpBB3/viewtopic.php?f=81&t=32186
        System.setProperty("gnu.io.rxtx.SerialPorts", "/dev/ttyACM0");

        CommPortIdentifier portId = null;
        Enumeration portEnum = CommPortIdentifier.getPortIdentifiers();

        //First, Find an instance of serial port as set in PORT_NAMES.
        while (portEnum.hasMoreElements()) {
            CommPortIdentifier currPortId = (CommPortIdentifier) portEnum.nextElement();
            for (String portName : PORT_NAMES) {
                if (currPortId.getName().equals(portName)) {
                    portId = currPortId;
                    break;
                }
            }
        }
        if (portId == null) {
            System.out.println("Could not find COM port.");
            return;
        }

        try {
            // open serial port, and use class name for the appName.
            serialPort = (SerialPort) portId.open(this.getClass().getName(),
                    TIME_OUT);

            // set port parameters
            serialPort.setSerialPortParams(DATA_RATE,
                    SerialPort.DATABITS_8,
                    SerialPort.STOPBITS_1,
                    SerialPort.PARITY_NONE);

            // open the streams
            input = new BufferedReader(new InputStreamReader(serialPort.getInputStream()));
            output = serialPort.getOutputStream();

            // add event listeners
            serialPort.addEventListener(this);
            serialPort.notifyOnDataAvailable(true);
        } catch (Exception e) {
            System.err.println(e.toString());
        }
    }

    /**
     * This should be called when you stop using the port.
     * This will prevent port locking on platforms like Linux.
     */
    public synchronized void close() {
        if (serialPort != null) {
            serialPort.removeEventListener();
            serialPort.close();
        }
    }

    /**
     * Handle an event on the serial port. Read the data and print it.
     */
    public synchronized void serialEvent(SerialPortEvent oEvent) {
        if (oEvent.getEventType() == SerialPortEvent.DATA_AVAILABLE) {
            try {
                String inputLine=input.readLine();
                System.out.println(inputLine);
            } catch (Exception e) {
                System.err.println(e.toString());
            }
        }
        // Ignore all the other eventTypes, but you should consider the other ones.
    }


    /**
     * Main function. Parses in angle data into something nice for the arduino
     * Gives the arduino the angles at the time specified for each angle
     */

    public static void main (String args[]) {
        // try or else it wont compile
        try{  
            System.out.println("starting up!!...");
            // java uses absolute file paths, get relative from this 
            String filePath = new File("").getAbsolutePath();
            filePath += relativePath;
            File file = new File(filePath);
            // read file
            FileReader fr = new FileReader(filePath);
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

            SerialTest main = new SerialTest();
            main.initialize();
         /*   Thread t=new Thread() {
                public void run() {
                    //the following line will keep this app alive for 1000 seconds,
                    //waiting for events to occur and responding to them (printing incoming messages to console).
                    try {Thread.sleep(1000000);} catch (InterruptedException ie) {}
                }
            };*/
            //t.start();

            System.out.println("Started");
            Calendar cal = Calendar.getInstance();
            SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss");
            while(true){
                System.out.println();
                cal = Calendar.getInstance();
                System.out.println("Time: "+sdf.format(cal.getTime()));


            }
        } 
        // nothing to see here folks
        catch (IOException e){
            e.printStackTrace();
        }

    }

}
