#pragma once
#include <curl/curl.h>
#include <memory>
#include <string>

class CurlHolder
{
public:
    CurlHolder()
    {
        init();
    }
    ~CurlHolder()
    {
        uninit();
    }

    void assign(CURL* curl)
    {
        curl_ = curl;
    }

    CURL* get_curl()
    {
        return curl_;
    }

    void reset()
    {
        if (nullptr != curl_)
            curl_easy_reset(curl_);
    }

    long get_response_code()
    {
        long status(-1);
        if (nullptr != curl_)
            curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &status);

        return status;
    }

protected:
    bool init()
    {
        curl_ = curl_easy_init();
        return (nullptr != curl_);
    }

    void uninit()
    {
        if (nullptr != curl_)
            curl_easy_cleanup(curl_);
    }

private:
    CURL* curl_ = nullptr;
};

class CurlReader
{
public:
    CurlReader(std::shared_ptr<CurlHolder> curl) : curl_(curl) {}
    virtual ~CurlReader() = default;

    template <typename ST>
    bool perform_request(const char* request, ST& response)
    {
        auto write_callback_ptr = +[](char* contents, size_t size, size_t nmemb, void* userp) -> size_t
        {
            ST* stream = reinterpret_cast<ST*>(userp);
            const size_t chunk_sz = size * nmemb;
            stream->write(contents, chunk_sz);
            return chunk_sz;
        };

        bool result(false);
        CURL* curl = curl_->get_curl();
        if (nullptr != curl)
        {
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_ptr);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            setup_curl_options(curl, request);

            CURLcode res = curl_easy_perform(curl);
            if (res == CURLE_OK)
                result = true;
            else
                last_error_ = curl_easy_strerror(res);
        }

        return result;
    }

    std::string get_last_error()
    {
        return last_error_;
    }

protected:
    virtual void setup_curl_options(CURL* curl, const char* request)
    {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
        curl_easy_setopt(curl, CURLOPT_URL, request);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    }

protected:
    std::shared_ptr<CurlHolder> curl_;
    std::string last_error_;
};

enum class HTTPResponseCodes : long
{
    OK = 200,
    Created = 201,
    NoContent = 204,
    BadRequest = 400,
    Unauthorized = 401,
    Forbidden = 403,
    NotFound = 404
};

inline bool is_http_status_ok(long status)
{
    return (static_cast<long>(HTTPResponseCodes::OK) == status);
}