//////////////////////////////////////////////////////////////////////////////
// ESP32-TaupunktLüfter
// mit dem ESP32 Wroom 32
// für Arduino-ESP32 Release v3.3.8
//
// Ulrich Schmerold
// 06/2026
//////////////////////////////////////////////////////////////////////////////

#include "LittleFS.h"
#define fileDateTime "/FDT.txt"  // File_Date_Time ==> Labels für die Charts
#define chronik_interval 3600    // 3600 = eine Stunde, 900 =15 Minuten, 60 = 1 Minuten
#define Chart_TimeDate_length 17 //'01.01.26 04:22', ==> 17 Zeichen
#define laenge_Datensatz 8       //'-16.4', ==> 8 Zeichen

unsigned long last_save = 0;
int Anzahl_Werte = 0;

String tmp_Chart_TimeDate;
String tmp_Chart_temp_in;
String tmp_Chart_temp_out;
String tmp_Chart_air_in;
String tmp_Chart_air_out;
String tmp_Chart_tp_delta;
String tmp_Chart_tp_in;
String tmp_Chart_tp_out;
String tmp_Chart_lz;
String tmp_Chart_lzR;
//------------------------------------- Alle Daten löschen -----------------------------------------------------------------------
void Datenstreams_leeren() {
  Chart_TimeDate = "";
  Chart_temp_in = "";
  Chart_temp_out = "";
  Chart_air_in = "";
  Chart_air_out = "";
  Chart_tp_delta = "";
  Chart_tp_in = "";
  Chart_tp_out = "";
  Chart_lz = "";
  Chart_lzR = "";
}

//--------------------------------------- Datenstream Anzahl der Datensätze Prüfen und begrenzen ------------------------------------
String trim_length_datastream(String Datenstream, int laenge_pro_Datensatz) {
  int Anzahl_Werte = Datenstream.length() / laenge_pro_Datensatz;
  while (Anzahl_Werte > (Datenspeicher - 1)) {
    Datenstream.remove(0, 1);
    while ((Datenstream.substring(0, 1) != ",") and (Datenstream.length() > 0)) {
      Datenstream.remove(0, 1);
    }
    if (Datenstream.substring(0, 1) == ",") {
      Datenstream.remove(0, 1); // verbleibendes Komma am am Anfang des Datenstreams entfernen
    }
    Anzahl_Werte = (Datenstream.length() / laenge_pro_Datensatz);
  }
  return Datenstream;
}

