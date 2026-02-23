// #########################################################################################
// ****************************
// *****   AH64D KDU     ******
// *****   Auto Seat     ******
// *****   by MilKris    ******
// **************************** 
//
// ONLY WORKS WITH ARDUINO MEGA 2560
// 
// This is a completely new sketch for an Arduino-only KDU. No BBI64 board is required!
// 
// This sketch automatically detects whether you are in the PLT or CP/G seat and switches automatically.
//
// SEE THE WIRING-INFO IN THE HOW-TO.PDF!!!
// 
// #########################################################################################



#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"
#include <Arduino.h>
#include <U8x8lib.h>

// =======================================================
// DISPLAY: SSD1322 256x64, 4-Wire HW SPI (U8x8)
// Constructor: (CS, DC, RESET)
// HW SPI pins on MEGA: MOSI=51, SCK=52, MISO=50 (MISO not needed for most Displays)
// =======================================================
U8X8_SSD1322_NHD_256X64_4W_HW_SPI u8x8(10, 9, 8); // CS=10, DC=9, RST=8

// =======================================================
// Seat state + safe switching 
// 0 = PLT, 1 = CPG
// =======================================================
volatile uint8_t status = 0;         // 0 = PLT, 1 = CPG
volatile bool gSeatChanged = true;    // true -> Set the layout once at startup
volatile bool g_bRun = true;

// Seat Position Callback (arrives here as 0/1 due to mask/shift in the IntegerBuffer)
void onSeatPositionChange(unsigned int newValueSeat) {
  uint8_t s = (uint8_t)newValueSeat; // 0 or 1
  if (s != status) {
    status = s;
    gSeatChanged = true;
  }
}
DcsBios::IntegerBuffer seatPositionBuffer(0x8750, 0x0100, 8, onSeatPositionChange);

// =======================================================
// Display helper (bigger font + moved down ~16px / 2 rows)
// U8x8 8px Raster -> 16px (row=2)
// =======================================================
static void updateDisplay(char* newValue) {
  static char last[22] = {0};
  static uint32_t lastDrawMs = 0;

  // 1) only if the content changes
  if (strncmp(last, newValue, sizeof(last) - 1) == 0) return;

  // 2) rate limit: max ~50 Hz (20 ms)
  uint32_t now = millis();
  if ((uint32_t)(now - lastDrawMs) < 20) return;
  lastDrawMs = now;

  // copy new value
  strncpy(last, newValue, sizeof(last) - 1);
  last[sizeof(last) - 1] = '\0';

  // 3) DO NOT clearDisplay() -> only overwrite the line
  u8x8.setCursor(0, 2);

  // Pad to 32 characters (one complete line) so that old characters disappear.
  char line[33];
  memset(line, ' ', 32);
  line[32] = '\0';

  // Copy text to the beginning (up to a maximum of 32 lines)
  size_t n = strnlen(last, 32);
  memcpy(line, last, n);

  u8x8.print(line);
}

// =======================================================
// Controls
// WARNING: SPI pins and OLED pins MUST NOT be used as switch pins
// Mega SPI: 50/51/52/53 -> leave clear!
// OLED uses: 10/9/8 -> leave blank!
// =======================================================
#define NUMBER_OF_CONTROLS 50

