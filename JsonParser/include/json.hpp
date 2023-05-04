#pragma once
#include <cctype>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <iostream>
#include <ranges>

namespace eee {

struct Json;

using Int = int;
using Bool = bool;
using Float = double;
using String = std::string;
using Null = std::nullptr_t;
using Array = std::vector<Json>;
using Object = std::map<String, Json>;

using Value = std::variant<Null, Bool, Int, Float, String, Array, Object>;

template<typename T>
concept is_value = requires(T val) { Value(val); };

struct Json {
    Value value_;

    Json() = default;
    Json(const Json &other) noexcept : value_(other.value_){};
    auto operator=(const Json &other) noexcept -> Json & {
        value_ = other.value_;
        return *this;
    };

    Json(Json &&other) noexcept : value_(std::move(other.value_)){};
    auto operator=(Json &&other) noexcept -> Json & {
        other.swap(*this);
        return *this;
    };

    auto swap(Json &other) noexcept -> void { std::swap(other.value_, value_); }

    template<typename T>
        requires is_value<T>
    Json(T &&value) : value_(std::forward<T>(value)){};
    template<typename T>
        requires is_value<T>
    auto operator=(T &&value) -> Json & {
        value_ = value;
        return *this;
    }

    auto size() const -> size_t {
        if (auto *arr = std::get_if<Array>(&value_)) { return arr->size(); }
        if (auto *obj = std::get_if<Object>(&value_)) { return obj->size(); }
        throw std::runtime_error("Type is not Object or Array");
    }

    auto operator[](const std::string &key) -> Json & {
        if (auto *val = std::get_if<Object>(&value_)) { return (*val)[key]; }
        throw std::runtime_error("Type is not Object");
    }
    auto operator[](size_t index) -> Json & {
        if (auto *val = std::get_if<Array>(&value_)) { return val->at(index); }
        throw std::runtime_error("Type is not Array");
    }
    template<typename T>
    auto insert(T &&value) -> void
        requires std::
            is_same_v<std::pair<String, Json>, std::remove_reference_t<T>>
    {
        if (auto *val = std::get_if<Object>(&value_)) {
            val->insert(std::forward<T>(value));
            return;
        }
        throw std::runtime_error("Type is not Object");
    }
    template<typename T>
    auto push_back(T &&x) -> void {
        if (auto *val = std::get_if<Array>(&value_)) {
            val->push_back(std::forward<T>(x));
            return;
        }
        throw std::runtime_error("Type is not Array");
    }
};

class JsonParser {
  private:
    std::string_view json_str_;
    size_t pos_;

    auto parse_whitespace() -> void {
        for (; pos_ < json_str_.size() and std::isspace(json_str_[pos_]);
             ++pos_)
            ;
    }

    auto parse_null() -> std::optional<Value> {
        parse_whitespace();
        if (json_str_.substr(pos_, 4) == "null") return pos_ += 4, Null{};
        return std::nullopt;
    }

    auto parse_bool() -> std::optional<Value> {
        parse_whitespace();
        if (json_str_.substr(pos_, 4) == "true") return pos_ += 4, Bool{true};
        if (json_str_.substr(pos_, 5) == "false") return pos_ += 5, Bool{false};
        return std::nullopt;
    }

    auto parse_number() -> std::optional<Value> {
        parse_whitespace();
        size_t tmp_pos = pos_;
        bool is_float = false;
        auto is_signs = [this] {
            return json_str_[pos_] == 'e' or json_str_[pos_] == 'E'
                   or json_str_[pos_] == '.';
        };
        for (; pos_ < json_str_.size()
               and (std::isdigit(json_str_[pos_]) or is_signs());
             ++pos_)
            if (is_signs()) is_float |= true;
        const std::string number{
            json_str_.begin() + tmp_pos, json_str_.begin() + pos_};
        if (is_float) {
            try {
                Float ret = std::stod(number);
                return ret;
            } catch (...) { return std::nullopt; }
        }
        try {
            Int ret = std::stoi(number);
            return ret;
        } catch (...) { return std::nullopt; }
    }

