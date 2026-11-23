#include <nlohmann/json.hpp>

#include "test_framework.h"

int main() {
    // Yazıdan JSON oluştur ve alanları oku
    auto j = nlohmann::json::parse(R"({"id": 1001, "name": "Palazzo Pantolon"})");
    CHECK_EQ(j["id"].get<int>(), 1001);
    CHECK_EQ(j["name"].get<std::string>(), "Palazzo Pantolon");

    // Bozuk JSON: çökmek yerine istisna (exception) fırlatmalı
    bool caught = false;
    try {
	(void)nlohmann::json::parse("{ bozuk json");
    } catch (const nlohmann::json::parse_error&) {
        caught = true;
    }
    CHECK(caught);

    return finish("test_json_smoke");
}