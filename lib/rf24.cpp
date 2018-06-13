#include "rf24.hpp"
#include "nrf24l01.hpp"

/*****************************************************************************************/
rf24::rf24(hwlib::spi_bus & bus, hwlib::pin_out & ce, hwlib::pin_out & csn):
	bus(bus),
	ce(ce),
	csn(csn)
{}

/*****************************************************************************************/
uint8_t rf24::read_register_1byte(const uint8_t & reg){
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
		auto value = read_register_1byte(R_REGISTER + (reg + i));
		hwlib::cout << "0x" << hwlib::hex << value << " ";
	}
	hwlib::cout << '\n';
}

/*****************************************************************************************/
void rf24::print_details(void){
	
	hwlib::cout << "NRF24L01+ configuration\n\n";
	
	print_address_register("RX_ADDR_P0", RX_ADDR_P0, 2);
	print_byte_register("RX_ADDR_P2-5", RX_ADDR_P2, 4);
	print_address_register("TX_ADDR\ts", TX_ADDR);

	print_byte_register("RX_PW_P0-5", RX_PW_P0,5);
	print_byte_register("EN_AA\t", EN_AA);
	print_byte_register("EN_RXADDR", EN_RXADDR);
	print_byte_register("RF_CH\t", RF_CH);
	print_byte_register("RF_SETUP", RF_SETUP);
	print_byte_register("CONFIG\t", NRF_CONFIG);
	print_byte_register("DYNPD/FEATURE",DYNPD, 2);
}

/*****************************************************************************************/
