#include "rf24.hpp"
#include "nrf24l01.hpp"

// All bitshift operation are with thanks to https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit

/*****************************************************************************************/
rf24::rf24(hwlib::spi_bus & bus, hwlib::pin_out & ce, hwlib::pin_out & csn):
	bus(bus),
	ce(ce),
	csn(csn),
	payload_size(32)
{}

/*****************************************************************************************/
uint8_t rf24::read_register(const uint8_t & reg){
	std::array<uint8_t, 2> input = {reg, 0x00};
	std::array<uint8_t, 2> output;
	bus.write_and_read(csn, input, output);
	return output[1];
}

/*****************************************************************************************/
std::array<uint8_t, 5> rf24::read_register_5byte(const uint8_t & reg){
	std::array<uint8_t, 6> input = {0};
	input[0] = reg;
	std::array<uint8_t, 6> output;
	bus.write_and_read(csn, input, output);
	std::array<uint8_t, 5> return_val = {	output[1],
											output[2],
											output[3],
											output[4],
											output[5]};
	return return_val;
}

/*****************************************************************************************/
uint8_t rf24::get_status(void){
	std::array<uint8_t, 1> input = {RF24_NOP};
	std::array<uint8_t, 1> output;
	bus.write_and_read(csn, input, output);
	return output[0];
}

/*****************************************************************************************/
void rf24::print_status(const uint8_t & status){
	hwlib::cout << "STATUS\t\t =" <<
	" RX_DR=" << ((status & (1<<RX_DR))?1:0) <<
	" TX_DS=" << ((status & (1<<TX_DS))?1:0) <<
	" MAX_RT=" << ((status & (1<<MAX_RT))?1:0) <<
	" RX_P_NO=" << ((status >> RX_P_NO) & 0x07) <<
	" TX_FULL=" << ((status & (1<<TX_FULL))?1:0) << '\n';
}

/*****************************************************************************************/
void rf24::print_address_register(hwlib::string<32> name, const uint8_t & reg, const uint8_t & qty){
	std::array<uint8_t, 5> value;
	hwlib::cout << name << "\t = ";
	for(uint8_t i = 0; i<qty; i++){
		value = read_register_5byte(R_REGISTER + (reg + i));
		hwlib::cout << "0x";
		for(uint8_t j = 0; j<5; j++){
			hwlib::cout << hwlib::hex <<value[j];
		}
		hwlib::cout << " ";
	}
	hwlib::cout << '\n';
}

/*****************************************************************************************/
void rf24::print_byte_register(hwlib::string<32> name, const uint8_t & reg, const uint8_t & qty){
	hwlib::cout << name << "\t = ";
	for(uint8_t i = 0; i<qty; i++){
		auto value = read_register(R_REGISTER + (reg + i));
		hwlib::cout << "0x" << hwlib::hex << value << " ";
	}
	hwlib::cout << '\n';
}

/*****************************************************************************************/
void rf24::print_details(void){
	
	hwlib::cout << "NRF24L01+ configuration\n\n";
	
	print_status(get_status());
	
	print_address_register("RX_ADDR_P0-1", RX_ADDR_P0, 2);
	print_byte_register("RX_ADDR_P2-5", RX_ADDR_P2, 4);
	print_address_register("TX_ADDR\t", TX_ADDR);

	print_byte_register("RX_PW_P0-5", RX_PW_P0,5);
	print_byte_register("EN_AA\t", EN_AA);
	print_byte_register("EN_RXADDR", EN_RXADDR);
	print_byte_register("RF_CH\t", RF_CH);
	print_byte_register("RF_SETUP", RF_SETUP);
	print_byte_register("CONFIG\t", NRF_CONFIG);
	print_byte_register("DYNPD/FEATURE",DYNPD, 2);
	hwlib::cout << '\n';
}

/*****************************************************************************************/
void rf24::write_register(const uint8_t & reg, const uint8_t & data){
	std::array<uint8_t, 2> input;
	std::array<uint8_t, 2> dummy;
	input[0] = W_REGISTER + reg;
	input[1] = data;
	bus.write_and_read(csn, input, dummy);
}

/*****************************************************************************************/
void rf24::write_register_5byte(const uint8_t & reg, const std::array<uint8_t, 5> & data){
	std::array<uint8_t, 6> input;
	std::array<uint8_t, 6> dummy;
	input[0] = W_REGISTER + reg;
	for(uint8_t i = 0; i < 5; i++){
		input[i+1] = data[i];
	}
	bus.write_and_read(csn, input, dummy);
}

