#include "hwlib.hpp"
#include "rf24.hpp"
#include "nrf24l01.hpp"
#include "rf_test.hpp"

int main( void ){	
   // kill the watchdog
   WDT->WDT_MR = WDT_MR_WDDIS;
   
   namespace target = hwlib::target;
   
	// Chip 1 definition 
	auto MISO = target::pin_in(target::pins::miso);
	auto MOSI = target::pin_out(target::pins::mosi);
	auto SCK = target::pin_out(target::pins::sck);
	auto CE = target::pin_out(target::pins::d7);
	auto CSN = target::pin_out(target::pins::d8);

	auto spi_bus = hwlib::spi_bus_bit_banged_sclk_mosi_miso(SCK, MOSI, MISO);
	
	rf24 radio(spi_bus, CE, CSN);
	
	hwlib::wait_ms(500);
	
	// Create data struct
	struct package{
		uint8_t temperature = 0;
		uint8_t humidity = 0;
		uint8_t counter = 0;
	};
	
	// Create struct to save data to:
	package data;
	
	// Wait 1sec to initialize everything
	hwlib::wait_ms(500);
	
	radio.begin();
	
	radio.set_power_level(pwr_high);
	radio.set_data_rate(rf24_1mbps);
	radio.set_retransmission(2,15);
	radio.set_channel(124);
	radio.set_transmit_address({0xF1,0xAB,0xAB,0xAB,0xAB});
	radio.print_details();
	// Start listening for incomming messages
	radio.start_listening();

	for(;;){
		if(radio.data_available()){
			hwlib::cout << "Data available! now reading\n";
			radio.read(data);
			hwlib::cout << "Recieved temperature: " << hwlib::dec << data.temperature << '\n';
			hwlib::cout << "Recieved humidity: " << hwlib::dec << data.humidity << "\n";
			hwlib::cout << "Counter: " << hwlib::dec << data.counter << "\n\n";
		}
		hwlib::wait_ms(100);
	}
}
