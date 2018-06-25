/**
 * @file rf24.hpp
 */
 
#ifndef RF24_HPP
#define RF24_HPP
#include "hwlib.hpp"


/**
 * \brief
 * NRF24L01+ implementation using hwlib
 */
class rf24
{
private:
	// Variables
	hwlib::spi_bus & bus;
	hwlib::pin_out & ce; // Chip Enable (activates TX or RX mode)
	hwlib::pin_out & csn; // SPI Chip select
	uint8_t payload_size;

public:
	/**
	* \brief
	* The library constructor
	* \details
	* Call this function to initialize the module
	* @param bus	The SPI-bus where the module is connected to
	* @param ce		The Chip Enable pin
	* @param csn	The Chip Select pin
	*/
	rf24(hwlib::spi_bus & bus, hwlib::pin_out & ce, hwlib::pin_out & csn);

	/**
	* \brief
	* Print radio configuration
	* \details
	* This function prints important register values in HEX to the serial monitor
	* for debugging purposes.
	*/
	void print_details(void);

	/**
	* \brief
	* Set radio channel
	* \details
	* Sets the frequency the radio operates on
	* @note Set frequency before calling start_listening() or stop_listening()
	* @note Value range is between 0-125, if a higher value is supplied then 125 will be used as value
	*/
	void set_channel(const uint8_t & channel);

	/**
	* \brief
	* Get radio channel
	* \details
	* This function gets the channel frequency that is set in the register
	* @returns 8-bit uint8_t value of the channel
	* @note Reciever and Transmitter must have the same frequency
	*/
	uint8_t get_channel(void);
	
	/**
	* \brief
	* Set power level
	* \details
	* This function sets the radio output power
	* Input variables are: pwr_min, pwr_low, pwr_max, pwr_high\n
	* These respectively set the output to -18dBm, -12dBm, -6dBm, 0dBm 
	*/
	void set_power_level(uint8_t level);
	
	/**
	* \brief
	* Print power level to the serial monitor
	*/
	void print_power_level(void);
	
	/**
	* \brief
	* Set radio data rate
	* \details
	* This function sets the data rate the radio operates on
	* Input variables are: rf24_250kbps, rf24_1mbs, rf24_2mbps
	*/
	void set_data_rate(uint8_t rate);
	
	/**
	* \brief
	* Print data rate to the serial monitor
	*/
	void print_data_rate(void);
	
	/**
	* \brief
	* Set retransmission parameters
	* \details
	* @param delay 	Set the retransmission delay between 0-15, each step is 250uS beginning at 250us
	* @param count	Set auto retransmission count between 0-15, 0 disables any retransmissions
	*/
	void set_retransmission(const uint8_t & delay, const uint8_t & count);
	
	/**
	* \brief
	* Set transmission address
	* \details
	* This function sets the 5-byte transmission address (LSB first)
	* @code
	* // Set transmission address to 0xABABABABFF
	* radio.set_transmit_address({0xFF, 0xAB, 0xAB, 0xAB, 0xAB});
	* @endcode
	* @note This function also sets RX address 0 to the same value for auto ack
	*/
	void set_transmit_address(const std::array<uint8_t, 5> & address);

	/**
	* \brief
	* Set recieve address
	* \details
	* This function sets the 5-byte recieving address on a the given pipe
	* @param pipe		The pipe number. Value between 0-5
	* @param address	The 5-byte address array (LSB first)
	* \details
	* Example:
	* @code
	* // Set recieving address on pipe 1 to 0xACACACACF1
	* radio.set_recieve_address(1,{0xF1, 0xAC, 0xAC, 0xC, 0xAC});
	* @endcode
	* @note If pipe 0 is set to a diffrent value then the recieving address auto ack will be disabled!
	*/
	void set_recieve_address(const uint8_t & pipe, const std::array<uint8_t, 5> & address);
	
	/**
	* \brief
	* Set radio in RX mode
	* \details
	* This function sets the radio into recieving mode
	*/
	void start_listening(void);
	
	/**
	* \brief
	* Set radio in TX mode
	* \details
	* This function sets the radio into transmission mode
	*/
	void stop_listening(void);
	
	/**
	* \brief
	* Check if there is data available to be read
	* @returns True if there is data available to be read from RX FIFO
	*/
	bool data_available(void);
	
	//void write(const hwlib::string<0> & data);
	
	/**
	* \brief
	* Write data to the TX FIFO
	* \details
	* Make sure to call set_transmis_address() first to set the address to transmit to.
	* @param[out] d	The data to be send, can be a struct, string etc.
	* @returns True if data has been send succesfully
	* @note Data is send with a fixed 32 byte payload size, anything more then 32 bytes will
	* be ignored. You can write less then 32 bytes, the remaining bytes will be filled with zero's.
	*/
	template<typename datatype>
	bool write(const datatype & d){
		// This typecast is written by https://github.com/wovo/
		write( *(std::array<uint8_t, sizeof(d)> *) & d, sizeof(d) );
		
		uint8_t status = read_register(0x07);
		if((status & (1<<4))){
			// Flush tx fifo since data transmission has failed and return 0
			flush_tx();
			// But reset MAX_RT first for the next round
			status = (1<<4);
			write_register(0x07, status);
			return 0;
		}
		return 1;
	}
	
	//void read(hwlib::string<32> & buffer);
	
	/**
	* \brief
	* Read available data from RX FIFO
	* \details
	* Make sure to call set_transmis_address() first to set the address to transmit to.
	* @param[in] d 	The variable where the data is to be stored into.
	* @note
	* Data that has been recieved always has an 32 byte size, make sure the supplied variables
	* size is big enough for the data or some of it might get lost.\n
	* Make sure the supplied data type is the same as the one being used with the write() function. 
	*/
	template<typename datatype>
	void read(datatype & d){
		// This typecast is written by https://github.com/wovo/
		read( *(std::array<uint8_t, sizeof(d)> *) & d, sizeof(d) );
	}
	
	/**
	* \brief
	* Begin operation of the chip
	* @note
	* Call this function before any other functions
	*/
	void begin(void);

private:
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
};

#endif // RF24_HPP