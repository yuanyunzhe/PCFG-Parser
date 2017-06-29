#ifndef STRUCTURE_HPP
#define STRUCTURE_HPP

#include <cstdlib>
#include "Math.hpp"

class Previous{
public:
	int i, j, k, x, y, z;
};

template<typename T>
void malloc(T ****p, int x, int y, int z){
    *p = (T ***)malloc(x * sizeof(T **));
    for (int i = 0; i < x; i++)
        (*p)[i] = (T **)malloc(y * sizeof(T *));
    for (int i = 0; i < x; i++)
        for (int j = 0; j < y; j++)
            (*p)[i][j] = (T *)malloc(z * sizeof(T));
}

template<typename T>
void malloc(T ***p, int x, int y){
    *p = (T **)malloc(x * sizeof(T *));
    for (int i = 0; i < x; i++)
        (*p)[i] = (T *)malloc(y * sizeof(T));
 }

template<typename T>
void free(T p, int x, int y, int z){
    for (int i = 0; i < x; i++)
        for (int j = 0; j < y; j++)
            free(p[i][j]);
    for (int i = 0; i < x; i++)
            free(p[i]);      
    free(p);
}

template<typename T>
void free(T p, int x, int y){
    for (int i = 0; i < x; i++)
            free(p[i]);      
    free(p);
 }

#endif