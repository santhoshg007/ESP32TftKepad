#include <TFT_eSPI.h>
#include <SPI.h>
#include <Keypad.h>
#include <Wire.h>

TFT_eSPI tft = TFT_eSPI();

// Color order setting
#define TFT_RGB_ORDER TFT_RGB // Colour order Red-Green-Blue

// Colors
#define BG_COLOR TFT_BLUE
#define TEXT_COLOR TFT_WHITE
#define HIGHLIGHT_COLOR 0xADD8E6

// Keypad Navigation Mapping
#define KEY_UP 'B'
#define KEY_DOWN 'C'
#define KEY_SELECT 'D'

// Positions and sizes
int logoX = tft.width() / 2;
int logoY = 30;
int logoFontSize = 2;
String logoText = "Sound Box";
int itemStartY = 80;
int itemSpacing = 30;
int iconSize = 24;
int textOffsetX = 30;
int itemFontSize = 2;

// Menu Items with Icons
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

// Function to get payment amount
String getPaymentAmount()
{
    tft.fillScreen(BG_COLOR);
    tft.setTextColor(TEXT_COLOR);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Enter Amount:", tft.width() / 2, tft.height() / 2 - 30, 2);

    String amount = "";
    bool decimalPointEntered = false; // Flag to track decimal point
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
                tft.fillRoundRect(xPos - 50, yPos - 15, 100, 30, 5, BG_COLOR); //erase
                tft.drawString(amount, xPos, yPos, 2);                                 //display the amount
            }
            else if (key == 'A')
            {
                if (!decimalPointEntered)
                { // Only one decimal point allowed
                    amount += '.';
                    decimalPointEntered = true;
                    tft.fillRoundRect(xPos - 50, yPos - 15, 100, 30, 5, BG_COLOR); //erase
                    tft.drawString(amount, xPos, yPos, 2);
                }
            }
            else if (key == 'D')
            { // Use 'D' as the Enter key
                if (amount.length() > 0)
                {
                    return amount; // Return the amount string
                }
            }
            else if (key == '*')
            { // Clear
                amount = "";
                decimalPointEntered = false;
                tft.fillScreen(BG_COLOR);
                tft.setTextColor(TEXT_COLOR);
                tft.setTextSize(2);
                tft.setTextDatum(MC_DATUM);
                tft.drawString("Enter Amount:", tft.width() / 2, tft.height() / 2 - 30, 2);
            }
            delay(100); // Debounce
        }
    }
}

// Function to generate QR code
void generateUpiQRCode(String upiId, String amount)
{
    tft.fillScreen(BG_COLOR);
    tft.setTextColor(TEXT_COLOR);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Generating QR Code...", tft.width() / 2, tft.height() / 2 - 30, 2);
    Serial.println("Generating QR Code for UPI ID: " + upiId + ", Amount: " + amount);

    // Construct the UPI QR code data string
    String qrData = "upi://pay?pa=" + upiId + "&am=" + amount; // Basic UPI format.  Add  currency code &cu=INR

    // Replace this with your actual QR code generation code
    // You might use a library like QRCode.h
    // The library will provide functions to encode the data string (qrData)
    // into a QR code, and then you'll use TFT_eSPI functions to draw the
    // pixels of the QR code on the screen.  The example below is a placeholder.

    delay(2000);       // Simulate QR code generation delay
    tft.fillScreen(BG_COLOR); // Clear the screen
    tft.setTextColor(TEXT_COLOR);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Scan this QR Code to Pay", tft.width() / 2, tft.height() / 2 - 60, 2);
     tft.setTextSize(1);
    tft.drawString(qrData, tft.width() / 2, tft.height() / 2 - 20, 2);
    // Example (Conceptual - Replace with actual drawing code):
    //  QRCode qrCode;
    //  qrCode.encodeText(qrData);
    //  for (int y = 0; y < qrCode.size; y++) {
    //    for (int x = 0; x < qrCode.size; x++) {
    //      if (qrCode.isBlack(x, y)) {
    //        tft.drawPixel(x, y, TFT_BLACK); // Or a suitable color
    //      }
    //    }
    //  }

    delay(5000); // Display the QR code for 5 seconds
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
            String amount = getPaymentAmount(); // Get amount as string
            Serial.println("Amount entered: " + amount);
            //  Pass a fixed UPI ID for this example
            String upiId = "6379175223@ptaxis"; // Replace with actual UPI ID
            generateUpiQRCode(upiId, amount);     // Pass UPI ID and amount
            drawHomeScreen();
        }
        else if (selectedItem == 1)
        {
            tft.println("Ready for NFC...");
            // Add code for NFC interaction
            delay(1000);
            drawHomeScreen();
        }
        else if (selectedItem == 2)
        {
            tft.println("Payment History...");
            // Add code to display payment history
            delay(1000);
            drawHomeScreen();
        }
        else if (selectedItem == 3)
        {
            tft.println("Settings Menu...");
            // Add code for settings menu
            delay(1000);
            drawHomeScreen();
        }
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("========= Before Setup End ============");
    // Initialize TFT display
    tft.init();
    tft.setRotation(1);
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
