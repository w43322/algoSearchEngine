#ifndef WORDLIST_H
#define WORDLIST_H

#include <vector>
#include "../hashtable/hashtable.h"

class WordList
{
public:
    uint32_t GetWordId(const std::string& word)
    {
        int wordId = hTable.At(word);
        if (hTable.At(word) == -1) // word is not yet in list of words
        {
            wordId = data.size();
            hTable.Insert(word, wordId);
            data.push_back(word);
        }
        return wordId;
    }
private:
    std::vector<std::string> data;
    HashTable hTable;
};

#endif // WORDLIST_H
