#ifndef NPIREGISTRY_H
#define NPIREGISTRY_H

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <pplx/pplx.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace web;
using namespace web::http;
using namespace web::http::client;

class Npiregistry {
private:
    std::string api_base = "https://npiregistry.cms.hhs.gov/api";
    http_client_config client_config;
    
    http_request create_request(const std::string& endpoint, const std::string& method, const std::string& data = "") {
        http_request request;
        
        if (method == "GET") {
            request.set_method(methods::GET);
        } else if (method == "POST") {
            request.set_method(methods::POST);
        } else if (method == "PUT") {
            request.set_method(methods::PUT);
        } else if (method == "DEL") {
            request.set_method(methods::DEL);
        }
        
        request.set_request_uri(utility::conversions::to_string_t(endpoint));
        
        // Set headers
        request.headers().add(U("Host"), U("npiregistry.cms.hhs.gov"));
        request.headers().add(U("Content-Type"), U("application/json"));
        request.headers().add(U("User-Agent"), U("Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/115.0"));
        
        if (!data.empty() && (method == "POST" || method == "PUT")) {
            request.set_body(data);
        }
        
        return request;
    }
    
    std::string build_query_params(const std::map<std::string, std::string>& params) {
        if (params.empty()) return "";
        
        std::string query = "?";
        bool first = true;
        for (const auto& param : params) {
            if (!param.second.empty()) {
                if (!first) query += "&";
                // Кодируем значения параметров
                auto encoded_value = web::uri::encode_data_string(utility::conversions::to_string_t(param.second));
                query += param.first + "=" + utility::conversions::to_utf8string(encoded_value);
                first = false;
            }
        }
        return query;
    }
    
    pplx::task<json::value> make_api_call(const std::string& endpoint, const std::string& method, const std::string& data = "") {
        http_client client(utility::conversions::to_string_t(api_base), client_config);
        auto request = create_request(endpoint, method, data);

        return client.request(request)
            .then([](http_response response) {
                if (response.status_code() == status_codes::OK) {
                    return response.extract_json();
                } else {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("HTTP Error: ") + utility::conversions::to_string_t(std::to_string(response.status_code())));
                    error_obj[U("success")] = json::value::boolean(false);
                    return pplx::task_from_result(error_obj);
                }
            })
            .then([](pplx::task<json::value> previousTask) {
                try {
                    return previousTask.get();
                } catch (const std::exception& e) {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("Exception: ") + utility::conversions::to_string_t(e.what()));
                    error_obj[U("success")] = json::value::boolean(false);
                    return error_obj;
                }
            });
    }
public:
    Npiregistry(){
        client_config.set_validate_certificates(false);
    }

    pplx::task<json::value> search_npi_records(int limit=0,const std::string& city="",const std::string& state="",const std::string& organization_name="",const std::string& last_name="",const std::string& first_name="",const std::string& taxonomy_description="",const std::string& enumeration_type="",int number=0,int postal_code=0,int skip=0) {
        std::map<std::string, std::string> params;
        params["version"] = "2.1";
        if (limit>0) params["limit"] = std::to_string(limit);
        if (!city.empty()) params["city"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(city)));
        if (!state.empty()) params["state"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(state)));
        if (!organization_name.empty()) params["organization_name"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(organization_name)));
        if (!last_name.empty()) params["last_name"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(last_name)));
        if (!first_name.empty()) params["first_name"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(first_name)));
        if (!taxonomy_description.empty()) params["taxonomy_description"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(taxonomy_description)));
        if (!enumeration_type.empty()) params["enumeration_type"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(enumeration_type)));
        if (number>0) params["number"] = std::to_string(number);
        if (postal_code>0) params["postal_code"] = std::to_string(postal_code);
        if (skip>0) params["skip"] = std::to_string(skip);
        return make_api_call("/" + build_query_params(params),"GET");
    }
};

#endif
