#pragma once
#include <cstddef>
#include <utility>
#include <vector>

namespace style {

namespace detail {

// a[lo], a[mid], a[hi] içinden ortancayı a[lo]'ya getirir (pivot olarak kullanılır).
template <typename T, typename Less>
void medianOfThreeToFront(std::vector<T>& a, std::size_t lo, std::size_t hi,
                          Less& less) {
    const std::size_t mid = lo + (hi - lo) / 2;
    // Üçünü küçükten büyüğe diz: a[lo] <= a[mid] <= a[hi]
    if (less(a[mid], a[lo])) std::swap(a[mid], a[lo]);
    if (less(a[hi], a[lo])) std::swap(a[hi], a[lo]);
    if (less(a[hi], a[mid])) std::swap(a[hi], a[mid]);
    // Ortanca a[mid]'de; pivotu a[lo]'ya al.
    std::swap(a[lo], a[mid]);
}

// [lo, hi] (iki uç dahil) aralığını sıralar.
template <typename T, typename Less>
void quickSortRange(std::vector<T>& a, std::size_t lo, std::size_t hi, Less& less) {
    while (lo < hi) {
        medianOfThreeToFront(a, lo, hi, less);
        const T pivot = a[lo];  // kopya: takaslar sırasında yerinden oynayabilir

        // Hoare bölmesi
        std::size_t i = lo;
        std::size_t j = hi + 1;
        while (true) {
            do { ++i; } while (i <= hi && less(a[i], pivot));
            do { --j; } while (less(pivot, a[j]));
            if (i >= j) break;
            std::swap(a[i], a[j]);
        }
        std::swap(a[lo], a[j]);  // pivot kesin yerine oturur: a[j]

        // Küçük tarafı özyinelemeyle, büyük tarafı döngüyle işle (derinlik O(log n)).
        const std::size_t leftSize = j - lo;
        const std::size_t rightSize = hi - j;
        if (leftSize < rightSize) {
            if (j > lo) quickSortRange(a, lo, j - 1, less);
            lo = j + 1;
        } else {
            quickSortRange(a, j + 1, hi, less);
            if (j == lo) return;
            hi = j - 1;
        }
    }
}

}  // namespace detail

// Kararsız, yerinde sıralama. less(x, y): x, y'den önce gelmeli mi?
// Zaman: ortalama O(n log n), en kötü O(n^2) (median-of-three ile nadir).
// Ek alan: O(log n) özyineleme yığını.
template <typename T, typename Less>
void quickSort(std::vector<T>& v, Less less) {
    if (v.size() < 2) return;
    detail::quickSortRange(v, 0, v.size() - 1, less);
}

}  // namespace style