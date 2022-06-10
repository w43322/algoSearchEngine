#ifndef INDEXING_H
#define INDEXING_H

#include <vector>
#include "../sorting/sorting.h"

class InvertedIndex
{
public:
    InvertedIndex(){}
    InvertedIndex(std::vector<uint64_t>& tempIndex)
    {
        QSort(&tempIndex[0], tempIndex.size(), sizeof(uint64_t), Comp64High32Low32);
        size_t siz = -1;
        for (auto&& record : tempIndex)
        {
            uint32_t wordId = record >> 32;
            uint32_t docId = record;
            if (wordId == siz)
            {
                auto&& docListOfCurrentWord = data.back();
                if ((uint32_t)(docListOfCurrentWord.back() >> 32) == docId)
                    ++docListOfCurrentWord.back();
                else
                    docListOfCurrentWord.push_back(((uint64_t)docId << 32) | 1);
            }
            else
            {
                ++siz;
                data.push_back({((uint64_t)docId << 32) | 1});
            }
        }
    }
    const std::vector<uint64_t>& GetDocList(uint32_t wordId)
    {
        return data[wordId];
    }
private:
    std::vector<std::vector<uint64_t> > data;
};

#endif
