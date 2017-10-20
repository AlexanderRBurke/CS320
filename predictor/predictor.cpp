#include "predictor.h"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cmath>

Predictor::Predictor(vector<long> _aaHexAddress, vector<String> _aaPrediction) : aaHexAddresses(_aaHexAddress), aaPredictions(_aaPrediction)
{
	if (aaHexAddresses.size() != aaPredictions.size())
	{
		printf("Inputs are not same size!\n");
		return;
	}
}

void Predictor::bimodalDoubleBit(int anTableSize)
{
	long lnTotalCorrect = 0;
	
	// Create table of size anTableSize w/ all values as 3 (Strongly Taken)
	vector<int> lanPredictionTable(anTableSize, 3);
	
	for (int i = 0; i < aaPredictions.size(); i++)
	{
		int lnTableIndex = aaHexAddresses.at(i) % anTableSize;
		int lsPrediction = lanPredictionTable.at(lnTableIndex);
		String lsRealResult = aaPredictions.at(i);
	
		if (lsPrediction >= 2) // Predict Taken
		{
			if (lsRealResult == "T")
			{
				// Was weakly taken, now make strong
				if (lsPrediction == 2)
				{
					lanPredictionTable.at(lnTableIndex) = 3;
				}
				
				lnTotalCorrect++;
			}
			else
			{
				// We were wrong, so reduce ST->WT or WT->WNT
				lanPredictionTable.at(lnTableIndex) = (lsPrediction - 1);
			}
		}
		else if (lsPrediction >= 0) // Predict Not Taken
		{
			if (lsRealResult == "NT")
			{
				// Was weakly not taken, now make strong
				if (lsPrediction == 1)
				{
					lanPredictionTable.at(lnTableIndex) = 0;
				}
				
				lnTotalCorrect++;
			}
			else
			{
				// We were wrong, so reduce SNT->WNT or WNT->WT
				lanPredictionTable.at(lnTableIndex) = (lsPrediction + 1);
			}
		}
	}
	
	printf("%ld,%lu; ", lnTotalCorrect, aaPredictions.size());
}

void Predictor::bimodalSingleBit(int anTableSize)
{
	long lnTotalCorrect = 0;

	// Create table of size anTableSize w/ all values as 1
	vector<int> lanPredictionTable(anTableSize, 1);
	
	for (int i = 0; i < aaPredictions.size(); i++)
	{
		int lnTableIndex = aaHexAddresses.at(i) % anTableSize;
		int lsPrediction = lanPredictionTable.at(lnTableIndex);
		String lsRealResult = aaPredictions.at(i);

		if (lsPrediction == 1) // Predict branch to be taken
		{
			if (lsRealResult == "T")
			{
				// Prediction was correct!
				lnTotalCorrect++;
			}
			else
			{
				// Prediction was wrong, flip bit
				lanPredictionTable.at(lnTableIndex) = 0;
			}
		}
		else if (lsPrediction == 0) // Predict branch not to be taken
		{
			if (lsRealResult == "NT")
			{
				// Prediction was correct!
				lnTotalCorrect++;
			}
			else
			{
				// Prediction was wrong, flip bit
				lanPredictionTable.at(lnTableIndex) = 1;
			}
		}
	}
	
	printf("%ld,%lu; ", lnTotalCorrect, aaPredictions.size());
}

void Predictor::branchAlwaysSingleAction(String asAction)
{
	long lnTotalCorrect = 0;
	
	for (int i = 0; i < aaPredictions.size(); i++)
	{
		if (aaPredictions.at(i) == asAction)
		{
			lnTotalCorrect++;
		}
	}
	
	printf("%ld,%lu;\n", lnTotalCorrect, aaPredictions.size());
}

void Predictor::branchAlwaysTaken()
{
	branchAlwaysSingleAction("T");
}

void Predictor::branchNeverTaken()
{
	branchAlwaysSingleAction("NT");
}