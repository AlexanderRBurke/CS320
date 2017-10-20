#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <string>
#include <vector>

#define String std::string
using namespace std;

class Predictor
{
	private:
		vector<long> aaHexAddresses;
		vector<String> aaPredictions;
		void branchAlwaysSingleAction(String asAction);

	public:
		Predictor(vector<long> aaHexAddress, vector<string> aaPrediction);
		void branchAlwaysTaken();
		void branchNeverTaken();
		void bimodalSingleBit(int anTableSize);
		void bimodalDoubleBit(int anTableSize);
		void gsharePredictor(int anHistoryLength);
		void tournamentPredictor();

};

#endif
