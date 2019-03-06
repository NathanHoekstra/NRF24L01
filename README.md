# NRF24L01
This is an NRF24L01+ library for the Arduino Due

## Example
Communication between two radios attached to the same Arduino DUE

```C++

#include "hwlib.hpp"
#include "rf24.hpp"
#include "nrf24l01.hpp"

int main( void ){	
  // kill the watchdog
  WDT->WDT_MR = WDT_MR_WDDIS;
   
  namespace target = hwlib::target;
   
  // NRF24L01+ Chip 1 definition 
  auto MISO = target::pin_in(target::pins::miso);
  auto MOSI = target::pin_out(target::pins::mosi);
  auto SCK = target::pin_out(target::pins::sck);
  auto CE = target::pin_out(target::pins::d7);
  auto CSN = target::pin_out(target::pins::d8);
	
  // NRF24L01+ Chip 2 definition 
  auto MISO_2 = target::pin_in(target::pins::d6);
  auto MOSI_2 = target::pin_out(target::pins::d4);
  auto SCK_2 = target::pin_out(target::pins::d5);
  auto CE_2 = target::pin_out(target::pins::d3);
  auto CSN_2 = target::pin_out(target::pins::d2);

  // Create SPI bus for both radios
  auto spi_bus = hwlib::spi_bus_bit_banged_sclk_mosi_miso(SCK, MOSI, MISO);
  auto spi_bus_2 = hwlib::spi_bus_bit_banged_sclk_mosi_miso(SCK_2, MOSI_2, MISO_2);
	
  // Call constructor and create radio object for both radios
  rf24 radio(spi_bus, CE, CSN);
  rf24 radio_2(spi_bus_2, CE_2, CSN_2);

  // Create data struct
  struct package{
    uint8_t temperature = 0;
    uint8_t humidity = 0;
  };

  // Create struct for recieving side with dummy values
  package payload;
  payload.temperature = 26;
  payload.humidity = 78;
		
  // Create struct for recieving side
  package recv;
		
  // Call begin on both radios
  radio.begin();
  radio_2.begin();
		
  // Set power level low since they are next to each other
  radio.set_power_level(pwr_low);
  radio_2.set_power_level(pwr_low);
		
  // Set data rate to 1mbps
  radio.set_data_rate(rf24_1mbps);
  radio_2.set_data_rate(rf24_1mbps);
		
  // Set transmission addresses on both radios
  radio.set_transmit_address({0xFF,0xAB,0xAB,0xAB,0xAB});
  radio_2.set_transmit_address({0xFF,0xAB,0xAB,0xAB,0xAB});
		
  // Stop listening on module01 to enable TX and run start_listening on module02 to enable RX
  radio.stop_listening();
  radio_2.start_listening();
		
  // Send an recieve data in a continues loop
  for(;;){
	  hwlib::cout << "Sending temp: " << payload.temperature << '\n';
	  hwlib::cout << "Sending humidity: " << payload.humidity << "\n\n";
			
	  // If write is failed, print error message
	  if(!radio.write(payload)){
		  hwlib::cout << "Data transmission failed!\n";
	  }
			
	  // If data is available read it and place data in struct
	  if(radio_2.data_available()){
		  hwlib::cout << "Data available! now reading\n";
		  radio_2.read(recv);
	  }
			
	  // Print out recieved data
	  hwlib::cout << "Recieved temperature: " << hwlib::dec << recv.temperature << '\n';
	  hwlib::cout << "Recieved humidity: " << hwlib::dec << recv.humidity << "\n\n";
			
	  // Set temp and humidity one value higher for the next loop
	  payload.temperature++;
	  payload.humidity++;
	  // wait some time for next loop
	  hwlib::wait_ms(500);
  }
}

```
## Pinout
![NRF24L01+ pinout](https://i.imgur.com/zvteGzl.png)
