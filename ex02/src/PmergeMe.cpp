#include "../include/PmergeMe.hpp"

/******** VALIDATION ********/
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


/******** PARSE INPUT ********/
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


/******** DISPLAY ********/
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


/******** BUILD INSERTION ORDER ********/
// 1, 3, 5, 11, 21, 43, 85, 171, 341, 683, 1365, 2731, 5461, 10923, 21845, 43691, 87381, 174763...
static std::vector<std::size_t> buildBounds(std::size_t pendingCount)
{
	std::vector<std::size_t> bounds;
	if (pendingCount == 0)
		return bounds;

	std::size_t j0 = 0;
	std::size_t j1 = 1;

	while (true)
	{
		std::size_t j2 = j1 + 2 * j0;

		if (j2 > pendingCount)
			break;

		bounds.push_back(j2);

		j0 = j1;
		j1 = j2;
	}

	if (bounds.empty() || bounds.back() != pendingCount)
		bounds.push_back(pendingCount);

	return bounds;
}

// buildInsertionOrder(5) = {0, 2, 1, 4, 3}
static std::vector<std::size_t> buildInsertionOrder(std::size_t pendingCount)
{
	std::vector<std::size_t> order;
	
	// 境界を出す
	std::vector<std::size_t> bounds = buildBounds(pendingCount);

	// 境界から挿入順を構築
	std::size_t prev = 0;
	for (std::size_t i = 0; i < bounds.size() ; ++i)
	{
		std::size_t curr = bounds[i];
		for (std::size_t x = curr; x > prev; --x)
			order.push_back(x - 1);// 1-based -> 0-based
		prev = curr;
	}
	return order;
}


/******** FORD-JOHNSON ALGORITHM FOR VECTORS ********/
static std::vector< std::pair<int, int> > makePairsVector(const std::vector<int> &input, bool &hasOdd, int &oddValue)
{
	std::vector< std::pair<int, int> > pairs;
	hasOdd = false;
	oddValue = 0;

	for (std::size_t i = 0; i + 1 < input.size(); i += 2)
	{
		int a = input[i];
		int b = input[i + 1];
		if (a < b)
			std::swap(a, b);
		pairs.push_back(std::make_pair(a, b));
	}
	if (input.size() % 2 != 0)
	{
   		hasOdd = true;
    	oddValue = input.back();
	}
	return pairs;
}

static std::vector<int> extractLargeValues(const std::vector< std::pair<int, int> > &pairs)
{
	std::vector<int> largeValues;
	for (std::size_t i = 0; i < pairs.size(); ++i)
		largeValues.push_back(pairs[i].first);
	return largeValues;
}

static std::vector< std::pair<int, int> > reorderPairsBySortedLarge(const std::vector< std::pair<int, int> > &pairs, const std::vector<int> &sortedLarge)
{
	std::vector< std::pair<int, int> > ordered;
	std::vector<bool> used(pairs.size(), false);

	for (std::size_t i = 0; i < sortedLarge.size(); ++i)
	{
		for (std::size_t j = 0; j < pairs.size(); ++j)
		{
			if (!used[j] && pairs[j].first == sortedLarge[i])
			{
				ordered.push_back(pairs[j]);
				used[j] = true;
				break;
			}
		}
	}
	return ordered;
}

static std::vector<int> mergeChains(const std::vector< std::pair<int, int> > &orderedPairs, bool hasOdd, int oddValue)
{
	std::vector<int>	mainChain;

	// mainChainの初期化
	for (std::size_t i = 0; i < orderedPairs.size(); ++i)
		mainChain.push_back(orderedPairs[i].first);
	
	// 挿入順の構築
	std::vector<std::size_t> order = buildInsertionOrder(orderedPairs.size());// {0, 2, 1, 4, 3, ...}

	// {0, 2, 1, 4, 3, ...}の順で、orderedPairsから値をmainChainに挿入
	for (std::size_t i = 0; i < order.size(); ++i)
	{
		std::size_t idx = order[i];
		int valueToInsert = orderedPairs[idx].second;
		int upperValue = orderedPairs[idx].first;

		// valueToInsertをmainChainに挿入
		std::vector<int>::iterator upperIt = std::find(mainChain.begin(), mainChain.end(), upperValue);
		std::vector<int>::iterator insertPos = std::lower_bound(mainChain.begin(), upperIt, valueToInsert);
		mainChain.insert(insertPos, valueToInsert);
	}
	if (hasOdd)
	{
		std::vector<int>::iterator insertPos = std::lower_bound(mainChain.begin(), mainChain.end(), oddValue);
		mainChain.insert(insertPos, oddValue);
	}
	return mainChain;
}

