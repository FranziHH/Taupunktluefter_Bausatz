//////////////////////////////////////////////////////////////////////////////
// ESP32-TaupunktLüfter 
// mit dem ESP32 Wroom 32
// für Arduino-ESP32 Release v3.3.8 
//
// Ulrich Schmerold
// 06/2026
//////////////////////////////////////////////////////////////////////////////
#define max_size_errorlist 4200  // Länge der Fehlerliste begrenzen auf 21 byte/Fehler * 200

String Get_Fehlertext_from_Nummer(int Fehlernummer)
{
 switch (Fehlernummer) {
  case 101:return ( print_Reset_reason(Fehlernummer)) ;
  case 102:return ( print_Reset_reason(Fehlernummer)) ;
  case 103:return ( print_Reset_reason(Fehlernummer)) ;
  case 104:return ( print_Reset_reason(Fehlernummer)) ;
  case 105:return ( print_Reset_reason(Fehlernummer)) ;
  case 106:return ( print_Reset_reason(Fehlernummer)) ;
  case 107:return ( print_Reset_reason(Fehlernummer)) ;
  case 108:return ( print_Reset_reason(Fehlernummer)) ;
  case 109:return ( print_Reset_reason(Fehlernummer)) ;
  case 110:return ( print_Reset_reason(Fehlernummer)) ;
  
  case 120:return (F("LittleFS: An Error has occurred while mounting LittleFS"));
  case 121:return (F("LitlteFS: Failed to open file for reading: fileDateTime"));
  case 122:return (F("LittleFS: An Error has occurred while mounting LittleFS"));
  case 123:return (F("LittleFS: Failed to open file for reading"));
  case 124:return (F("LittleFS: An Error has occurred while mounting LittleFS"));
  case 126:return (F("LittleFS: Fehler beim Schreiben der Datei ..."));
  case 127:return (F("LittleFS: An Error has occurred while mounting LittleFS"));
  case 128:return (F("LittleFS: Fehler beim öffnen der Datei /Fehler.txt"));
  case 129:return (F("LittleFS: Fehler beim Formatieren"));
  case 130:return (F("LittleFS: Datei ist leer(evtl. formatiert?)"));

  case 140:return (F("LittleFS: Wurde formatiert"));
  case 141:return (F("LittleFS: Wurde formatiert(kurz)"));
    
  case 201:return (F("Fehler beim Auslesen von Sensor 1 (Innen)")); 
  case 202:return (F("Fehler beim Auslesen von Sensor 2 (Aussen)")); 

   case 300:return (F("get_Local_Time: Keine Zeit ermittelt"));
   case 301:return (F("WIFI ist nicht verbunden - kein weiterer Versuch!"));
   case 302:return (F("WIFI ist unterbrochen! Versuche neu zu verbinden - Leider kein Erfolg! - ESP32 Neustart"));
   
   case 400:return (F("Radon: Der RadonLüfter läuft los"));   
   case 401:return (F("Radon: Der RadonLüfter schaltet ab"));

  case 500:return  (F("Fehler: Chronik: Datum und Zeit testen"));
  case 501:return  (F("Fehler: Chronik: Innentemperatur testen"));
  case 502:return  (F("Fehler: Chronik: Außentemperatur testen"));
  case 503:return  (F("Fehler: Chronik: Innen-Luftfeuchte testen"));
  case 504:return  (F("Fehler: Chronik: Außen-Luftfeuchte testen")); 
  case 505:return  (F("Fehler: Chronik: Unterschied der Taupunkte testen"));
  case 506:return  (F("Fehler: Chronik: Taupunkt 1 testen"));
  case 507:return  (F("Fehler: Chronik: Taupunkt 2 testen"));
  case 508:return  (F("Fehler: Chronik: Lüfter_Laufzeit"));
  
  default: return("");
 }          
}

//---------------------------------------------------------------------- Fehleraufzeichnung -----------------------------

void trim_length_errorlist() {
  String path ="/Fehler.txt";
  String tempPath = "/temp.bin";
  if (!LittleFS.begin()) Serial_Debugging_println(F("An Error has occurred while mounting LittleFS")); 
  File original = LittleFS.open(path, "r");
  //if(!original) {Fehler_speichern(128); return;}

  size_t currentSize = original.size();
   if (currentSize < max_size_errorlist) 
  {
    original.close();    
    return;
  }
  int to_erase = currentSize - max_size_errorlist;
  if (to_erase <= 420) return; // nichts zu tun!
  File temp = LittleFS.open(tempPath, "w");
  if (!temp) {
    Serial_Debugging_println(F("Fehler: Temp-Datei konnte nicht erstellt werden"));
    original.close();
    return;
  }
   original.seek(to_erase);
  uint8_t buffer[128]; 
  while (original.available()) 
  {
    size_t bytesRead = original.read(buffer, sizeof(buffer));
    if (bytesRead > 0) {
      temp.write(buffer, bytesRead);
    }
  }
  original.close();
  temp.close();

  if (LittleFS.remove(path)) {
    if (LittleFS.rename(tempPath, path)) {
      Serial.println("Erfolg: Datei verkleinert.");
    } else {
      Serial.println("Fehler: Rename gescheitert (Temp vorhanden, aber Rename blockiert).");
    }
  } else {
    Serial.println("Fehler: Original belegt oder kann nicht gelöscht werden!");
  }
}

void Fehler_speichern(int Nummer) {

   trim_length_errorlist();
  if (!LittleFS.begin()) Serial_Debugging_println(F("An Error has occurred while mounting LittleFS"));
   else{
   File file = LittleFS.open("/Fehler.txt", FILE_APPEND); 
   String buffer =   String(Nummer) + "-" + make_time_stamp() + ';';
   file.print(buffer);
   file.close();  
   Serial.println(buffer);  
   }
 }

void read_Fehler_from_FS()
{
    char Zeichen;
    String Puffer = "";
    String Fehlernummer;


if (!LittleFS.begin())Serial_Debugging_println("Fehler: read_Fehler_from_FS()/LittleFS.begin()");
   else{
   File file = LittleFS.open("/Fehler.txt", FILE_READ);
   if (file) 
   {
    FehlerString = "";    
     while(file.available())
     {
      Zeichen = (char(file.read()));
      Puffer += Zeichen;
      if (Zeichen == ';')
      {
        Fehlernummer = (Puffer.substring(0,3));
        FehlerString =  FehlerString + "<br>" + (Puffer + "=>" + Get_Fehlertext_from_Nummer(Fehlernummer.toInt()));
        Puffer="";
      }
     }file.close();
   }  
  }
  
}
