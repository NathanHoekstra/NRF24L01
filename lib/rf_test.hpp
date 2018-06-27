//          Copyright Nathan Hoekstra 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef RF_TEST_HPP
#define RF_TEST_HPP
#include "rf24.hpp"
#include "hwlib.hpp"
/**
 * @file rf_test.hpp
 */

/**
 * \brief
 * RF24 test class
 */
class rf_test
{
private:
	rf24 & module01;
	rf24 & module02;
public:
	/**
	* \brief
	* The test class constructor
	* \details
	* Call this function to initialize the test
	* @param module01	The constructor for the first radio
	* @param module02	The constructor for the second radio
	*/
	rf_test(rf24 & module01, rf24 & module02):
		module01(module01),
		module02(module02)
	{};
	/**
	* \brief
	* Test module connectivity
	* \details
	* This function tries to do a read from each module, the test outcome will be printed to the terminal.
	* @note
	* Make sure that the modules are completely clear from any previous configurations, this can be accomplished by a complete power down.
	*/
	void test_spi_communication(void){
		// Read RX_ADDR_P0 from both modules
		hwlib::cout << "When testing, make sure the modules where completely powered down,\n to ensure the modules have default values!\n\n";
		hwlib::cout << "Testing SPI communication\n";
		std::array<uint8_t, 5> mod01 = module01.read_register_5byte(R_REGISTER + RX_ADDR_P0);
		std::array<uint8_t, 5> mod02 = module02.read_register_5byte(R_REGISTER + RX_ADDR_P0);
		// If the recieved address equels 0xE7E7E7E7E7 then the module is attached correctly
		std::array<uint8_t, 5> default_addr = {0xE7,0xE7,0xE7,0xE7,0xE7};
		if(mod01 == default_addr){
			hwlib::cout << "[OK]	Connection with module #1\n";
		}else{
			hwlib::cout << "[FAIL]	No connection with module #1\n";
		}
		if(mod02 == default_addr){
			hwlib::cout << "[OK]	Connection with module #2\n";
		}else{
			hwlib::cout << "[FAIL]	No connection with module #2\n";
		}
	}
	/**
	* \brief
	* Test write functions
	* \details
	* This function goes through all functions and reads the respectively register to confirm the register has been set properly.
	* The test outcome will be printed to the terminal
	* @note
	* Make sure that the modules are completely clear from any previous configurations, this can be accomplished by a complete power down.
	*/
	void test_write_functions(void){
		hwlib::cout << "Testing functions which write to the register\n\n";
		hwlib::cout << "Testing set_channel function\n";
		// Setting channel to 130, max channel is 125 so get_channel should return 125
		module01.set_channel(130);
		module02.set_channel(130);
		if(module01.get_channel() == 125){
			hwlib::cout << "[OK]	Channel succesfully set on module #1\n";
		}else{
			hwlib::cout << "[FAIL]	Channel returned incorrect number on module #1\n";
		}
		if(module02.get_channel() == 125){
			hwlib::cout << "[OK]	Channel succesfully set on module #2\n";
		}else{
			hwlib::cout << "[FAIL]	Channel returned incorrect number on module #2\n";
		}
		// End channel test
		
		hwlib::cout << "\nTesting set_data_rate function\n";
		hwlib::cout << "Setting data rate to rf24_2mbps, if it's printed out twice then the test is succesful\n";
		module01.set_data_rate(rf24_2mbps);
		module02.set_data_rate(rf24_2mbps);
		module01.print_data_rate();
		module02.print_data_rate();
		// End data rate test
		
		hwlib::cout << "\nTesting set_power_level function\n";
		hwlib::cout << "Setting power level to pwr_max, if it's printed out twice then the test is succesful\n";
		module01.set_power_level(pwr_max);
		module02.set_power_level(pwr_max);
		module01.print_power_level();
		module02.print_power_level();
		// End power level test
		
		hwlib::cout << "\nTesting set_recieve_address function\n";
		std::array<uint8_t, 5> rx_addr = {0x1A,0xAB,0xAB,0xAB,0xAB};
		module01.set_recieve_address(1,rx_addr);
		module02.set_recieve_address(1,rx_addr);
		std::array<uint8_t, 5> rx_01 = module01.read_register_5byte(R_REGISTER + RX_ADDR_P1);
		std::array<uint8_t, 5> rx_02 = module02.read_register_5byte(R_REGISTER + RX_ADDR_P1);
		if(rx_addr == rx_01){
			hwlib::cout << "[OK]	set_recieve_address succesful on module #1\n";
		}else{
			hwlib::cout << "[FAIL]	set_recieve_address failed on module #1\n";
		}
		if(rx_addr == rx_02){
			hwlib::cout << "[OK]	set_recieve_address succesful on module #2\n";
		}else{
			hwlib::cout << "[FAIL]	set_recieve_address failed on module #2\n";
		}
		// End set recieve test
		
		hwlib::cout << "\nTesting set_transmit_address function\n";
		std::array<uint8_t, 5> tx_addr = {0x1F,0xAC,0xAC,0xAC,0xAC};
		module01.set_transmit_address(tx_addr);
		module02.set_transmit_address(tx_addr);
		std::array<uint8_t, 5> tx_01 = module01.read_register_5byte(R_REGISTER + TX_ADDR);
		std::array<uint8_t, 5> tx_02 = module02.read_register_5byte(R_REGISTER + TX_ADDR);
		if(tx_addr == tx_01){
			hwlib::cout << "[OK]	set_transmit_address succesful on module #1\n";
		}else{
			hwlib::cout << "[FAIL]	set_transmit_address failed on module #1\n";
		}
		if(tx_addr == tx_02){
			hwlib::cout << "[OK]	set_transmit_address succesful on module #2\n";
		}else{
			hwlib::cout << "[FAIL]	set_transmit_address failed on module #2\n";
		}
		// End set transmit test
		
		hwlib::cout << "\nTesting enable_dyn_payload/ disable_dyn_payload function\n";
		module01.enable_dyn_payload();
		module02.enable_dyn_payload();
		uint8_t dyn01 = module01.read_register(R_REGISTER + FEATURE);
		uint8_t dyn02 = module02.read_register(R_REGISTER + FEATURE);
		if(dyn01 == (1<<EN_DPL)){
			hwlib::cout << "[OK]	enable_dyn_payload succesful on module #1\n";
		}else{
			hwlib::cout << "[FAIL]	enable_dyn_payload failed on module #1\n";
		}
		if(dyn02 == (1<<EN_DPL)){
			hwlib::cout << "[OK]	enable_dyn_payload succesful on module #2\n";
		}else{
			hwlib::cout << "[FAIL]	enable_dyn_payload failed on module #2\n";
		}
		module01.disable_dyn_payload();
		module02.disable_dyn_payload();
		dyn01 = module01.read_register(R_REGISTER + FEATURE);
		dyn02 = module02.read_register(R_REGISTER + FEATURE);
		if(dyn01 == 0){
			hwlib::cout << "[OK]	disable_dyn_payload succesful on module #1\n";
		}else{
			hwlib::cout << "[FAIL]	disable_dyn_payload failed on module #1\n";
		}
		if(dyn02 == 0){
			hwlib::cout << "[OK]	disable_dyn_payload succesful on module #2\n";
		}else{
			hwlib::cout << "[FAIL]	disable_dyn_payload failed on module #2\n";
		}
		// End dynamic payload test
		
		hwlib::cout << "\nTesting enable_ack_payload function\n";
		module01.enable_ack_payload();
		module02.enable_ack_payload();
		uint8_t ack01 = module01.read_register(R_REGISTER + FEATURE);
		uint8_t ack02 = module02.read_register(R_REGISTER + FEATURE);
		if(ack01 == ( (1<<EN_DPL) | (1<<EN_ACK_PAY) ) ){
			hwlib::cout << "[OK]	enable_dyn_ack succesful on module #1\n";
		}else{
			hwlib::cout << "[FAIL]	enable_dyn_ack failed on module #1\n";
		}
		if(ack02 == ( (1<<EN_DPL) | (1<<EN_ACK_PAY) ) ){
			hwlib::cout << "[OK]	enable_dyn_ack succesful on module #2\n";
		}else{
			hwlib::cout << "[FAIL]	enable_dyn_ack failed on module #2\n";
		}
		// End enable ack payload test
		
		hwlib::cout << "\nTesting disable_features function\n";
		module01.disable_features();
		module02.disable_features();
		uint8_t feature01 = module01.read_register(R_REGISTER + FEATURE);
		uint8_t feature02 = module02.read_register(R_REGISTER + FEATURE);
		if(feature01 == 0){
			hwlib::cout << "[OK]	disable_features succesful on module #1\n";
		}else{
			hwlib::cout << "[FAIL]	disable_features failed on module #1\n";
		}
		if(feature02 == 0){
			hwlib::cout << "[OK]	disable_features succesful on module #2\n";
		}else{
			hwlib::cout << "[FAIL]	disable_features failed on module #2\n";
		}
		// End disable features test
	}
	/**
	* \brief
	* Test communication between two radio's
	* \details
	* This test writes an constructor with two values from one module to the other.
	* This will be done 4 times, the outcome will be printed to the terminal.
	*/
	void test_read_write(void){
		hwlib::cout << "\nTesting communication between two radio's\n";
		hwlib::cout << "If there are any failed messages or send message does not equel recieved message,\n";
		hwlib::cout << "then the test has been failed\n\n";
		
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
		module01.begin();
		module02.begin();
		
		// Set power level low since they are next to each other
		module01.set_power_level(pwr_low);
		module02.set_power_level(pwr_low);
		
		// Set data rate to 1mbps
		module01.set_data_rate(rf24_1mbps);
		module02.set_data_rate(rf24_1mbps);
		
		// Set transmission addresses on both radios
		module01.set_transmit_address({0xFF,0xAB,0xAB,0xAB,0xAB});
		module02.set_transmit_address({0xFF,0xAB,0xAB,0xAB,0xAB});
		
		// Stop listening on module01 to enable TX and run start_listening on module02 to enable RX
		module01.stop_listening();
		module02.start_listening();
		
		// Send an recieve data 4 times
		for(uint8_t i = 0; i<4; i++){
			hwlib::cout << "Loop #" << i+1 << "\n";
			
			hwlib::cout << "Sending temp: " << payload.temperature << '\n';
			hwlib::cout << "Sending humidity: " << payload.humidity << "\n\n";
			
			// If write is failed, print error message
			if(!module01.write(payload)){
				hwlib::cout << "Data transmission failed!\n";
			}
			
			// If data is available read it and place data in struct
			if(module02.data_available()){
				hwlib::cout << "Data available! now reading\n";
				module02.read(recv);
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
		hwlib::cout << "Two-radio communication test finished!\n";
}
};

#endif // RF_TEST_HPP