std::vector<int> fordJohnsonVector(const std::vector<int> &input)
{
	bool hasOdd = false;
	int oddValue = 0;

	// ベースケース
	if (input.size() <= 1)
		return input;


	// 1: ペア作成
	std::vector< std::pair<int, int> > pairs = makePairsVector(input, hasOdd, oddValue);


	// 2: ペアの大きい順にpairsをソート
	std::vector<int> largeValues = extractLargeValues(pairs);
	std::vector<int> sortedLarge = fordJohnsonVector(largeValues);
	std::vector< std::pair<int, int> >	orderedPairs = reorderPairsBySortedLarge(pairs, sortedLarge);


	// 3: orderedPairsを基に、mainchainとsubchainをマージ
	return mergeChains(orderedPairs, hasOdd, oddValue);
}


/******** FORD-JOHNSON ALGORITHM FOR DEQUES ********/
static std::deque< std::pair<int, int> > makePairsDeque(const std::deque<int> &input, bool &hasOdd, int &oddValue)
{
	std::deque< std::pair<int, int> > pairs;
	hasOdd = false;
	oddValue = 0;

	for (std::size_t i = 0; i + 1 < input.size(); i += 2)
	{
		int a = input[i];
		int b = input[i + 1];
		if (a < b)
			std::swap(a, b);
		pairs.push_back(std::make_pair(a, b));
	}
	if (input.size() % 2 != 0)
	{
		hasOdd = true;
		oddValue = input.back();
	}
	return pairs;
}

static std::deque<int> extractLargeValuesDeque(const std::deque< std::pair<int, int> > &pairs)
{
	std::deque<int> largeValues;
	for (std::size_t i = 0; i < pairs.size(); ++i)
		largeValues.push_back(pairs[i].first);
	return largeValues;
}

static std::deque< std::pair<int, int> > reorderPairsBySortedLargeDeque(const std::deque< std::pair<int, int> > &pairs, const std::deque<int> &sortedLarge)
{
	std::deque< std::pair<int, int> > ordered;
	std::vector<bool> used(pairs.size(), false);

	for (std::size_t i = 0; i < sortedLarge.size(); ++i)
	{
		for (std::size_t j = 0; j < pairs.size(); ++j)
		{
			if (!used[j] && pairs[j].first == sortedLarge[i])
			{
				ordered.push_back(pairs[j]);
				used[j] = true;
				break;
			}
		}
	}
	return ordered;
}

static std::deque<int> mergeChainsDeque(const std::deque< std::pair<int, int> > &orderedPairs, bool hasOdd, int oddValue)
{
	std::deque<int> mainChain;

	// mainChainの初期化
	for (std::size_t i = 0; i < orderedPairs.size(); ++i)
		mainChain.push_back(orderedPairs[i].first);

	// 挿入順の構築
	std::vector<std::size_t> order = buildInsertionOrder(orderedPairs.size()); // {0, 2, 1, 4, 3, ...}

	// {0, 2, 1, 4, 3, ...}の順で、orderedPairsから値をmainChainに挿入
	for (std::size_t i = 0; i < order.size(); ++i)
	{
		std::size_t idx = order[i];
		int valueToInsert = orderedPairs[idx].second;
		int upperValue = orderedPairs[idx].first;

		// valueToInsertをmainChainに挿入
		std::deque<int>::iterator upperIt = std::find(mainChain.begin(), mainChain.end(), upperValue);
		std::deque<int>::iterator insertPos = std::lower_bound(mainChain.begin(), upperIt, valueToInsert);
		mainChain.insert(insertPos, valueToInsert);
	}
	if (hasOdd)
	{
		std::deque<int>::iterator insertPos = std::lower_bound(mainChain.begin(), mainChain.end(), oddValue);
		mainChain.insert(insertPos, oddValue);
	}
	return mainChain;
}

std::deque<int> fordJohnsonDeque(const std::deque<int> &input)
{
	bool hasOdd = false;
	int oddValue = 0;

	// ベースケース
	if (input.size() <= 1)
		return input;

	// 1: ペア作成
	std::deque< std::pair<int, int> > pairs = makePairsDeque(input, hasOdd, oddValue);

	// 2: ペアの大きい順にpairsをソート
	std::deque<int> largeValues = extractLargeValuesDeque(pairs);
	std::deque<int> sortedLarge = fordJohnsonDeque(largeValues);
	std::deque< std::pair<int, int> > orderedPairs = reorderPairsBySortedLargeDeque(pairs, sortedLarge);

	// 3: orderedPairsを基に、mainchainとsubchainをマージ
	return mergeChainsDeque(orderedPairs, hasOdd, oddValue);
}

