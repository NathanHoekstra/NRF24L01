#ifndef RF24_HPP
#define RF24_HPP
#include "hwlib.hpp"

class rf24
{
private:
	// Variables
	hwlib::spi_bus & bus;
	hwlib::pin_out & ce; // Chip Enable (activates TX or RX mode)
	hwlib::pin_out & csn; // SPI Chip select
	uint8_t payload_size;
	
	// Functions
	uint8_t read_register(const uint8_t & reg);
	std::array<uint8_t, 5> read_register_5byte(const uint8_t & reg);
	
	uint8_t get_status(void);
	void print_status(const uint8_t & status);
	
	void print_address_register(hwlib::string<32> name, const uint8_t & reg, const uint8_t & length = 1);
	void print_byte_register(hwlib::string<32> name, const uint8_t & reg, const uint8_t & length = 1);
	
	void write_register(const uint8_t & reg, const uint8_t & data);
	void write_register_5byte(const uint8_t & reg, const std::array<uint8_t, 5> & data);
	
	void flush_tx(void);
	void flush_rx(void);
	
	void power_down(void);
	void power_up(void);
	
	void enable_dyn_payload(void);
	void enable_ack_payload(void);
	void enable_dyn_ack(void);
	void disable_features(void);
	
	void write_payload(const std::array<uint8_t, 32> & data, const uint8_t & length);
	void read_payload(std::array<uint8_t, 32> & buffer);
	
	template<size_t size>
	void write(const std::array<uint8_t, size> & data, const uint8_t length){
		//hwlib::cout << "size: " << length << '\n';
		//hwlib::cout << "data: " << data[1] << '\n';
		uint8_t max_length = 32;
		std::array<uint8_t, 32> input = {0};
		for(uint8_t i = 0; i < std::min(length, max_length); i++){
			input[i] = data[i];
		}
		write_payload(input, 32);
	}
	template<size_t size>
	void read(std::array<uint8_t, size> & data, const uint8_t length){
		std::array<uint8_t, 32> buffer;
		uint8_t max_length = 32;
		read_payload(buffer);
		for(uint8_t i = 0; i < std::min(length, max_length); i++){
			data[i] = buffer[i];
		}
	}
	
public:
	rf24(hwlib::spi_bus & bus, hwlib::pin_out & ce, hwlib::pin_out & csn);
	
	void print_details(void);
	
	void set_channel(const uint8_t & channel);
	uint8_t get_channel(void);
	
	void set_power_level(uint8_t level);
	void print_power_level(void);
	
	void set_data_rate(uint8_t rate);
	void print_data_rate(void);
	
	void set_transmit_address(const std::array<uint8_t, 5> & address);
	void set_recieve_address(const uint8_t & pipe, const std::array<uint8_t, 5> & address);
	
	void start_listening(void);
	void stop_listening(void);
	
	bool data_available(void);
	
	//void write(const hwlib::string<0> & data);
	
	template<typename datatype>
	bool write(const datatype & d){
		// This typecall is one big hack written by https://github.com/wovo/
		write( *(std::array<uint8_t, sizeof(d)> *) & d, sizeof(d) );
		
		uint8_t status = read_register(0x07);
		if((status & (1<<4))){
			// Flush tx fifo since data transmission has failed and return 0
			flush_tx();
			return 0;
		}
		return 1;
	}
	
	//void read(hwlib::string<32> & buffer);
	
	template<typename datatype>
	void read(datatype & d){
		// This typecall is one big hack written by https://github.com/wovo/
		read( *(std::array<uint8_t, sizeof(d)> *) & d, sizeof(d) );
	}
	
	void begin(void);
};

#endif // RF24_HPP
