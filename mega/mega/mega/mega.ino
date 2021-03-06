/*
  Arduino Mega Server
  
  Version: 0.1 alpha
  Year: 2015
  License: free, witout any guarantee
  Author: AleX
  Home: majordomo.smartliving.ru/forum/
*/

#define RTC_FEATURE
#define UPLOAD_FEATURE
#define PIRS_FEATURE
#define CONTACTS_FEATURE
//#define TEMP_FEATURE
#define ELECTRO_FEATURE
#define LEDS_FEATURE
//#define KEYS_FEATURE
#define MAJORDOMO_FEATURE
//#define LAURENT_FEATURE
#define SERVER_FEATURE
#define SD_FEATURE
//#define PING_FEATURE

#include <avr/pgmspace.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

#include <Time.h> 
#include <EthernetUdp.h>

byte MODUL_DISABLE = 0;
byte MODUL_ENABLE = 1;
byte MODUL_NOT_COMPILLED = 2;

byte modulRtc = 2;
byte modulEthernet = 2;
byte modulSdCard = 2;
byte modulServer = 2;
byte modulMajor = 2;
byte modulLaurent = 2;
byte modulPing = 2;
byte modulUpload = 2;
byte modulPirs = 2;
byte modulContacts = 2;
byte modulTemp = 2;
byte modulElectro = 2;
byte modulLeds = 2; 
byte modulKeys = 2;

// strings
char buf[200];

// Duration
time_t prevEventElectroCycle;
time_t prevEventMegaLive;

char fn[20] = ""; // for name <!>
char SELF_NAME[] = "MEGA";

byte MODE_SERVER = 1;
byte MODE_UPDATE = 2;
byte mode = MODE_SERVER;

boolean LED_state[4] = {0}; // stores the states of the LEDs
boolean LED_state2[4] = {0};
boolean buttonElectro = 0;

// timers
unsigned long timeSec; // time in seconds
unsigned long timer1s;  boolean cycle1s = false;
unsigned long timer4s;  boolean cycle4s = false;
unsigned long timer20s; boolean cycle20s = false;
unsigned long timer30s; boolean cycle30s = false;
unsigned long timer1m;  boolean cycle1m = false;
unsigned long timer3m;  boolean cycle3m = false;
unsigned long timer5m;  boolean cycle5m = false;
int startSendTime = 10;

#ifdef SD_FEATURE
  File SDroot; // web page file on the SD card
  File SDdir;
  File entry;
  Sd2Card card;
  SdVolume volume;
  SdFile root;
  const int chipSelect = 4;
#endif

#ifdef LEDS_FEATURE
  // LED modes
  const int LED_EMPTY = 0; // подсветка отключена
  const int LED_PIR_01 = 1; // подсветка состояния PIR-ов, режим 01
  const int LED_ANALOG_01 = 2; // "аналогоыая" подсветка
  int MODE = LED_EMPTY; // режим светодиодной подсветки
#endif

/* setup
-------------------------------------------------- */

void setup() {
  Serial.begin(9600);
  Serial.println("");
  
  Serial.print(SELF_NAME);
  Serial.println(F(" started..."));
  
  // init timers
  timersInit();
  
  ethernetInit();
  
  #ifdef SERVER_FEATURE
    //disableSDcard();
    SDcardInit();
    serverInit();
  #endif  
  
  #ifdef RTC_FEATURE
    rtcInit();
  #endif  
  
  
  #ifdef MAJORDOMO_FEATURE
    majordomoInit();
    majordomoMegaLive(); // уведомление MajorDoMo, что Mega01 работает
  #endif
  
  #ifdef LAURENT_FEATURE
    laurentInit();
  #endif  
  
  
  #ifdef SD_FEATURE
    SDroot = SD.open("/");
    SDdir = SD.open("/");
  
    Serial.print("Init SD card... ");
    if (!card.init(SPI_HALF_SPEED, chipSelect)) {
      Serial.println("failed");
      return;
    } else {
        Serial.println("OK");
      } 
  #endif // SD_FEATURE

  #ifdef PING_FEATURE
    pingInit();
  #endif 

  #ifdef UPLOAD_FEATURE
    uploadInit();
  #endif

  // init sensors
  #ifdef PIRS_FEATURE
    pirsInit();
  #endif

  #ifdef CONTACTS_FEATURE
    contactsInit();
  #endif
 
  #ifdef TEMP_FEATURE
    tempInit();
  #endif
 
  #ifdef ELECTRO_FEATURE
    electroInit();
  #endif
  
  #ifdef KEYS_FEATURE
    keysInit();
  #endif 
  
  #ifdef LEDS_FEATURE
    ledsInit();
  #endif
} // setup

/* loop
-------------------------------------------------- */

void loop() {
  #ifdef RTC_FEATURE
    rtcWorks();
  #endif  
 
  #ifdef UPLOAD_FEATURE
    uploadWorks();
  #endif
  
  if (mode == MODE_SERVER) {
    //prof works
    profStart();
    timersWorks();

    #ifdef SERVER_FEATURE
      serverWorks();
    #endif 
  
    // Light works
  
    #ifdef KEYS_FEATURE
      keysWorks();
    #endif  
  
    // Sensors works
 
    #ifdef PIRS_FEATURE
      pirsWorks();
    #endif   
  
    #ifdef CONTACTS_FEATURE
      contactsWorks();
    #endif   

    #ifdef TEMP_FEATURE
      if (cycle1m || (timeSec < startSendTime)) {
        tempWorks();
      }
    #endif 
  
    #ifdef ELECTRO_FEATURE
      if (modulElectro == 1) {
        if (cycle20s || (timeSec < startSendTime)) {
          electroWorks();
        }
      }
    #endif
  
    #ifdef MAJORDOMO_FEATURE
      majordomoMegaLive();
    #endif

    #ifdef SD_FEATURE
      if (cycle30s) {
        //printSDcontent(SDroot, 0);
        //cardInfo();
        //Serial.println("Root SD card:");
        //printDirectory(SDdir);
        //Serial.println("/js/");
        //SDdir = SD.open("/js/");
        //printDirectory(SDdir);
      }
      // if (cycle1m) {printSDcontent(SDroot, 0);}
    #endif

    #ifdef PING_FEATURE
      if (cycle30s) {
        pingWorks();
      }
    #endif
    
    cyclosInSecWork();

    if (cycle1s) {

      }

    
    
    // End loop works
    eraseCyclos();
    profCalc();

    cyclosDelayWork();

  } // if (mode == MODE_SERVER)
  
} // loop

