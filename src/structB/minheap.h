#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

struct HeapItem {
    long long w = 0;
    int from = 0;
    int to = 0;
};

struct MinHeap {
    std::vector<HeapItem> data;
    bool empty() const { return data.empty(); }
    std::size_t size() const { return data.size(); }

    static bool less(const HeapItem& a, const HeapItem& b) {
        if (a.w != b.w) return a.w < b.w;
        if (a.from != b.from) return a.from < b.from;
        return a.to < b.to;
    }

    void push(const HeapItem& item) {
        data.push_back(item);
        std::size_t i = data.size() - 1;
        while (i > 0) {
            const std::size_t parent = (i - 1) / 2;
            if (!less(data[i], data[parent])) break;
            std::swap(data[i], data[parent]);
            i = parent;
        }
    }

    HeapItem pop() {
        if (data.empty()) throw std::logic_error("pop from empty MinHeap");
        const HeapItem result = data.front();
        data.front() = data.back();
        data.pop_back();
        std::size_t i = 0;
        while (true) {
            const std::size_t left = 2 * i + 1;
            const std::size_t right = left + 1;
            std::size_t smallest = i;
            if (left < data.size() && less(data[left], data[smallest])) smallest = left;
            if (right < data.size() && less(data[right], data[smallest])) smallest = right;
            if (smallest == i) break;
            std::swap(data[i], data[smallest]);
            i = smallest;
        }
        return result;
    }
};
