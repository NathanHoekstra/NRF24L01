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
	
	// Oled definition
	auto SCL = target::pin_oc(target::pins::scl);
	auto SDA = target::pin_oc(target::pins::sda);
	
	// Arduino Due v1.0 shield pin definitions
	auto green_led = target::pin_out(target::pins::d4);
	auto red_led = target::pin_out(target::pins::d5);
	auto enter_btn = target::pin_in(target::pins::d2);
	auto select_btn = target::pin_in(target::pins::d3);
	green_led.set(0);
	red_led.set(0);
	
	// Create SPI bus
	auto spi_bus = hwlib::spi_bus_bit_banged_sclk_mosi_miso(SCK, MOSI, MISO);
	// Create radio object
	rf24 radio(spi_bus, CE, CSN);
	
	// Create I2C bus
	auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(SCL, SDA);
	// Create oled object
	auto oled = hwlib::glcd_oled(i2c_bus, 0x3c);
	auto font = hwlib::font_default_8x8();
	auto display = hwlib::window_ostream(oled, font);
	
	// Wait for the pc console to start
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
	//radio.print_details();
	// Start listening for incomming messages
	radio.start_listening();
	
	uint8_t timeout = 0;
	for(;;){
		if(radio.data_available()){
			red_led.set(1);
			hwlib::wait_ms(100);
			red_led.set(0);
			//hwlib::cout << "Data available, now reading!\n";
			radio.read(data);
			display
				<< "\f" << "Weather station"
				<< "\n\n" << "Temperature: " << "\t1302" << data.temperature
				<< "\n" << "Humidity: " << "\t1303" << data.humidity
				<< "\t0006" << "Counter: " << "\t1306" << data.counter
				<< hwlib::flush;
			// Reset timeout
			timeout = 0;
		}else{
			timeout++;
			if(timeout > 50){
				display << "\f" << "Weather station"
						<< "\t0403" << "Timeout!"
						<< "\t0004" << "No data recieved"
						<< hwlib::flush;
			}
		}
		hwlib::wait_ms(200);
	}
}