DcsBios::Switch2Pos switch2_01("UNDEF", 24);
DcsBios::Switch2Pos switch2_02("UNDEF", 34);
DcsBios::Switch2Pos switch2_03("UNDEF", 35);
DcsBios::Switch2Pos switch2_04("UNDEF", 36);
DcsBios::Switch2Pos switch2_05("UNDEF", 43);
DcsBios::Switch2Pos switch2_06("UNDEF", 44);
DcsBios::Switch2Pos switch2_07("UNDEF", 45);
DcsBios::Switch2Pos switch2_08("UNDEF", A2);
DcsBios::Switch2Pos switch2_09("UNDEF", A5);
DcsBios::Switch2Pos switch2_10("UNDEF", 31);
DcsBios::Switch2Pos switch2_11("UNDEF", 14);
DcsBios::Switch2Pos switch2_12("UNDEF", A1);
DcsBios::Switch2Pos switch2_13("UNDEF", 12);
DcsBios::Switch2Pos switch2_14("UNDEF", 11);
DcsBios::Switch2Pos switch2_15("UNDEF", 32);
DcsBios::Switch2Pos switch2_16("UNDEF", 33);
DcsBios::Switch2Pos switch2_17("UNDEF", 37);
DcsBios::Switch2Pos switch2_18("UNDEF", 38);
DcsBios::Switch2Pos switch2_19("UNDEF", 39);
DcsBios::Switch2Pos switch2_20("UNDEF", 40);
DcsBios::Switch2Pos switch2_21("UNDEF", 41);
DcsBios::Switch2Pos switch2_22("UNDEF", 42);
DcsBios::Switch2Pos switch2_23("UNDEF", 46);
DcsBios::Switch2Pos switch2_24("UNDEF", 47);
DcsBios::Switch2Pos switch2_25("UNDEF", 48);
DcsBios::Switch2Pos switch2_26("UNDEF", 49);
DcsBios::Switch2Pos switch2_27("UNDEF", A3);
DcsBios::Switch2Pos switch2_28("UNDEF", A4);
DcsBios::Switch2Pos switch2_29("UNDEF", 30);
DcsBios::Switch2Pos switch2_30("UNDEF", 29);
DcsBios::Switch2Pos switch2_31("UNDEF", 28);
DcsBios::Switch2Pos switch2_32("UNDEF", 27);
DcsBios::Switch2Pos switch2_33("UNDEF", 26);
DcsBios::Switch2Pos switch2_34("UNDEF", 25);
DcsBios::Switch2Pos switch2_35("UNDEF", A7);
DcsBios::Switch2Pos switch2_36("UNDEF", A8);
DcsBios::Switch2Pos switch2_37("UNDEF", 18);
DcsBios::Switch2Pos switch2_38("UNDEF", 17);
DcsBios::Switch2Pos switch2_39("UNDEF", 7);
DcsBios::Switch2Pos switch2_40("UNDEF", 19);
DcsBios::Switch2Pos switch2_41("UNDEF", 4);
DcsBios::Switch2Pos switch2_42("UNDEF", 23);
DcsBios::Switch2Pos switch2_43("UNDEF", 16);
DcsBios::Switch2Pos switch2_44("UNDEF", 2);
DcsBios::Switch2Pos switch2_45("UNDEF", 5);
DcsBios::Switch2Pos switch2_46("UNDEF", 3);
DcsBios::Switch2Pos switch2_47("UNDEF", 22);
DcsBios::Switch2Pos switch2_48("UNDEF", A9);
DcsBios::Switch2Pos switch2_49("UNDEF", 6);
DcsBios::Potentiometer pot_01("UNDEF", A0);

// =======================================================
// Control layout mapper
// =======================================================
struct _controllayout_type {
  const char* cntrl_name[NUMBER_OF_CONTROLS];

  _controllayout_type(const char* _name[]) {
    for (int i = 0; i < NUMBER_OF_CONTROLS; i++) cntrl_name[i] = _name[i];
  }

  void MakeCurrent() {
    switch2_01.SetControl(cntrl_name[0]);
    switch2_02.SetControl(cntrl_name[1]);
    switch2_03.SetControl(cntrl_name[2]);
    switch2_04.SetControl(cntrl_name[3]);
    switch2_05.SetControl(cntrl_name[4]);
    switch2_06.SetControl(cntrl_name[5]);
    switch2_07.SetControl(cntrl_name[6]);
    switch2_08.SetControl(cntrl_name[7]);
    switch2_09.SetControl(cntrl_name[8]);
    switch2_10.SetControl(cntrl_name[9]);
    switch2_11.SetControl(cntrl_name[10]);
    switch2_12.SetControl(cntrl_name[11]);
    switch2_13.SetControl(cntrl_name[12]);
    switch2_14.SetControl(cntrl_name[13]);
    switch2_15.SetControl(cntrl_name[14]);
    switch2_16.SetControl(cntrl_name[15]);
    switch2_17.SetControl(cntrl_name[16]);
    switch2_18.SetControl(cntrl_name[17]);
    switch2_19.SetControl(cntrl_name[18]);
    switch2_20.SetControl(cntrl_name[19]);
    switch2_21.SetControl(cntrl_name[20]);
    switch2_22.SetControl(cntrl_name[21]);
    switch2_23.SetControl(cntrl_name[22]);
    switch2_24.SetControl(cntrl_name[23]);
    switch2_25.SetControl(cntrl_name[24]);
    switch2_26.SetControl(cntrl_name[25]);
    switch2_27.SetControl(cntrl_name[26]);
    switch2_28.SetControl(cntrl_name[27]);
    switch2_29.SetControl(cntrl_name[28]);
    switch2_30.SetControl(cntrl_name[29]);
    switch2_31.SetControl(cntrl_name[30]);
    switch2_32.SetControl(cntrl_name[31]);
    switch2_33.SetControl(cntrl_name[32]);
    switch2_34.SetControl(cntrl_name[33]);
    switch2_35.SetControl(cntrl_name[34]);
    switch2_36.SetControl(cntrl_name[35]);
    switch2_37.SetControl(cntrl_name[36]);
    switch2_38.SetControl(cntrl_name[37]);
    switch2_39.SetControl(cntrl_name[38]);
    switch2_40.SetControl(cntrl_name[39]);
    switch2_41.SetControl(cntrl_name[40]);
    switch2_42.SetControl(cntrl_name[41]);
    switch2_43.SetControl(cntrl_name[42]);
    switch2_44.SetControl(cntrl_name[43]);
    switch2_45.SetControl(cntrl_name[44]);
    switch2_46.SetControl(cntrl_name[45]);
    switch2_47.SetControl(cntrl_name[46]);
    switch2_48.SetControl(cntrl_name[47]);
    switch2_49.SetControl(cntrl_name[48]);
    pot_01.SetControl(cntrl_name[49]);
  }
};

