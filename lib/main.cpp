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
	
	auto spi_bus = hwlib::spi_bus_bit_banged_sclk_mosi_miso(SCK, MOSI, MISO);
	auto spi_bus_2 = hwlib::spi_bus_bit_banged_sclk_mosi_miso(SCK_2, MOSI_2, MISO_2);
	
	rf24 radio(spi_bus, CE, CSN);
	rf24 radio_2(spi_bus_2, CE_2, CSN_2);
	
	
	hwlib::wait_ms(1000);
	
	radio.set_power_level(pwr_max);
	radio.print_power_level();
	
	radio.set_data_rate(rf24_250kbps);
	radio.print_details();

	radio.begin();
	radio_2.begin();
	
	/*
	radio.stop_listening();
	radio_2.start_listening();
	
	
	struct temp_humidity{
		uint8_t temperature = 0;
		uint8_t humidity = 0;
	};
	temp_humidity payload;
	payload.temperature = 28;
	payload.humidity = 80;
	
	radio.write(payload);
	
	// Wait 100 ms to ensure the payload is send
	hwlib::wait_ms(100);
	
	temp_humidity recv;
	radio_2.read(recv);
	
	hwlib::cout << "Recieved temperature: " << hwlib::dec << recv.temperature << '\n';
	hwlib::cout << "Recieved humidity: " << hwlib::dec << recv.humidity << '\n';
	
	// Print debugging information
	hwlib::cout << "Radio #1:\n";
	radio.print_details();
	hwlib::cout << "Radio #2:\n";
	radio_2.print_details();
	 */
}
