//////////////////////////////////////////////////////////////////////////////
// ESP32-TaupunktLüfter 
// mit dem ESP32 Wroom 32
// für Arduino-ESP32 Release v3.3.8 
//
// Ulrich Schmerold
// 06/2026
//////////////////////////////////////////////////////////////////////////////

void Serial_Debugging_println(String f){ if (debugging == true) Serial.println(f);}
void Serial_Debugging_println_and_wait(String f, int t){ if (debugging == true) {Serial.println(f);}}
void Serial_Debugging_print(String f){ if (debugging == true) Serial.print(f);}
void Serial_Debugging_write(char c){ if (debugging == true) Serial.write(c);}
//-----------------------------------------------------------------------------------------------------------
void TONE (int Frequenz, int Dauer)
{
  tone(Tone_pin, Frequenz); // 1000 Hz Ton an
  delay(Dauer);             // warten...
  noTone(Tone_pin);         // Ton aus
}



void list_all_LittleFS_files()
{
    if (!LittleFS.begin()) {
    Serial.println(F("An Error has occurred while mounting LittleFS"));
    return;
  } 
  File root = LittleFS.open("/"); 
  File file = root.openNextFile();
   
  while(file){ 
      Serial.print("FILE: ");
      Serial.print(file.name()); 
      Serial.print(" | Filesize: ");
      Serial.println(file.size());
      file = root.openNextFile();
  }
}

void Flash_available()
{
  Serial.println("Momentan sind "+String(ESP.getFreeHeap())+" Byte Flash-Speicher verfügbar");
}

void print_Stack_size()
{
  Serial.printf("Arduino Stack was set to %d bytes\n", getArduinoLoopTaskStackSize());
  // Print unused stack for the task that is running setup()
  Serial.printf("Free Stack Space: %d\n", uxTaskGetStackHighWaterMark(NULL));
 
 // SET_LOOP_TASK_STACK_SIZE(16*1024); // 16KB
}


void Stack_Size()
{
  ///////////////////////////////////////////////////////
  // Hinweis: SET_LOOP_TASK_STACK_SIZE(16*1024); // 16KB
  // würde die Größe des Stacks auf 15KB vergrößern.
  // muss vor Setup() eingefügt werden
  //////////////////////////////////////////////////////
 void * StackPtrAtStart;
 void * StackPtrEnd;
 UBaseType_t watermarkStart;
 {
   void* SpStart = NULL;
   StackPtrAtStart = (void *)&SpStart;
   watermarkStart =  uxTaskGetStackHighWaterMark(NULL);
   StackPtrEnd = StackPtrAtStart - watermarkStart;

  Serial.printf("\r\n\r\nAddress of Stackpointer near start is:  %p \r\n",  (void *)StackPtrAtStart);
  Serial.printf("End of Stack is near: %p \r\n",  (void *)StackPtrEnd);
  Serial.printf("Free Stack near start is:  %d \r\n",  (uint32_t)StackPtrAtStart - (uint32_t)StackPtrEnd);
 
 void* SpActual = NULL;
 Serial.printf("Free Stack at actual position is: %d \r\n", (uint32_t)&SpActual - (uint32_t)StackPtrEnd);
 }
}
//-----------------------------------------------------------------------------------------------------------

void delay_Timer(String Text , int interval )
{
  int n = interval/100; 
  interval = interval/10;
  Serial.print("--> Wait without delay for "+Text + ' '+ String(n) +"sec.:");
  while (n>0)
  {
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > interval)
    {
     previousMillis = currentMillis;
     Serial.print(String(n) + '-'); 
     n--;
    }
  }Serial.println(" fertig!");
}
//-----------------------------------------------------------------------------------------------------------
/*
void soft_reset(byte Timeout)
{
  for (Timeout; Timeout > 0; Timeout--)
  { 
    Serial.println(" Neustart wird in " + String(Timeout) + " Sekunden ausgeführt");
    delay(1000);
  }
  ESP.restart();
}*/
//-------------------------------------------

void soft_reset(byte Timeout)
{
   oled.invertText(false);
   oled.setScale(1);
   OLED_clear();
   OLED_println("CPU Neustart in",1,2); 

  for (Timeout; Timeout > 0; Timeout--)
  { 
    oled.setScale(2);
    OLED_println(String(Timeout - 1),3,7);
    oled.setScale(1);
    OLED_println("Sekunden",6,4);
    OLED_update();
    delay(1000);
  }
  OLED_clear();
  OLED_update();
  ESP.restart();
}
//-----------------------------------------------------------------------------------------------------------

void print_Core_task_is_running(String Hinweistext)  //-Ermittelt, auf welchem Core die aktuelle Task läuft
{
  Serial.println(Hinweistext+ "Core-"+String(xPortGetCoreID()));
}
//-----------------------------------------------------------------------------------------------------------