/*****************************************************************************************/
void rf24::flush_tx(void){
	std::array<uint8_t, 1> input = {FLUSH_TX};
	std::array<uint8_t, 1> dummy;
	bus.write_and_read(csn, input, dummy);
}

/*****************************************************************************************/
void rf24::flush_rx(void){
	std::array<uint8_t, 1> input = {FLUSH_RX};
	std::array<uint8_t, 1> dummy;
	bus.write_and_read(csn, input, dummy);
}

/*****************************************************************************************/
void rf24::power_down(void){
	ce.set(0);
	uint8_t config = read_register(NRF_CONFIG);
	write_register(NRF_CONFIG, config & ~(1<<PWR_UP));
}

/*****************************************************************************************/
void rf24::power_up(void){
	uint8_t config = read_register(NRF_CONFIG);
	// Check if the radio is not already powered up, if not power up
	if(!(config & (1<<PWR_UP))){
		write_register(NRF_CONFIG, config | (1<<PWR_UP));
	}
}

/*****************************************************************************************/
void rf24::enable_dyn_payload(void){
	uint8_t feature = read_register(FEATURE);
	write_register(FEATURE, feature | (1<<EN_DPL));
	
	// Also enable dynamic payload on all the pipes
	uint8_t dynpd = read_register(DYNPD);
	write_register(DYNPD, dynpd | (1<<DPL_P0) | (1<<DPL_P1) | (1<<DPL_P2) | (1<<DPL_P3) | (1<<DPL_P4) | (1<<DPL_P5));
}

/*****************************************************************************************/
void rf24::enable_ack_payload(void){
	uint8_t feature = read_register(FEATURE);
	write_register(FEATURE, feature | (1<<EN_ACK_PAY));
	// For this feature to work the dynamic payload length needs to be enabled
	enable_dyn_payload();
}

/*****************************************************************************************/
void rf24::enable_dyn_ack(void){
	uint8_t feature = read_register(FEATURE);
	write_register(FEATURE, feature | (1<<EN_DYN_ACK));
}

/*****************************************************************************************/
void rf24::disable_features(void){
	write_register(FEATURE, 0);
	write_register(DYNPD, 0);
}

