\# Style Studio: Zaman ve Uzay Karmaşıklığı Analizi



\## Notasyon



| Sembol | Anlamı |

|---|---|

| n | Katalogdaki ürün sayısı |

| L | Ürün adının uzunluğu (normalize sonrası karakter sayısı) |

| σ | Trie düğümünün çocuk sayısı (`std::map` kullanıldığı için erişim O(log σ)) |

| k | İstenen öneri sayısı |

| D | En uzun ürün adının uzunluğu |

| V, E | Graf düğüm ve kenar sayısı |

| deg | Bir düğümün komşu sayısı |

| m | Bir kombindeki ürün sayısı |

| h | Undo/Redo geçmişinin uzunluğu |

| q | İnceleme kuyruğundaki ürün sayısı |

| r | Fiyat aralığına düşen ürün sayısı |



\## 1. Zaman karmaşıklığı



\### AVL ağacı (`AVLTree<int, Clothing>`, anahtar: ürün ID)



| İşlem | En iyi | Ortalama | En kötü | Açıklama |

|---|---|---|---|---|

| Insert | O(log n) | O(log n) | O(log n) | Her zaman yaprağa kadar inilir, dönüşte yükseklik güncellenir, en fazla bir (çift) rotasyon |

| Search | O(1) | O(log n) | O(log n) | Anahtar kökteyse O(1). Yükseklik ≤ 1.44·log₂ n garantisi |

| Delete | O(log n) | O(log n) | O(log n) | Silmede dönüş yolundaki her düğümde `rebalance` çalışır, birden fazla rotasyon olabilir |

| In-order gezinti | O(n) | O(n) | O(n) | Her düğüm bir kez |

| Sol/sağ rotasyon | O(1) | O(1) | O(1) | Sabit sayıda işaretçi değişir |



Bu garanti AVL'nin dengeli olmasından gelir. Sıradan BST'de sıralı ekleme O(n) verirdi. Testte 1..1000 sıralı eklemede yükseklik ≤ 14 kaldı.



\### Trie (ürün adı → ID)



| İşlem | En iyi | Ortalama | En kötü | Açıklama |

|---|---|---|---|---|

| normalize | O(L) | O(L) | O(L) | Tek geçiş |

| Insert | O(L log σ) | O(L log σ) | O(L log σ) | Her harfte `map` araması + normalize |

| Find / startsWith | O(1)\* | O(L log σ) | O(L log σ) | \*Boş önek. Ürün sayısından bağımsız |

| Remove | O(L log σ) | O(L log σ) | O(L log σ) | Yol kaydı + aşağıdan yukarı budama |

| Suggest(önek, k) | O(L log σ) | O(L log σ + k·D) | O(L log σ + k·D) | Önek düğümüne in, sonra DFS. k sonuca ulaşınca durur. Budama sayesinde her dal en az bir ID taşır |



\*\*Düzeltme:\*\* İlk promptta Insert için O(L) ve öneri için O(L + K) yazıyordu. Bizim implementasyonda düğüm çocukları `std::map` olduğu için her harf adımı O(log σ) tutar. σ küçük ve sınırlı olduğundan pratikte O(L) gibi davranır, ama formülde log σ çarpanını yazıyoruz. Öneri üretiminde de alt ağaç gezintisi, toplanan her ID için en fazla D düğüm dolaştığından O(k·D) ile sınırlıdır.



\### Graf ve BFS (adjacency list, `std::map<int, vector<int>>`)



| İşlem | Zaman | Açıklama |

|---|---|---|

| addNode | O(log V) | `map` ekleme |

| hasEdge | O(log V + deg) | Komşu listesi taranır |

| addEdge | O(log V + deg) | Tekrar kontrolü için `hasEdge` |

| removeEdge | O(log V + deg) | İki listeden silme |

| removeNode | O(deg · (log V + deg)) | Her komşunun listesinden de silinir |

| BFS | O((V + E) log V) | Her düğüm bir kez kuyruğa girer, her kenar iki yönden bir kez bakılır. `dist` ve `adj\_` erişimleri `map` olduğu için log V |

| rebuild (graf kurma) | O(n²) kural kontrolü + kenar ekleme maliyeti | Yoğun grafta kenar ekleme O(deg) olduğundan en kötü durumda O(n³) |

| onAdded | O(n · deg) | Yeni ürün her mevcut ürünle karşılaştırılır |

| recommend | O((V + E) log V + V log n) | BFS + her ziyaret için AVL araması |



\*\*Düzeltme:\*\* İlk promptta BFS için O(V + E) yazıyordu. Bu, düğümler 0..n-1 dizi indeksi olduğunda geçerlidir. Bizde düğüm kimliği ürün ID'si ve `std::map` kullanıldığı için O((V + E) log V). Ayrıca bizim kuralımızda nötr renkler her şeyle uyumlu olduğundan graf yoğun çıkıyor (çiftlerin yaklaşık %36-37'si bağlı), yani E = Θ(n²).



\### Sıralama ve arama



| Algoritma | En iyi | Ortalama | En kötü | Kararlı mı? |

|---|---|---|---|---|

| Merge Sort | O(n log n) | O(n log n) | O(n log n) | Evet |

| Quick Sort (median-of-three, Hoare) | O(n log n) | O(n log n) | O(n²) | Hayır |

| Binary Search (lowerBound / upperBound / binarySearch) | O(log n) | O(log n) | O(log n) | - |

| isSorted | O(n) | O(n) | O(n) | - |



\- Ada göre Merge Sort'ta karşılaştırıcı her çağrıda `normalize` çalıştırdığı için toplam O(n log n · L).

