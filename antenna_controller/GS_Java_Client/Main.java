/*
	Main.java - Written by Brian Luc - 14th March 2015
	Ground Control Software used to control Sattelite from the ground
	Parses in data and sends them to satellite
	TO DO:
		* Code Optimization, especially in Date comparison!!! Not even speed friendly
		* Server capabilities 
	WOULD BE NICE:
		* Interactive GUI/console which accepts commands and executes stuff
		* Find a way to send signal direct via serial

*/
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.text.DateFormat;
import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.TimeZone;
import java.util.Date;
import java.util.LinkedList;
import java.io.FileOutputStream;
import java.text.ParseException;
import java.net.*;
import java.io.*;


public class Main {
	private LinkedList<Coordinates> list;
	private String file;
	private String arduinoDirectory;
	public Main(){
		list = new LinkedList<Coordinates>();
		// arduino code directory relative to Main.java
		file = "/home/bluesatsdr/Desktop/groundstation_software-master/antenna_controller/GS_Arduino_Code/GS_Arduino_Code.ino";
		// arduino directory relative to Main.java
		arduinoDirectory = "/home/bluesatsdr/Documents/JavaController/arduino-1.6.4/arduino";
	}
	public static void main(String[] args) throws Exception {
		final Main obj = new Main();
		obj.read(args[0]);
		
		// infinite loop, updates every second
		while(true){
			if(obj.list.isEmpty()){
				break;
			}
				
			/*
			 *	Server Component
			 */
			/*System.out.println("Server");
			Socket connectionSocket = server.accept();
			BufferedReader input = new BufferedReader(new InputStreamReader(connectionSocket.getInputStream()));
			DataOutputStream output = new DataOutputStream(connectionSocket.getOutputStream());
			String request = input.readLine();
			// to be worked on
			if(!request.isEmpty()){
				String retString = "UTC Time: " + obj.list.getFirst().getTime() + "\tAzimuth (degrees):" + obj.list.getFirst().getAzimuth() + "\tElevation (degrees): " + obj.list.getFirst().getElevation() + "\tRange (km): " + obj.list.getFirst().getRange() + "\n";
				output.writeBytes(retString);
			}*/
			/*
			 *	Date Comparison Component
			 *	Major Optimization required!!!
			 */
			// get current time UTC
			Calendar cal = new GregorianCalendar();
			DateFormat currDate = new SimpleDateFormat("dd-MM-yyyy HH:mm:ss");
			currDate.setCalendar(cal);
			currDate.setTimeZone(TimeZone.getTimeZone("UTC"));
			// check if time on top of list is still valid
			DateFormat listDateFormat = new SimpleDateFormat("dd-MM-yyyy HH:mm:ss");
			try{
				Date listDate = listDateFormat.parse(obj.list.getFirst().getTime());
				
				// compare with time on top of list
				// if time matches, then send [Elevation,Azimuth] to Arduino Due Program and then delete top entry in list
				if((currDate.format(cal.getTime())).equals(obj.list.getFirst().getTime())){
					System.out.print("\n Sending Coordinates: ["+obj.list.getFirst().getElevation()+","+obj.list.getFirst().getAzimuth()+"]\r\n");
					// send [Elevation,Azimuth] to GS_Arduino_Code.ino
					obj.replaceDefines(obj.list.getFirst().getElevation(), obj.list.getFirst().getAzimuth());
					try{
						// Below command compiles arduino code and uploads to board					
						String[] command = {obj.arduinoDirectory,"--port", "/dev/ttyACM0","-v","--upload", obj.file};
						Process p = new ProcessBuilder(command).start();
					}
					catch(IOException e){
						System.out.println("ERROR: File not found! Please check file directory (Main())");
						e.printStackTrace();
					}
					obj.list.removeFirst();
				}
				else if(listDate.before(cal.getTime())){
					obj.list.removeFirst();
					System.out.println("Invalid Date! Moving to next element in list");
				}
				else{
					System.out.print("\r"+currDate.format(cal.getTime()));
					// Test line, prints current Date
					// otherwise sleep
					try {
						Thread.sleep(1000);
					} 
					catch (InterruptedException e) {
						System.out.println("ERROR: Cannot thread for shit!");
						e.printStackTrace();
					}
				}
				
			}
			catch(ParseException e){
				System.out.println("ERROR: Unable to parse string to date");
				e.printStackTrace();
			}
		}

	}
	
	// Reads Data
	public void read(String arg){
		String File = arg;
		BufferedReader br = null;
		String line = "";
		String split = "\t";
		try{
			br = new BufferedReader(new FileReader(File));
			// list to store all coordinates
			while((line = br.readLine()) != null){
				// placeholder name, array with each column
				String[] array = line.split(split);
				// Checks if line contains actual data
				if(array[0].length() < 18){
					// Do nothing
				}
				else{
					String time = array[0];
					double azi = Double.parseDouble(array[1]);
					double ele = Double.parseDouble(array[2]);
					double ran = Double.parseDouble(array[3]);
					if(ele < 0){
						System.out.println("ERROR: Elevation less than 0! [ele: "+ele+", azi: "+azi+", ran: "+ran+"]");
					}
					else{
						Coordinates c = new Coordinates(time,azi,ele,ran);
						list.add(c);
					}
				}
			}
		}
		catch(FileNotFoundException e){
			System.out.println("ERROR: File not found! Please check file directory (read(String))");
			e.printStackTrace();
		}
		catch(IOException e){
			System.out.println("ERROR: File not found! Please check file directory (read(String))");
			e.printStackTrace();
		}
		finally{
			if(br != null){
				try{
					br.close();
				}
				catch(IOException e){
					e.printStackTrace();
				}
			}
		}
	}
	// Test function to print list
	public void printList(){
		for(int i = 0; i < list.size(); i++){
			list.get(i).print();
		}
	}

	public void replaceDefines(Double Ele, Double Azi){
		try{
			BufferedReader br = new BufferedReader(new FileReader(file));
			String newFile = "";
			String line = "";
			String redefineEL = "#define EL " + Ele;
			String redefineAZ = "#define AZ " + Azi;
			while((line = br.readLine()) != null){
				if(line.contains("#define EL") && !line.contains("VOLTS") && !line.contains("ANGLE")){
					newFile = newFile + redefineEL+"\n";
				}
				else if(line.contains("#define AZ") && !line.contains("VOLTS") && !line.contains("ANGLE")){
					newFile = newFile + redefineAZ+"\n";
				}
				else{
					newFile = newFile + line + "\n";
				}
			}
			FileOutputStream fileOut = new FileOutputStream(file);
			fileOut.write(newFile.getBytes());
			fileOut.close();
		}
		catch(FileNotFoundException e){
			System.out.println("ERROR: File not found! Please check file directory (replaceDefines(Double, Double))");
			e.printStackTrace();
		}
		catch(IOException e){
			System.out.println("ERROR: File not found! Please check file directory (replaceDefines(Double, Double))");
			e.printStackTrace();
		}
	}

}
