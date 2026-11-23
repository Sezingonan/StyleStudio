#include "test_framework.h"
#include "structures/AVLTree.h"

using namespace style;

int main() {
    AVLTree<int, int> tree;

    // Boş ağaç
    CHECK(tree.empty());
    CHECK_EQ(tree.size(), 0u);
    CHECK_EQ(tree.height(), 0);

    return finish("test_avl");
}