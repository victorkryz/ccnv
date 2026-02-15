
#include <iostream>
#include <string>

#include "cxxopts.hpp"
#include <fmt/core.h>

#include "curr_rate_svc.h"

constexpr auto app_name = APP_NAME;
constexpr auto app_version = APP_VERSION;

struct ClArguments
{
    std::string curr_from;
    std::string curr_to;
    double amount = 1;
    bool show_curr_list = false;
};

std::pair<int, bool> process_arguments(int argc, char* argv[], ClArguments& args);
void show_usage(const cxxopts::Options& options);
void show_version();
void print_currencies_list(const CurrencyRateService::CurrencyList& list);
void print_rating_result(const CurrencyRateService::CurrencyRate& rate_info,
                        const Currency& value_from, const Currency& value_to);

int main(int argc, char* argv[])
{
    int result(1);

    try
    {
        ClArguments args;

        const auto& [parsing_result, usage_requested] =
            process_arguments(argc, argv, args);
        if (parsing_result != 0)
            return parsing_result;
        else if (usage_requested)
            return 0;

        CurrencyRateService rate_svc;
        if (args.show_curr_list)
        {
            const auto curr_list = rate_svc.get_all_curr_list();
            print_currencies_list(curr_list);
        }
        else if (!args.curr_from.empty())
        {
            const auto rate_info = rate_svc.rate(args.curr_from, args.curr_to);

            const auto& [from, to] = rate_info.from_to;
            Currency value_from(args.amount, from);
            Currency value_to = Currency::convert(value_from, to, rate_info.rate);

            print_rating_result(rate_info, value_from, value_to);
        }
        result = 0;
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return result;
}

void print_currencies_list(const CurrencyRateService::CurrencyList& curr_list)
{
    for (const auto& [currency, country] : curr_list)
    {
        std::cout << currency << " : " << country << std::endl;
    }
}

void print_rating_result(const CurrencyRateService::CurrencyRate& rate_info, 
                         const Currency& value_from, const Currency& value_to)
{
    std::cout << "[" << rate_info.date << "]" << " [rate: " << rate_info.rate << "] "
                << value_from << " -> " << value_to << " " << std::endl;
}

std::pair<int, bool> process_arguments(int argc, char* argv[], ClArguments& args)
{
    std::pair<int, bool> result = {0, false};
    auto& [exit_code, usage ] = result;

    try
    {
        cxxopts::Options options("ccnv", "Currency converter");
        options.positional_help("[optional args]")
            .show_positional_help();

        // clang-format off     
        options.add_options()("l, list", "list all available currencies", 
                             cxxopts::value<bool>(args.show_curr_list))
                             ("f, from", "currency convert from (usd, eur, ...)", 
                             cxxopts::value<std::string>(args.curr_from))
                             ("t, to", "currency convert to (usd, eur, ... )", 
                             cxxopts::value<std::string>(args.curr_to))
                             ("a, amount", "amount (10, 50, 100, ...)", 
                             cxxopts::value<double>(args.amount))
                             ("v, version", "print version")
                             ("h, help", "print usage");
        // clang-format on

        auto parsed_args = options.parse(argc, argv);

        if (parsed_args.count("help"))
        {
            usage = true;
        }
        else if (parsed_args.count("version"))
        {
            show_version();
        }
        else if ( !(parsed_args.count("list") || 
                    (parsed_args.count("from") && parsed_args.count("to"))))
            exit_code = 1;

        if (usage || exit_code)
            show_usage(options);
    }
    catch (const cxxopts::exceptions::exception& e)
    {
        std::cout << "command line arguments parsing error: " << e.what() << std::endl;
        exit_code = 1;
    }

    return result;
}

void show_usage(const cxxopts::Options& options)
{
    const char* samples_of_using_templ =
        R"(Command line samples:
    {app_name} -l 
    {app_name} -f eur -t usd
    {app_name} -f usd -a 10 -t eur 
    {app_name}v -f usd -a 25 -t uah)";

    std::string samples_of_using =
        fmt::format(samples_of_using_templ, fmt::arg("app_name", app_name));

    std::string help = options.help();
    std::cout << std::endl
              << help << std::endl;

    std::cout << samples_of_using << std::endl << std::endl;
}

void show_version()
{
    std::string version = fmt::format("{} version {}", app_name, app_version);
    std::cout << version << std::endl;
}
