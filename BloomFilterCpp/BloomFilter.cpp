// BloomFilterCpp.cpp
//

#include "BloomFilter.h"
#include "MurmurHash3.h"
#include <iostream>

BloomFilter::BloomFilter() {}
BloomFilter::~BloomFilter() {}

uint64_t BloomFilter::GetBitIndexFromHash(uint32_t hashCount, uint64_t hashPart1, uint64_t hashPart2) const
{
    // (Hash(Part1) + hashCount × Hash(Part2)) Mod FilterSize
    return (hashPart1 + hashCount * hashPart2) % k_bitsInFilter;
}

template<typename TFunctor>
void BloomFilter::ForEachHash(const wchar_t* elementData, TFunctor Func) const
{
    uint64_t hashParts[2];
    MurmurHash3_x64_128(elementData, wcslen(elementData), k_murmurHashSeed, hashParts);

    for (uint32_t i = 0; i < k_numHashes; i++)
    {
        auto bitIndex = GetBitIndexFromHash(i, hashParts[0], hashParts[1]);
        Func(bitIndex);
    }
}

void BloomFilter::Add(const wchar_t* elementData)
{
    ForEachHash(elementData,
        [this](auto bitToSet)
        {
            m_bloomFilter.set(static_cast<uint32_t>(bitToSet));
        });
}

bool BloomFilter::Check(const wchar_t* elementData)
{
    bool elementExists = false;

    ForEachHash(elementData,
        [this, &elementExists](auto bitToTest)
        {
            elementExists = m_bloomFilter.test(static_cast<uint32_t>(bitToTest));
        });

    return elementExists;
}