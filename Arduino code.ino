#include <SoftwareSerial.h>

SoftwareSerial gps(4,5);
SoftwareSerial xbee(2,3);

char mode='S';                                                      // Declaració de variables globals
int pas=0;                                                          // Per contar els 8 passos del periode en modus auto
int primercop=1;                                                    // Indetifica que al entrar en mode Auto comencem pel pricipi
int manual=0;                                                        // Serveix per no perdre el mode Manual quan rebem les ordres manuals
String posicio="";                                                  // Gestió d'strings pel GPS
char posicio2[68];                                                  // Posició final del GPS
String posicio3;                                                    // Ultima posició bona llegida del GPS
unsigned long milisinicials=0;                                      // Per fer les temporitzacions
long tiradallarga=3000;                                            // deu segons
long tiradacurta=700;                                              // dos segons
long tiradagiro=480;                                               // un segons
unsigned long retard=0;                                             // per contar els 5 segons que habilitarem el GPS
int retardactiu=0;                                                  // per contar els 5 segons que habilitarem el GPS
int indicapos=0;                                                    // Quan valgui 1 es que volem llegir el GPS

void setup()  {                                                     // Configuració + inicialització variables
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

void endavant() {                                                     // Funció per anar endavant a maxima velocitat
   analogWrite(10,95);
   analogWrite(11,100);
   digitalWrite(7,HIGH);
   digitalWrite(8,LOW);
   digitalWrite(9,HIGH);
   digitalWrite(12,LOW);
  
}

void endarrera() {                                                    // Funció per anar endarrera a mitja velocitat
   analogWrite(10,95);
   analogWrite(11,100);
   digitalWrite(7,LOW);
   digitalWrite(8,HIGH);
   digitalWrite(9,LOW);
   digitalWrite(12,HIGH);
}

void esquerra() {                                                     // Funció per girar a l'esquerra a mitja velocitat
   analogWrite(10,190);
   analogWrite(11,200);
   digitalWrite(7,HIGH);
   digitalWrite(8,LOW);
   digitalWrite(9,LOW);
   digitalWrite(12,HIGH);
}

void dreta() {                                                        // Funció per girar a la dreta a mitja velocitat
   analogWrite(10,190);
   analogWrite(11,200);
   digitalWrite(7,LOW);
   digitalWrite(8,HIGH);
   digitalWrite(9,HIGH);
   digitalWrite(12,LOW);
}

void stopmotors() {                                                   // Funció per parar tots els motors
   digitalWrite(7,LOW);
   digitalWrite(8,LOW);
   digitalWrite(9,LOW);
   digitalWrite(12,LOW);
   analogWrite(10,0);
   analogWrite(11,0);
}


void loop() {                                                         // Llaç principal del programa
char c;
if (xbee.isListening()) {                                             // Si està habilitat XBEE
  c = xbee.read();                                                    // llegim un caracter via wifi
  if ((c>='A') && (c<='z')) {                                         // Només fem cas de les lletres
     // Serial.write(mode);
     if ((c =='g') || (c=='G')) {                                     // Si rebem G inicarem la posició actual
      indicapos=1;
      } else  {                                                       // En la resta de casos será l'ordre del PC
        mode=c;
      }
  } 
}

if (indicapos==1) {                                                   // Quan indicapos valgui 1 habilitarem el GPS
  if (retardactiu==0) {                                               // durant 5 segons. Aixo anularà momentaniament
    xbee.println("....connectant amb GPS");                           // el XBEE
    gps.listen();
    retard=millis();
    retardactiu=1;
    posicio3="Poscio no trobada, comprovar cobertura GPS";
  }
  c = gps.read();                                                     // Llegim un caracter dek GPS
  if (c=='$') gps.readBytesUntil('*', posicio2, 67);                  // Si comença per $ vol dir que tenim una trama OK
  posicio=posicio2;
  if(posicio.substring(0,4) == "GPRM") posicio3=posicio2;             // Només ens interessa les trames que comenceen per GPRM
  if (millis()-retard > 5000) {
    xbee.listen();                                                    // Si han passat 5 segons tornem a habilitar la Xbee
    retardactiu=0;
    indicapos=0;
    xbee.println(posicio3);                                           // i escribim la posició
  }
} 

if (digitalRead(6)) {                                                 // Si hem trobat metall, ho indiquem i parem
   mode='S';
   xbee.println("MINA TROBADA");
   xbee.println("Possible mina detectada");
   xbee.println("Examinar les coordenada següent");
   xbee.println("=================================");
}

if ((mode=='A') || (mode=='a')) {                                     // Modus automatic
  if (primercop==1)  {
    xbee.println("Iniciant mode automatic");
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
  if (manual==0)  xbee.println("Iniciant mode manual");
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





