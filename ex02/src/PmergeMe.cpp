#include "../include/PmergeMe.hpp"

// validation
static void validateToken(const std::string &s)
{
	if (s.empty())
		throw std::runtime_error("invalid token: empty");

	char *end = NULL;
	long value = std::strtol(s.c_str(), &end, 10);

	if (*end != '\0' ||  value <= 0 || INT_MAX < value)
		throw std::runtime_error("invalid token: " + s);
}

void validateInput(int argc, char **argv)
{
	for(int i = 1; i < argc; ++i)
		validateToken(argv[i]);
}


// ParseInput
std::vector<int>	parseVector(int argc, char **argv)
{
	std::vector<int> vec;
	for (int i = 1; i < argc; ++i)
		vec.push_back(std::atoi(argv[i]));
	return vec;
}

std::deque<int>		parseDeque(int argc, char **argv)
{
	std::deque<int> deq;
	for (int i = 1; i < argc; ++i)
		deq.push_back(std::atoi(argv[i]));
	return deq;
}


// display
void printBefore(const std::vector<int> &vec)
{
	std::cout << "Before: ";
	for (std::size_t i = 0; i < vec.size(); ++i)
		std::cout << " " << vec[i];
	std::cout << std::endl;
}

void printAfter(const std::vector<int> &vec)
{
	std::cout << "After: ";
	for (std::size_t i = 0; i < vec.size(); ++i)
		std::cout << " " << vec[i];
	std::cout << std::endl;
}

void printTime(const std::string& containerName,std::size_t size, double time)
{
	std::cout 
		<< "Time to process a range of " << size
		<< " elements with " << containerName
		<< ": " << time << " us" << std::endl;
}


// sorting algorithm
std::vector<int> fordJohnsonVector(const std::vector<int> &input)
{

}