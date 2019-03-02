#include <SoftwareSerial.h>

SoftwareSerial gps(4,5);
SoftwareSerial xbee(2,3);

char mode='S';                                                      // Declaration of global variables
int pas=0;                                                          // To count the 8 steps of the period in auto mode
int primercop=1;                                                    // Identify that when entering Auto mode start at the beginning
int manual=0;                                                       // It is useful not to lose Manual mode when we receive the manual orders
String posicio="";                                                  // Management of strings for GPS
char posicio2[68];                                                  // Final position of the GPS
String posicio3;                                                    // Last good read position of the GPS
unsigned long milisinicials=0;                                      // To make the timings
long tiradallarga=3000;                                             // 3 seconds
long tiradacurta=700;                                               // 0'7 seconds
long tiradagiro=480;                                                // 0'48 seconds
unsigned long retard=0;                                             // To count the 5 seconds we will enable the GPS
int retardactiu=0;                                                  
int indicapos=0;                                                    // When it's worth 1, we want to read the GPS

void setup()  {                                                     // Configuration + variable initialization
   Serial.begin(9600);
   while (!Serial) {
   ;
   }
   gps.begin(9600);
   xbee.begin(9600);
   pinMode(6,INPUT);  
   pinMode(7,OUTPUT);
   pinMode(8,OUTPUT);
   pinMode(9,OUTPUT);
   pinMode(10,OUTPUT);
   pinMode(11,OUTPUT);
   pinMode(12,OUTPUT);
   stopmotors();
   manual=0;
   primercop=1;
   indicapos=0;
   xbee.listen();
}

void endavant() {                                                     // Function to go forward at maximum speed
   analogWrite(10,95);
   analogWrite(11,100);
   digitalWrite(7,HIGH);
   digitalWrite(8,LOW);
   digitalWrite(9,HIGH);
   digitalWrite(12,LOW);
  
}

void endarrera() {                                                    // Function to go back at medium speed
   analogWrite(10,95);
   analogWrite(11,100);
   digitalWrite(7,LOW);
   digitalWrite(8,HIGH);
   digitalWrite(9,LOW);
   digitalWrite(12,HIGH);
}

void esquerra() {                                                     // Function to turn left at medium speed
   analogWrite(10,190);
   analogWrite(11,200);
   digitalWrite(7,HIGH);
   digitalWrite(8,LOW);
   digitalWrite(9,LOW);
   digitalWrite(12,HIGH);
}

void dreta() {                                                        // Function to turn right at medium speed
   analogWrite(10,190);
   analogWrite(11,200);
   digitalWrite(7,LOW);
   digitalWrite(8,HIGH);
   digitalWrite(9,HIGH);
   digitalWrite(12,LOW);
}

void stopmotors() {                                                   // Function to stop all engines
   digitalWrite(7,LOW);
   digitalWrite(8,LOW);
   digitalWrite(9,LOW);
   digitalWrite(12,LOW);
   analogWrite(10,0);
   analogWrite(11,0);
}


void loop() {                                                         // Main loop of the program
char c;
if (xbee.isListening()) {                                             // If XBEE is enabled
  c = xbee.read();                                                    // We read a character via Wi-Fi
  if ((c>='A') && (c<='z')) {                                         
     // Serial.write(mode);
     if ((c =='g') || (c=='G')) {                                     // If we receive G, we will indicate the current position
      indicapos=1;
      } else  {                                                       // In all other cases it will be the order of the PC
        mode=c;
      }
  } 
}

if (indicapos==1) {                                                   // When indicapos = 1 then we will enable the GPS
  if (retardactiu==0) {                                               
    xbee.println("....connecting with GPS");                          
    gps.listen();
    retard=millis();
    retardactiu=1;
    posicio3="Position not found, check GPS coverage";
  }
  c = gps.read();                                                     // We read a character of GPS
  if (c=='$') gps.readBytesUntil('*', posicio2, 67);                  // If it begins with $ it means that we have a plot OK
  posicio=posicio2;
  if(posicio.substring(0,4) == "GPRM") posicio3=posicio2;             
  if (millis()-retard > 5000) {
    xbee.listen();                                                    // If we have upgraded the Xbee for 5 seconds
    retardactiu=0;
    indicapos=0;
    xbee.println(posicio3);                                           // Print the position
  }
} 

if (digitalRead(6)) {                                                 // If we found metal, we indicated it and stopped
   mode='S';
   xbee.println("Mine encountered");
   xbee.println("Possible mine detected");
   xbee.println("Browse the following coordinates");
   xbee.println("=================================");
}

if ((mode=='A') || (mode=='a')) {                                     // Modus automatic
  if (primercop==1)  {
    xbee.println("Mode automatic");
    primercop=0;
    manual=0;
    pas=0;
  }
  if (pas==0) {
    endavant();
    pas=1;
    milisinicials=millis();
  }
  if (pas==1) {
    if (millis()-milisinicials > tiradallarga) {
      //stopmotors();
      dreta();
      pas=2;
      milisinicials=millis();
    }
  }
  if (pas==2) {
    if (millis() - milisinicials > tiradagiro) {
      endavant();
      pas=3;
      milisinicials=millis();
    }
  }
  if (pas==3) {
    if (millis()- milisinicials > tiradacurta) {
      //stopmotors();  
      dreta();
      pas=4;
      milisinicials=millis();
    }
  }
  if (pas==4) {
    if (millis()- milisinicials > tiradagiro) {
      endavant();
      pas=5;
      milisinicials=millis();
    }
  }
  if (pas==5) {
    if (millis()- milisinicials > tiradallarga) {
      //stopmotors();
      esquerra();
      pas=6;
      milisinicials=millis();
    }
  }
  if (pas==6) {
    if (millis()- milisinicials > tiradagiro) {
      endavant();
      pas=7;
      milisinicials=millis();
    }
  }
  if (pas==7) {
    if (millis()- milisinicials > tiradacurta) {
      //stopmotors();
      esquerra();
      pas=8;
      milisinicials=millis();
    }
  }
  if ((pas==8) && (millis()- milisinicials > tiradagiro))  pas=0;
} 

if ((mode=='M') || (mode=='m') || (manual==1))  {                     // Modus manual
  if (manual==0)  xbee.println("Mode manual");
  manual =1;
  primercop=1;
  if (manual && ((mode=='F') || (mode=='f'))) endavant(); 
  if (manual && ((mode=='B') || (mode=='b'))) endarrera();
  if (manual && ((mode=='L') || (mode=='l'))) esquerra(); 
  if (manual && ((mode=='R') || (mode=='r'))) dreta(); 
  if (manual && ((mode=='P') || (mode=='p'))) stopmotors(); 
}

if ((mode =='S') || (mode=='s')) {                                    // Modus stopped
  stopmotors();
  manual=0;
  primercop=1;
  indicapos=1;
  mode='z';
}
}





