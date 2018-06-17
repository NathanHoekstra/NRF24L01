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
	
	radio.stop_listening();
	radio_2.start_listening();
	
	// Print debugging information
	hwlib::cout << "Radio #1:\n";
	radio.print_details();
	hwlib::cout << "Radio #2:\n";
	radio_2.print_details();
	
	radio.write_payload({0x01,0x02,0x03,0x04,0x05,0x06,0x07}, 7);
	// Wait 500 ms to ensure that the message is send
	hwlib::wait_ms(1000);
	
	std::array<uint8_t, 32> buffer;
	radio_2.read_payload(buffer);
	hwlib::cout << "Payload: ";
	for(uint8_t i = 0; i < 32; i++){
		hwlib::cout << hwlib::hex << buffer[i];
	}
	hwlib::cout << "\n Again, checking radio status:\n";
	radio.print_details();
	radio_2.print_details();
}
