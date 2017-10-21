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
        pair<long, unsigned long> branchAlwaysSingleAction(String asAction);
		void updateGlobalRegister(int *anGlobalHistory, const String asRealResult, int anHistoryLength);
		bool gshareSingular(int anIndexPosition, vector<int> *aapnGsharePredictionTable, int *anpGshareGlobalHistory, int anTableSize);
        bool bimodalSingular(int anIndexPosition, vector<int> *aapnBimodalPredictionTable, int lnTableSize);

	public:
		Predictor(vector<long> aaHexAddress, vector<string> aaPrediction);
        pair<long, unsigned long> branchAlwaysTaken();
        pair<long, unsigned long> branchNeverTaken();
        pair<long, unsigned long> bimodalSingleBit(int anTableSize);
        pair<long, unsigned long> bimodalDoubleBit(int anTableSize);
        pair<long, unsigned long> gsharePredictor(int anHistoryLength);
        pair<long, unsigned long> tournamentPredictor();

};

#endif
