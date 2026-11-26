#pragma once
#include <cstddef>
#include <vector>

namespace style {

// v, less'e göre sıralı olmalıdır (çağıranın sorumluluğu).

// less(a, b): a, b'den önce gelir mi?
// Dizinin less'e göre sıralı olup olmadığını denetler. Zaman: O(n).
template <typename T, typename Less>
bool isSorted(const std::vector<T>& v, Less less) {
    for (std::size_t i = 1; i < v.size(); ++i) {
        if (less(v[i], v[i - 1])) return false;
    }
    return true;
}

// value'dan küçük olmayan (>= value) ilk elemanın indeksi; yoksa v.size().
// Zaman: O(log n).
template <typename T, typename V, typename Less>
std::size_t lowerBound(const std::vector<T>& v, const V& value, Less less) {
    std::size_t lo = 0, hi = v.size();
    while (lo < hi) {
        const std::size_t mid = lo + (hi - lo) / 2;
        if (less(v[mid], value)) lo = mid + 1;
        else hi = mid;
    }
    return lo;
}

// value'dan büyük (> value) ilk elemanın indeksi; yoksa v.size().
// Zaman: O(log n).
template <typename T, typename V, typename Less>
std::size_t upperBound(const std::vector<T>& v, const V& value, Less less) {
    std::size_t lo = 0, hi = v.size();
    while (lo < hi) {
        const std::size_t mid = lo + (hi - lo) / 2;
        if (!less(value, v[mid])) lo = mid + 1;
        else hi = mid;
    }
    return lo;
}

// value'ya eşit bir eleman varsa ilkinin indeksini, yoksa v.size() döner.
// Zaman: O(log n).
template <typename T, typename V, typename Less>
std::size_t binarySearch(const std::vector<T>& v, const V& value, Less less) {
    const std::size_t pos = lowerBound(v, value, less);
    if (pos < v.size() && !less(value, v[pos])) return pos;
    return v.size();
}

}  // namespace style