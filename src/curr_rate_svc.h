#pragma once
#include "nlohmann/json.hpp"
#include "utils/curl_helper.h"
#include "utils/currency.h"
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>

using namespace std::literals;

using json = nlohmann::json;

class CurrencyRateService
{
    inline static constexpr const auto CURR_LIST_REF = "https://cdn.jsdelivr.net/npm/@fawazahmed0/currency-api@latest/v1/currencies.json"sv;
    inline static constexpr const auto CURR_LIST_RATE_BY_REF = "https://cdn.jsdelivr.net/npm/@fawazahmed0/currency-api@latest/v1/currencies/"sv;

public:
    using CurrencyList = std::map<std::string, std::string>;
    struct CurrencyRate
    {
        std::pair<std::string, std::string> from_to;
        double rate = 1;
        std::string date;
    };

public:
    CurrencyRateService() : curl_(std::make_shared<CurlHolder>()) {}

    CurrencyList get_all_curr_list();
    CurrencyRate rate(const std::string& from, const std::string& to);

protected:
    virtual std::pair<bool, long> request_from_remote(const std::string& request, std::stringstream& response);
    std::string compose_rest_request(const std::string& currency);
    std::string obtain_date(const json& js);
    template <typename K, typename T>
    static bool obtain_value(const json& js, const K& key, T& val)
    {
        bool result(false);

        auto it = js.find(key);
        if (it != js.end())
        {
            val = *it;
            result = true;
        }

        return result;
    }
    void raise_not_found(const std::string& currency);

private:
    std::shared_ptr<CurlHolder> curl_ = nullptr;
};