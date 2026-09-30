#pragma once

extern std::string EncodeBase32(const std::string& sRaw, bool bPadding = false);
extern std::string EncodeBase64(const std::string& sRaw);
extern std::string DecodeBase64(const std::string& sRaw);