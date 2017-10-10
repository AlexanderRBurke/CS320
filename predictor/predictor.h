#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <string>
#define String std::string

class Predictor
{
	private:

	public:
		Predictor();
		Predictor(String asInputFile, String asOutputFile);
		void branchAlwaysTaken();
		void branchNeverTaken();
		void bimodalSingleBit(int anTableSize);
		void bimodalDoubleBit(int anTableSize);
		void gsharePredictor(int anHistoryLength);
		void tournamentPredictor();

};

#endif
