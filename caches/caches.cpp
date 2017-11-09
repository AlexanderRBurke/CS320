#include "caches.h"
#include <cmath>

#define BLOCK_SIZE 32

Caches::Caches(vector<pair<char, int> > aooInputs) : gooInputs(aooInputs)
{

}

int Caches::getCountEntries()
{
    return gooInputs.size();
}

int Caches::directMapped(int anCacheEntries)
{
    /*
     * Size = Indexes * Data.Size = anCacheEntries * 2^5 bytes
     * 2^5 entries with 1KB (2^10) as data size is 2^5
     * 2^7 entries with 4KB (2^12) as data size is 2^5
     * 2^9 entries with 16KB (2^14) as data size is 2^5
     * 2^10 entries with 32KB (2^15) as data size is 2^5
     */

    int lnHit = 0;

    // Dynamic allocation of 2D array
    int **lanPageTable = new int*[anCacheEntries];
    for (int i = 0; i < anCacheEntries; i++)
    {
        lanPageTable[i] = new int[2];
    }

    // Zero out new 2D array
    for (int i = 0; i < anCacheEntries; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            lanPageTable[i][j] = 0;
        }
    }

    for (auto loIterator = gooInputs.begin(); loIterator != gooInputs.end(); loIterator++)
    {
        int lnBlockAddress = floor(loIterator->second / BLOCK_SIZE);
        int lnPTIndex = lnBlockAddress % anCacheEntries;
        int lnPTTag = lnBlockAddress >> (int)(log(anCacheEntries)/log(2)); // TODO: This might be wrong

        // Page is Valid
        if (lanPageTable[lnPTIndex][0] == 1)
        {
            // Tag is a match
            if (lanPageTable[lnPTIndex][1] == lnPTTag)
            {
                lnHit++;
            }
            else
            {
                lanPageTable[lnPTIndex][1] = lnPTTag;
                lanPageTable[lnPTIndex][0] = 1;
            }
        }
        else
        {
            lanPageTable[lnPTIndex][1] = lnPTTag;
            lanPageTable[lnPTIndex][0] = 1;
        }
    }


    // Free dynamic 2D array
    for (int i = 0; i < anCacheEntries; i++)
    {
        delete[] lanPageTable[i];
    }
    delete[] lanPageTable;

    return lnHit;
}