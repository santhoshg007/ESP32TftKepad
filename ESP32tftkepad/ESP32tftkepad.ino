#include <TFT_eSPI.h>
#include <SPI.h>
#include <Keypad.h>
#include <Wire.h>
#include <esp_err.h>
#include <qrcode.h> // ESP-IDF qrcode.h
#include <string.h>

TFT_eSPI tft = TFT_eSPI();

// Color order setting
#define TFT_RGB_ORDER TFT_RGB

// Colors
#define BG_COLOR 0x00008B      // Dark Blue
#define TEXT_COLOR TFT_WHITE    // White
#define HIGHLIGHT_COLOR 0xADD8E6 // Light Blue

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
char gcaKeyMapMtx[MAX_KEYPAD_ROWS][MAX_KEYPAD_COLMS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
uint8_t gacKeypadRowPins[MAX_KEYPAD_ROWS] = {14, 21, 45, 38};
uint8_t gacKeypadColPins[MAX_KEYPAD_COLMS] = {7, 5, 15, 6};
Keypad keypad = Keypad(makeKeymap(gcaKeyMapMtx), gacKeypadRowPins, gacKeypadColPins, MAX_KEYPAD_ROWS, MAX_KEYPAD_COLMS);

// Assume this is your UPI ID
String upiID = "6379175223@ptaxis";

// Function to get payment amount
String getPaymentAmount()
{
    tft.fillScreen(BG_COLOR);
    tft.setTextColor(TEXT_COLOR);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Enter Amount:", tft.width() / 2, tft.height() / 2 - 30, 2);

    String amount = "";
    bool decimalPointEntered = false;
    int xPos = tft.width() / 2;
    int yPos = tft.height() / 2;

    while (true)
    {
        char key = keypad.getKey();
        if (key)
        {
            if (key >= '0' && key <= '9')
            {
                amount += key;
                tft.fillRoundRect(xPos - 50, yPos - 15, 100, 30, 5, BG_COLOR);
                tft.drawString(amount, xPos, yPos, 2);
            }
            else if (key == 'A')
            {
                if (!decimalPointEntered)
                {
                    amount += '.';
                    decimalPointEntered = true;
                    tft.fillRoundRect(xPos - 50, yPos - 15, 100, 30, 5, BG_COLOR);
                    tft.drawString(amount, xPos, yPos, 2);
                }
            }
            else if (key == 'D')
            {
                if (amount.length() > 0)
                {
                    return amount;
                }
            }
            else if (key == '*')
            {
                amount = "";
                decimalPointEntered = false;
                tft.fillScreen(BG_COLOR);
                tft.setTextColor(TEXT_COLOR);
                tft.setTextSize(2);
                tft.setTextDatum(MC_DATUM);
                tft.drawString("Enter Amount:", tft.width() / 2, tft.height() / 2 - 30, 2);
            }
            delay(100);
        }
    }
}

// Function to display the QR code on the TFT using ESP-IDF data
void displayQRCodeOnTFT(esp_qrcode_handle_t qrcode)
{
    int size = esp_qrcode_get_size(qrcode);
    int moduleSize;
    int offsetX, offsetY;

    // Calculate module size to fit QR code with a margin
    if (size > 0) {
        moduleSize = min((tft.width() - 20) / size, (tft.height() - 20) / size);  // 20 for margin
        if (moduleSize < 1) {
          moduleSize = 1;
        }
        offsetX = (tft.width() - size * moduleSize) / 2;
        offsetY = (tft.height() - size * moduleSize) / 2;
    } else {
        Serial.println("Error: QR code size is invalid.");
        return;
    }

    tft.fillScreen(TFT_BLUE); // Clear the screen with the defined background color
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            if (esp_qrcode_get_module(qrcode, x, y))
            {
                tft.fillRect(offsetX + x * moduleSize, offsetY + y * moduleSize, moduleSize, moduleSize, TFT_BLACK);
            }
        }
    }
}

// Function to generate and display QR code using ESP-IDF
void GenerateAndDisplayQR(const char *upiData)
{
    esp_qrcode_config_t qrcode_config = ESP_QRCODE_CONFIG_DEFAULT();
    qrcode_config.display_func = displayQRCodeOnTFT; // Set our display function

    esp_err_t err = esp_qrcode_generate(&qrcode_config, upiData);
    if (err != ESP_OK)
    {
        Serial.println("QR code generation failed!");
        if (err == ESP_ERR_NO_MEM)
        {
            Serial.println("Error: Not enough memory to generate QR code.");
        }
        // Consider adding a TFT display message here to inform the user
        return; // Exit the function on error
    }
    // The display function is called by esp_qrcode_generate
}