bool test_TimeDate() //--------------------------------------- Datum und Zeit  testen für Chart labels
{
  int i;
  char c;
  Watchdog_reset();
  make_time_stamp(); // '01.01.26 04:22',

  // Tag
  i = stamp.substring(0, 2).toInt();
  if ((i < 1) or (i > 31)) {
    return false;
  }
  c = stamp.charAt(2);
  if (c != '.') {
    return false;
  }

  // Monat
  i = stamp.substring(3, 5).toInt();
  if ((i < 1) or (i > 12)) {
    return false;
  }
  c = stamp.charAt(5);
  if (c != '.') {
    return false;
  }

  // Jahr
  i = stamp.substring(6, 8).toInt();
  if ((i < 26) or (i > 99)) {
    return false;
  }
  c = stamp.charAt(8);
  if (c != ' ') {
    return false;
  }

  // Stunde
  i = stamp.substring(9, 11).toInt();
  if ((i < 0) or (i > 23)) {
    return false;
  }
  c = stamp.charAt(11);
  if (c != ':') {
    return false;
  }

  // Minute
  i = stamp.substring(12, 14).toInt();
  if ((i < 0) or (i > 59)) {
    return false;
  }

  tmp_Chart_TimeDate = Chart_TimeDate;
  if (tmp_Chart_TimeDate.length() < 16) {
    tmp_Chart_TimeDate = "";
  } else {
    tmp_Chart_TimeDate += ",";
  }
  tmp_Chart_TimeDate = trim_length_datastream(tmp_Chart_TimeDate, Chart_TimeDate_length);
  tmp_Chart_TimeDate = tmp_Chart_TimeDate + "'" + stamp + "'";
  return true;
} //-------------------------------------------------------------------------------------------
bool test_temp_in() //--------------------------------------- Innentemperatur testen
{
  char buffer[6];
  Watchdog_reset(); // Watchdog zurücksetzen
  if (isnan(t1) or (t1 < -50) or (t1 > 60)) {
    return false; // Testen, ob der Wert gültig sein kann, wenn nicht, wird die Funktion abgebrochen und false zurückgegeben
  }
  tmp_Chart_temp_in = Chart_temp_in; // Stream sicherheitshalber ersteinmal einem temporär-Streem zuweisen
  if (tmp_Chart_temp_in.length() < 7) {
    tmp_Chart_temp_in = "";
  } else {
    tmp_Chart_temp_in += ","; // Ist schon ein Wert vorhanden dann "," einfügen, sonst Datenstream komplett leeren
  }
  tmp_Chart_temp_in = trim_length_datastream(tmp_Chart_temp_in, laenge_Datensatz); // temporären Datenstream auf vorgegebene Länge prüfen / kürzen
  dtostrf(t1, 5, 1, buffer);                                                       // Den Sensorwert in das korrekte Format bringen: 5 Stellen, eine Nachkommastelle (-10,2)
  tmp_Chart_temp_in = tmp_Chart_temp_in + "'" + buffer + "'";                      // Den Sensorwert an den temporären Datenstream anhängen
  return true;                                                                     // Rückgabewert: alles gut gelaufen
} //-------------------------------------------------------------------------------------------
bool test_temp_Out() //--------------------------------------- Außentemperatur testen
{
  char buffer[6];
  Watchdog_reset(); // Watchdog zurücksetzen
  if (isnan(t2) or (t2 < -50) or (t2 > 60)) {
    return false; // Testen, ob der Wert gültig sein kann, wenn nicht, wird die Funktion abgebrochen und false zurückgegeben
  }
  tmp_Chart_temp_out = Chart_temp_out; // Stream sicherheitshalber ersteinmal einem temporär-Streem zuweisen
  if (tmp_Chart_temp_out.length() < 7) {
    tmp_Chart_temp_out = "";
  } else {
    tmp_Chart_temp_out += ","; // Ist schon ein Wert vorhanden dann "," einfügen, sonst Datenstream komplett leeren
  }
  tmp_Chart_temp_out = trim_length_datastream(tmp_Chart_temp_out, laenge_Datensatz); // temporären Datenstream auf vorgegebene Länge prüfen / kürzen
  dtostrf(t2, 5, 1, buffer);                                                         // Den Sensorwert in das korrekte Format bringen: 5 Stellen, eine Nachkommastelle (-10,2)
  tmp_Chart_temp_out = tmp_Chart_temp_out + "'" + buffer + "'";                      // Den Sensorwert an den temporären Datenstream anhängen
  return true;                                                                       // Rückgabewert: alles gut gelaufen

} //-----------------------------------------------------------------------------------------------
bool test_air_in() //--------------------------------------- Innen-Luftfeuchte testen
{
  char buffer[6];
  Watchdog_reset(); // Watchdog zurücksetzen
  if (isnan(h1) or (h1 < 0) or (h1 > 100)) {
    return false; // Testen, ob der Wert gültig sein kann, wenn nicht, wird die Funktion abgebrochen und false zurückgegeben
  }
  tmp_Chart_air_in = Chart_air_in; // Stream sicherheitshalber ersteinmal einem temporär-Streem zuweisen
  if (tmp_Chart_air_in.length() < 7) {
    tmp_Chart_air_in = "";
  } else {
    tmp_Chart_air_in += ","; // Ist schon ein Wert vorhanden dann "," einfügen, sonst Datenstream komplett leeren
  }
  tmp_Chart_air_in = trim_length_datastream(tmp_Chart_air_in, laenge_Datensatz); // temporären Datenstream auf vorgegebene Länge prüfen / kürzen
  dtostrf(h1, 5, 0, buffer);                                                     // Den Sensorwert in das korrekte Format bringen: 5 Stellen, eine Nachkommastelle (-10,2)
  tmp_Chart_air_in = tmp_Chart_air_in + "'" + buffer + "'";                      // Den Sensorwert an den temporären Datenstream anhängen
  return true;                                                                   // Rückgabewert: alles gut gelaufen

} //-------------------------------------------------------------------------------------------
bool test_air_out() //--------------------------------------- Außen-Luftfeuchte testen
{
  char buffer[6];
  Watchdog_reset(); // Watchdog zurücksetzen
  if (isnan(h2) or (h2 < 0) or (h2 > 100)) {
    return false; // Testen, ob der Wert gültig sein kann, wenn nicht, wird die Funktion abgebrochen und false zurückgegeben
  }
  tmp_Chart_air_out = Chart_air_out; // Stream sicherheitshalber ersteinmal einem temporär-Streem zuweisen
  if (tmp_Chart_air_out.length() < 7) {
    tmp_Chart_air_out = "";
  } else {
    tmp_Chart_air_out += ","; // Ist schon ein Wert vorhanden dann "," einfügen, sonst Datenstream komplett leeren
  }
  tmp_Chart_air_out = trim_length_datastream(tmp_Chart_air_out, laenge_Datensatz); // temporären Datenstream auf vorgegebene Länge prüfen / kürzen
  dtostrf(h2, 5, 0, buffer);                                                       // Den Sensorwert in das korrekte Format bringen: 5 Stellen, eine Nachkommastelle (-10,2)
  tmp_Chart_air_out = tmp_Chart_air_out + "'" + buffer + "'";                      // Den Sensorwert an den temporären Datenstream anhängen
  return true;                                                                     // Rückgabewert: alles gut gelaufen

} //--------------------------------------------------------------------------------------------
bool test_delta_TP() //--------------------------------------- Unterschied der Taupunkte 1/2 testen
{
  char buffer[6];
  Watchdog_reset(); // Watchdog zurücksetzen
  if (isnan(DeltaTP) or (DeltaTP < -50) or (DeltaTP > 60)) {
    return false; // Testen, ob der Wert gültig sein kann, wenn nicht, wird die Funktion abgebrochen und false zurückgegeben
  }
  tmp_Chart_tp_delta = Chart_tp_delta; // Stream sicherheitshalber ersteinmal einem temporär-Streem zuweisen
  if (tmp_Chart_tp_delta.length() < 7) {
    tmp_Chart_tp_delta = "";
  } else {
    tmp_Chart_tp_delta += ","; // Ist schon ein Wert vorhanden dann "," einfügen, sonst Datenstream komplett leeren
  }
  tmp_Chart_tp_delta = trim_length_datastream(tmp_Chart_tp_delta, laenge_Datensatz); // temporären Datenstream auf vorgegebene Länge prüfen / kürzen
  dtostrf(DeltaTP, 5, 1, buffer);                                                    // Den Sensorwert in das korrekte Format bringen: 5 Stellen, eine Nachkommastelle (-10,2)
  tmp_Chart_tp_delta = tmp_Chart_tp_delta + "'" + buffer + "'";                      // Den Sensorwert an den temporären Datenstream anhängen
  return true;                                                                       // Rückgabewert: alles gut gelaufen

} //----------------------------------------------------------------------------------------------

