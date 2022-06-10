#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <list>
#include "../rapidcsv/rapidcsv.h"
#include "../wordlist/wordlist.h"
#include "../indexing/indexing.h"

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
        QSort(&output[0], output.size(), sizeof(uint64_t), RevComp64Low32High32);

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
    SearchEngine(const std::string& docPath) :
        doc(docPath, rapidcsv::LabelParams(-1, -1),
            rapidcsv::SeparatorParams(',' /* pSeparator */, 
                                    false /* pTrim */, 
                                    rapidcsv::sPlatformHasCR /* pHasCR */,
                                    true /* pQuotedLinebreaks */, 
                                    true /* pAutoQuote */))
    {
        
        std::vector<uint64_t> tempIndex;

        // split words from article, and insert into wordList
        std::string nowWord;
        auto&& colTitle = doc.GetColumn<std::string>(1);
        auto&& colText = doc.GetColumn<std::string>(2);
        for (uint32_t docId = 0, siz = doc.GetRowCount(); docId < siz; ++docId)
        {
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
        ii = InvertedIndex(tempIndex);
    }
private:
    WordList wl;
    InvertedIndex ii;
    rapidcsv::Document doc;
};

#endif // SEARCHENGINE_H
