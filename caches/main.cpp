#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#define String std::string

using namespace std;

//void runBranchPredictions(Predictor aoPredictor, String asOutputFileName);

int main(int argc, char *argv[])
{
	vector<pair<char, long>> cooInputVector;

    if (argc != 3)
    {
        cerr << "Usage: ./cache-sim <input_file.txt> <output_file.txt>\n";
        exit(1);
    }

    ifstream coInputFile(argv[1]);
	String lsLine;

	if (coInputFile.is_open())
	{
		while (getline(coInputFile, lsLine))
		{
			char lsAction = lsLine.at(0); // "S"tore or "L"oad
			String lsAddress = lsLine.substr(2); // 0x00000000 - 0xFFFFFFFF
			unsigned long llHexAddress = stoul(lsAddress, nullptr, 16);

			cooInputVector.push_back(pair<char, long>(lsAction, llHexAddress));
		}

		coInputFile.close();
	}
	else
	{
		cerr << "Error opening file " << argv[1] << "\n";
        exit(3);
	}
	
	/*
	Predictor loPredictor(cooInputVector, coActionInputVector);
	runBranchPredictions(loPredictor, argv[2]);
	*/
	
	for (int i = 0; i < cooInputVector.size(); i++)
	{
	     cout << cooInputVector[i].first << ", " << cooInputVector[i].second << endl;
	}

    return 0;
}

/*void runBranchPredictions(Predictor aoPredictor, String asOutputFileName)
{
    vector<int> lanTableValues = {16, 32, 128, 256, 512, 1024, 2048};
    pair<long, unsigned long> loRetVal;
    ofstream loOutputFile(asOutputFileName);

    if (! loOutputFile.is_open())
    {
        cerr << "Unable to open file " << asOutputFileName << "\n";
        exit(2);
    }

    // Q1: Predict Always Taken
    loRetVal = aoPredictor.branchAlwaysTaken();
    loOutputFile << loRetVal.first << "," << loRetVal.second << ";\n";

    // Q2: Predict Never Taken
    loRetVal = aoPredictor.branchNeverTaken();
    loOutputFile << loRetVal.first << "," << loRetVal.second << ";\n";

    // Q3: Single-Bit BiModal (16, 32, 128, 512, 1024, 2048 bit tables)
    for (int i = 0; i < lanTableValues.size(); i++)
    {
        loRetVal = aoPredictor.bimodalSingleBit(lanTableValues.at(i));
        loOutputFile << loRetVal.first << "," << loRetVal.second;

        loOutputFile << ((i == lanTableValues.size() - 1) ? ";" : "; ");
    }
    loOutputFile << "\n";

    // Q4: Double-Bit BiModal (16, 32, 128, 512, 1024, 2048 bit tables)
    for (int i = 0; i < lanTableValues.size(); i++)
    {
        loRetVal = aoPredictor.bimodalDoubleBit(lanTableValues.at(i));
        loOutputFile << loRetVal.first << "," << loRetVal.second;

        loOutputFile << ((i == lanTableValues.size() - 1) ? ";" : "; ");
    }
    loOutputFile << "\n";

    // Q5: GShare (3-11 bit global history)
    for (int i = 3; i <= 11; i++)
    {
        loRetVal = aoPredictor.gsharePredictor(i);
        loOutputFile << loRetVal.first << "," << loRetVal.second;

        loOutputFile << ((i == 11) ? ";" : "; ");
    }
    loOutputFile << "\n";

    // Q6: Tournament Predictor
    loRetVal = aoPredictor.tournamentPredictor();
    loOutputFile << loRetVal.first << "," << loRetVal.second << ";\n";

    loOutputFile.close();
}*/