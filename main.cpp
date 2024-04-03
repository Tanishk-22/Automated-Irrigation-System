#include "mbed.h"
#include "DHT.h"
#include "LCDi2c.h"

// Assuming you're using D4 for the DHT11 data pin
DHT sensor(PTA1, DHT11);

// Initialize the LCD. Assuming default I2C pins for your board (D14 = SDA, D15 = SCL)
// and the common I2C address for the LCD 0x27. Adjust if necessary.
LCDi2c lcd(PTE25, PTE24, LCD16x2, 0x27);

AnalogIn photoresistor(PTB11);
DigitalOut pumpRelay(PTA2);
AnalogIn soilMoistureSensor(PTB10);
AnalogIn waterLevelSensor(PTB3);


int main() {
    // Wait for the sensor to stabilize
    ThisThread::sleep_for(2s);

    while (true) {
        float temperature = 0.0f;
        float humidity = 0.0f;
        eError err = sensor.readData();
        // Read the photoresistor value (0.0 to 1.0)
        float lightLevel = photoresistor.read();
        float moistureLevel = soilMoistureSensor.read();
        float waterLevel = waterLevelSensor.read();

        if (err == ERROR_NONE) {
            temperature = sensor.ReadTemperature(CELCIUS);
            humidity = sensor.ReadHumidity();

            // Clear the LCD
            lcd.cls();

            // Set cursor to first row, first column
            lcd.locate(0, 0);
            lcd.printf("Temp: %.2f C", temperature);

            // Set cursor to second row, first column
            lcd.locate(0, 1);
            lcd.printf("Humid: %.2f %%", humidity);
        } else {
            // If there's an error, display it on the LCD
            lcd.cls();
            lcd.locate(0, 0);
            lcd.printf("DHT Error: %d", err);
        }

        // bool pumpCondition = lightLevel > 0.50;
        //Prioritize displaying critical information
        // if (lightLevel < 0.10) {
        //     lcd.locate(0, 1);
        //     lcd.printf("Low sunlight   ");
        // } 
        // else if (moistureLevel < 0.20) {
        //     lcd.locate(0, 1);
        //     lcd.printf("Soil dry       ");
        // } else if (waterLevel < 0.20) { // Assuming low water level is indicated by values < 0.20
        //     lcd.locate(0, 1);
        //     lcd.printf("Low water lvl  ");
        // }
         bool pumpCondition = (lightLevel > 0.5) && waterLevel > 0.2;
        
        if (pumpCondition) {
            pumpRelay = 1; // Turn on the pump
            // lcd.locate(0, 1);
            // lcd.printf("Pumping water ");
        } else {
            pumpRelay = 0; // Turn off the pump
        }
        
        
        // Print sensor values to the console for debugging
        printf("Light: %.2f, Moisture: %.2f, Water: %.2f\n", lightLevel, moistureLevel, waterLevel);
        // Update every 2 seconds
        ThisThread::sleep_for(2s);
    }
}