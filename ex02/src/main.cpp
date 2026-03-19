#include "../include/PmergeMe.hpp"

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <numbers>" << std::endl;
		return 1;
	}
	try
	{
		// Validate input
		validateInput(argc, argv);

		
		// Start soring
		std::vector<int> vecTemp = parseVector(argc, argv);
		printBefore(vecTemp);


		// Sort and time
		clock_t vStart = clock();
		std::vector<int> vec = parseVector(argc, argv);
		std::vector<int> sortedVec = fordJohnsonVector(vec);
		clock_t vEnd = clock();
		
		clock_t dStart = clock();
		std::deque<int> deq = parseDeque(argc, argv);
		std::deque<int> sortedDeq = fordJohnsonDeque(deq);
		clock_t dEnd = clock();

		printAfter(sortedVec);


		// calculate time and print
		double vTime = static_cast<double>(vEnd - vStart) * 1000000.0 / CLOCKS_PER_SEC;
		double dTime = static_cast<double>(dEnd - dStart) * 1000000.0 / CLOCKS_PER_SEC;

		printTime("std::vector", vec.size(), vTime);
		printTime("std::deque", deq.size(), dTime);
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
