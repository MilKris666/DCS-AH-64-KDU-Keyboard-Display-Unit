#include <SPI.h>
#include <Wire.h>
#include <U8x8lib.h>
#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"

// SSD1322 OLED mit 4-Wire SPI
U8X8_SSD1322_NHD_256X64_4W_HW_SPI u8x8(10, 9, 8); 

// Buffer für das AH-64D Pilot Keyboard Unit Display
char pltKuDisplayText[23] = " ";  // Speicher für bis zu 22 Zeichen + Null-Terminator
char lastDisplayText[23] = " ";   // Letzter gespeicherter Text zur Flackervermeidung

// Callback für DCS-BIOS
void onPltKuDisplayChange(char* newValue) {
    if (strcmp(pltKuDisplayText, newValue) != 0) {  // Nur aktualisieren, wenn sich etwas ändert
        strncpy(pltKuDisplayText, newValue, 22);
        pltKuDisplayText[22] = '\0';  // Sicherheitsnullterminierung
        updateDisplay();
    }
}
DcsBios::StringBuffer<22> pltKuDisplayBuffer(0x808e, onPltKuDisplayChange);

//  Funktion zum sicheren Aktualisieren des OLED-Displays
void updateDisplay() {
    if (strcmp(lastDisplayText, pltKuDisplayText) != 0) {  // Nur neu zeichnen, wenn sich der Text geändert hat
        strcpy(lastDisplayText, pltKuDisplayText);  // Speichert letzten angezeigten Text
        
        u8x8.setFont(u8x8_font_8x13B_1x2_f);

  // Sichere & fette Schrift
        u8x8.setCursor(0, 2);

        // **Erst alte Zeichen mit Leerzeichen überschreiben (volle Länge)**
        u8x8.print("                      "); // 22 Leerzeichen (löscht alte Zeichen)
        
        u8x8.setCursor(0, 2);
        u8x8.print(pltKuDisplayText); // Dann neuen Text schreiben
    }
}

void setup() {
    DcsBios::setup();
    u8x8.begin();
    
    // Schriftgröße beim Start setzen
    u8x8.setFont(u8x8_font_8x13B_1x2_f);

    // Start-Nachricht anzeigen
    u8x8.setCursor(0, 2);
    u8x8.print("   AH64 KDU BY MILKRIS");

    delay(2000);  // 2 Sekunden warten
    updateDisplay();
}

void loop() {
    DcsBios::loop();
}
