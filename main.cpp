#include "./include/searchengine/searchengine.h"

int main()
{
    SearchEngine se("./news.csv");
    se.GetResult({"team", "usa"});
    return 0;
}
