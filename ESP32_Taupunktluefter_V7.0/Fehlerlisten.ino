//////////////////////////////////////////////////////////////////////////////
// ESP32-TaupunktLüfter
// mit dem ESP32 Wroom 32
// für Arduino-ESP32 Release v3.3.8
//
// Ulrich Schmerold
// 06/2026
//////////////////////////////////////////////////////////////////////////////
#define max_size_errorlist 4200 // Länge der Fehlerliste begrenzen auf 21 byte/Fehler * 200

String Get_Fehlertext_from_Nummer(int Fehlernummer) {
  // Wenn es sich um die Reset-Codes handelt, direkt die Funktion aufrufen
  if (Fehlernummer >= 101 && Fehlernummer <= 110) {
    return print_Reset_reason(Fehlernummer);
  }

  const __FlashStringHelper* fehlerText = F("");

  switch (Fehlernummer) {
    // Gleiche LittleFS-Fehler zusammenfassen
    case 120:
    case 122:
    case 124:
    case 127: fehlerText = F("LittleFS: An Error has occurred while mounting LittleFS"); break;
    
    case 121: fehlerText = F("LittleFS: Failed to open file for reading: Datum/Zeit"); break;

    case 123: fehlerText = F("LittleFS: Failed to open file for reading"); break;

    case 924: fehlerText = F("LittleFS: Failed to open file for reading: Innentemperatur"); break;
    case 925: fehlerText = F("LittleFS: Failed to open file for reading: Außentemperatur"); break;
    case 926: fehlerText = F("LittleFS: Failed to open file for reading: Luftfeuchte innen"); break;
    case 927: fehlerText = F("LittleFS: Failed to open file for reading: Luftfeuchte außen"); break;
    case 928: fehlerText = F("LittleFS: Failed to open file for reading: Taupunkt innen"); break;
    case 929: fehlerText = F("LittleFS: Failed to open file for reading: Taupunkt außen"); break;
    case 930: fehlerText = F("LittleFS: Failed to open file for reading: Delta Taupunkt"); break;
    case 931: fehlerText = F("LittleFS: Failed to open file for reading: Lüfter Laufzeit"); break;
    case 932: fehlerText = F("LittleFS: Failed to open file for reading: Radonsignal"); break;
    
    case 126: fehlerText = F("LittleFS: Fehler beim Schreiben der Datei ..."); break;
    case 128: fehlerText = F("LittleFS: Fehler beim öffnen der Datei /Fehler.txt"); break;
    case 129: fehlerText = F("LittleFS: Fehler beim Formatieren"); break;
    case 130: fehlerText = F("LittleFS: Datei ist leer(evtl. formatiert?)"); break;

    case 140: fehlerText = F("LittleFS: Wurde formatiert"); break;
    case 141: fehlerText = F("LittleFS: Wurde formatiert(kurz)"); break;

    case 201: fehlerText = F("Fehler beim Auslesen von Sensor 1 (Innen)"); break;
    case 202: fehlerText = F("Fehler beim Auslesen von Sensor 2 (Aussen)"); break;

    case 300: fehlerText = F("get_Local_Time: Keine Zeit ermittelt"); break;
    case 301: fehlerText = F("WIFI ist nicht verbunden - kein weiterer Versuch!"); break;
    case 302: fehlerText = F("WIFI ist unterbrochen! Versuche neu zu verbinden - Leider kein Erfolg! - ESP32 Neustart"); break;

    case 400: fehlerText = F("Radon: Der RadonLüfter läuft los"); break;
    case 401: fehlerText = F("Radon: Der RadonLüfter schaltet ab"); break;

    case 500: fehlerText = F("Fehler: Chronik: Datum und Zeit testen"); break;
    case 501: fehlerText = F("Fehler: Chronik: Innentemperatur testen"); break;
    case 502: fehlerText = F("Fehler: Chronik: Außentemperatur testen"); break;
    case 503: fehlerText = F("Fehler: Chronik: Innen-Luftfeuchte testen"); break;
    case 504: fehlerText = F("Fehler: Chronik: Außen-Luftfeuchte testen"); break;
    case 505: fehlerText = F("Fehler: Chronik: Unterschied der Taupunkte testen"); break;
    case 506: fehlerText = F("Fehler: Chronik: Taupunkt 1 testen"); break;
    case 507: fehlerText = F("Fehler: Chronik: Taupunkt 2 testen"); break;
    case 508: fehlerText = F("Fehler: Chronik: Lüfter_Laufzeit"); break;

    default:  return (""); // Unbekannte Nummer
  }

  // Erst ganz am Ende wird die Nummer einmal zentral vorangestellt!
  return String(Fehlernummer) + "|" + FPSTR(fehlerText);
}

//---------------------------------------------------------------------- Fehleraufzeichnung -----------------------------

void trim_length_errorlist() {
  String path = "/Fehler.txt";
  String tempPath = "/temp.bin";
  if (!LittleFS.begin()) {
    Serial_Debugging_println(F("An Error has occurred while mounting LittleFS"));
  }
  File original = LittleFS.open(path, "r");
  // if(!original) {Fehler_speichern(128); return;}

  size_t currentSize = original.size();
  if (currentSize < max_size_errorlist) {
    original.close();
    return;
  }
  int to_erase = currentSize - max_size_errorlist;
  if (to_erase <= 420) {
    return; // nichts zu tun!
  }
  File temp = LittleFS.open(tempPath, "w");
  if (!temp) {
    Serial_Debugging_println(F("Fehler: Temp-Datei konnte nicht erstellt werden"));
    original.close();
    return;
  }
  original.seek(to_erase);
  uint8_t buffer[128];
  while (original.available()) {
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
  if (!LittleFS.begin()) {
    Serial_Debugging_println(F("An Error has occurred while mounting LittleFS"));
  } else {
    File file = LittleFS.open("/Fehler.txt", FILE_APPEND);
    String buffer = String(Nummer) + "-" + make_time_stamp() + ';';
    file.print(buffer);
    file.close();
    Serial.println(buffer);
  }
}

void read_Fehler_from_FS() {
  char Zeichen;
  String Puffer = "";

  if (!LittleFS.begin()) {
    Serial_Debugging_println("Fehler: read_Fehler_from_FS()/LittleFS.begin()");
  } else {
    File file = LittleFS.open("/Fehler.txt", FILE_READ);
    if (file) {
      FehlerString = "";
      while (file.available()) {
        Zeichen = (char(file.read()));
        Puffer += Zeichen;
        if (Zeichen == ';') {
          Puffer.remove(Puffer.length() - 1);
          Puffer.trim(); // Leerzeichen entfernen
          
          // Prüfen, ob es der Lösch-Eintrag ist
          if (Puffer.startsWith("---")) {
            // zB --- Protokoll gelöscht ---
            FehlerString = FehlerString + "<br>" + Puffer;
          } else {
            // Normaler Fehlerdatensatz (z.B. "201-30.07.26 12:08;")
            // Die ID sind die ersten 3 Zeichen vor dem Bindestrich
            String Fehlernummer = Puffer.substring(0, 3);
            String Zeitstempel = Puffer.substring(4);
            FehlerString = FehlerString + "<br>" + Zeitstempel + " => " + Get_Fehlertext_from_Nummer(Fehlernummer.toInt());
          }
          Puffer = "";
        }
      }
      file.close();
    }
  }
}