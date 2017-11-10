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

        // Index.size + Offset.size
        int lnBitsToOffset = (int)((log(anCacheEntries)/log(2)) + (log(BLOCK_SIZE)/log(2)));
        int lnPTTag = loIterator->second / anCacheEntries;

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

int Caches::setAssociative(int anAssociativityEntries)
{
    /*
     * Set-Associative Cache.
     * Again, assume that the cache line size is 32 bytes and
     * model a 16KB cache with
     * associativity of 2, 4, 8 and 16
     * Assume that the least recently used (LRU) replacement policy is implemented.
     */

    // 2^9 entries w/ 2^5 line size = 16KB cache
    const int lnCacheEntries = 512 / anAssociativityEntries;

    // Row will be [VALID | TAG | timestamp | VALID | TAG | timestamp | ...]
    const int lnCacheRowLength = 3 * anAssociativityEntries;

    int lnHit = 0;

    // Dynamic allocation of 2D array
    int **lanPageTable = new int*[lnCacheEntries];
    for (int i = 0; i < lnCacheEntries; i++)
    {
        lanPageTable[i] = new int[lnCacheRowLength];
    }

    // Zero out new 2D array
    for (int i = 0; i < lnCacheEntries; i++)
    {
        for (int j = 0; j < lnCacheRowLength; j++)
        {
            lanPageTable[i][j] = 0;
        }
    }

    for (auto loIterator = gooInputs.begin(); loIterator != gooInputs.end(); loIterator++)
    {
        int lnBlockAddress = floor(loIterator->second / BLOCK_SIZE);
        int lnPTIndex = lnBlockAddress % lnCacheEntries;

        // Index.size + Offset.size
        int lnPTTag = loIterator->second / lnCacheEntries; // Removes offset from actual address

        // Iterate through each "way" in the cache row
        int *lanPageTableRow = lanPageTable[lnPTIndex];
        bool lbEntryFound = false;
        for (int i = 0; i < lnCacheRowLength; i += lnCacheRowLength / anAssociativityEntries)
        {
            if (lanPageTableRow[i] == 1)
            {
                if (lanPageTableRow[i + 1] == lnPTTag)
                {
                    lnHit++;
                    lanPageTableRow[i + 2] = distance(gooInputs.begin(), loIterator);
                    lbEntryFound = true;
                    break; // Exit the for() as we have a hit
                }
            }
        }

        // If entry not found, will need to import it to the cache
        if (! lbEntryFound)
        {
            // We will need to now implement the least recently used policy
            bool lbEntryInputted = false;

            // If there's an invalid entry, can write in that location
            for (int i = 0; i < lnCacheRowLength; i += lnCacheRowLength / anAssociativityEntries)
            {
                // Page is invalid, so we can write here
                if (lanPageTableRow[i] == 0)
                {
                    lanPageTableRow[i] = 1;
                    lanPageTableRow[i + 1] = lnPTTag;
                    lanPageTableRow[i + 2] = distance(gooInputs.begin(), loIterator);
                    lbEntryInputted = true;
                    break;
                }
            }

            if (! lbEntryInputted)
            {
                // Find the least recently used and overwrite it
                int lnLeastRecentlyUsedIndex = 0;
                int lnMinDistance = INT_MAX;
                for (int i = 0; i < lnCacheRowLength; i += lnCacheRowLength / anAssociativityEntries)
                {
                    if (lanPageTableRow[i + 2] < lnMinDistance)
                    {
                        lnMinDistance = lanPageTableRow[i + 2];
                        lnLeastRecentlyUsedIndex = i;
                    }
                }

                //lanPageTableRow[lnLeastRecentlyUsedIndex] = 1; // Already Valid
                lanPageTableRow[lnLeastRecentlyUsedIndex + 1] = lnPTTag;
                lanPageTableRow[lnLeastRecentlyUsedIndex + 2] = distance(gooInputs.begin(), loIterator);
            }
        }
    }

    // Free dynamic 2D array
    for (int i = 0; i < lnCacheEntries; i++)
    {
        delete[] lanPageTable[i];
    }
    delete[] lanPageTable;

    return lnHit;
}