    auto parse_string() -> std::optional<String> {
        ++pos_; // "
        if (size_t endpos = json_str_.find('"', pos_);
            endpos != json_str_.npos) {
            std::swap(++endpos, pos_);
            return std::string(
                json_str_.begin() + endpos, json_str_.begin() + pos_ - 1);
        }
        return std::nullopt;
    }

    auto parse_array() -> std::optional<Value> {
        ++pos_; // [
        for (Array vec{}; pos_ < json_str_.size();) {
            if (auto ret = parse_value(); ret.has_value())
                vec.push_back(std::move(ret.value()));
            else
                break;
            parse_whitespace();
            if (json_str_.at(pos_) == ',') ++pos_;
            else if (json_str_.at(pos_) == ']')
                return ++pos_, vec;
            else
                break;
        }
        return std::nullopt;
    }

    auto parse_object() -> std::optional<Value> {
        ++pos_; // {
        auto skip_signs = [this](char sign) {
            parse_whitespace();
            if (json_str_.at(pos_) == sign) return true;
            return false;
        };
        for (Object map{}; pos_ < json_str_.size();) {
            auto key = parse_string(); // key
            if (!key.has_value()) break;

            if (skip_signs(':')) ++pos_;
            else
                break;

            auto value = parse_value(); // value
                                        //
            if (!value.has_value()) break;

            map[key.value()] = value.value();

            if (skip_signs(',')) ++pos_;
            else if (skip_signs('}'))
                return ++pos_, map;
        }
        return std::nullopt;
    }

    auto parse_value() -> std::optional<Value> {
        parse_whitespace();
        switch (json_str_[pos_]) {
            case 'n':
                return parse_null();
            case 't':
                return parse_bool();
            case 'f':
                return parse_bool();
            case '"':
                return parse_string();
            case '[':
                return parse_array();
            case '{':
                return parse_object();
            default:
                return parse_number();
        }
    }

  public:
    JsonParser(const JsonParser &other) = delete;
    auto operator=(const JsonParser &other) -> JsonParser & = delete;
    JsonParser(JsonParser &&other) noexcept = delete;
    auto operator=(JsonParser &&other) noexcept -> JsonParser & = delete;

    JsonParser() : json_str_(), pos_(0) {}
    JsonParser(const std::string_view &json_str)
        : json_str_(json_str), pos_(0) {}

    auto parse() -> std::optional<Json> {
        if (auto ret = parse_value(); ret.has_value()) {
            return Json(ret.value());
        }
        return std::nullopt;
    }
};

auto parse(const std::string_view &json) -> std::optional<Json> {
    JsonParser tmp{json};
    return tmp.parse();
}

class JsonGenerator {
  public:
    auto generate(const Json &json) noexcept -> std::string {
        return std::visit(
            [this](auto &&arg) -> std::string {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<Null, T>) {
                    return "null";
                } else if constexpr (std::is_same_v<Bool, T>) {
                    return arg ? "true" : "false";
                } else if constexpr (
                    std::is_same_v<Int, T> or std::is_same_v<Float, T>) {
                    return std::to_string(arg);
                } else if constexpr (std::is_same_v<String, T>) {
                    return "\"" + arg + "\"";
                } else if constexpr (std::string ret{'['};
                                     std::is_same_v<Array, T>) {
                    for (auto it = arg.begin(); it != arg.end(); ++it) {
                        if (it != arg.begin()) ret += ',';
                        ret += generate(*it);
                    }
                    return ret + ']';
                } else if constexpr (std::string ret{'{'};
                                     std::is_same_v<Object, T>) {
                    for (auto it = arg.begin(); it != arg.end(); ++it) {
                        if (it != arg.begin()) ret += ',';
                        auto [key, value] = *it;
                        ret += "\"" + key + "\":" + generate(value);
                    }
                    return ret + '}';
                }
            },
            json.value_);
    }
};

auto generate(const Json &json) noexcept -> std::string {
    JsonGenerator tmp;
    return tmp.generate(json);
}

auto operator<<(std::ostream &out, const Json &json) -> std::ostream & {
    out << generate(json);
    return out;
}

} // namespace eee
