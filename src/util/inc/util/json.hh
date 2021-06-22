#ifndef UTIL_JSON_HH_
#define UTIL_JSON_HH_

#include <string>

#include <nlohmann/json.hpp>

#include <log/log.hh>

namespace util::json {

/** has<T>
 * Check if JSON object contains key of type T
 * 
 * @param key key to look for
 * 
 * @return true if such a key of type T is found, false otherwise
 */
template <typename T>
bool has(const nlohmann::json& j, const std::string& key);

struct _json_constraint {
    virtual ~_json_constraint() {}
    virtual bool check(const std::string& context, const nlohmann::json& j) = 0;
};

template<typename T>
struct required : public _json_constraint {
    T& target;
    const std::string& name;

    required(T& t, const std::string& n) : target(t), name(n) {}
    required(std::initializer_list<required> r) {}
    ~required() override {}

    bool check(const std::string& context, const nlohmann::json& j) override
    {
        if (has<T>(j, name)) {
            target = j[name].get<T>();
            return true;
        }

        logging::warn("json") << "Failed to find mandatory configuration element '" << name << "' for " << context << logging::endl;
        return false;
    }
};

template<typename T>
struct optionnal : public _json_constraint {
    T& target;
    const std::string& name;
    T def;

    optionnal(T& t, const std::string& n, const char* d) : target(t), name(n), def(d) {}
    optionnal(T& t, const std::string& n, T d) : target(t), name(n), def(d) {}
    optionnal(std::initializer_list<optionnal> o) {}
    ~optionnal() override {}

    bool check(const std::string& /* context */, const nlohmann::json& j) override
    {
        if (has<T>(j, name)) {
            target = j[name].get<T>();
        } else {
            target = def;
        }

        return true;
    }
};

template <class ... Args>
bool validate(const std::string& context, const nlohmann::json& j, Args ... args)
{
    std::vector<_json_constraint*> constraints = {&args ...};

    for (_json_constraint* constraint: constraints) {
        if (!constraint->check(context, j)) {
            return false;
        }
    }

    return true;
}

}

using json = nlohmann::json;

#endif