bool test_Taupunkt_1() //--------------------------------------- Taupunkt 1  testen
{
  char buffer[6];
  Watchdog_reset(); // Watchdog zurücksetzen
  if (isnan(Taupunkt_1) or (Taupunkt_1 < -50) or (Taupunkt_1 > 60)) {
    return false; // Testen, ob der Wert gültig sein kann, wenn nicht, wird die Funktion abgebrochen und false zurückgegeben
  }
  tmp_Chart_tp_in = Chart_tp_in; // Stream sicherheitshalber ersteinmal einem temporär-Streem zuweisen
  if (tmp_Chart_tp_in.length() < 7) {
    tmp_Chart_tp_in = "";
  } else {
    tmp_Chart_tp_in += ","; // Ist schon ein Wert vorhanden dann "," einfügen, sonst Datenstream komplett leeren
  }
  tmp_Chart_tp_in = trim_length_datastream(tmp_Chart_tp_in, laenge_Datensatz); // temporären Datenstream auf vorgegebene Länge prüfen / kürzen
  dtostrf(Taupunkt_1, 5, 1, buffer);                                           // Den Sensorwert in das korrekte Format bringen: 5 Stellen, eine Nachkommastelle (-10,2)
  tmp_Chart_tp_in = tmp_Chart_tp_in + "'" + buffer + "'";                      // Den Sensorwert an den temporären Datenstream anhängen
  return true;                                                                 // Rückgabewert: alles gut gelaufen

} //------------------------------------------------------------------------------------------

