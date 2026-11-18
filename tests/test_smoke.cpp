#include "test_framework.h"
#include "Version.h"

int main() {
    CHECK_EQ(style::version(), "0.1.0");
    CHECK(!style::version().empty());
    return finish("test_smoke");
}