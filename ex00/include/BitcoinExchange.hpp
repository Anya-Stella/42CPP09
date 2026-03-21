#pragma once
#include <map>
#include <string>

class BitcoinExchange
{
private:
    std::map<std::string, double> _rates;

public:
	// default
    BitcoinExchange();
    BitcoinExchange(const BitcoinExchange& other);
    BitcoinExchange& operator=(const BitcoinExchange& rhs);
    ~BitcoinExchange();

	// using
    void loadDatabase(const std::string& filename);
    void processInput(const std::string& filename) const;

	// utils
private:
	std::string trim(const std::string& s) const;
    bool isValidDate(const std::string& date) const;
    bool validationAndParseValue(const std::string& str, double& value) const;
    double findRate(const std::string& date) const;
};