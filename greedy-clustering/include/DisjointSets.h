#pragma once

#include <vector>


class DisjointSetUnion {
private:
    std::vector<int> parent_;
    std::vector<int> ranks_;

public:
    explicit DisjointSetUnion(size_t size)
            : parent_()
            , ranks_(size, 0)
    {
        parent_.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            parent_.emplace_back(i);
        }
    }

    int find(int node) {
        if (parent_[node] != node) {
            parent_[node] = find(parent_[node]);
        }
        return parent_[node];
    }

    void union_sets(int first, int second) {
        int first_root = find(first);
        int second_root = find(second);
        if (first_root == second_root) {
            return;
        }

        if (ranks_[first_root] < ranks_[second_root]) {
            parent_[first_root] = second_root;
        } else if (ranks_[first_root] > ranks_[second_root]) {
            parent_[second_root] = first_root;
        } else {
            parent_[second_root] = first_root;
            ++ranks_[first_root];
        }
    }
};

struct Point2D {
    double x, y;
};

