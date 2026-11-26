#pragma once
#include <algorithm>
#include <cstddef>
#include <map>
#include <vector>

#include "structures/Queue.h"

namespace style {

// Yönsüz graf, adjacency list ile. Düğümler int ID.
class Graph {
public:
    struct Visit {
        int id;
        int level;  // başlangıçtan kenar sayısı olarak uzaklık
    };

    // Düğüm ekler. Zaten varsa false.
    bool addNode(int id) {
        return adj_.emplace(id, std::vector<int>{}).second;
    }

    bool hasNode(int id) const { return adj_.count(id) > 0; }

    // Düğümü ve ona bağlı tüm kenarları siler. Yoksa false.
    bool removeNode(int id) {
        auto it = adj_.find(id);
        if (it == adj_.end()) return false;
        for (int other : it->second) {
            std::vector<int>& list = adj_[other];
            list.erase(std::find(list.begin(), list.end(), id));
        }
        edgeCount_ -= it->second.size();
        adj_.erase(it);
        return true;
    }

    // a-b kenarı ekler. İki düğüm de var olmalı; a == b ya da tekrar ise false.
    bool addEdge(int a, int b) {
        if (a == b || !hasNode(a) || !hasNode(b) || hasEdge(a, b)) return false;
        adj_[a].push_back(b);
        adj_[b].push_back(a);
        ++edgeCount_;
        return true;
    }

    bool removeEdge(int a, int b) {
        if (!hasEdge(a, b)) return false;
        std::vector<int>& la = adj_[a];
        std::vector<int>& lb = adj_[b];
        la.erase(std::find(la.begin(), la.end(), b));
        lb.erase(std::find(lb.begin(), lb.end(), a));
        --edgeCount_;
        return true;
    }

    // Zaman: O(derece).
    bool hasEdge(int a, int b) const {
        auto it = adj_.find(a);
        if (it == adj_.end()) return false;
        return std::find(it->second.begin(), it->second.end(), b) != it->second.end();
    }

    // Komşular (ekleme sırasıyla), düğüm yoksa nullptr.
    const std::vector<int>* neighbors(int id) const {
        auto it = adj_.find(id);
        return it == adj_.end() ? nullptr : &it->second;
    }

    std::size_t nodeCount() const { return adj_.size(); }
    std::size_t edgeCount() const { return edgeCount_; }

    // Genişlik öncelikli gezinti. Düğümleri ziyaret sırasıyla döner
    // (başlangıç level 0). maxLevel >= 0 ise o seviyeden öteye inmez.
    // Başlangıç düğümü yoksa boş liste.
    // Zaman: O((V + E) log V) (std::map aramaları yüzünden).
    std::vector<Visit> bfs(int start, int maxLevel = -1) const {
        std::vector<Visit> order;
        if (!hasNode(start)) return order;

        std::map<int, int> dist;  // ziyaret edilenler -> seviye
        Queue<int> queue;
        dist[start] = 0;
        queue.enqueue(start);

        int current = 0;
        while (queue.dequeue(current)) {
            const int level = dist[current];
            order.push_back({current, level});
            if (maxLevel >= 0 && level >= maxLevel) continue;
            for (int next : adj_.at(current)) {
                if (dist.count(next) == 0) {
                    dist[next] = level + 1;
                    queue.enqueue(next);
                }
            }
        }
        return order;
    }

private:
    std::map<int, std::vector<int>> adj_;
    std::size_t edgeCount_ = 0;
};

}  // namespace style