const char* controlnames_PLT[] = {
  "PLT_KU_0","PLT_KU_1","PLT_KU_2","PLT_KU_3","PLT_KU_4","PLT_KU_5","PLT_KU_6","PLT_KU_7","PLT_KU_8","PLT_KU_9",
  "PLT_KU_A","PLT_KU_B","PLT_KU_C","PLT_KU_D","PLT_KU_E","PLT_KU_F","PLT_KU_G","PLT_KU_H","PLT_KU_I","PLT_KU_J",
  "PLT_KU_K","PLT_KU_L","PLT_KU_M","PLT_KU_N","PLT_KU_O","PLT_KU_P","PLT_KU_Q","PLT_KU_R","PLT_KU_S","PLT_KU_T",
  "PLT_KU_U","PLT_KU_V","PLT_KU_W","PLT_KU_X","PLT_KU_Y","PLT_KU_Z",
  "PLT_KU_LEFT","PLT_KU_RIGHT","PLT_KU_BKS","PLT_KU_CLR","PLT_KU_DIV","PLT_KU_DOT","PLT_KU_ENT","PLT_KU_MINUS",
  "PLT_KU_MULTI","PLT_KU_PLUS","PLT_KU_SIGN","PLT_KU_SLASH","PLT_KU_SPC","PLT_KU_BRT",
};
_controllayout_type cl_PLT(controlnames_PLT);

const char* controlnames_CPG[] = {
  "CPG_KU_0","CPG_KU_1","CPG_KU_2","CPG_KU_3","CPG_KU_4","CPG_KU_5","CPG_KU_6","CPG_KU_7","CPG_KU_8","CPG_KU_9",
  "CPG_KU_A","CPG_KU_B","CPG_KU_C","CPG_KU_D","CPG_KU_E","CPG_KU_F","CPG_KU_G","CPG_KU_H","CPG_KU_I","CPG_KU_J",
  "CPG_KU_K","CPG_KU_L","CPG_KU_M","CPG_KU_N","CPG_KU_O","CPG_KU_P","CPG_KU_Q","CPG_KU_R","CPG_KU_S","CPG_KU_T",
  "CPG_KU_U","CPG_KU_V","CPG_KU_W","CPG_KU_X","CPG_KU_Y","CPG_KU_Z",
  "CPG_KU_LEFT","CPG_KU_RIGHT","CPG_KU_BKS","CPG_KU_CLR","CPG_KU_DIV","CPG_KU_DOT","CPG_KU_ENT","CPG_KU_MINUS",
  "CPG_KU_MULTI","CPG_KU_PLUS","CPG_KU_SIGN","CPG_KU_SLASH","CPG_KU_SPC","CPG_KU_BRT",
};
_controllayout_type cl_CPG(controlnames_CPG);

// =======================================================
// Apply seat layout (called from loop when change detected)
// =======================================================
static void applySeatLayout() {
  g_bRun = false;                 // Briefly pause DcsBios::loop
  if (status == 0) cl_PLT.MakeCurrent();
  else             cl_CPG.MakeCurrent();
  g_bRun = true;

  // brief status feedback 
  u8x8.clearDisplay();
  u8x8.setCursor(0, 2);
  u8x8.print(status == 0 ? "SEAT: PLT" : "SEAT: CPG");
}

// =======================================================
// KU Display callbacks (only show if Seat fits)
// =======================================================
void onCpgKuDisplayChange(char* newValue) {
  if (status == 1) updateDisplay(newValue);
}
DcsBios::StringBuffer<22> cpgKuDisplayBuffer(AH_64D_CPG_KU_DISPLAY, onCpgKuDisplayChange);

void onPltKuDisplayChange(char* newValue) {
  if (status == 0) updateDisplay(newValue);
}
DcsBios::StringBuffer<22> pltKuDisplayBuffer(AH_64D_PLT_KU_DISPLAY, onPltKuDisplayChange);

// =======================================================
// Setup / Loop
// =======================================================
void setup() {
  DcsBios::setup();

  u8x8.begin();
  u8x8.setPowerSave(0);

  // Bigger font (double height-ish). If this doesn't compile, tell me your u8g2/u8x8 version.
  u8x8.setFont(u8x8_font_8x13B_1x2_f);

  u8x8.clearDisplay();
  u8x8.drawString(0, 2, "KU READY"); // moved down

  //The layout is initially set in the loop() by gSeatChanged=true
}

void loop() {
  // Handle seat changes smoothly (outside of callback)
  if (gSeatChanged) {
    gSeatChanged = false;
    applySeatLayout();
  }

  if (g_bRun) {
    DcsBios::loop();
  }
}
