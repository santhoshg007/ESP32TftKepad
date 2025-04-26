#include <TFT_eSPI.h>
#include <SPI.h>
#include <Keypad.h>
#include <Wire.h>

TFT_eSPI tft = TFT_eSPI();  // TFT instance

#define MAX_KEYPAD_ROWS    4
#define MAX_KEYPAD_COLMS   4

#define KEYPAD_TASK_NAME        "KEYTSK"
#define KEYPAD_TASK_STACK_SIZE  (2 * 1024) // 2KB

// Keypad matrix layout
char gcaKeyMapMtx[MAX_KEYPAD_ROWS][MAX_KEYPAD_COLMS] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};

// Row and column pins - Based on your initial request (may need adjustment for your board)
uint8_t gacKeypadRowPins[MAX_KEYPAD_ROWS] = {14, 21, 45, 38};
uint8_t gacKeypadColPins[MAX_KEYPAD_COLMS] = {7, 5, 15, 6};

// Keypad object
Keypad keypad = Keypad(makeKeymap(gcaKeyMapMtx), gacKeypadRowPins, gacKeypadColPins, MAX_KEYPAD_ROWS, MAX_KEYPAD_COLMS);

// FreeRTOS task handle
TaskHandle_t gtskkeypadHandle = NULL;

#if 0
// TFT Display Pin Definitions (Based on our last conflict-free suggestion)
#define TFT_MOSI 3   // Connect to the pin labeled "3"
#define TFT_SCLK 4   // Connect to the pin labeled "4"
#define TFT_CS   18  // Connect to the pin labeled "18"
#define TFT_DC   16  // Connect to the pin labeled "16 (TX0)"
#define TFT_RST  17  // Connect to the pin labeled "17 (RX0)"
#endif
//
// ─── Float Input Function ───────────────────────────────────────────
//
float readFloatFromKeypadAndDisplay() {
  String inputBuffer = "";
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 20);
  tft.setTextSize(2);
  tft.println("Enter float:");

  tft.setCursor(10, 50);
  tft.setTextSize(3);

  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key >= '0' && key <= '9') {
        inputBuffer += key;
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(10, 20);
        tft.setTextSize(2);
        tft.println("Enter float:");
        tft.setCursor(10, 50);
        tft.setTextSize(3);
        tft.print(inputBuffer);
      } else if (key == 'A') {
        if (inputBuffer.indexOf('.') == -1) {
          inputBuffer += '.';
          tft.fillScreen(TFT_BLACK);
          tft.setCursor(10, 20);
          tft.setTextSize(2);
          tft.println("Enter float:");
          tft.setCursor(10, 50);
          tft.setTextSize(3);
          tft.print(inputBuffer);
        }
      } else if (key == 'D') {
        float number = inputBuffer.toFloat();
        Serial.print("Entered Float: ");
        Serial.println(number);
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(10, 20);
        tft.setTextSize(2);
        tft.println("You entered:");
        tft.setCursor(10, 50);
        tft.setTextSize(3);
        tft.print(number);
        delay(1000); // Show result for a short time
        return number;
      } else if (key == '*') {
        inputBuffer = "";
        Serial.println("Input cleared");
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(10, 20);
        tft.setTextSize(2);
        tft.println("Enter float:");
        tft.setCursor(10, 50);
        tft.setTextSize(3);
        tft.print(inputBuffer);
      }

      Serial.print("Current Input: ");
      Serial.println(inputBuffer);
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

//
// ─── FreeRTOS Task to Read Float Input and Display ────────────────
//
void KeypadReadProcess(void *pvParameters) {
  while (1) {
    Serial.println("Enter a float number using keypad. Press D to confirm:");
    float value = readFloatFromKeypadAndDisplay();

    // Do something with the float value here
    Serial.print("Final float value received: ");
    Serial.println(value);

    vTaskDelay(1000 / portTICK_PERIOD_MS); // Wait before asking again
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("========= Before Setup End ============");

  // Initialize TFT display with the correct pins
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(30, 60);
  tft.setTextSize(2);
  tft.println("Keypad Input:");

#if 1
  // Create keypad task pinned to core 1
  xTaskCreatePinnedToCore(
    KeypadReadProcess,
    KEYPAD_TASK_NAME,
    KEYPAD_TASK_STACK_SIZE,
    NULL,
    1,
    &gtskkeypadHandle,
    1 // Core 1
  );
#endif
}

void loop() {
  // nothing
}