void WiFiEvent(WiFiEvent_t event)
{
    if(debugging == true) Serial.printf("[WiFi-event] event: %d\n", WiFiEvent);
    TONE (800,400);
    
    switch (event) {
        case ARDUINO_EVENT_WIFI_READY: 
            Serial.println("WiFi interface ready");
            break;
        case ARDUINO_EVENT_WIFI_SCAN_DONE:
            Serial.println("Completed scan for access points");
            break;
        case ARDUINO_EVENT_WIFI_STA_START:
            Serial.println("WiFi client started");
            break;
        case ARDUINO_EVENT_WIFI_STA_STOP:
            Serial.println("WiFi clients stopped");
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.println("Connected to access point");
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("Disconnected from WiFi access point");
            break;
        case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
            Serial.println("Authentication mode of access point has changed");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.print("Obtained IP address: ");
            Serial.println(WiFi.localIP());
            break;
        case ARDUINO_EVENT_WIFI_STA_LOST_IP:
            Serial.println("Lost IP address and IP address is reset to 0");
            break;
        case ARDUINO_EVENT_WPS_ER_SUCCESS:
            Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_FAILED:
            Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_TIMEOUT:
            Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_PIN:
            Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
            break;
        case ARDUINO_EVENT_WIFI_AP_START:
            Serial.println("WiFi access point started");
            break;
        case ARDUINO_EVENT_WIFI_AP_STOP:
            Serial.println("WiFi access point  stopped");
            break;
        case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
            Serial.println("Client connected");
            break;
        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
            Serial.println("Client disconnected");
            break;
        case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
            Serial.println("Assigned IP address to client");
            break;
        case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
            Serial.println("Received probe request");
            break;
        case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
            Serial.println("AP IPv6 is preferred");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
            Serial.println("STA IPv6 is preferred");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP6:
            Serial.println("Ethernet IPv6 is preferred");
            break;
        case ARDUINO_EVENT_ETH_START:
            Serial.println("Ethernet started");
            break;
        case ARDUINO_EVENT_ETH_STOP:
            Serial.println("Ethernet stopped");
            break;
        case ARDUINO_EVENT_ETH_CONNECTED:
            Serial.println("Ethernet connected");
            break;
        case ARDUINO_EVENT_ETH_DISCONNECTED:
            Serial.println("Ethernet disconnected");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP:
            Serial.println("Obtained IP address");
            break;
        default: break;
    }
  }

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
}

String print_Reset_reason(int Fehlernummer = 0)
{
  String Fehlertext = "";
  esp_reset_reason_t reason = esp_reset_reason();
     
    if ((reason == ESP_RST_UNKNOWN) or (Fehlernummer == 100))
      {
        Fehlertext=("Reset reason can not be determined");
        if (Fehlernummer == 0) Fehler_speichern(100);
      }
    if ((reason == ESP_RST_POWERON) or (Fehlernummer == 101))
      {  if (Fehlernummer == 0)  Fehler_speichern(101);
          Fehlertext = ("Reset due to power-on event");
      }   
    if ((reason == ESP_RST_EXT) or (Fehlernummer == 102))
        {
         if (Fehlernummer == 0) Fehler_speichern(102);
          Fehlertext = ("Reset by external pin (not applicable for ESP32)");
        }    
    if ((reason == ESP_RST_SW) or (Fehlernummer == 103))
        {  
         if (Fehlernummer == 0) Fehler_speichern(103);
          Fehlertext = ("Software reset via esp_restart");
        }
    if ((reason == ESP_RST_PANIC) or (Fehlernummer == 104))
        {
         if (Fehlernummer == 0) Fehler_speichern(104);
          Fehlertext = ("Software reset due to exception/panic");
        }
    if ((reason == ESP_RST_INT_WDT) or (Fehlernummer == 105))
        {
         if (Fehlernummer == 0) Fehler_speichern(105);
          Fehlertext = ("Reset (software or hardware) due to interrupt watchdog");
        }
    if ((reason == ESP_RST_TASK_WDT) or (Fehlernummer == 106))
        {
         if (Fehlernummer == 0) Fehler_speichern(106);
          Fehlertext = ("Reset due to task watchdog");
        }
    if ((reason == ESP_RST_WDT) or (Fehlernummer == 107))
        {         
         if (Fehlernummer == 0) Fehler_speichern(107);
          Fehlertext = ("Reset due to other watchdogs");
        }
    if ((reason == ESP_RST_DEEPSLEEP) or (Fehlernummer == 108))
        {
         if (Fehlernummer == 0) Fehler_speichern(108);
          Fehlertext = ("Reset after exiting deep sleep mode");
        }
    if ((reason == ESP_RST_BROWNOUT) or (Fehlernummer == 109))
        {
         if (Fehlernummer == 0)  Fehler_speichern(109);
         Fehlertext = ("Brownout reset (software or hardware)");
        }
    if ((reason == ESP_RST_SDIO) or (Fehlernummer == 110))
        {       
         if (Fehlernummer == 0) Fehler_speichern(110);
          Fehlertext = ("Reset over SDIO");
        }
 return (String(Fehlernummer) + "|" + Fehlertext);
}