// This function generates a UPI string with the given amount
String CreateUPIString(float f32Amount)
{
    String upiString = "upi://pay?pa=" + upiID + "&pn=Merchant&mc=123456&tid=1234567890&tr=1234567890&tn=Payment%20for%20goods&am=";
    upiString += String(f32Amount, 2);
    upiString += "&cu=INR&url=https://www.merchantwebsite.com";
    return upiString;
}

void drawHomeScreen()
{
    tft.fillScreen(BG_COLOR);
    tft.setTextColor(TEXT_COLOR);

    // Draw Logo
    tft.setTextSize(logoFontSize);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(logoText, logoX, logoY, 2);

    // Draw Menu Items
    tft.setTextSize(itemFontSize);
    tft.setTextDatum(ML_DATUM);
    for (int i = 0; i < 4; i++)
    {
        int yPos = itemStartY + i * itemSpacing;
        tft.setTextColor(TEXT_COLOR);
        tft.setTextSize(1);
        tft.drawString(menuIcons[i], 25, yPos, 2);
        tft.setTextSize(itemFontSize);
        tft.drawString(menuItems[i], 25 + iconSize + textOffsetX - 5, yPos, 2);
    }
    //draw initial highlight
    int yPos = itemStartY + selectedItem * itemSpacing;
    tft.fillRect(10, yPos - 12, tft.width() - 20, 24, HIGHLIGHT_COLOR);
    tft.setTextColor(TEXT_COLOR);
    tft.setTextSize(1);
    tft.drawString(menuIcons[selectedItem], 25, yPos, 2);
    tft.setTextSize(itemFontSize);
    tft.drawString(menuItems[selectedItem], 25 + iconSize + textOffsetX - 5, yPos, 2);
}

void updateSelection(char key)
{
    if (key == KEY_UP)
    {
        previousItem = selectedItem;
        selectedItem--;
        if (selectedItem < 0)
            selectedItem = 3;
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
        tft.setTextColor(TEXT_COLOR);
        tft.setTextSize(1);
        tft.drawString(menuIcons[selectedItem], 25, yPos, 2);
        tft.setTextSize(itemFontSize);
        tft.drawString(menuItems[selectedItem], 25 + iconSize + textOffsetX - 5, yPos, 2);
    }
    else if (key == KEY_DOWN)
    {
        previousItem = selectedItem;
        selectedItem++;
        if (selectedItem > 3)
            selectedItem = 0;
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
        tft.setTextColor(TEXT_COLOR);
        tft.setTextSize(1);
        tft.drawString(menuIcons[selectedItem], 25, yPos, 2);
        tft.setTextSize(itemFontSize);
        tft.drawString(menuItems[selectedItem], 25 + iconSize + textOffsetX - 5, yPos, 2);
    }
}

void handleSelection(char key)
{
    if (key == KEY_SELECT)
    {
        tft.fillScreen(BG_COLOR);
        tft.setTextColor(TEXT_COLOR);
        tft.setTextSize(2);
        if (selectedItem == 0)
        {
            // QR Code Pay selected
            String amount = getPaymentAmount();
            Serial.println("Amount entered: " + amount);
            String upi_string = CreateUPIString(amount.toFloat());
            GenerateAndDisplayQR(upi_string.c_str()); // Pass as const char*
            delay(5000); // Display QR code for 5 seconds
            //drawHomeScreen();     
        }
        else if (selectedItem == 1)
        {
            tft.println("Ready for NFC...");
            delay(1000);
            drawHomeScreen();
        }
        else if (selectedItem == 2)
        {
            tft.println("Payment History...");
            delay(1000);
            drawHomeScreen();
        }
        else if (selectedItem == 3)
        {
            tft.println("Settings Menu...");
            delay(1000);
            drawHomeScreen();
        }
    }
}

void setup()
{
    Serial.begin(115200);
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(BG_COLOR);
    drawHomeScreen();
}

void loop()
{
    char key = keypad.getKey();
    if (key)
    {
        Serial.print("Key Pressed: ");
        Serial.println(key);
        updateSelection(key);
        handleSelection(key);
    }
    delay(100);
}
