// BloomFilterCpp.h
//

#include "Filter.h"
#include <bitset>

// Future: This class was written with preset perf goals however, there's an opportunity here to:
// 1) Create a base Filter class that can work with different filter types
// 2) Compute optimal performance for a given M, N, P, or K parameters
// 3) Serialize select data, at least the filter itself for persistence
// 4) Notify (real-time or telemetry) when quality is regressed ex: filter is becoming full, or operations are taking longer
// 5) Further abstract away hashing pieces so that they can be unit-tested
class BloomFilter : public IFilter
{
public:
    BloomFilter();
    virtual ~BloomFilter();

    // Adds or tests for UTF data in the filter
    // Future: Allow other data types to scale to requirements
    // Future: Add support for case-insensivity based on requirements
    // Future: Explore if we can bound input size to prevent buffer overruns
    // Future: Make locale resilient
    void Add(const wchar_t* elementData);
    bool Check(const wchar_t* elementData);

protected:

    // This filter uses MurmuerHash3 from: https://github.com/aappleby/smhasher
    // Double-hashing was used for computing N hash functions as described in https://en.wikipedia.org/wiki/Double_hashing
    // To use a different hash function or a different approach for computation, derived classes should override the following two methods

    // Templatized method to share code across filter operations
    // Func is called for every generated hash
    template<typename TFunctor>
    void ForEachHash(const wchar_t* elementData, TFunctor Func) const;

    // Helper function for double-hashing and getting the bit index
    uint64_t GetBitIndexFromHash(uint32_t hashCount, uint64_t hashPart1, uint64_t hashPart2) const;

private:
    /*--------------------------------------
    Desired Performance computed using https://hur.st/bloomfilter/?n=10000000&p=1.0E-4&m=209715200&k=
    Number of Elements: 10 Million
    Probability of False-Positives: 1.0E-4
    Filter Size: 25 MiB (209715200 Bits)
    Optimal Hashes: 8
    --------------------------------------*/
    static const int k_bitsInFilter = 209715200; // 25 MiB, based on desired performance
    static const uint32_t k_numHashes = 8; // 8, based on desired performance
    static const uint32_t k_murmurHashSeed = 7; // Random prime number to be used as the seed

    std::bitset<k_bitsInFilter> m_bloomFilter;
};