//--------------------------------------- Watchdog --------------------------------------------
void Watchdog_reset()
{
  esp_task_wdt_reset();
}


void hw_wdt_disable(){
   Serial_Debugging_println(" Der Watchdog (  hw_wdt ) wird ausgeschaltet");
  *((volatile uint32_t*) 0x60000900) &= ~(1); // Hardware WDT OFF
}

void hw_wdt_enable(){
    Serial_Debugging_println(" Der Watchdog ( hw_wdt ) wird eingeschaltet");
  *((volatile uint32_t*) 0x60000900) |= 1; // Hardware WDT ON
}

//---------------------------- Internet Einstellungen auf Serial-Monitor ausgeben ----------
void Serial_print_Optionen()
{
  Serial.print("SSID:      ");Serial.println(ssid);
  Serial.print("password   ");Serial.println(password);
  Serial.print("local_IP:  ");Serial.println(local_IP);
  Serial.print("gateway:   ");Serial.println(gateway);
  Serial.print("dns1:      ");Serial.println(dns1);
  Serial.print("dns2:      ");Serial.println(dns2);
}

void serial_debugging_all()
  { 
    Serial.println(F("========= Speicherverwaltung ========="));
    Flash_available();
    //Stack_Size();
    print_Stack_size();
    
    Serial.println(F("========= Dateien LittleFS ==========="));
    LittleFS_print_space();
    list_all_LittleFS_files();
    Serial.println();
    Serial.println(F("=========== gesetzte Optionen ======="));
    Optionen_Serial_ausgeben();
    
    Serial.println(F("=========== Daten-Verwaltung ========="));
    daten_serial_ausgeben();
    
    Serial.println(F("================== WIFI =============="));
    SerialprintWifiStatus();
    groesse_raw_Literal();
    Serial.println(F("======================================="));
    Serial.println("Fehlerliste: ");
    read_Fehler_from_FS();
    Serial.println(FehlerString);
  }

 
void SpeicherGroessen() //------------------------------ Für HTML Ausgabe -------------------------
{
  SpeicherString  = "Freier Stack - Speicher: "    + String(uxTaskGetStackHighWaterMark(NULL)) + " Byte<br>";
  SpeicherString += "Freier Flash - Speicher: "    + String(ESP.getFreeHeap())                 + " Byte<br>";
  SpeicherString += "Min. freier Flash-Speicher: " + String(ESP.getMinFreeHeap())              + " Byte<br><br>";
   LittleFS.begin();
    int tBytes = LittleFS.totalBytes(); 
    int uBytes = LittleFS.usedBytes();
   SpeicherString += "LittleFS.totalBytes: "+ String(tBytes) + "<br>"; 
   SpeicherString += "LittleFS.usedBytes: " + String(uBytes) + "<br>"; 
   SpeicherString += "LittleFS.freeBytes: " + String(tBytes-uBytes) + "<br>";
   SpeicherString += " das entspricht "   + String((tBytes-uBytes)/(Chart_TimeDate_length+laenge_Datensatz*7)) + " Datens&aumltze <br>"; 
}

  void serial_WIFI_Status()
  {
    if (WiFi.status() == WL_CONNECTED) Serial.println("WIFI ist CONNECTED"); else Serial.println("WIFI ist DISCONNECTED");
  }

  void SerialprintWifiStatus()
{ 
  if (WiFi.status() == WL_CONNECTED) Serial.println("* WIFI_Status: CONNECTED"); else Serial.println("* WIFI_Status: DISCONNECTED");
  Serial.print("* Hostname: ");
  Serial.println(WiFi.getHostname());
  Serial.print("* SSID: ");
  Serial.println(WiFi.SSID());
  Serial.println("* IP Addresse: " + WiFi.localIP().toString());
  Serial.println("* Signalstärke (RSSI):" + String(WiFi.RSSI()) + " dBm");
  make_time_stamp ();
  Serial.println("* TimeStamp: " + stamp); 
}

 void Make_WIFI_Status_String()
 {
  WIFI_Status_String  = "Hostname:..........: " + String(WiFi.getHostname()) + "<br>";
  WIFI_Status_String += "IP Addresse:.......: " + String(WiFi.localIP().toString()) + "<br>";
  WIFI_Status_String += "SSID:..................: " + WiFi.SSID()+ "<br>";
  //WIFI_Status_String += "DNS1...............: "+ WiFi.DNS1().toString() + "<br>";
  //WIFI_Status_String += "DNS2...............: "+ WiFi.dns2().toString() + "<br>";
  WIFI_Status_String += "Gateway IP.........: "+ WiFi.gatewayIP().toString() + "<br>";
  WIFI_Status_String += "Signalst&aumlnrke (RSSI): " + String(WiFi.RSSI()) + " dBm<br>";
  WIFI_Status_String += "<br>============ TimeStamp ============<br>";
  make_time_stamp ();
  WIFI_Status_String += "TimeStamp..........: " + stamp;
 }
 
