#include "./include/searchengine/searchengine.h"
#include <ctime>


int main()
{
    //#define SE_CN

    #ifdef SE_CN
    SearchEngine se_cn("./cn.csv", true);
    #else
    SearchEngine se("./news.csv", false);
    #endif
auto t0 = std::clock();
    #ifdef SE_CN
    se_cn.GetResult({"演员"});
    #else
    se.GetResult({"usa", "team"});
    #endif
auto t1 = std::clock();
    std::cout << "result sorting is finished in " << (t1 - t0) / (double)(CLOCKS_PER_SEC / 1000) << "msec\n";
    return 0;
}
