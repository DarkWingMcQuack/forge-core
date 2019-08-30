#pragma once

#include "resource_path.hpp"
#include <fstream>
#include <iterator>
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
#include <json/writer.h>


inline auto readFile(const std::string& path)
    -> std::string
{
    std::ifstream t{resource_path + "/" + path};
    std::string str{std::istreambuf_iterator<char>(t),
                    std::istreambuf_iterator<char>()};

    return str;
}

inline auto parseString(const std::string& str)
    -> Json::Value
{
    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();

    Json::Value json;
    std::string errors;

    reader->parse(
        str.c_str(),
        str.c_str() + str.size(),
        &json,
        &errors);
    delete reader;

    return json;
}