bool test_Taupunkt_2() //--------------------------------------- Taupunkt 2 testen
{
  char buffer[6];
  Watchdog_reset(); // Watchdog zurücksetzen
  if (isnan(Taupunkt_2) or (Taupunkt_2 < -50) or (Taupunkt_2 > 60)) {
    return false; // Testen, ob der Wert gültig sein kann, wenn nicht, wird die Funktion abgebrochen und false zurückgegeben
  }
  tmp_Chart_tp_out = Chart_tp_out; // Stream sicherheitshalber ersteinmal einem temporär-Streem zuweisen
  if (tmp_Chart_tp_out.length() < 7) {
    tmp_Chart_tp_out = "";
  } else {
    tmp_Chart_tp_out += ","; // Ist schon ein Wert vorhanden dann "," einfügen, sonst Datenstream komplett leeren
  }
  tmp_Chart_tp_out = trim_length_datastream(tmp_Chart_tp_out, laenge_Datensatz); // temporären Datenstream auf vorgegebene Länge prüfen / kürzen
  dtostrf(Taupunkt_2, 5, 1, buffer);                                             // Den Sensorwert in das korrekte Format bringen: 5 Stellen, eine Nachkommastelle (-10,2)
  tmp_Chart_tp_out = tmp_Chart_tp_out + "'" + buffer + "'";                      // Den Sensorwert an den temporären Datenstream anhängen
  return true;                                                                   // Rückgabewert: alles gut gelaufen
} //-----------------------------------------------------------------------------------------

bool test_lz() //--------------------------------------- Lüfterzeit testen
{
  char buffer[6];
  int LZ_in_Min = Luefter_Laufzeit / 60000;
  Watchdog_reset(); // Watchdog zurücksetzen
  if (isnan(LZ_in_Min) or (LZ_in_Min < 0) or (LZ_in_Min > chronik_interval)) {
    return false; // Testen, ob der Wert gültig sein kann, wenn nicht, wird die Funktion abgebrochen und false zurückgegeben
  }
  tmp_Chart_lz = Chart_lz; // Stream sicherheitshalber ersteinmal einem temporär-Streem zuweisen
  if (tmp_Chart_lz.length() < 7) {
    tmp_Chart_lz = "";
  } else {
    tmp_Chart_lz += ","; // Ist schon ein Wert vorhanden dann "," einfügen, sonst Datenstream komplett leeren
  }
  tmp_Chart_lz = trim_length_datastream(tmp_Chart_lz, laenge_Datensatz); // temporären Datenstream auf vorgegebene Länge prüfen / kürzen
  dtostrf(LZ_in_Min, 5, 0, buffer);                                      // Den Lüterwert in das korrekte Format bringen: 5 Stellen,( 3600)
  tmp_Chart_lz = tmp_Chart_lz + "'" + buffer + "'";                      // Den Lüfterwert an den temporären Datenstream anhängen
  Luefter_Laufzeit = 0;
  if (Luefter_Start > 0) {
    Luefter_Start = millis(); // Lüfter Laufzeit zurücksetzen
  }
  return true; // Rückgabewert: alles gut gelaufen
} //-----------------------------------------------------------------------------------------

