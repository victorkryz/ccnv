#include "curr_rate_svc.h"
#include "cxxopts/cxxopts.hpp"
#include <iostream>
#include <string>

struct ClArguments
{
    std::string curr_from;
    std::string curr_to;
    double amount = 1;
    bool show_curr_list = false;
};

bool process_arguments(int argc, char* argv[], ClArguments& args);
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
        if (process_arguments(argc, argv, args))
        {
            CurrencyRateService rate_svc;
            if (args.show_curr_list)
            {
                const auto curr_list = rate_svc.get_all_curr_list();
                print_currencies_list(curr_list);
            }
            else if (!args.curr_from.empty())
            {
                const auto rate_info = rate_svc.rate(args.curr_from, args.curr_to);

                auto [from, to] = rate_info.from_to;
                Currency value_from(args.amount, from);
                Currency value_to = Currency::convert(value_from, to, rate_info.rate);

                print_rating_result(rate_info, value_from, value_to);
            }
            result = 0;
        }
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
    for (const auto& item : curr_list)
    {
        auto [currency, country] = item;
        std::cout << currency << " : " << country << std::endl;
    }
}

void print_rating_result(const CurrencyRateService::CurrencyRate& rate_info, 
                         const Currency& value_from, const Currency& value_to)
{
    std::cout << "[" << rate_info.date << "]" << " [rate: " << rate_info.rate << "] "
                << value_from << " -> " << value_to << " " << std::endl;
}

bool process_arguments(int argc, char* argv[], ClArguments& args)
{
    bool result(false), usage(false), version(false);

    try
    {
        cxxopts::Options options("ccnv", "Currency converter");
        options.positional_help("[optional args]")
            .show_positional_help();

        options.add_options()("l, list", "list all available currencies", 
                             cxxopts::value<bool>(args.show_curr_list))("f, from", "currency convert from (usd, eur, ...)", 
                             cxxopts::value<std::string>(args.curr_from))("t, to", "currency convert to (usd, eur, ... )", 
                             cxxopts::value<std::string>(args.curr_to))("a, amount", "amount (10, 50, 100, ...)", 
                             cxxopts::value<double>(args.amount))("v, version", "print version")
                             ("h, help", "print usage");

        auto parsed_args = options.parse(argc, argv);

        if (parsed_args.count("help"))
        {
            usage = true;
            result = true;
        }
        else if (parsed_args.count("version"))
        {
            version = true;
            result = true;
        }
        else if ( parsed_args.count("list") || 
                    (parsed_args.count("from") && parsed_args.count("to")))
            result = true;

        if (version)
            show_version();

        if (usage || !result)
            show_usage(options);
    }
    catch (const cxxopts::exceptions::exception& e)
    {
        std::cout << "command line arguments parsing error: " << e.what() << std::endl;
        result = false;
    }

    return result;
}

void show_usage(const cxxopts::Options& options)
{
    const char* samples_of_using =
        R"(Command line samples:
    ccnv -l 
    ccnv -f eur -t usd
    ccnv -f usd -a 10 -t eur 
    ccnv -f usd -a 25 -t uah)";

    std::string help = options.help();
    std::cout << std::endl
              << help << std::endl;

    std::cout << samples_of_using << std::endl << std::endl;
}

void show_version()
{
    std::cout << "ccnv version 1.0" << std::endl;
}
