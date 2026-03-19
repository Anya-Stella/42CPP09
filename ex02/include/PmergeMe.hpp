#pragma once
#include <vector>
#include <deque>
#include <string>
#include <iostream>
#include <ctime>

// validation
void				validateInput(int argc, char **argv);

// parse input
std::vector<int>	parseVector(int argc, char **argv);
std::deque<int>		parseDeque(int argc, char **argv);

// display
void				printBefore(const std::vector<int> &vec);
void				printAfter(const std::vector<int> &vec);
void				printTime(const std::string& containerName,std::size_t size, double time);

// sorting algorithm
std::vector<int>	fordJohnsonVector(const std::vector<int> &input);
// std::deque<int>		fordJohnsonDeque(const std::deque<int> &input);