#pragma once
#include <cstddef>

#include "models/ClothingCollection.h"

namespace style {

// Tutarlı (kategori/stil/mevsim/fiyat) örnek ürünler üretir.
// Aynı (count, seed) her zaman aynı sonucu verir. ID'ler firstId'den başlar.
ClothingCollection generateSampleClothes(std::size_t count, unsigned seed = 42,
                                         int firstId = 1001);

}  // namespace style