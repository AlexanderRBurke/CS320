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

void Predictor::tournamentPredictor()
{
	long lnTotalCorrect = 0;
	int lnTableSize = 2048;

	// Create table of size 2048 w/ all values as 3 (Strongly Taken)
	vector<int> lanTournamentPredictionTable(lnTableSize, 3);
	vector<int> lanGsharePredictionTable(lnTableSize, 3);
	vector<int> lanBimodalPredictionTable(lnTableSize, 3);

	int lnGshareGlobalHistory = 0;

    bool lbGShareResults;
    bool lbBimodalResults;

    for (int i = 0; i < aaPredictions.size(); i++)
    {
        long lnTableIndex = aaHexAddresses.at(i) % lnTableSize;
        int lsPrediction = lanTournamentPredictionTable.at(lnTableIndex);

        lbGShareResults = gshareSingular(i, &lanGsharePredictionTable, &lnGshareGlobalHistory, lnTableSize);
        lbBimodalResults = bimodalSingular(i, &lanBimodalPredictionTable, lnTableSize);

        if (lsPrediction >= 2) // GShare
        {
            if (lbGShareResults)
            {
                // GShare was correct, BiModal was not
                // Was weakly taken, now make strong
                if (!lbBimodalResults && lsPrediction == 2)
                {
                    lanTournamentPredictionTable.at(lnTableIndex) = 3;
                }

                lnTotalCorrect++;
            }
            else
            {
                // GShare was wrong, BiModal was correct
                // We were wrong, so reduce ST->WT or WT->WNT
                if (lbBimodalResults)
                {
                    lanTournamentPredictionTable.at(lnTableIndex) = (lsPrediction - 1);
                }
            }
        }
        else if (lsPrediction >= 0) // BiModal
        {
            if (lbBimodalResults)
            {
                // BiModal was correct, GShare was not
                // Was weakly not taken, now make strong
                if (!lbGShareResults && lsPrediction == 1)
                {
                    lanTournamentPredictionTable.at(lnTableIndex) = 0;
                }

                lnTotalCorrect++;
            }
            else
            {
                // GShare was correct, BiModal was wrong
                // We were wrong, so reduce SNT->WNT or WNT->WT
                if (lbGShareResults)
                {
                    lanTournamentPredictionTable.at(lnTableIndex) = (lsPrediction + 1);
                }
            }
        }
    }

    printf("%ld,%lu;\n", lnTotalCorrect, aaPredictions.size());
}

bool Predictor::bimodalSingular(int anIndexPosition, vector<int> *aapnBimodalPredictionTable, int lnTableSize)
{
    long lnTotalCorrect = 0;

    bool lnRetVal;

    long lnTableIndex = aaHexAddresses.at(anIndexPosition) % lnTableSize;
    long lsPrediction = (*aapnBimodalPredictionTable).at(lnTableIndex);
    String lsRealResult = aaPredictions.at(anIndexPosition);

    if (lsPrediction >= 2) // Predict Taken
    {
        if (lsRealResult == "T")
        {
            // Was weakly taken, now make strong
            if (lsPrediction == 2)
            {
                (*aapnBimodalPredictionTable).at(lnTableIndex) = 3;
            }

            lnRetVal = true;
        }
        else
        {
            // We were wrong, so reduce ST->WT or WT->WNT
            (*aapnBimodalPredictionTable).at(lnTableIndex) = (lsPrediction - 1);
            lnRetVal = false;
        }
    }
    else if (lsPrediction >= 0) // Predict Not Taken
    {
        if (lsRealResult == "NT")
        {
            // Was weakly not taken, now make strong
            if (lsPrediction == 1)
            {
                (*aapnBimodalPredictionTable).at(lnTableIndex) = 0;
            }
            lnRetVal = true;
        }
        else
        {
            // We were wrong, so reduce SNT->WNT or WNT->WT
            (*aapnBimodalPredictionTable).at(lnTableIndex) = (lsPrediction + 1);
            lnRetVal = false;
        }
    }

    return lnRetVal;
}

bool Predictor::gshareSingular(int anIndexPosition, vector<int> *aapnGsharePredictionTable, int *anpGshareGlobalHistory, int anTableSize)
{
    bool lsRetVal;
    long lnModResult = aaHexAddresses.at(anIndexPosition) % anTableSize;
    long lnTableIndex = lnModResult ^ *anpGshareGlobalHistory; // XOR
    int lsPrediction = (*aapnGsharePredictionTable).at(lnTableIndex);
    String lsRealResult = aaPredictions.at(anIndexPosition);

    if (lsPrediction >= 2) // Predict Taken
    {
        if (lsRealResult == "T")
        {
            // Was weakly taken, now make strong
            if (lsPrediction == 2)
            {
                (*aapnGsharePredictionTable).at(lnTableIndex) = 3;
            }

            lsRetVal = true;
        }
        else
        {
            // We were wrong, so reduce ST->WT or WT->WNT
            (*aapnGsharePredictionTable).at(lnTableIndex) = (lsPrediction - 1);
            lsRetVal = false;
        }
    }
    else if (lsPrediction >= 0) // Predict Not Taken
    {
        if (lsRealResult == "NT")
        {
            // Was weakly not taken, now make strong
            if (lsPrediction == 1)
            {
                (*aapnGsharePredictionTable).at(lnTableIndex) = 0;
            }
            lsRetVal = true;
        }
        else
        {
            // We were wrong, so reduce SNT->WNT or WNT->WT
            (*aapnGsharePredictionTable).at(lnTableIndex) = (lsPrediction + 1);
            lsRetVal = false;
        }
    }

    // Update the register based on results (11 bit)
    updateGlobalRegister(anpGshareGlobalHistory, lsRealResult, 11);
    return lsRetVal;
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

void Predictor::updateGlobalRegister(int *anpGlobalHistory, const String asRealResult, int anHistoryLength)
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
