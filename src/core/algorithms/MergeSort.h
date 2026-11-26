#pragma once
#include <cstddef>
#include <utility>
#include <vector>

namespace style {

namespace detail {

// [lo, hi) aralığını sıralar. tmp, birleştirme için yardımcı dizidir.
template <typename T, typename Less>
void mergeSortRange(std::vector<T>& a, std::vector<T>& tmp, std::size_t lo,
                    std::size_t hi, Less& less) {
    if (hi - lo < 2) return;  // 0 ya da 1 eleman: zaten sıralı

    const std::size_t mid = lo + (hi - lo) / 2;
    mergeSortRange(a, tmp, lo, mid, less);
    mergeSortRange(a, tmp, mid, hi, less);

    // Birleştirme: iki sıralı yarıyı tmp'ye yaz.
    std::size_t i = lo, j = mid, k = lo;
    while (i < mid && j < hi) {
        // Sağ eleman kesinlikle küçükse onu al; eşitse sol (kararlılık).
        if (less(a[j], a[i])) tmp[k++] = std::move(a[j++]);
        else tmp[k++] = std::move(a[i++]);
    }
    while (i < mid) tmp[k++] = std::move(a[i++]);
    while (j < hi) tmp[k++] = std::move(a[j++]);

    for (k = lo; k < hi; ++k) a[k] = std::move(tmp[k]);
}

}  // namespace detail

// Kararlı sıralama. less(x, y): x, y'den önce gelmeli mi?
// Zaman: O(n log n) (en iyi, ortalama, en kötü). Ek alan: O(n).
template <typename T, typename Less>
void mergeSort(std::vector<T>& v, Less less) {
    std::vector<T> tmp(v.size());
    detail::mergeSortRange(v, tmp, 0, v.size(), less);
}

}  // namespace style