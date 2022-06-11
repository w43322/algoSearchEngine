#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <list>
#include "../rapidcsv/rapidcsv.h"
#include "../wordlist/wordlist.h"
#include "../indexing/indexing.h"
#include "../cppjieba/Jieba.hpp"

const char* const DICT_PATH = "./dict/jieba.dict.utf8";
const char* const HMM_PATH = "./dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "./dict/user.dict.utf8";
const char* const IDF_PATH = "./dict/idf.utf8";
const char* const STOP_WORD_PATH = "./dict/stop_words.utf8";

class SearchEngine
{
public:
    void GetResult(const std::vector<std::string>& words)
    {
        std::list<uint64_t> result;
        for (auto&& word : words)
        {
            auto&& docList = ii.GetDocList(wl.GetWordId(word));

            // merge result of current word with already existing result
            auto&& it = result.begin();
            for (auto&& record : docList)
            {
                while (it != result.end() && (*it >> 32) < (record >> 32))
                    ++it;
                if (it == result.end() || ((*it) >> 32) > (record >> 32))
                    result.insert(it, record);
                else
                    (*it) += record & 0xffffffff;
            }
        }

        // sort by lower 32bits
        std::vector<uint64_t> output(result.begin(), result.end());
        QSort(output, RevComp64Low32High32);

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
    }
    SearchEngine(const std::string& docPath, bool isChinese = false) :
        doc(docPath, rapidcsv::LabelParams(-1, -1),
            rapidcsv::SeparatorParams(',' /* pSeparator */, 
                                    false /* pTrim */, 
                                    rapidcsv::sPlatformHasCR /* pHasCR */,
                                    true /* pQuotedLinebreaks */, 
                                    true /* pAutoQuote */)),
        jieba(DICT_PATH,
                HMM_PATH,
                USER_DICT_PATH,
                IDF_PATH,
                STOP_WORD_PATH)
    {
        // time temp index generation
        auto t0 = std::clock();
        std::vector<uint64_t> tempIndex;

        // split words from article, and insert into wordList
        auto&& colTitle = doc.GetColumn<std::string>(1);
        auto&& colText = doc.GetColumn<std::string>(2);

        // if chinese
        if (isChinese) for (uint32_t docId = 0, siz = doc.GetRowCount(); docId < siz; ++docId)
        {
            std::vector<std::string> words;
            jieba.Cut(colTitle[docId]+colText[docId], words, true);
            for (auto&& word : words)
            {
                // insert the word into wordlist
                auto wordId = wl.GetWordId(word);

                // insert word id and doc id into temp index
                tempIndex.push_back(((uint64_t)wordId << 32) | docId);
            }
        }

        // if english
        else for (uint32_t docId = 0, siz = doc.GetRowCount(); docId < siz; ++docId)
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
        // inverted reverse index generation
        auto t1 = std::clock();
        ii = InvertedIndex(tempIndex);
        auto t2 = std::clock();
        std::cout << "temp index generation is finished in " << (t1 - t0) / (double)(CLOCKS_PER_SEC / 1000) << "msecs.\n";
        std::cout << "inverted index generation is finished in " << (t2 - t1) / (double)(CLOCKS_PER_SEC / 1000) << "msecs.\n";
    }
private:
    WordList wl;
    InvertedIndex ii;
    rapidcsv::Document doc;
    cppjieba::Jieba jieba;
};

#endif // SEARCHENGINE_H
