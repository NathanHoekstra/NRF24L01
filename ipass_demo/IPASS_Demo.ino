// Includes
#include <dht.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <printf.h>

// Port definition
const int green_led = 2;
const int red_led = 3;
const int button = 4;
const int dht22 = 9;

// Create DHT object
dht DHT;

// Create RF24 object
RF24 radio(7, 8); // CE, CSN
uint8_t address[] = {0xF1,0xAB,0xAB,0xAB,0xAB};
// Setup struct for data
struct package
{
  uint8_t temperature = 0;
  uint8_t humidity = 0;
  uint8_t counter = 0;
};
typedef struct package Package;
Package data;

// SETUP
void setup() {
  Serial.begin(115200);
  printf_begin();
  // Setup port modes
  pinMode(green_led, OUTPUT);
  pinMode(red_led, OUTPUT);
  pinMode(button, INPUT);
  data.counter = 1;
  //Setup transmission
  radio.begin();
  radio.setAutoAck(1);                      // Ensure autoACK is enabled
  radio.enableDynamicAck();
  radio.enableDynamicPayloads();
  radio.setRetries(2,15);                   // Smallest time between retries, max no. of retries
  radio.setPALevel(RF24_PA_HIGH);            // Set transmit power to lowest available to prevent power issues
  radio.setDataRate(RF24_1MBPS);            // Set transmission speed to the slowest available
  radio.setChannel(124);                    // Set channel to be used
  radio.setCRCLength(RF24_CRC_8);
  radio.openWritingPipe(address);      // Open a writing and reading pipe
  radio.stopListening();
  radio.printDetails();                     // Dump the configuration of the rf unit for debugging
  Serial.println("Reset!");
  
}

void loop() {
  // Read data from input pins
  int button_state = digitalRead(button);
  int read_dht = DHT.read22(dht22);
  data.temperature = DHT.temperature;
  data.humidity = DHT.humidity;

  Serial.print("Now sending payload ");
  Serial.print(data.temperature);
  Serial.print(" and ");
  Serial.print(data.humidity);
  Serial.print(" and ");
  Serial.println(data.counter);

  digitalWrite(green_led,HIGH);
  delay(100);
  digitalWrite(green_led,LOW);
  
  if (!radio.write( &data, sizeof(data)) ){
    Serial.println(F("failed sending data!"));      
  }
  data.counter++;
  delay(2000); // Delay for 2 seconds since the DHT22 sampling rate is 0.5Hz
}
