#include "../include/BitcoinExchange.hpp"
#include <iostream>

int	main(int argc, char **argv)
{
	// validation
	if (argc != 2)
	{
		std::cerr << "Error: could not open file." << std::endl;
		return 1;
	}

	// generate obj(To store DB value in a map structure)
	// read DB
	// output
	try
	{
		BitcoinExchange	btc;

		btc.loadDatabase("data.csv");
		btc.processInput(argv[1]);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