/*****************************************************************************************/
void rf24::write_payload(const std::array<uint8_t, 32> & data, const uint8_t & length){
	const uint8_t max_lenght = 32;
	std::array<uint8_t, 33> input = {0};
	std::array<uint8_t, 33> dummy;
	input[0] = W_TX_PAYLOAD;
	for(uint8_t i = 0; i < std::min(length, max_lenght); i++){
		input[i+1] = data[i];
	}
	/*
	hwlib::cout << "Writing payload: ";
	for(uint8_t i = 0; i < 33; i++){
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

/*****************************************************************************************/
void rf24::read_payload(std::array<uint8_t, 32> & buffer){
	std::array<uint8_t, 33> input = {0};
	std::array<uint8_t, 33> dummy;
	input[0] = R_RX_PAYLOAD;
	bus.write_and_read(csn, input, dummy);
	for(uint8_t i = 0; i < 32; i++){
		buffer[i] = dummy[i+1];
	}
}

/*****************************************************************************************/
void rf24::set_channel(const uint8_t & channel){
	const uint8_t max_channel = 125;
	write_register(RF_CH, std::min(channel, max_channel));
}

/*****************************************************************************************/
uint8_t rf24::get_channel(void){
	return read_register(RF_CH);
}

/*****************************************************************************************/
void rf24::set_power_level(uint8_t level){
	uint8_t setup = read_register(RF_SETUP) & 0xF8;
	
	if(level > pwr_max){
		level = (pwr_max << 1) + 1;
	}else{
		level = (level << 1) + 1;
	}
	write_register(RF_SETUP, (setup |= level));
}

/*****************************************************************************************/
void rf24::print_power_level(void){
	std::array<hwlib::string<7>, 4> pwr_str = {"pwr_min", "pwr_low", "pwr_high", "pwr_max"};
	uint8_t setup = read_register(RF_SETUP);
	uint8_t level = (setup & ((1 << pwr_low) | (1<<pwr_high))) >> 1;
	hwlib::cout << "Power level: " << pwr_str[level] << '\n';
}

/*****************************************************************************************/
void rf24::set_data_rate(uint8_t rate){
	uint8_t setup = read_register(RF_SETUP);
	if(rate == rf24_250kbps){
		setup ^= (-0 ^ setup) & (1 << RF_DR_HIGH);
		setup = setup | (1<< RF_DR_LOW);
		write_register(RF_SETUP, setup);
	}else if(rate == rf24_1mbps){
		// Check if RF_DR_LOW bit has been set, if so clear it first
		if((setup & (1<<RF_DR_LOW))){
			setup &= ~(1 << 5);
		}
		setup ^= (-0 ^ setup) & (1 << RF_DR_HIGH);
		write_register(RF_SETUP, setup);
	}else if(rate == rf24_2mbps){
		// Check if RF_DR_LOW bit has been set, if so clear it first
		if((setup & (1<<RF_DR_LOW))){
			setup &= ~(1 << 5);
		}
		setup ^= (-1 ^ setup) & (1 << RF_DR_HIGH);
		write_register(RF_SETUP, setup);
	}else{
		hwlib::cout << "Invalid data rate, please change parameter!\n";
	}
}

/*****************************************************************************************/
void rf24::print_data_rate(void){
	std::array<hwlib::string<12>, 3> rate_str = {"rf24_1mbps", "rf24_2mbps", "rf24_250kbps"};
	uint8_t setup = read_register(RF_SETUP);
	uint8_t rate = 0;
	if((setup & (1<<RF_DR_LOW))){
		rate = 2;
	}else if((setup & (1<<RF_DR_HIGH))){
		rate = 1;
	}
	hwlib::cout << "Data rate: " << rate_str[rate] << '\n';
}

/*****************************************************************************************/
void rf24::set_transmit_address(const std::array<uint8_t, 5> & address){
	write_register_5byte(TX_ADDR, address);
	// Also set RX_ADDR_P0 equal to this address to enable auto ack
	write_register_5byte(RX_ADDR_P0, address);
}

/*****************************************************************************************/
void rf24::set_recieve_address(const uint8_t & pipe, const std::array<uint8_t, 5> & address){
	std::array<uint8_t, 6> pipe_names = {RX_ADDR_P0, RX_ADDR_P1, RX_ADDR_P2, RX_ADDR_P3, RX_ADDR_P4, RX_ADDR_P5};
	std::array<uint8_t, 6> enable_rx = {ERX_P0, ERX_P1, ERX_P2, ERX_P3, ERX_P4, ERX_P5};
	if(pipe < 6){
		if(pipe < 2){
			write_register_5byte(pipe_names[pipe], address);
		}else{
			write_register(pipe_names[pipe], address[0]);
			// Also enable the corresponding pipe
			write_register(EN_RXADDR, (1<<enable_rx[pipe]));
		}
	}
}

/*****************************************************************************************/
void rf24::start_listening(void){
	ce.set(0);
	power_up();
	uint8_t config = read_register(NRF_CONFIG);
	write_register(NRF_CONFIG, config | (1<<PRIM_RX));
	// Reset RX_DR, TX_DS and MAX_RT to ensure good communcation
	uint8_t status = (1<<RX_DR) | (1<<TX_DS) | (1<<MAX_RT);
	write_register(NRF_STATUS, status);
	ce.set(1);
	flush_rx();
}

/*****************************************************************************************/
void rf24::stop_listening(void){
	ce.set(0);
	hwlib::wait_ms(200);
	uint8_t config = read_register(NRF_CONFIG);
	write_register(NRF_CONFIG, config & ~(1<<PRIM_RX));
	// Reset RX_DR, TX_DS and MAX_RT to ensure good communcation
	uint8_t status = (1<<RX_DR) | (1<<TX_DS) | (1<<MAX_RT);
	write_register(NRF_STATUS, status);
	
	flush_tx();
	power_up();
}

/*****************************************************************************************/
bool rf24::data_available(void){
	uint8_t status = read_register(FIFO_STATUS);
	// Check if RX_EMPTY bit has been set, if not data is available
	if(!(status & (1<<RX_EMPTY) )){
		return 1;
	}
	return 0;
}

// Deprecated code

/*****************************************************************************************/
/*
void rf24::write(const hwlib::string<0> & data){
	std::array<uint8_t, 32> buffer = {0};
	for(uint8_t i = 0; i < data.length(); i ++){
		buffer[i] = data[i];
	}
	write_payload(buffer, 32);
}
*/
/*****************************************************************************************/
/*
void rf24::read(hwlib::string<32> & buffer){
	std::array<uint8_t, 32> output = {0};
	read_payload(output);
	for(uint8_t i = 0; i < 32; i++){
		buffer[i] = output[i];
	}
}
 */
/*****************************************************************************************/
void rf24::begin(void){
	// Enable features
	enable_ack_payload();
	enable_dyn_ack();
	// Change from default channel on start, can always be changed after calling begin
	set_channel(60);
}