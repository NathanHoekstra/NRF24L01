//          Copyright Nathan Hoekstra 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef RF24_HPP
#define RF24_HPP
#include "hwlib.hpp"
/**
 * @file rf24.hpp
 */

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
	bool dyn_payloads = false;
	bool payload_no_ack = false;

public:

	/** @name Primary functions
	*  These are the main functions for operation with the chip.
	*/
	///@{

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
	* Begin operation of the chip
	* @note
	* Call this function before any other functions
	*/
	void begin(void);
	
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
	* Set radio data rate
	* \details
	* This function sets the data rate the radio operates on
	* Input variables are: rf24_250kbps, rf24_1mbs, rf24_2mbps
	*/
	void set_data_rate(uint8_t rate);
	
	/**
	* \brief
	* Write data to the TX FIFO
	* \details
	* Make sure to call set_transmis_address() first to set the address to transmit to.
	* @param[out] d	The data to be send, can be a struct, string etc.
	* @returns True if data has been send succesfully
	* @note Data is send with a variable payload size by default, the maximum size is 32 bytes,
	* any more bytes will be ignored. To disable variable payload length call disable_dyn_payload()
	* data will then be send with a fixed 32 byte payload size.
	*/
	template<typename datatype>
	bool write(const datatype & d){
		if(dyn_payloads){
			write_payload( *(std::array<uint8_t, sizeof(d)> *) & d, sizeof(d));
		}else{
			// This typecast is written by https://github.com/wovo/
			write( *(std::array<uint8_t, sizeof(d)> *) & d, sizeof(d) );
		}
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
	
	// --- End Primary functions ---
	///@}
	/** @name Advanced functions
	*  These are for advanced operations of the chip and are not particular necessary for normal operations.
	* A lot of these functions are also for debugging purposes
	*/
	///@{
	
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
	* Print power level to the serial monitor
	*/
	void print_power_level(void);
	
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
	* Print data rate to the serial monitor
	*/
	void print_data_rate(void);
	
	/**
	* \brief
	* Enable dynamic payload size
	* @note
	* This function is already called on begin()
	*/
	void enable_dyn_payload(void);
	
	/**
	* \brief
	* Disable dynamic payload size
	* @note
	* Calling this function will make the write() function use a fixed payload size
	*/
	void disable_dyn_payload(void);
	
	/**
	* \brief
	* Enable payloads with auto acknowledge
	* \details
	* This function also toggles enable_dyn_payload() since it is needed by this function.
	* @note
	* This function is already called on begin() thus also enabling enable_dyn_payload()
	*/
	void enable_ack_payload(void);
	
	/**
	* \brief
	* Disable all features
	* \warning
	* This function will disable all features this is not recommended!
	*/
	void disable_features(void);
	
	/**
	* \brief
	* Get payload size of RX FIFO
	* \details
	* This function gets the payload size from the RX FIFO and returns it
	*/
	uint8_t get_payload_size(void);
	
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
	* Read a 1-byte register
	* \details
	* Read a one byte register from the module, valid parameters are specified in nrf24l01.hpp or
	* refer to the module's datasheet.
	*/
	uint8_t read_register(const uint8_t & reg);
	
	/**
	* \brief
	* Read a 5-byte register
	* \details
	* Read a 5-byte register from the module, valid parameters are specified in nrf24l01.hpp or
	* refer to the module's datasheet.
	*/
	std::array<uint8_t, 5> read_register_5byte(const uint8_t & reg);
	
	// End Advanced functions
	///@}
private:
	
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
	
	void enable_dyn_ack(void);
	void disable_dyn_ack(void);
	
	void write_payload(const std::array<uint8_t, 32> & data, const uint8_t & length);
	
	template<size_t size>
	void write_payload(const std::array<uint8_t, size> & data, const uint8_t & length){
		std::array<uint8_t, size +1> input = {0};
		std::array<uint8_t, size +1> dummy;
		input[0] = 0xA0;
		for(uint8_t i = 0; i < length; i++){
			input[i+1] = data[i];
		}
		/*
		hwlib::cout << "Writing payload: ";
		for(uint8_t i = 0; i < length; i++){
			hwlib::cout << hwlib::hex << input[i];
		}
		hwlib::cout << '\n';
		*/
		bus.write_and_read(csn, input, dummy);
		// Give high pulse to ce for 20ns (minimum specified is 10ns)
		ce.set(1);
		hwlib::wait_us(20);
		ce.set(0);
	}
	
	void read_payload(std::array<uint8_t, 32> & buffer);
	
	template<size_t size>
	void write(const std::array<uint8_t, size> & data, const uint8_t length){
		uint8_t max_length = 32;
		std::array<uint8_t, 32> input = {0};
		for(uint8_t i = 0; i < std::min(length, max_length); i++){
			input[i] = data[i];
		}
		write_payload(input, 32);
	}
	template<size_t size>
	void read(std::array<uint8_t, size> & data, uint8_t length){
		if(dyn_payloads){
			length = get_payload_size();
			//hwlib::cout << "Recieved payload length: " << hwlib::dec << length << '\n';
		}
		std::array<uint8_t, 32> buffer;
		uint8_t max_length = 32;
		read_payload(buffer);
		for(uint8_t i = 0; i < std::min(length, max_length); i++){
				data[i] = buffer[i];
		}
	}
};

#endif // RF24_HPP
