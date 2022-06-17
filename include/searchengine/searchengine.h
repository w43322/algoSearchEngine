#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <list>
#include "../rapidcsv/rapidcsv.h"
#include "../wordlist/wordlist.h"
#include "../indexing/indexing.h"
#include "../cppjieba/Jieba.hpp"

class SearchEngine
{
public:
    void OutputResult(
        clock_t t0, 
        const std::vector<std::string>& words,
        std::vector<uint64_t>& output)
    {
        // sort by lower 32bits
        radixSortLowHigh(&output[0], output.size());

        for (auto&& record : output)
        {
            // underlying output
            //printf("%8llx, %8llx\n", record >> 32, record & 0xffffffff);
            
            // real output
            uint32_t docId = record >> 32;
            auto&& url = doc.GetCell<std::string>(0, docId);
            auto&& title = doc.GetCell<std::string>(1, docId);
            printf("%3llu ", record & 0xffffffff);
            std::cout << url << ' ' << title << '\n';
        }

        auto t1 = std::clock();
        
        std::cout << " - Your Input:\n";
        for (const auto &str : words)
            std::cout << '"' << str << '"' << '\n';
        
        std::cout << " - Timing:\nResult fetched in " << (t1 - t0) / (double)(CLOCKS_PER_SEC / 1000) << "msec\n";
    }
    void GetResultAllWords(const std::vector<std::string>& words)
    {
        auto t0 = std::clock();
        
        auto output = ii.GetDocList(wl.GetWordId(words[0]));
        for (size_t i = 1, siz = words.size(); i < siz; ++i)
        {
            std::vector<uint64_t> newOutput;
            auto&& docList = ii.GetDocList(wl.GetWordId(words[i]));

            // merge result of current word with already existing result
            auto&& first1 = output.begin(), last1 = output.end();
            auto&& first2 = docList.begin(), last2 = docList.end();
            while (first1 != last1 && first2 != last2)
            {
                if ((uint32_t)(*first1 >> 32) < (uint32_t)(*first2 >> 32))
                    ++first1;
                else
                {
                    if ((uint32_t)(*first1 >> 32) == (uint32_t)(*first2 >> 32))
                        newOutput.push_back(*first1 + (*first2 & 0xffffffff));// *first1 and *first2 are equivalent.
                    ++first2;
                }
            }
            output = newOutput;
        }

        OutputResult(t0, words, output);
    }
    void GetResultAnyWord(const std::vector<std::string>& words)
    {
        auto t0 = std::clock();

        std::list<uint64_t> result;
        for (auto&& word : words)
        {
            auto&& docList = ii.GetDocList(wl.GetWordId(word));

            // merge result of current word with already existing result
            auto&& it = result.begin();
            for (auto&& record : docList)
            {
                while (it != result.end() && (uint32_t)(*it >> 32) < (uint32_t)(record >> 32))
                    ++it;
                if (it == result.end() || (uint32_t)((*it) >> 32) > (uint32_t)(record >> 32))
                    result.insert(it, record);
                else
                    (*it) += record & 0xffffffff;
            }
        }

        std::vector<uint64_t> output(result.begin(), result.end());
        OutputResult(t0, words, output);
    }

    // English SearchEngine
    SearchEngine(const std::string& docPath) :
        doc(docPath, rapidcsv::LabelParams(-1, -1),
            rapidcsv::SeparatorParams(',' /* pSeparator */, 
                                    false /* pTrim */, 
                                    rapidcsv::sPlatformHasCR /* pHasCR */,
                                    true /* pQuotedLinebreaks */, 
                                    true /* pAutoQuote */))
    {
        // time temp index generation
        auto t0 = std::clock();
        std::vector<uint64_t> tempIndex;

        // split words from article, and insert into wordList
        auto&& colTitle = doc.GetColumn<std::string>(1);
        auto&& colText = doc.GetColumn<std::string>(2);


        std::cout << "----Init English Search Engine----\n"; 
        for (uint32_t docId = 0, siz = doc.GetRowCount(); docId < siz; ++docId)
        {
            std::string nowWord;
            for (auto&& chr : colTitle[docId] + colText[docId])
            {
                if (isalpha(chr))
                    nowWord.push_back(tolower(chr));
                else if (!nowWord.empty()) // found a word
                {
                    // insert the word into wordlist
                    auto wordId = wl.GetWordId(nowWord);
                    nowWord.clear();

                    // insert word id and doc id into temp index
                    tempIndex.push_back(((uint64_t)wordId << 32) | docId);
                }
            }
        }

        InitInvertedIndex(t0, tempIndex, "en");
    }

