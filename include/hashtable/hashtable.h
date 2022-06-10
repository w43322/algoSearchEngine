#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <vector>
#include <string>

class HashTable
{
public:
    std::vector<std::pair<std::string, int> >data[65536];
    void Insert(const std::string& str, int n)
    {
        data[GetHash16(str)].push_back(std::make_pair(str, n));
    }
    int At(const std::string& str)
    {
        uint16_t hash = GetHash16(str);
        for (auto&& record : data[hash])
            if (record.first == str)
                return record.second;
        return -1;
    }
    static uint16_t GetHash16(const std::string& str)
    {
        uint16_t hash = 0;
        size_t len = str.length();
        for (size_t i = 0; i + 1 < len; i += 2)
        {
            uint16_t word = (str[i] << 8) | (uint8_t)str[i | 1];
            size_t k = ((len + 1 - i) >> 1) - 1;
            hash ^= (word << (k & 0xf)) | (word >> (-k & 0xf));
        }
        if (len & 1)
            hash ^= str[len - 1] << 8;
        return hash;
    }
};

#endif // HASHTABLE_H
