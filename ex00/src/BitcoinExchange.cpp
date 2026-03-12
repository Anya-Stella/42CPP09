#include "../include/BitcoinExchange.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>

// default
BitcoinExchange::BitcoinExchange() {};
BitcoinExchange::BitcoinExchange(const BitcoinExchange& other) : _rates(other._rates) {};
BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& rhs)
{
	if (this != &rhs)
		_rates = rhs._rates;
	return (*this);
}
BitcoinExchange::~BitcoinExchange() {}

// utils
// " hello " -> "hello"
std::string	BitcoinExchange::trim(const std::string& s) const
{
	std::string::size_type	start = s.find_first_not_of(" \t\n\r\f\v");
	if (start == std::string::npos)
		return ("");
	std::string::size_type	end = s.find_last_not_of(" \t\n\r\f\v");
	return (s.substr(start, end - start + 1));
}

// validation
bool BitcoinExchange::isValidDate(const std::string& date) const
{
	// length
	if (date.size() != 10)
		return false;
	if (date[4] != '-' || date[7] != '-')
		return false;
	
	// format
	for (int i = 0; i < 10; ++i)
	{
		if (i == 4 || i == 7)
			continue;
		if (!std::isdigit(date[i]))
			return false;
	}

	// correct number
	int year = std::atoi(date.substr(0, 4).c_str());
	int month = std::atoi(date.substr(5, 2).c_str());
	int day = std::atoi(date.substr(8, 2).c_str());

	// month
	if (month < 1 || 12 < month)
		return false;
	
	// day
	if (day < 1)
		return false;
	int daysInMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	bool leap = false;
	if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
		leap = true;
	if(leap)
		daysInMonth[1] = 29;
	if (day > daysInMonth[month - 1])
		return false;
	
	return true;
}

double BitcoinExchange::findRate(const std::string& date) const
{
	std::map<std::string, double>::const_iterator it = _rates.lower_bound(date);

	if (it != _rates.end() && it->first == date)
		return it->second;
	if (it == _rates.begin())
		throw std::runtime_error("Error: bad Input => " + date);
	
	// return past value
	--it;
	return it->second;
}

bool BitcoinExchange::validationAndParseValue(const std::string& str, double& value) const
{
	std::stringstream	ss(str);
	char				extra;

	if (!(ss >> value))
		return false;
	if (ss >> extra)
		return false;
	return true;
}

// loadDatabase
void BitcoinExchange::loadDatabase(const std::string& filename)
{
	// fileOpen
	std::ifstream file(filename.c_str());
	if (!file.is_open())
		throw std::runtime_error("Error: Could not open DataBase file.");

	// getNextLine
	std::string	Line;// 2009-02-04,0
	if (!std::getline(file, Line))
		throw std::runtime_error("Error: Empty database file.");
	
	// parse->mapping
	while(std::getline(file, Line))
	{
		// check empty
		if (Line.empty())
			continue;
		
		// get commmaPos
		std::string::size_type commaPos = Line.find(',');
		if (commaPos == std::string::npos)
			throw std::runtime_error("Error: Bad Database Line => " + Line);

		// get key and value
		std::string	dateStr = trim(Line.substr(0, commaPos));// 2009-02-04
		std::string	rateStr = trim(Line.substr(commaPos + 1));// 0
		double rate;

		// validation and input map structure
		if (!isValidDate(dateStr))
			throw std::runtime_error("Error: bad date in Database => " + dateStr);
		if (!validationAndParseValue(rateStr, rate) || rate < 0)
			throw std::runtime_error("Error: bad rate in Database => " + rateStr);
		_rates[dateStr] = rate;
	}

	if (_rates.empty())
		throw std::runtime_error("Error: no data in Database.");
}

// processInput
void BitcoinExchange::processInput(const std::string& filename) const
{
	// file Open
	std::ifstream file(filename.c_str());
	if (!file.is_open())
		throw std::runtime_error("Error: could not open file.");
	
	// get next line
	std::string line;
	if (!std::getline(file, line))
		throw std::runtime_error("Error: empty input file.");
	while (std::getline(file, line))
	{
		if (line.empty())
			continue;
		
		try
		{
			std::string::size_type pipePos = line.find('|');
			if (pipePos == std::string::npos)
				throw std::runtime_error("Error: bad input => " + line);
			std::string dateStr = trim(line.substr(0, pipePos));
			std::string valueStr = trim(line.substr(pipePos + 1));
			double value;

			if (!isValidDate(dateStr))
				throw std::runtime_error("Error: bad input => " + line);
			if (!validationAndParseValue(valueStr, value))
				throw std::runtime_error("Error: bad input => " + line);
			
			if (value < 0)
				throw std::runtime_error("Error: not a positive number.");
			if (value > 1000)
				throw std::runtime_error("Error: too large a number.");
			
			// output
			double rate = findRate(dateStr);
			std::cout << dateStr << " => " << value << " = " << value * rate << std::endl;
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}
}
