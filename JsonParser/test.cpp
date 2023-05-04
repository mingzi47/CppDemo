#include "include/json.hpp"
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <vector>

void test0() {
    std::string s =
        "{\"numbers\":[23,66,5,33,46,78],\"checked\":true,\"id\":38934,\"object\":{\"t\":\"json校验器\",\"w\":\"json检查\"},\"host\":\"json-online.com\"}";
    auto json = eee::parse(s);
    std::string g = eee::generate(json.value());
    std::cout << g << "\n";
    return;
}

void test1() {
    eee::Json json{eee::Object{}};
    json["1111"] = eee::Json(1111);
    std::string key = "111";
    std::string value = "1111";
    int k = 111;
    eee::Json j(100);
    std::pair<std::string, eee::Json> p1 = {key, k};
    json.insert(std::move(p1));
    json.insert(std::pair<std::string, eee::Json>{"111", 1111});
    std::vector<eee::Json> v1{
        eee::Json(1), eee::Json(3), eee::Json(4),
        eee::Json(std::string("111"))};
    std::vector<eee::Json> v2{1, 3, 4, "111"};
    eee::Json j2(std::move(v1));
    eee::Json j3(v2);
    j2[3] = json;
    json.insert(std::pair<std::string, eee::Json>(
        "222", std::vector<eee::Json>{1, 1.0001, 1e10}));
    json["111"] = false;
    json["33333333"] = nullptr;
    json["444444"] = true;
    std::cout << json << "\n";
    std::cout << j << "\n";
    std::cout << j2 << "\n";
    std::cout << j3 << "\n";
}

auto main() -> int {
    test1();
    test0();
    std::vector<int> vec(100);
    return 0;
}
