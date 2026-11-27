\# Style Studio



Dijital gardırop ve kombin oluşturma uygulaması. İstanbul Medeniyet Üniversitesi Bilgisayar Mühendisliği, Veri Yapıları ve Algoritmalar dönem projesi.



\*\*Dil:\*\* C++17 (tek ana dil) · \*\*Derleme:\*\* CMake · \*\*Veri:\*\* JSON · \*\*Arayüz:\*\* komut satırı (CLI)



\## Amaç



Kullanıcı kıyafet kataloğunu arayabilir, filtreleyebilir, sıralayabilir. Seçtiği ürüne uyumlu ürünler önerilir. Ürünlerden kombin oluşturur, son işlemi geri alır, kombini kalıcı olarak kaydeder.



\## Özellikler



\- Ürünleri JSON dosyasından yükleme (hatalı kayıtlar atlanır ve raporlanır, program çökmez)

\- ID ile hızlı ürün erişimi

\- Ürün adına göre ön ek araması (Türkçe karakter ve büyük/küçük harf duyarsız)

\- Kategori, renk, stil, mevsim ve fiyat aralığı filtresi; fiyat/ad sıralaması

\- Renk çarkı kuralına göre uyumlu ürün önerisi (BFS, kategoriye göre gruplu)

\- Kombin oluşturma, Undo/Redo, kaydetme ve yeniden açma

\- İnceleme sırası ("sonra bakacağım" listesi, FIFO)

\- 60 ürünlük örnek veri ve 100/500/1000/5000 ürünlük sentetik veri üretici

\- Performans ölçüm programı



\## Veri yapıları ve algoritmalar (hepsi kendi implementasyonumuz)



| Yapı / algoritma | Dosya | Projedeki kullanımı |

|---|---|---|

| AVL Tree | `src/core/structures/AVLTree.h` | Ürünlerin tek deposu, ID ile arama, ekleme, silme, rotasyonlar |

| Trie | `src/core/structures/Trie.h` | Ürün adı ön ek araması ve otomatik tamamlama |

| Graph (adjacency list) | `src/core/structures/Graph.h` | Ürünler arası uyumluluk ilişkileri |

| BFS | `Graph::bfs` | Seviyeli (doğrudan/dolaylı) uyumlu ürün önerisi |

| Merge Sort | `src/core/algorithms/MergeSort.h` | Ada göre sıralama (kararlı) |

| Quick Sort | `src/core/algorithms/QuickSort.h` | Fiyata göre sıralama (median-of-three) |

| Binary Search | `src/core/algorithms/BinarySearch.h` | Fiyat aralığı sorgusu (alt/üst sınır) |

| Stack | `src/core/structures/Stack.h` | Kombin Undo/Redo (iki yığın) |

| Queue | `src/core/structures/Queue.h` | BFS ve kullanıcının inceleme sırası |



Ayrıntılı zaman ve uzay analizi: \[docs/KARMASIKLIK.md](docs/KARMASIKLIK.md)



