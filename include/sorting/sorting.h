#ifndef SORTING_H
#define SORTING_H

#include <cstdlib>
#include <algorithm>

inline bool Comp64High32Low32(uint64_t A, uint64_t B)
{
    return A < B;
}

inline bool RevComp64Low32High32(uint64_t A, uint64_t B)
{
    return (A << 32 | (A & 0xffffffff)) > (B << 32 | (B & 0xffffffff));
}

/*void QSort(void *base, size_t nel, size_t width,
	    int (* compar)(const void *, const void *))
{
    qsort(base, nel, width, compar);
}*/
void QSort(std::vector<uint64_t>& arr, bool (*comp)(uint64_t A, uint64_t B))
{
    std::sort(arr.begin(), arr.end(), comp);
}

#endif // SORTING_H
