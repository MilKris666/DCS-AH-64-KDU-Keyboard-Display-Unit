// Creative Commons Attribution-NonCommercial 4.0 International Public License
#include <SPI.h>
#include <Wire.h>
#include <U8x8lib.h>
#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"

// SSD1322 OLED mit 4-Wire SPI
U8X8_SSD1322_NHD_256X64_4W_HW_SPI u8x8(10, 9, 8); 

// Buffer für das AH-64D Keyboard Unit Display
char displayText[23] = " ";  // Speicher für den aktuellen Text (max. 22 Zeichen + Nullterminator)
char lastDisplayText[23] = " ";   // Letzter angezeigter Text zur Flackervermeidung

// Variable zur Auswahl der aktiven Quelle (0 = Pilot, 1 = CPG)
int activeSource = 0; // Standardmäßig Pilot

// Callback für das Pilot Keyboard Unit Display
void onPltKuDisplayChange(char* newValue) {
    if (activeSource == 0) { // Nur aktualisieren, wenn Pilot aktiv ist
        if (strcmp(displayText, newValue) != 0) {  
            strncpy(displayText, newValue, 22);
            displayText[22] = '\0';  // Sicherheitsnullterminierung
            updateDisplay();
        }
    }
}
DcsBios::StringBuffer<22> pltKuDisplayBuffer(0x808e, onPltKuDisplayChange);

// Callback für das Gunner Keyboard Unit Display
void onCpgKuDisplayChange(char* newValue) {
    if (activeSource == 1) { // Nur aktualisieren, wenn CPG aktiv ist
        if (strcmp(displayText, newValue) != 0) {  
            strncpy(displayText, newValue, 22);
            displayText[22] = '\0';  // Sicherheitsnullterminierung
            updateDisplay();
        }
    }
}
DcsBios::StringBuffer<22> cpgKuDisplayBuffer(0x80ac, onCpgKuDisplayChange);

// Funktion zum sicheren Aktualisieren des OLED-Displays
void updateDisplay() {
    if (strcmp(lastDisplayText, displayText) != 0) {  // Nur neu zeichnen, wenn sich der Text geändert hat
        strcpy(lastDisplayText, displayText);  // Speichert letzten angezeigten Text
        
        u8x8.setFont(u8x8_font_8x13B_1x2_f);
        u8x8.setCursor(0, 2);

        // **Erst alte Zeichen mit Leerzeichen überschreiben (volle Länge)**
        u8x8.print("                      "); // 22 Leerzeichen (löscht alte Zeichen)
        
        u8x8.setCursor(0, 2);
        u8x8.print(displayText); // Dann neuen Text schreiben
    }
}

// Funktion zum Anzeigen der aktiven Quelle (z.B. "KDU PILOT" oder "KDU CPG")
void showActiveSource() {
    u8x8.setFont(u8x8_font_8x13B_1x2_f);
    u8x8.setCursor(0, 2);
    u8x8.print("                      "); // Display leeren
    
    u8x8.setCursor(4, 2); // Etwas zentriert setzen
    if (activeSource == 0) {
        u8x8.print("KDU PILOT");
    } else {
        u8x8.print("KDU CPG");
    }
    delay(1000); // 1 Sekunde warten
}

void setup() {
    DcsBios::setup();
    u8x8.begin();
    
    pinMode(7, INPUT_PULLUP); // Pin für Umschaltbutton als Eingang setzen

    // Schriftgröße beim Start setzen
    u8x8.setFont(u8x8_font_8x13B_1x2_f);

    // Start-Nachricht anzeigen
    u8x8.setCursor(0, 2);
    u8x8.print("AH64 KDU BY MILKRIS");

    delay(2000);  // 2 Sekunden warten
    updateDisplay();
}

void loop() {
    DcsBios::loop();

    // Umschaltung der Quelle mit einem Taster an Pin 7 (LOW = gedrückt)
    if (digitalRead(7) == LOW) {  
        activeSource = (activeSource == 0) ? 1 : 0; // Wechsel zwischen Pilot und CPG
        strcpy(displayText, ""); // Löscht den aktuellen Text, um Flackern zu vermeiden
        strcpy(lastDisplayText, ""); // Setzt den letzten angezeigten Text zurück
        
        showActiveSource(); // Zeigt "KDU PILOT" oder "KDU CPG" für 1 Sekunde an
        
        updateDisplay(); // Danach normales Display aktualisieren
        delay(500); // Entprellzeit, um mehrfaches Umschalten zu verhindern
    }
}
