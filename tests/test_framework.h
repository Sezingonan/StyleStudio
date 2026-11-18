#pragma once
#include <iostream>

namespace testing {
inline int checks = 0;
inline int failures = 0;
}

#define CHECK(cond)                                                         \
    do {                                                                    \
        ++testing::checks;                                                  \
        if (!(cond)) {                                                      \
            ++testing::failures;                                            \
            std::cerr << "FAIL: " << #cond << "  (" << __FILE__ << ":"      \
                      << __LINE__ << ")\n";                                 \
        }                                                                   \
    } while (0)

#define CHECK_EQ(a, b) CHECK((a) == (b))

inline int finish(const char* name) {
    std::cout << name << ": " << (testing::checks - testing::failures) << "/"
              << testing::checks << " kontrol basarili\n";
    return testing::failures == 0 ? 0 : 1;
}