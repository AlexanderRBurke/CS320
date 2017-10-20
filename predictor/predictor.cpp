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

void Predictor::gsharePredictor(int anHistoryLength)
{
	long lnTotalCorrect = 0;
	int lnTableSize = 2048;
	
	// Create table of size 2048 w/ all values as 3 (Strongly Taken)
	vector<int> lanPredictionTable(lnTableSize, 3);
	
	int lnGlobalHistory = 0;
	
	for (int i = 0; i < aaPredictions.size(); i++)
	{
		int lnModResult = aaHexAddresses.at(i) % lnTableSize;
		int lnTableIndex = lnModResult ^ lnGlobalHistory; // XOR
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
		
		// Update the register based on results
		updateGlobalRegister(&lnGlobalHistory, lsRealResult, anHistoryLength);
		
		
	}
	
	printf("%ld,%lu; ", lnTotalCorrect, aaPredictions.size());
}

void Predictor::updateGlobalRegister(int *anpGlobalHistory, String asRealResult, int anHistoryLength)
{
	/**
	 * lnGlobalHistory = # which gets us `N N ... N N`
	 * To add new bit to the right:
	 * -Left shift 1 which gets us `N N ... N N 0`
	 * -OR with the result (0 or 1) from the prediction, which gets us `N N ... N N M`
	 * -Get result of 2^(#)-1 which is `0 1 1 ... 1 1`
	 * -AND lnGlobalHistory with 2^(#)-1
	 * -This will return result of `0 0 ... 0 N N ... N N M` which is what we want
	 */
	
	int lnGlobalHistory = *anpGlobalHistory;
	int lnNewBitToAdd = (asRealResult == "T") ? 1 : 0;
	lnGlobalHistory = (int)(lnGlobalHistory << 1);
	lnGlobalHistory = (int)(lnGlobalHistory | lnNewBitToAdd);
	lnGlobalHistory = (int)(lnGlobalHistory & (int)(pow(2, anHistoryLength) - 1));
	*anpGlobalHistory = lnGlobalHistory;
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