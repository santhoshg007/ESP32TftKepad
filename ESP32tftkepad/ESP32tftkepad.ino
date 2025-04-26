#include <TFT_eSPI.h>
#include <SPI.h>
#include <Keypad.h>
#include <Wire.h>

TFT_eSPI tft = TFT_eSPI();

// Color order setting
#define TFT_RGB_ORDER TFT_RGB   // Colour order Red-Green-Blue

// Colors - Updated to match the provided image
#define BG_COLOR TFT_BLUE
#define TEXT_COLOR TFT_WHITE
#define HIGHLIGHT_COLOR 0xADD8E6

// Keypad Navigation Mapping
#define KEY_UP 'B'
#define KEY_DOWN 'C'
#define KEY_SELECT 'D'

// Positions and sizes (adjust based on your display)
int logoX = tft.width() / 2;
int logoY = 30;
int logoFontSize = 2;
String logoText = "Sound Box";

int itemStartY = 80;
int itemSpacing = 30;
int iconSize = 24;
int textOffsetX = 30;
int itemFontSize = 2;

// Menu Items with Icons (Basic Text Icons for this example)
String menuItems[] = {"QR Code Pay", "NFC Pay", "History", "Settings"};
String menuIcons[] = {"QR", "NFC", "Hist", "Set"};
int selectedItem = 0;
int previousItem = 0;

// Keypad
#define MAX_KEYPAD_ROWS 4
#define MAX_KEYPAD_COLMS 4

// Keypad matrix layout
char gcaKeyMapMtx[MAX_KEYPAD_ROWS][MAX_KEYPAD_COLMS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Row and column pins -  Match your ESP32-S3 connections.  Make sure these match your actual wiring.
uint8_t gacKeypadRowPins[MAX_KEYPAD_ROWS] = {14, 21, 45, 38};
uint8_t gacKeypadColPins[MAX_KEYPAD_COLMS] = {7, 5, 15, 6};

// Keypad object
Keypad keypad = Keypad(makeKeymap(gcaKeyMapMtx), gacKeypadRowPins, gacKeypadColPins, MAX_KEYPAD_ROWS, MAX_KEYPAD_COLMS);

void drawHomeScreen() {
  tft.fillScreen(BG_COLOR);
  tft.setTextColor(TEXT_COLOR);

  // Draw Logo with color
  tft.setTextSize(logoFontSize);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TEXT_COLOR); // Logo is white in your example
  tft.drawString(logoText, logoX, logoY, 2);
  tft.setTextColor(TEXT_COLOR); // Reset text color

  // Draw Menu Items (without highlighting initially)
  tft.setTextSize(itemFontSize);
  tft.setTextDatum(ML_DATUM);
  for (int i = 0; i < 4; i++) {
    int yPos = itemStartY + i * itemSpacing;
    tft.setTextColor(TEXT_COLOR); // Ensure default text color
    tft.setTextSize(1);
    tft.drawString(menuIcons[i], 25, yPos, 2);
    tft.setTextSize(itemFontSize);
    tft.drawString(menuItems[i], 25 + iconSize + textOffsetX - 5, yPos, 2);
  }
  //draw initial highlight
  int yPos = itemStartY + selectedItem * itemSpacing;
  tft.fillRect(10, yPos - 12, tft.width() - 20, 24, HIGHLIGHT_COLOR);
  tft.setTextColor(TEXT_COLOR);  //highlighted text is white
  tft.setTextSize(1);
  tft.drawString(menuIcons[selectedItem], 25, yPos, 2);
  tft.setTextSize(itemFontSize);
  tft.drawString(menuItems[selectedItem], 25 + iconSize + textOffsetX - 5, yPos, 2);
}

void updateSelection(char key) {
  if (key == KEY_UP) {
    previousItem = selectedItem;
    selectedItem--;
    if (selectedItem < 0) selectedItem = 3;
    //erase previous highlight
    int yPos = itemStartY + previousItem * itemSpacing;
    tft.fillRect(10, yPos - 12, tft.width() - 20, 24, BG_COLOR);
    tft.setTextColor(TEXT_COLOR);
    tft.setTextSize(1);
    tft.drawString(menuIcons[previousItem], 25, yPos, 2);
    tft.setTextSize(itemFontSize);
    tft.drawString(menuItems[previousItem], 25 + iconSize + textOffsetX - 5, yPos, 2);

    //draw new highlight
    yPos = itemStartY + selectedItem * itemSpacing;
    tft.fillRect(10, yPos - 12, tft.width() - 20, 24, HIGHLIGHT_COLOR);
    tft.setTextColor(TEXT_COLOR);  //highlighted text is white
    tft.setTextSize(1);
    tft.drawString(menuIcons[selectedItem], 25, yPos, 2);
    tft.setTextSize(itemFontSize);
    tft.drawString(menuItems[selectedItem], 25 + iconSize + textOffsetX - 5, yPos, 2);

  } else if (key == KEY_DOWN) {
    previousItem = selectedItem;
    selectedItem++;
    if (selectedItem > 3) selectedItem = 0;

    //erase previous highlight.
    int yPos = itemStartY + previousItem * itemSpacing;
    tft.fillRect(10, yPos - 12, tft.width() - 20, 24, BG_COLOR);
    tft.setTextColor(TEXT_COLOR);
    tft.setTextSize(1);
    tft.drawString(menuIcons[previousItem], 25, yPos, 2);
    tft.setTextSize(itemFontSize);
    tft.drawString(menuItems[previousItem], 25 + iconSize + textOffsetX - 5, yPos, 2);

    //draw new highlight
    yPos = itemStartY + selectedItem * itemSpacing;
    tft.fillRect(10, yPos - 12, tft.width() - 20, 24, HIGHLIGHT_COLOR);
    tft.setTextColor(TEXT_COLOR);  //highlighted text is white
    tft.setTextSize(1);
    tft.drawString(menuIcons[selectedItem], 25, yPos, 2);
    tft.setTextSize(itemFontSize);
    tft.drawString(menuItems[selectedItem], 25 + iconSize + textOffsetX - 5, yPos, 2);
  }
}

void handleSelection(char key) {
  if (key == KEY_SELECT) {
    tft.fillScreen(BG_COLOR);
    tft.setTextColor(TEXT_COLOR);
    tft.setTextSize(2);
    if (selectedItem == 0) {
      tft.println("Displaying QR Code...");
      // Add code to generate and display QR code
    } else if (selectedItem == 1) {
      tft.println("Ready for NFC...");
      // Add code for NFC interaction
    } else if (selectedItem == 2) {
      tft.println("Payment History...");
      // Add code to display payment history
    } else if (selectedItem == 3) {
      tft.println("Settings Menu...");
      // Add code for settings menu
    }
    delay(1000);
    drawHomeScreen();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("========= Before Setup End ============");
  // Initialize TFT display with the correct pins
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(BG_COLOR);
  //tft.invertDisplay(false); // Remove this line.  The inversion is handled in User_Setup.h
  drawHomeScreen();
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    Serial.print("Key Pressed: ");
    Serial.println(key);
    updateSelection(key);
    handleSelection(key);
  }
  delay(100);
}
