#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include "predictor.h"

#define String std::string
using namespace std;

void runBranchPredictions(Predictor aoPredictor)
{
	vector<int> lanTableValues = {16, 32, 128, 256, 512, 1024, 2048};

	aoPredictor.branchAlwaysTaken();
	aoPredictor.branchNeverTaken();
	
	for (int i = 0; i < lanTableValues.size(); i++)
	{
		aoPredictor.bimodalSingleBit(lanTableValues.at(i));
	}
	cout << "\n";
	
	for (int i = 0; i < lanTableValues.size(); i++)
	{
		aoPredictor.bimodalDoubleBit(lanTableValues.at(i));
	}
	cout << "\n";
	
	for (int i = 3; i <= 11; i++)
	{
		aoPredictor.gsharePredictor(i);
	}
	cout << "\n";

    aoPredictor.tournamentPredictor();
}

int main(int argc, char *argv[])
{
	vector<long> coInputVector;
	vector<String> coActionInputVector;

	ifstream coInputFile(argv[1]);
	String lsLine;

	if (coInputFile.is_open())
	{
		while (getline(coInputFile, lsLine))
		{
			String lsAddress = lsLine.substr(0, lsLine.find(" "));
			String lsAction = lsLine.substr(lsLine.find(" ") + 1, lsAddress.length());
			unsigned long llHexAddress = stoul(lsAddress, nullptr, 16);

			coInputVector.push_back(llHexAddress);
			coActionInputVector.push_back(lsAction);
		}

		coInputFile.close();
	}
	else
	{
		cout << "Error opening file " << argv[1] << "\n";
	}
	
	Predictor loPredictor(coInputVector, coActionInputVector);
	runBranchPredictions(loPredictor);
}