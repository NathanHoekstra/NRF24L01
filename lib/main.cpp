#include "hwlib.hpp"
#include "rf24.hpp"
#include "nrf24l01.hpp"
#include "rf_test.hpp"

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

	auto spi_bus = hwlib::spi_bus_bit_banged_sclk_mosi_miso(SCK, MOSI, MISO);
	auto spi_bus_2 = hwlib::spi_bus_bit_banged_sclk_mosi_miso(SCK_2, MOSI_2, MISO_2);
	
	rf24 radio(spi_bus, CE, CSN);
	rf24 radio_2(spi_bus_2, CE_2, CSN_2);
	
	hwlib::wait_ms(500);
	rf_test test(radio, radio_2);
	//test.test_spi_communication();
	test.test_write_functions();
	//test.test_read_write();
	
	//radio.print_details();
	
	/*
	// Create data struct
	struct temp_humidity{
		uint8_t temperature = 0;
		uint8_t humidity = 0;
	};
	
	temp_humidity payload;
	payload.temperature = 26;
	payload.humidity = 78;
	
	// Create struct to save data to:
	temp_humidity recv;
	
	// Wait 1sec to initialize everything
	hwlib::wait_ms(500);
	
	radio.begin();
	radio_2.begin();
	
	radio.set_power_level(pwr_low);
	radio.set_data_rate(rf24_1mbps);
	radio.set_transmit_address({0xFF,0xAB,0xAB,0xAB,0xAB});
	
	radio_2.set_power_level(pwr_low);
	radio_2.set_data_rate(rf24_1mbps);
	radio_2.set_transmit_address({0xFF,0xAB,0xAB,0xAB,0xAB});
	
	//radio.print_details();
	
	
	radio.stop_listening();
	radio_2.start_listening();

	radio.write(payload);
	radio_2.read(recv);
	hwlib::cout << "Recieved temp: " << hwlib::dec << recv.temperature << '\n';
	hwlib::cout << "Recieved humidity: " << hwlib::dec << recv.humidity << '\n';

	for(uint8_t i = 0; i < 10; i++){
		payload.temperature += 1;
		payload.humidity += 1;
		
		hwlib::cout << "Sending temp: " << payload.temperature << '\n';
		hwlib::cout << "Sending humidity: " << payload.humidity << "\n\n";
		
		if(!radio.write(payload)){
			hwlib::cout << "Data transmission failed!\n";
		}
		 
		if(radio_2.data_available()){
			hwlib::cout << "Data available! now reading\n";
			radio_2.read(recv);
		}
		hwlib::cout << "Recieved temperature: " << hwlib::dec << recv.temperature << '\n';
		hwlib::cout << "Recieved humidity: " << hwlib::dec << recv.humidity << "\n\n";
		hwlib::wait_ms(1000);

	}
	*/
}