bool test_lzR() //--------------------------------------- Radon Lüfterzeit testen
{
  char buffer[6];
  int LZ_in_Min = Luefter_Laufzeit_Radon / 60000;
  Watchdog_reset(); // Watchdog zurücksetzen
  if (isnan(LZ_in_Min) or (LZ_in_Min < 0) or (LZ_in_Min > chronik_interval)) {
    return false; // Testen, ob der Wert gültig sein kann, wenn nicht, wird die Funktion abgebrochen und false zurückgegeben
  }
  tmp_Chart_lzR = Chart_lzR; // Stream sicherheitshalber ersteinmal einem temporär-Streem zuweisen
  if (tmp_Chart_lzR.length() < 7) {
    tmp_Chart_lzR = "";
  } else {
    tmp_Chart_lzR += ","; // Ist schon ein Wert vorhanden dann "," einfügen, sonst Datenstream komplett leeren
  }
  tmp_Chart_lzR = trim_length_datastream(tmp_Chart_lzR, laenge_Datensatz); // temporären Datenstream auf vorgegebene Länge prüfen / kürzen
  dtostrf(LZ_in_Min, 5, 0, buffer);                                        // Den Lüterwert in das korrekte Format bringen: 5 Stellen,( 3600)
  tmp_Chart_lzR = tmp_Chart_lzR + "'" + buffer + "'";                      // Den Lüfterwert an den temporären Datenstream anhängen
  Luefter_Laufzeit_Radon = 0;
  if (Radonsignal == true) {
    Luefter_Radon_Start = millis(); // Lüfter Laufzeit zurücksetzen
  }
  return true; // Rückgabewert: alles gut gelaufen

} //-----------------------------------------------------------------------------------------

