#pragma once
#include <utility>
#include <vector>

struct HeapItem {
    long long w;
    int from;
    int to;
};

struct MinHeap {
    std::vector<HeapItem> data;

    bool empty() const {
        return data.empty();
    }

    long long size() const {
        return (long long)data.size();
    }

    void push(HeapItem item) {
        data.push_back(item);
        int i = (int)data.size() - 1;
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (data[parent].w <= data[i].w) break;
            std::swap(data[parent], data[i]);
            i = parent;
        }
    }

    HeapItem pop() {
        HeapItem top = data[0];
        data[0] = data.back();
        data.pop_back();
        int size = (int)data.size();
        int i = 0;
        while (true) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            int smallest = i;
            if (left < size && data[left].w < data[smallest].w) smallest = left;
            if (right < size && data[right].w < data[smallest].w) smallest = right;
            if (smallest == i) break;
            std::swap(data[i], data[smallest]);
            i = smallest;
        }
        return top;
    }
};
