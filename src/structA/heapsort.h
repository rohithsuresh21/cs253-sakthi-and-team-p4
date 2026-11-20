#pragma once
#include <utility>
#include <vector>
#include "../common/graph.h"


inline void siftDown(std::vector<Edge>& a, int i, int size) {
    while (true) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int biggest = i;