bool load_TimeDate() {
  String buffer = "";
  Watchdog_reset();
  if (!LittleFS.begin(true)) {
    Serial_Debugging_println(F("An Error has occurred while mounting LittleFS"));
    Fehler_speichern(120);
    return false;
  }

  File file = LittleFS.open(fileDateTime);
  if (!file) {
    Serial_Debugging_println(F("Failed to open file for reading: fileDateTime"));
    Fehler_speichern(121);
    OLED_Zeile_loeschen(3);
    OLED_println("Fehler: fileDateTime", 3, 0);
    OLED_update();

    return false;
  } else {
    if (file.size() < Chart_TimeDate_length) {
      LittleFS.remove(fileDateTime);
      OLED_Zeile_loeschen(3);
      OLED_println("fileDateTime gelöscht", 3, 0);
      return false;
    }

    Anzahl_Werte = (file.size() + 1) / Chart_TimeDate_length;
    Serial_Debugging_println("/FDT.txt:(" + String(Anzahl_Werte) + ")...");
    OLED_Zeile_loeschen(3);
    OLED_println("Lade: fileDateTime", 3, 0);
    OLED_println("Anzahl Werte: " + String(Anzahl_Werte) + "  ", 4, 0);
    OLED_update();

    while (file.available()) {
      buffer = buffer + char(file.read());
    }

    file.close();
    Chart_TimeDate = buffer;
  }

  return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------

String load_Datensatz(String name) {
  String buffer = "";
  Watchdog_reset();

  if (!LittleFS.begin()) {
    Serial_Debugging_println(F("An Error has occurred while mounting LittleFS"));
    Fehler_speichern(122);
    return "";
  }

  File file = LittleFS.open(name);
  if ((file.size()) < 7) {
    Fehler_speichern(130);
    return "";
  }

  if (!file) {
    Serial_Debugging_println("Failed to open file for reading: " + name);
    Fehler_speichern(123);
    OLED_Zeile_loeschen(3);
    OLED_println("Fehler: " + name, 3, 0);
    OLED_update();

    return "";
  } else {
    if (file.size() < laenge_Datensatz) {
      LittleFS.remove(name);
      OLED_Zeile_loeschen(3);
      OLED_println("fileDateTime gelöscht", 3, 0);
      return "";
    }

    Anzahl_Werte = (file.size() + 1) / laenge_Datensatz;
    Serial_Debugging_println(name + ":(" + String(Anzahl_Werte) + ")...");
    OLED_Zeile_loeschen(3);
    OLED_println("Lade: " + name, 3, 0);
    OLED_println("Anzahl Werte: " + String(Anzahl_Werte) + "  ", 4, 0);
    OLED_update();

    while (file.available()) {
      buffer = buffer + char(file.read());
    }

    file.close();
  }

  return buffer;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
bool write_LittleFS(String name, String daten) {
  Watchdog_reset();
  OLED_clear();
  OLED_println("Sensordaten speichern", 0, 0);

  if (!LittleFS.begin()) {
    Serial_Debugging_println("An Error has occurred while mounting LittleFS");
    Fehler_speichern(124);
    OLED_Zeile_loeschen(3);
    OLED_println("Fehler mount LittleFS", 3, 0);
    OLED_update();

    return false;
  }

  File file = LittleFS.open(name, FILE_WRITE);
  OLED_Zeile_loeschen(3);
  OLED_println("write:" + name, 3, 0);
  OLED_update();

  if (file.print(daten)) {
  } else {
    Serial_Debugging_print("Fehler beim Schreiben der Datei " + name + " in den LittleFS");
    Fehler_speichern(125);
    OLED_Zeile_loeschen(3);
    OLED_println("Fehler: " + name, 3, 0);
    OLED_update();

    return false;
  }

  file.close();
  Serial_Debugging_println(name + " erfolgreich gespeichert.");
  OLED_Zeile_loeschen(3);
  OLED_println("Ok:" + name, 3, 0);
  OLED_update();

  return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------

unsigned int LittleFS_print_space() {
  if (!LittleFS.begin()) {
    Serial_Debugging_println(F("An Error has occurred while mounting LittleFS"));
    return 0;
  }

  int tBytes = LittleFS.totalBytes();
  int uBytes = LittleFS.usedBytes();
  Serial.println(" LittleFS.totalBytes: " + String(tBytes));
  Serial.println(" LittleFS.usedBytes: " + String(uBytes));
  Serial.println(" LittleFS.freeBytes: " + String(tBytes - uBytes));
  Serial.println(" das entspricht " + String((tBytes - uBytes) / (laenge_Datensatz * 7 + Chart_TimeDate_length)) + " Datensätze");

  return (tBytes - uBytes);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------

bool remove_LittleFS(String name) {
  if (!LittleFS.begin()) {
    Serial_Debugging_println(F("An Error has occurred while mounting LittleFS"));
  }
  return LittleFS.remove(name);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
bool format_LittleFS_kurz() {
  if (!LittleFS.format()) {
    Fehler_speichern(129);
    return false;
  };
  Fehler_speichern(140);
  return true;
}

bool format_LittleFS() {
  Serial_Debugging_println(F("Formatiere LittleFS ...."));
  if (use_OLED == true) {
    OLED_clear();
    OLED_println(F("Formatiere LittleFS"), 2, 0);
    OLED_println(F("Bitte warten..."), 4, 0);
    OLED_update();
  }

  bool formatted = LittleFS.format();
  if (formatted) {
    if (use_OLED == true) {
      OLED_clear();
      OLED_println("Erfolgreich!", 4, 0);
      OLED_update();
    }

    Serial_Debugging_println("\n\nSuccess formatting");
  } else {
    if (use_OLED == true) {
      OLED_clear();
      OLED_println("Fehler aufgetreten!", 4, 0);
      OLED_update();
    }
    Serial_Debugging_println("\n\nError formatting");
    Fehler_speichern(126);
  }
  Fehler_speichern(141);
  return formatted;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------

bool load_Chronik_from_LittleFS() {
  int n = 0;
  while ((HTML_processor_is_working == true) and (n < 50)) {
    delay(100);
    Watchdog_reset();
    n++;
  }
  if (HTML_processor_is_working == false) {
    if (!LittleFS.begin()) {
      Serial_Debugging_println(F("An Error has occurred while mounting LittleFS"));
      Fehler_speichern(127);
      return false;
    }
    chronik_is_busy = true;
    OLED_clear();
    OLED_println("Lade LittleFS Daten", 0, 0);
    Serial_Debugging_println(F("Die Chronik wird aus dem LittleFS geladen..."));

    load_TimeDate();
    Chart_temp_in = load_Datensatz("/temp_in.txt");
    Chart_air_in = load_Datensatz("/air_in.txt");
    Chart_tp_in = load_Datensatz("/tp_in.txt");

    Chart_temp_out = load_Datensatz("/temp_out.txt");
    Chart_air_out = load_Datensatz("/air_out.txt");
    Chart_tp_out = load_Datensatz("/tp_out.txt");

    Chart_tp_delta = load_Datensatz("/tp_delta.txt");
    Chart_lz = load_Datensatz("/lz.txt");
    Chart_lzR = load_Datensatz("/lzR.txt");
    chronik_is_busy = false;
    Serial_Debugging_println("fertig!");
  }
  return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------

bool chronik_update_all() {
  if (Datenspeicher > 0) // Datenspeicherung findet nur statt, wenn die Konstante "Datenspeicherung" mit einer Zahl > 0 belegt ist
  {
    if (last_save == 0) {
      // Setze last_save so weit in die Vergangenheit,
      // dass der erste Schreibvorgang sofort stattfindet (sofern die RTC bereit ist)
      if (rtc.getEpoch() > chronik_interval) {
        last_save = rtc.getEpoch() - chronik_interval - 1;
      } else {
        return false; // RTC hat noch keine gültige Zeit
      }
    } // wurde die chronik_update_all schon einmal ausgeführt?
    if (HTML_processor_is_working == false) // Überschneidungen mit dem Server zu verhindern
    {
      if (last_save + chronik_interval < rtc.getEpoch()) // wie lange ist die letzte Datenspeicherung her?
      {
        chronik_is_busy = true;
        Serial_Debugging_println(F("Die Chronik wird überprüft und im LittleFS gespeichert..."));

        //----------------------------------- Die neuen Daten überprüfen -------------------------------------------
        if (test_TimeDate() == false) {
          Fehler_speichern(500);
          return false;
        } //--------------------------------------- Datum und Zeit testen
        if (test_temp_in() == false) {
          Fehler_speichern(501);
          return false;
        } //--------------------------------------- Innentemperatur testen
        if (test_temp_Out() == false) {
          Fehler_speichern(502);
          return false;
        } //--------------------------------------- Außentemperatur testen
        if (test_air_in() == false) {
          Fehler_speichern(503);
          return false;
        } //--------------------------------------- Innen-Luftfeuchte testen
        if (test_air_out() == false) {
          Fehler_speichern(504);
          return false;
        } //--------------------------------------- Außen-Luftfeuchte testen
        if (test_delta_TP() == false) {
          Fehler_speichern(505);
          return false;
        } //--------------------------------------- Unterschied der Taupunkte testen
        if (test_Taupunkt_1() == false) {
          Fehler_speichern(506);
          return false;
        } //--------------------------------------- Taupunkt 1  testen
        if (test_Taupunkt_2() == false) {
          Fehler_speichern(507);
          return false;
        } //--------------------------------------- Taupunkt 2 testen
        if (test_lz() == false) {
          Fehler_speichern(508);
          return false;
        } //--------------------------------- ------Lüfterzeit testen
        if (test_lzR() == false) {
          Fehler_speichern(509);
          return false;
        } //-------------------------------------- Radon Lüfterzeit testen
        // --------------------- Jetzt werden die Datenstreams gespeichert --------------------------------
        write_LittleFS(fileDateTime, tmp_Chart_TimeDate);
        write_LittleFS("/temp_in.txt", tmp_Chart_temp_in);
        write_LittleFS("/temp_out.txt", tmp_Chart_temp_out);
        write_LittleFS("/air_in.txt", tmp_Chart_air_in);
        write_LittleFS("/air_out.txt", tmp_Chart_air_out);
        write_LittleFS("/tp_delta.txt", tmp_Chart_tp_delta);
        write_LittleFS("/tp_in.txt", tmp_Chart_tp_in);
        write_LittleFS("/tp_out.txt", tmp_Chart_tp_out);
        write_LittleFS("/lz.txt", tmp_Chart_lz);
        write_LittleFS("/lzR.txt", tmp_Chart_lzR);
        // --------------------------------- Datenstreams aktualisieren ----------------------------------
        Chart_TimeDate = tmp_Chart_TimeDate;
        Chart_temp_in = tmp_Chart_temp_in;
        Chart_temp_out = tmp_Chart_temp_out;
        Chart_air_in = tmp_Chart_air_in;
        Chart_air_out = tmp_Chart_air_out;
        Chart_tp_delta = tmp_Chart_tp_delta;
        Chart_tp_in = tmp_Chart_tp_in;
        Chart_tp_out = tmp_Chart_tp_out;
        Chart_lz = tmp_Chart_lz;
        Chart_lzR = tmp_Chart_lzR;

        last_save = rtc.getEpoch(); //------------------------------------------- Speicheruhr zurücksetzen
        chronik_is_busy = false;    //-------------------------------------------- Chronik daten wieder freigeben
        Serial_Debugging_println("fertig!");
      }
    } else {
      Serial_Debugging_println(F("HTML_processor_is_working"));
    }
  }
  return true;
}
//-------------------------------------------------------------------------------------------------------------------------

void daten_serial_ausgeben() {
  int z = 0;
  z = (Chart_TimeDate.length() + 1) / Chart_TimeDate_length;
  Serial.println(String(z) + " - Chart_TimeDate: " + Chart_TimeDate);
  z = (Chart_temp_in.length() + 1) / laenge_Datensatz;
  Serial.println(String(z) + " - Chart_temp_in: " + Chart_temp_in);
  z = (Chart_air_in.length() + 1) / laenge_Datensatz;
  Serial.println(String(z) + " - Chart_air_in:  " + Chart_air_in);
  z = (Chart_tp_in.length() + 1) / laenge_Datensatz;
  Serial.println(String(z) + " - Chart_tp_in:   " + Chart_tp_in);

  z = (Chart_temp_out.length() + 1) / laenge_Datensatz;
  Serial.println(String(z) + " - Chart_temp_out:" + Chart_temp_out);
  z = (Chart_air_out.length() + 1) / laenge_Datensatz;
  Serial.println(String(z) + " - Chart_air_out: " + Chart_air_out);
  z = (Chart_tp_out.length() + 1) / laenge_Datensatz;
  Serial.println(String(z) + " - Chart_tp_out:  " + Chart_tp_out);

  z = (Chart_tp_delta.length() + 1) / laenge_Datensatz;
  Serial.println(String(z) + " - Chart_tp_delta:" + Chart_tp_delta);
  z = (Chart_lz.length() + 1) / laenge_Datensatz;
  Serial.println(String(z) + " - Chart_lz:" + Chart_lz);
  z = (Chart_lzR.length() + 1) / laenge_Datensatz;
  Serial.println(String(z) + " - Chart_lzR:" + Chart_lzR);
}
