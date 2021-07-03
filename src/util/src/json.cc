#include <util/json.hh>

namespace util::json {

template <>
bool has<std::string>(const nlohmann::json& j, const std::string& key)
{
    return j.count(key) && j[key].is_string();
}

template <>
bool has<double>(const nlohmann::json& j, const std::string& key)
{
    return j.count(key) && j[key].is_number();
}

template <>
bool has<int>(const nlohmann::json& j, const std::string& key)
{
    return j.count(key) && j[key].is_number_integer();
}

template <>
bool has<unsigned>(const nlohmann::json& j, const std::string& key)
{
    return j.count(key) && j[key].is_number_unsigned();
}

template <>
bool has<unsigned long> (const nlohmann::json& j, const std::string& key)
{
    return j.count(key) && j[key].is_number_unsigned();
}

template <>
bool has<bool>(const nlohmann::json& j, const std::string& key)
{
    return j.count(key) && j[key].is_boolean();
}

template<>
bool has<std::vector<int>>(const nlohmann::json& j, const std::string& key)
{
    bool ret = j.count(key) && j[key].is_array();

    if (ret) {
        for (const nlohmann::json& e : j) {
            ret = ret || e.is_number_integer();
        }
    }

    return ret;
}

template<>
bool has<std::vector<unsigned>>(const nlohmann::json& j, const std::string& key)
{
    bool ret = j.count(key) && j[key].is_array();

    if (ret) {
        for (const nlohmann::json& e : j) {
            ret = ret || e.is_number_unsigned();
        }
    }

    return ret;
}

template<>
bool has<std::vector<std::string>>(const nlohmann::json& j, const std::string& key)
{
    bool ret = j.count(key) && j[key].is_array();

    if (ret) {
        for (const nlohmann::json& e : j) {
            ret = ret || e.is_string();
        }
    }

    return ret;
}


}