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
        radixSortHighLow(&tempIndex[0], tempIndex.size());

        size_t siz = -1; // 上一个word_id
        for (auto&& record : tempIndex) // 遍历临时索引
        {
            uint32_t wordId = record >> 32; // 临时索引中当前记录的word_id
            uint32_t docId = record; // 临时索引中当前记录的doc_id
            if (wordId == siz) // 如果当前word_id等于上个word_id，在上个元素的尾部继续处理doc_id
            {
                auto&& docListOfCurrentWord = data.back(); // 取尾部元素
                if ((uint32_t)(docListOfCurrentWord.back() >> 32) == docId) // 如果当前doc_id等于上一个doc_id，即当前单词在当前文档中又出现了一次
                    ++docListOfCurrentWord.back(); // 增加这个文档中这个单词出现的次数
                else // 当前doc_id不等于上一个doc_id
                    docListOfCurrentWord.push_back(((uint64_t)docId << 32) | 1); // 在当前word_id尾部加入新的doc_id节点
            }
            else // 当前word_id不等于上一个word_id
            {
                ++siz;
                data.push_back({((uint64_t)docId << 32) | 1}); // 增加新的word_id节点
            }
        }
    }
    const std::vector<uint64_t>& GetDocList(uint32_t wordId)
    {
        return data[wordId]; // 返回当前word_id所对应的文档列表
    }
private:
    std::vector<std::vector<uint64_t> > data;
};

#endif
