#pragma once

#include <json/json.h>
#include <string>

namespace buddy::cli {

inline std::string KEY = "";

inline bool IS_STRING = false;

inline std::string OWNER = "";

inline std::string ENTRY_VALUE_STR =
    "{\"type\" : \"none\","
    "\"value\":\"\"}";

inline Json::Value ENTRY_VALUE = Json::nullValue;

inline int BURN_VALUE = 0;

inline Json::Value RESPONSE;

} // namespace buddy::cli
