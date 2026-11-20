#pragma once
#include <utility>
#include <vector>
#include "../common/graph.cpp"


inline void siftDown(std::vector<Edge>& a, int i, int size) {
    while (true) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int biggest = i;


        if (left < size && a[left].w > a[biggest].w) biggest = left;
        if (right < size && a[right].w > a[biggest].w) biggest = right;
        if (biggest == i) break;
        std::swap(a[i], a[biggest]);
        i = biggest;
    }
}


inline void heapSort(std::vector<Edge>& a) {
    int size = (int)a.size();
    for (int i = size / 2 - 1; i >= 0; i--) {
        siftDown(a, i, size);
    }
    for (int end = size - 1; end > 0; end--) {
        std::swap(a[0], a[end]);
        siftDown(a, 0, end);
    }
}
