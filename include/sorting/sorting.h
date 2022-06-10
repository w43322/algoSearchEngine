#ifndef SORTING_H
#define SORTING_H

#include <cstdlib>

int Comp64High32Low32(const void* A, const void* B)
{
    int highDif = (*(int64_t*)A >> 32) - (*(int64_t*)B >> 32);
    if (highDif)
        return highDif;
    else
        return (int)(*(int64_t*)A & 0xffffffff) - (*(int64_t*)B & 0xffffffff);
}

int Comp64Low32High32(const void* A, const void* B)
{
    int lowDif = (*(int64_t*)A & 0xffffffff) - (*(int64_t*)B & 0xffffffff);
    if (lowDif)
        return lowDif;
    else
        return (*(int64_t*)A >> 32) - (*(int64_t*)B >> 32);
}

int RevComp64Low32High32(const void* A, const void* B)
{
    return -Comp64Low32High32(A, B);
}

void QSort(void *base, size_t nel, size_t width,
	    int (* compar)(const void *, const void *))
{
    qsort(base, nel, width, compar);
}

#endif // SORTING_H