    // Chinese SearchEngine
    SearchEngine(const std::string& docPath, cppjieba::Jieba& jieba) :
        doc(docPath, rapidcsv::LabelParams(-1, -1),
            rapidcsv::SeparatorParams(',' /* pSeparator */, 
                                    false /* pTrim */, 
                                    rapidcsv::sPlatformHasCR /* pHasCR */,
                                    true /* pQuotedLinebreaks */, 
                                    true /* pAutoQuote */))
    {
        // time temp index generation
        auto t0 = std::clock();
        std::vector<uint64_t> tempIndex;

        // split words from article, and insert into wordList
        auto&& colTitle = doc.GetColumn<std::string>(1);
        auto&& colText = doc.GetColumn<std::string>(2);

        std::cout << "----Init Chinese Search Engine----\n"; 
        for (uint32_t docId = 0, siz = doc.GetRowCount(); docId < siz; ++docId)
        {
            std::vector<std::string> words;
            jieba.CutForSearch(colTitle[docId]+colText[docId], words, true);
            for (auto&& word : words)
            {
                // insert the word into wordlist
                auto wordId = wl.GetWordId(word);

                // insert word id and doc id into temp index
                tempIndex.push_back(((uint64_t)wordId << 32) | docId);
            }
        }

        InitInvertedIndex(t0, tempIndex, "cn");
    }
    void InitInvertedIndex(clock_t t0, std::vector<uint64_t>& tempIndex, const std::string& suffix)
    {
        system("mkdir wordList_cn");
        system("mkdir wordList_en");
        system("mkdir tempIndex_cn");
        system("mkdir tempIndex_en");

        auto t1 = std::clock();

        // output word list
        std::ofstream outFileWordList("./wordList_"+suffix+"/hashedWordList_"+suffix+".dat", std::ios::out);
        wl.Output(outFileWordList);
        outFileWordList.close();

        auto t2 = std::clock();

        // output temporary index
        const size_t fileSiz = 1 << 18;
        auto siz = tempIndex.size();
        for (size_t i = 0; i < siz; i += fileSiz)
        {
            std::ofstream outFileTempIndex("./tempIndex_"+suffix+"/tempIndex_"+suffix+"_"+std::to_string(i >> 18)+".dat", std::ios::out | std::ios::binary);

            if (i == siz - 1) for (; i < siz; ++i)
                outFileTempIndex.write((char*)&tempIndex[i], sizeof(uint64_t));
            else for (size_t j = 0; j < fileSiz; ++j)
                outFileTempIndex.write((char*)&tempIndex[i+j], sizeof(uint64_t));
            outFileTempIndex.close();
        }

        // inverted reverse index generation
        auto t3 = std::clock();
        ii = InvertedIndex(tempIndex);
        auto t4 = std::clock();

        // time output
        std::cout << " - Temp index generation is finished in " << (t1 - t0) / (double)(CLOCKS_PER_SEC / 1000) << "msecs.\n";
        std::cout << "   - Word list output is finished in " << (t2 - t1) / (double)(CLOCKS_PER_SEC / 1000) << "msecs.\n";
        std::cout << "   - Temp index output is finished in " << (t3 - t2) / (double)(CLOCKS_PER_SEC / 1000) << "msecs.\n";
        std::cout << " - Inverted index generation is finished in " << (t4 - t3) / (double)(CLOCKS_PER_SEC / 1000) << "msecs.\n";
    }
private:
    WordList wl;
    InvertedIndex ii;
    rapidcsv::Document doc;
};

#endif // SEARCHENGINE_H