\- Quick Sort'ta median-of-three, sıralı ve ters sıralı girdide O(n²)'yi önler ama özel hazırlanmış girdide en kötü durum mümkündür.

\- Binary Search öncesi dizinin sıralı olması çağıranın sorumluluğudur. `isSorted` ile O(n)'de doğrulanabilir. Katalog sorgusu fiyat aralığında diziyi önce Quick Sort ile sıralar.



\### Stack, Queue, kombin ve katalog



| İşlem | Zaman | Açıklama |

|---|---|---|

| Stack push / pop / top | O(1) | Bağlı liste, tepe başta |

| Queue enqueue / dequeue / front | O(1) | Bağlı liste, baş ve son işaretçisi |

| OutfitEditor add / remove / clear | O(m) | Tekrar kontrolü ve snapshot kopyası |

| OutfitEditor undo / redo | O(m) | Karşı yığına kopya atılır |

| ReviewQueue enqueue / dequeue | O(log q) | Kuyruk O(1), tekrar kontrolü için `std::set` log q |

| ReviewQueue processNext | O(s · (log q + log n)) | s = atlanan silinmiş ürün sayısı |

| ClothingManager add / remove / findById | O(L log σ + log n) / aynı / O(log n) | Trie + AVL |

| ClothingManager suggestByName | O(L log σ + k·D + k log n) | Trie önerisi + k AVL araması |

| queryCatalog (fiyat filtreli) | O(n log n) ortalama | Quick Sort + 2 Binary Search + filtre + sonuç sıralama O(r log r) |

| JSON yükleme (`ClothingCollection`) | O(n²) | `ClothingCollection::add` tekrar kontrolü için doğrusal arar |



\*\*Dürüst notlar:\*\*

1\. Fiyat filtresinde dizi her sorguda yeniden sıralandığından Binary Search sorguyu tek başına hızlandırmıyor. Sıralı diziyi önbellekte tutmak iyileştirme önerisidir.

2\. `ClothingCollection::add` doğrusal arama yaptığı için dosyadan yükleme O(n²)'dir. 5000 üründe test geçiyor ama büyük veride AVL'ye taşınabilir.



\## 2. Uzay karmaşıklığı



| Yapı / algoritma | Uzay | Açıklama |

|---|---|---|

| AVL ağacı | O(n) | Düğüm başına ürün kopyası + 2 çocuk işaretçisi + yükseklik. Özyineleme yığını O(log n) |

| Trie | O(toplam harf sayısı) en kötü O(n · D) | Ortak önekler paylaşılır. Her düğümde `map` ek yükü vardır |

| Graf | O(V + E) | Yoğun grafta O(n²) |

| BFS | O(V) | `dist` haritası + kuyruk |

| Merge Sort | O(n) | Yardımcı dizi |

| Quick Sort | O(log n) | Yerinde. Küçük taraf özyinelemeyle, büyük taraf döngüyle işlendiği için yığın derinliği O(log n) |

| Binary Search | O(1) | Döngüyle |

| Stack / Queue | O(n) | Eleman başına bir düğüm |

| Undo/Redo geçmişi | O(h · m) | Her adımda kombinin kopyası. Kombinler küçük (5-8 ürün) olduğundan önemsiz |

| ReviewQueue | O(q) | Kuyruk + tekrar kontrol kümesi |



\## 3. Teori ve ölçüm karşılaştırması (Release derleme, tek çalıştırma)



| Ölçüm | n → 5n | Gözlenen oran | Teorik beklenti |

|---|---|---|---|

| AVL insert (n=1000 → 5000) | 0.228 → 1.348 ms | 5.9× | 5 · (log 5000 / log 1000) ≈ 6.2× ✓ |

| Dizide doğrusal arama (n arama) | 0.213 → 5.360 ms | 25× | 5² = 25× ✓ (O(n²)) |

| AVL search (n arama) | 0.073 → 0.660 ms | 9× | ≈ 6.2×, önbellek etkisi ve gürültü |

| Trie suggest | ≈ 0.4 µs sabit | \~1× | n'den bağımsız ✓ |

| Merge Sort (fiyat) | 0.304 → 2.382 ms | 7.8× | ≈ 6.2× |

| Quick Sort (fiyat) | 0.176 → 0.985 ms | 5.6× | ≈ 6.2× ✓ |

| Binary Search (tek sorgu) | 59 → 77 ns | 1.3× | log 5000 / log 1000 ≈ 1.23× ✓ |

| Graf kenar sayısı (500 → 1000) | 44768 → 182516 | 4.1× | 2² = 4× ✓ (E = Θ(n²)) |

| Graf kurma (500 → 1000) | 11.2 → 54.7 ms | 4.9× | ≥ 4×, kenar ekleme maliyeti ek yük bindiriyor ✓ |



\*\*Yorum.\*\* Ölçümler teoriyle büyük ölçüde uyumlu. n=100'de doğrusal arama AVL'den biraz hızlı çıktı (sabit çarpanlar baskın), n=5000'de AVL yaklaşık 8 kat hızlı. Quick Sort, Merge Sort'tan yaklaşık 2 kat hızlı (yerinde çalışır, ek dizi yok), buna karşılık Merge Sort kararlıdır. Ada göre sıralama `normalize` çağrıları yüzünden fiyata göre sıralamadan yaklaşık 5 kat yavaş.



\*\*Ölçümün sınırları.\*\* Değerler tek çalıştırmadır, n=100'de süreler mikrosaniye düzeyinde olduğu için gürültülüdür. Ölçümler tek bir makinede alındı (işlemci modelini rapora yaz). Ölçülen süreler teorik analizin yerine geçmez, onu doğrular.

