//Mark Yeo; mark.yeo@student.unsw.edu.au
//Created 25Jan15, Modified 26Jan15
//APRS Packet Translator
//Translator derived from the "APRS PROTOCOL REFERENCE"(v1.0.1)(Ch10: MIC-E DATA FORMAT)


/* Notes
 
=== Instructions (Unix) ===
 Compile using:
gcc -Werror -Wall -O -o aprsTranslator aprsTranslator.c
 Run using:
./aprsTranslator
 
 Input: an APRS packet (from gqrx or SDR#), e.g:
11:17:05$ fm VK2MRW-0 to S2P4T0-0 via WIDE2-1 UI  PID=F0
          `N;TlH:O/'";"}|$T%F'I|!wz+!|3
 Output:
 - Time (24hr)
 - Coordinates
 - Altitude
 - Vertical Speed (using adjacent packets)
 - Horizontal Speed
 - Course (direction of horizontal speed, measured CW from North)
 
 ctrl-C to quit program

 
 
=== Example APRS data ===
 
09:22:32$ fm VK2MRW-0 to S2Q5V3-0 via WIDE2-1 UI  PID=F0
          `NRRl7"O/'"=<}MT-AIO HA 1|"#%[("|!wPc!|3
09:23:02$ fm VK2MRW-0 to S2Q5V2-0 via WIDE2-1 UI  PID=F0
          `NRNl@PO/'">Q}|"$%[("|!wa'!|3
11:04:12$ fm VK2MRW-0 to S2P3U7-0 via WIDE2-1 UI  PID=F0
          `N<_m>nO/'#8j}|$:%I'O|!wj`!|3
11:17:05$ fm VK2MRW-0 to S2P4T0-0 via WIDE2-1 UI  PID=F0
          `N;TlH:O/'";"}|$T%F'I|!wz+!|3
 
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FALSE 0
#define TRUE 1



int main() {
    int prevAlt = 0;
    int prevTimeS = 0;
    
    while (1){
        printf("================================\n");
        printf("Enter APRS String:\n");
        char s[2][100] = {{0}};
        
        
        
        
        // === Read in APRS packet from stdin ===
        
        char c;
        int line;
        for (line = 0; line < 2; line++){
            c = fgetc(stdin);
            int i = 0;
            while (c != '\n'){
                s[line][i] = c;
                i++;
                c = fgetc(stdin);
            }
            s[line][i] = '\0';
        }
        //printf("Line 1: %s\n", s[0]);
        //printf("Line 2: %s\n", s[1]);
        
        
        
        
        // === Locate data ===
        
        int found;
        int i = 0;
        int j;
        
        //Time
        int time[3];
        time[0] = (s[0][0]-'0')*10 + (s[0][1]-'0');
        time[1] = (s[0][3]-'0')*10 + (s[0][4]-'0');
        time[2] = (s[0][6]-'0')*10 + (s[0][7]-'0');
        
        //Latitude
        char cLat[7];
        found = FALSE;
        int spaces = 0;
        while (!found){
            if (s[0][i] == ' '){
                spaces++;
            }
            if (spaces == 4){
                i++;
                for (j = 0; j < 6; j++){
                    cLat[j] = s[0][i+j];
                }
                found = TRUE;
            }
            i++;
            if (i >= 100) {fprintf(stderr, "Error in processing APRS data\n"); abort();}
        }
        cLat[6] = '\0';
        
        //Longitude
        char cLon[4];
        found = FALSE;
        i = 0;
        while (!found){
            if (s[1][i] == '`'){
                for (j = 0; j < 3; j++){
                    cLon[j] = s[1][++i];
                }
                found = TRUE;
            }
            i++;
            if (i >= 100) {fprintf(stderr, "Error in processing APRS data\n"); abort();}
        }
        cLon[3] = '\0';
        
        //Horizontal Speed
        char cHor[4];
        for (j = 0; j < 3; j++){
            cHor[j] = s[1][i++];
        }
        cHor[j] = '\0';
        
        //Altitude
        char cAlt[4];
        found = FALSE;
        while (!found){
            if (s[1][i] == '}'){
                int j;
                for (j = 0; j < 3; j++){
                    cAlt[j] = s[1][i-3+j];
                }
                found = TRUE;
            }
            i++;
            if (i >= 100) {fprintf(stderr, "Error in processing APRS data\n"); abort();}
        }
        cAlt[3] = '\0';
        
        /*
        printf("time: {%d, %d, %d}\n", time[0], time[1], time[2]);
        printf("cLat: %s\n", cLat);
        printf("cLon: %s\n", cLon);
        printf("cAlt: %s\n", cAlt);
        printf("cHor: %s\n", cHor);
        */
        
        
        
        
        // == Decode APRS ==
        
        //Latitude
        int ilat[3] = {0,0,0};
        char ns = 'S';
        int lonOffset = 0;
        char ew = 'E';
        for (i = 0; i < 6; i++){
            if (cLat[i] >= 'P' && cLat[i] <= 'Y'){
                cLat[i] -= 'P';
                if (i == 3) ns = 'N';
                if (i == 4) lonOffset = 100;
                if (i == 5) ew = 'W';
            } else if (cLat[i] >= '0' && cLat[i] <= '9'){
                cLat[i] -= '0';
            } else {
                printf("Error in Latitude Data\n");
                abort();
            }
            if (i%2 == 0){
                ilat[i/2] += (int)cLat[i] * 10;
            } else {
                ilat[i/2] += (int)cLat[i];
            }
        }
        
        //Longitude
        int ilon[3];
        ilon[0] = (int)cLon[0] - 28 + lonOffset;
        ilon[1] = ((int)cLon[1] - 28) % 60;
        ilon[2] = (int)cLon[2] - 28;
        
        //Altitude
        int alt = 0;
        alt += (int) cAlt[2] - 33;
        alt += (int) (cAlt[1] - 33) * 91;
        alt += (int) (cAlt[0] - 33) * 91 * 91;
        alt -= 10000;
        
        //Horizontal Speed
        int hSpeed = ((int)cHor[0] - 28) * 10;
        hSpeed += ((int)cHor[1] - 28) / 10;
        if (hSpeed >= 800) hSpeed -= 800;
        hSpeed = hSpeed * 1.852 + 0.5; //1.852 kph per knot
        
        //Course (direction of travel)
        int direction = ((((int)cHor[1] - 28) % 10) - 4) * 100;
        direction += (int)cHor[2] - 28;
        if (direction >= 400) direction -= 400;
        
        
        
        
        // == Print Data ==
        
        printf("--------------------------------\n");
        
        //Time
        printf("Time:             %02d:%02d:%02d\n", time[0], time[1], time[2]);
        
        //Coordinates (degrees, minutes, seconds)(for Google maps)
        printf("Coordinates:      %d°%d'%d\"%c %d°%d'%d\"%c\n", ilat[0], ilat[1], ilat[2]*60/100, ns, ilon[0], ilon[1], ilon[2]*60/100, ew);
        
        //Coordinates (degrees only)
        double dLat = ilat[0] + (ilat[1] + ilat[2]/100.0) / 60.0;
        double dLon = ilon[0] + (ilon[1] + ilon[2]/100.0) / 60.0;
        printf("Coordinates(deg): %lf°%c %lf°%c\n", dLat, ns, dLon, ew);
        
        //Altitude
        printf("Altitude:         %dm\n", alt);
        
        //Vertical Speed (based off current & previous reading)
        int timeS = 60*60*time[0] + 60*time[1] + time[2];
        if (prevAlt != 0 && timeS != prevTimeS){
            double vSpeed = (alt - prevAlt) * 1.0 / (timeS - prevTimeS);
            printf("Vertical Speed:   %.2fm/s\n", vSpeed);
        }
        
        //Horizontal Speed
        printf("Horizontal Speed: %dkm/h\n", hSpeed);
        
        //Course
        printf("Course:           %d°\n", direction);
        
        
        
        
        prevAlt = alt;
        prevTimeS = 60*60*time[0] + 60*time[1] + time[2];
    }
    return 0;
}

