
public class Coordinates {
	private String UTCTime;
	private double Azimuth;
	private double Elevation;
	private double Range;
	public Coordinates(String time, double azi, double ele, double ran){
		UTCTime = time;
		Azimuth = azi;
		Elevation = ele;
		Range = ran;
	}
	
	public String getTime(){
		return UTCTime;
	}
	public double getAzimuth(){
		return Azimuth;
	}
	public double getElevation(){
		return Elevation;
	}
	public double getRange(){
		return Range;
	}
	//test function
	public void print(){
		System.out.println("Time:\t" + UTCTime);
		System.out.println("Azimuth:\t" + Azimuth);
		System.out.println("Elevation:\t" + Elevation);
		System.out.println("Range:\t" + Range);
		System.out.println("-------------------------------------------------------------------");
	}
}
