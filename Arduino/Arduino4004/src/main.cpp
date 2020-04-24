// Import needed
#include <Arduino.h>

#include <Wire.h>

#include <LiquidCrystal.h>

// Init LCD Objects
// Two are needed because an 40x04 LCD utilizes two LCD Controller, one for 2 Rows
LiquidCrystal lcd(10, 8, 5, 4, 3, 2); // First two rows
LiquidCrystal lcd2(10, 9, 5, 4, 3, 2); // Last two rows

// Initialize needed Variables
String header = "        ++++Corona News Blog++++        ";

String nrw_cases, nrw_diff, nrw_deaths, de_cases, de_diff, de_deaths, title, pubtime, headline_mvt;
String data;

// define LCD Size
int lcdcol = 40;
int lcdrow = 2;

int position = 0;

boolean ascroll = false;

// Credit to https://arduino.stadckexchange.com/a/1237
// Used to split String into multiple by using delimiter
String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = {
        0,
        -1};
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++)
    {
        if (data.charAt(i) == separator || i == maxIndex)
        {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// Read needed data over Serial
void getData()
{
    // Wait for serial availability
    while (!(Serial.available()))
    {
    }
    // Wait for the entire data to arrive
    delay(30);

    // Save transmitted data in data
    data = Serial.readStringUntil('\n');

    // Extract data into different Variables
    nrw_cases = getValue(data, '~', 0);
    nrw_diff = getValue(data, '~', 1);
    nrw_deaths = getValue(data, '~', 2);
    de_cases = getValue(data, '~', 3);
    de_diff = getValue(data, '~', 4);
    de_deaths = getValue(data, '~', 5);
    title = getValue(data, '~', 6);
    pubtime = getValue(data, '~', 7);
    headline_mvt = getValue(data, '~', 8);

    // Merge publication time and title
    title = pubtime + " | " + title;

}


// Print case data on LCD
void print_cases()
{

    // Merge case description with case numbers
    nrw_cases = "NRW: " + nrw_cases;
    nrw_diff = "Diff.: " + nrw_diff;
    nrw_deaths = "Tode: " + nrw_deaths;

    // calculate total String length and number of whitespaces needed
    int len = nrw_cases.length() + nrw_diff.length() + nrw_deaths.length();
    int spaces = (40 - len) / 2;

    // Uniformly space the Strings and print to LCD
    lcd2.setCursor(0, 1);
    lcd2.print(nrw_cases);
    lcd2.setCursor((nrw_cases.length() + spaces), 1);
    lcd2.print(nrw_diff);
    lcd2.setCursor(lcdcol - nrw_deaths.length(), 1);
    lcd2.print(nrw_deaths);

    // Merge case description with case numbers
    de_cases = "DE : " + de_cases;
    de_diff = "Diff.: " + de_diff;
    de_deaths = "Tode: " + de_deaths;

    // calculate total String length and number of whitespaces needed
    len = de_cases.length() + de_diff.length() + de_deaths.length();
    spaces = (40 - len) / 2;

    // Uniformly space the Strings and print to LCD
    lcd2.setCursor(0, 0);
    lcd2.print(de_cases);
    lcd2.setCursor((de_cases.length() + spaces + 1), 0);
    lcd2.print(de_diff);
    lcd2.setCursor(lcdcol - de_deaths.length(), 0);
    lcd2.print(de_deaths);

}

// Initialize the LCD and print start String
void lcd_init()
{
    // Define the LCD size
    lcd.begin(lcdcol, lcdrow);
    lcd2.begin(lcdcol, lcdrow);
    
    // Clear both screens
    lcd.clear();
    lcd2.clear();
}

// Print start screen 
void lcd_start_screen(){

    // Print start screen 
    lcd.setCursor(0, 0);
    lcd.print(header);
    lcd.setCursor(0,1);
    lcd.print("Warte auf Daten...");

}

// Display title and scroll display
void display_data(String data)
{

    lcd.setCursor(0, 1);
    int hilfe = position + 40;
    
    // Check for short String and only print it --> No Scrolling needed
    if (data.length() < 40)
    {
        lcd.print(data);
        return;
    }

    // Move through whole String
    for (; position < hilfe; position++)
    {

        // Print char from String according to current index
        lcd.print(data[position]);

        // Add delay while scrolling for better readability
        if (ascroll)
        {
            // We found 350 milliseconds to be the sweetspot
            delay(350);
        }

        // Delay printing of each character for better readability and looks
        delay(150);

        // Enable autoscroll when end of LCD row was reached  
        if (position > lcdcol && ascroll == false)
        {
            lcd.autoscroll();
            ascroll = true;
        }

        // Reset LCD when end String was reached
        if ((position > (data.length())))
        {

            // Set aux. Variables
            position = -1;
            hilfe = 40;

            // Disable autoscroll
            lcd.noAutoscroll();
            ascroll = false;

            // Delay erasing of screen for better readability
            delay(1000);
            
            // Clear screen and print Headline
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(header);
            lcd.setCursor(0, 1);
        }
    }
}

// Executed on startup
void setup()
{
    // Open serial port with 9600 Baud
    Serial.begin(9600);

    // Call LCD initialization method
    lcd_init();

    // Display the wait for data Screen
    lcd_start_screen();

    // Get serial data
    getData();

    // Reprint Headline
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(header);
    
    // Print case data
    print_cases();
}


// Gets constantly repeated
void loop()
{
    // Display the title and Scroll
    display_data(title);
}