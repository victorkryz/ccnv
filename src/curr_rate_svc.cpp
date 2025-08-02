#include "curr_rate_svc.h"
#include "nlohmann/json.hpp"
#include <curl/curl.h>
#include <iostream>

CurrencyRateService::CurrencyList CurrencyRateService::get_all_curr_list()
{
    CurrencyList curr_list;

    std::stringstream response;
    auto result = request_from_remote(std::string(CURR_LIST_REF.data(), CURR_LIST_REF.size()).c_str(), response);
    if (result.first && is_http_status_ok(result.second))
    {
        nlohmann::json js = json::parse(response.str());
        curr_list = js.get<std::map<std::string, std::string>>();
    }

    return curr_list;
}

std::string CurrencyRateService::compose_rest_request(const std::string& currency)
{
    std::string request(CURR_LIST_RATE_BY.data(), CURR_LIST_RATE_BY.size());
    request += (currency + ".json");
    return request;
}

CurrencyRateService::CurrencyRate CurrencyRateService::rate(const std::string& from, const std::string& to)
{
    CurrencyRate rate;

    std::stringstream response;
    auto result = request_from_remote(compose_rest_request(from), response);
    if (result.first)
    {
        std::string rsp_str(response.str());
        if ((static_cast<long>(HTTPResponseCodes::NotFound) == result.second) || rsp_str.empty())
            raise_not_found(from);

        json js_rsp;
        try
        {
            js_rsp = json::parse(rsp_str);

            std::string date_val(obtain_date(js_rsp));

            json js_from;
            if (!obtain_value(js_rsp, from, js_from))
                raise_not_found(from);

            double rate_val(0);
            if (!obtain_value(js_from, to, rate_val))
                raise_not_found(to);

            rate = {{from, to}, rate_val, date_val};
        }
        catch (const json::exception&)
        {
            throw;
        }
    }

    return rate;
}

std::string CurrencyRateService::obtain_date(const json& js)
{
    std::string date_val;
    obtain_value(js, "date", date_val);
    return date_val;
}

void CurrencyRateService::raise_not_found(const std::string& currency)
{
    std::string msg("\"" + currency + "\"");
    msg += " currency not found!";
    throw std::invalid_argument(msg);
}

std::pair<bool, long> CurrencyRateService::request_from_remote(const std::string& request, std::stringstream& response)
{
    std::pair<bool, long> result = std::make_pair<bool, long>(false, 0);

    CurlReader reader(curl_);
    curl_->reset();
    result.first = reader.perform_request(request.c_str(), response);
    if (result.first)
        result.second = curl_->get_response_code();

    return result;
}
