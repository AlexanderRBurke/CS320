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
    const int lnCacheRowEntries = 512 / anAssociativityEntries;

    // Row will be [VALID | TAG | timestamp | VALID | TAG | timestamp | ...]
    const int lnCacheRowLength = 3 * anAssociativityEntries;

    int lnHit = 0;

    // Dynamic allocation of 2D array
    int **lanPageTable = new int*[lnCacheRowEntries];
    for (int i = 0; i < lnCacheRowEntries; i++)
    {
        lanPageTable[i] = new int[lnCacheRowLength];
    }

    // Zero out new 2D array
    for (int i = 0; i < lnCacheRowEntries; i++)
    {
        for (int j = 0; j < lnCacheRowLength; j++)
        {
            lanPageTable[i][j] = 0;
        }
    }

    for (auto loIterator = gooInputs.begin(); loIterator != gooInputs.end(); loIterator++)
    {
        int lnBlockAddress = floor(loIterator->second / BLOCK_SIZE);
        int lnPTIndex = lnBlockAddress % lnCacheRowEntries;

        // Index.size + Offset.size
        int lnPTTag = loIterator->second / BLOCK_SIZE;

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
    for (int i = 0; i < lnCacheRowEntries; i++)
    {
        delete[] lanPageTable[i];
    }
    delete[] lanPageTable;

    return lnHit;
}

int Caches::fullAssociativeLRU()
{
    /*
     * Fully-Associative cache.
     * Assume that each cache line is 32 bytes
     * the total cache size is 16KB.
     * Implement Least Recently Used (LRU)
     */

    return setAssociative(512);
}

int Caches::fullAssociativeHCR()
{
    /*
     * Fully-Associative cache.
     * Assume that each cache line is 32 bytes
     * the total cache size is 16KB.
     * Hot-cold LRU approximation policies.
     */

    // 2^9 entries w/ 2^5 line size = 16KB cache
    const int lnWayCount = 512;

    // Row will be [VALID | TAG | VALID | TAG | ...]
    const int lnCacheRowLength = 2 * lnWayCount;

    int lnHit = 0;

    // Dynamic allocation of arrays
    int *lanPageTable = new int[lnCacheRowLength];
    int *lanHotColdTable = new int[lnWayCount - 1];

    for (int i = 0; i < lnCacheRowLength; i++)
    {
        lanPageTable[i] = 0;
    }

    for (int i = 0; i < lnWayCount; i++)
    {
        lanHotColdTable[i] = 0;
    }

    for (auto loIterator = gooInputs.begin(); loIterator != gooInputs.end(); loIterator++)
    {
        int lnBlockAddress = floor(loIterator->second / BLOCK_SIZE);

        // Index.size + Offset.size
        int lnPTTag = loIterator->second / BLOCK_SIZE;

        // Iterate through each "way" in the cache row
        bool lbEntryFound = false;
        for (int i = 0; i < lnCacheRowLength; i += lnCacheRowLength / lnWayCount)
        {
            if (lanPageTable[i] == 1)
            {
                if (lanPageTable[i + 1] == lnPTTag)
                {
                    lnHit++;

                    /*
                     * To update here, start with MID_BIT (SIZE/2^1).
                     * If (index/2)%2 == 0 // even then END_BIT = (index/2)+1
                     * If (index/2)%2 != 0 // odd then END_BIT = (index/2)
                     *
                     * int i = 1;
                     * while (MID_BIT != END_BIT) {
                     * if MID_BIT > END_BIT:
                     *   ARR[MID_BIT] = 0
                     *   MID_BIT -= SIZE/2^i
                     * else:
                     *   ARR[MID_BIT] = 1
                     *   MID_BIT += SIZE/2^i
                     * }
                     */

                    int lnMiddleBit = lnWayCount/2;

                    int lnEndBit = i/2;
                    if ((i/2)%2 == 0)
                    {
                        lnEndBit++;
                    }

                    for (int lnDivRaiser = 1; lnMiddleBit != lnEndBit; lnDivRaiser++)
                    {
                       // cout << lnMiddleBit << " " <<  lnEndBit << "\n";
                        if (lnMiddleBit > lnEndBit)
                        {
                            lanHotColdTable[lnMiddleBit] = 0;
                            lnMiddleBit -= lnWayCount/pow(2, lnDivRaiser);
                        }
                        else
                        {
                            lanHotColdTable[lnMiddleBit] = 1;
                            lnMiddleBit += lnWayCount/pow(2, lnDivRaiser);
                        }
                    }

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
            for (int i = 0; i < lnCacheRowLength; i += 2)
            {
                // Page is invalid, so we can write here
                if (lanPageTable[i] == 0)
                {
                    lanPageTable[i] = 1;
                    lanPageTable[i + 1] = lnPTTag;
                    //lanPageTableRow[i + 2] = distance(gooInputs.begin(), loIterator);
                    lbEntryInputted = true;
                    break;
                }
            }

            if (! lbEntryInputted)
            {

                /*
                 * Bit field is
                 * i = 1;
                 * 0: size/2^i
                 * 1: Index(0) +- size/2^i
                 * 2: Index(1) +- size/2^i
                 * ...
                 * n: Index(n-1) +- size/2^(n+1)
                 */

                // Find the coldest bit and swap that
                int lnMiddleBit = lnWayCount/2;
                for (int lnDivRaiser = 0; (lnWayCount / pow(2, lnDivRaiser)) > 0; lnDivRaiser++)
                {
                    if (lanHotColdTable[lnMiddleBit] == 0)
                    {
                        lnMiddleBit -= lnWayCount / pow(2, lnDivRaiser);
                    } else
                    {
                        lnMiddleBit += lnWayCount / pow(2, lnDivRaiser);
                    }
                }

                if (lanHotColdTable[lnMiddleBit] == 0)
                {
                    // index to trash = lnMiddleBit - 1
                    lanPageTable[lnMiddleBit - 1] = lnPTTag;

                }
                else
                {
                    // index to trash = lnMiddleBit
                    lanPageTable[lnMiddleBit] = lnPTTag;
                }

                /*
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
                 */
            }
        }
    }

    // Free dynamic arrays
    delete[] lanPageTable;
    delete[] lanHotColdTable;

    return lnHit;
}