#ifndef RF24_HPP
#define RF24_HPP
#include "hwlib.hpp"

class rf24
{
private:
	hwlib::spi_bus & bus;
	hwlib::pin_out & ce; // Chip Enable (activates TX or RX mode)
	hwlib::pin_out & csn; // SPI Chip select
	
	uint8_t read_register_1byte(const uint8_t & reg);
	std::array<uint8_t, 5> read_register_5byte(const uint8_t & reg);
	void print_address_register(hwlib::string<32> name, const uint8_t & reg, const uint8_t & length = 1);
	void print_byte_register(hwlib::string<32> name, const uint8_t & reg, const uint8_t & length = 1);
	
public:
	rf24(hwlib::spi_bus & bus, hwlib::pin_out & ce, hwlib::pin_out & csn);
	
	void print_details(void);
};

#endif // RF24_HPP