Üçüncü parti: yalnızca JSON okuma/yazma için \[nlohmann/json](https://github.com/nlohmann/json) (`third\_party/`). Zorunlu veri yapıları ve algoritmalar hazır kütüphaneyle değil, projede yazıldı.



\## Kurulum (Windows, MSVC)



Gerekenler: Visual Studio Build Tools ("Desktop development with C++"), CMake 3.16+, Git.



"x64 Native Tools" ya da Developer Command Prompt'ta:



```

git clone https://github.com/Sezingonan/StyleStudio.git

cd StyleStudio

cmake -S . -B build -G "NMake Makefiles"

cmake --build build

ctest --test-dir build --output-on-failure

```



Hız ölçümü için Release derleme:



```

cmake -S . -B build\_release -G "NMake Makefiles" -DCMAKE\_BUILD\_TYPE=Release

cmake --build build\_release

build\_release\\src\\tools\\style\_bench.exe

```



\## Çalıştırma



Proje klasöründen (veri yolları buna göre):



```

build\\src\\cli\\style\_cli.exe

```



| Komut | Açıklama |

|---|---|

| `list` | Tüm ürünler (ID sırasıyla) |

| `id <no>` | ID ile ürün |

| `ara <önek>` | Ada göre önek araması |

| `filtre kategori=.. renk=.. stil=.. sezon=.. min=.. max=.. sirala=fiyat-artan\\|fiyat-azalan\\|ad` | Filtre ve sıralama |

| `oner <id> \[seviye]` | Uyumlu ürün önerisi |

| `kombin ekle <id>` / `cikar <id>` / `goster` / `temizle` / `geri` / `ileri` | Kombin düzenleme |

| `kombin kaydet <ad>` / `ac <no>` / `kombinler` | Kalıcı kombinler (`data/outfits.json`) |

| `sira ekle <id>` / `goster` / `isle` | İnceleme sırası |

| `yardim` / `cik` | Yardım / çıkış |



Kategori, renk, stil ve mevsim değerleri İngilizce enum adlarıdır: `Top, Bottom, Outerwear, Shoes, Bag, Accessory`; `White, Black, Gray, Beige, Brown, Blue, Navy, Red, Green, Pink, Yellow, Other`; `Casual, Formal, Sport, Street, Classic`; `Spring, Summer, Autumn, Winter, All`.



\### Örnek oturum



```

> ara beyaz

> filtre kategori=Shoes sezon=Winter sirala=fiyat-azalan

> oner 1001

> kombin ekle 1001

> kombin ekle 1008

> kombin geri

> kombin ileri

> kombin kaydet Hafta Sonu

```



<!-- Ekran görüntüsü: docs/screenshot-cli.png -->



\## Veri üretimi



```

build\\src\\tools\\style\_gen.exe 60 data\\clothes.json

build\\src\\tools\\style\_gen.exe 5000 data\\big.json

```



Aynı sayı ve tohumla her zaman aynı veri üretilir.



\## Veri formatı



`data/clothes.json`, ürün dizisi: `id, name, category, color, style, season, price` zorunlu; `imagePath, productUrl, isUserAdded` isteğe bağlı. Kombinler `data/outfits.json` içinde yalnızca ürün ID'leriyle tutulur, toplam fiyat ürünlerden hesaplanır. Katalogdan silinen ürünler kombinde "eksik" sayılır, program çökmez.



\## Proje yapısı



```

src/core/      çekirdek kütüphane (yapılar, algoritmalar, yöneticiler, JSON)

src/cli/       komut satırı uygulaması

src/tools/     veri üretici (style\_gen) ve ölçüm (style\_bench)

tests/         22 test programı (kendi mini test altyapımız)

docs/          karmaşıklık analizi

data/          örnek veri

```



Çekirdek (`style\_core`) arayüzden bağımsızdır.



\## Öneri kuralı



İki ürün şu üç koşulu birlikte sağlarsa uyumludur: kategorileri farklı, renkleri renk çarkına göre uyumlu (aynı ton, komşu ya da karşıt; nötr renkler her şeyle), stilleri aynı grupta (Casual/Street/Sport ya da Formal/Classic). Kural `src/core/managers/Compatibility.h` içindedir.



\## Bilinen sınırlar



\- Ön ek araması adın başından eşleşir ("Pantolon" yazınca "Siyah Pantolon" bulunmaz).

\- Kural geniş olduğundan graf yoğundur (çiftlerin yaklaşık %37'si bağlı), graf kurma O(n²); 5000 üründe ölçümde atlanır.

\- Fiyat filtresinde dizi her sorguda yeniden sıralanır.

\- Grafik arayüz henüz yok.



\## Yazar



Sezin Gönan: İstanbul Medeniyet Üniversitesi, Bilgisayar Mühendisliği.

