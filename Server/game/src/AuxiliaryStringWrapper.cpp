#include "stdafx.h"
#include "AuxiliaryStringWrapper.hpp"

#include <boost/algorithm/string.hpp>
#include <typeinfo>

// Static member definition (required for linking)
std::list<std::pair<std::type_index, std::function<bool(const std::string&, CAuxiliaryStringWrapper::SSupportVariants&)>>> CAuxiliaryStringWrapper::l_verificators;

void CAuxiliaryStringWrapper::InitializeVerificators()
{
	if (!l_verificators.empty())
		return;

	l_verificators = {
		{typeid(float), [](const std::string& sStr, SSupportVariants& rVariant) -> bool { try { if (sStr.find(".") == std::string::npos) throw std::bad_cast{}; rVariant = std::stof(sStr); } catch (...) { return false; } return true; }},
		{typeid(int), [](const std::string& sStr, SSupportVariants& rVariant) -> bool { try { rVariant = std::stoi(sStr); } catch (...) { return false; } return true; }},
		{typeid(std::string), [](const std::string& sStr, SSupportVariants& rVariant) -> bool { rVariant = sStr; return true; }},
	};
}

CAuxiliaryStringWrapper::SSupportVariants CAuxiliaryStringWrapper::AutoDetectAndAssign(const std::string & sValue)
{
	SSupportVariants supportVariant{};
	for (const auto& [sKey, lValue] : l_verificators)
	{
		if (lValue(sValue, supportVariant))
			break;
	}

	return supportVariant;
}

void CAuxiliaryStringWrapper::GetValueByType(std::ostringstream& sStream, const SSupportVariants& rVariant)
{
	if (std::holds_alternative<int>(rVariant))
		sStream << std::get<int>(rVariant);
	else if (std::holds_alternative<float>(rVariant))
		sStream << std::defaultfloat << std::get<float>(rVariant);
	else if (std::holds_alternative<std::string>(rVariant))
		sStream << std::get<std::string>(rVariant);
	else
		sys_err("Unsupported value type for variant!");
}

void CAuxiliaryStringWrapper::ParseString(const std::string& sStr)
{
	// Clear old data
	um_results.clear();

	if (sStr.empty())
	{
		// sys_err("Cannot process empty string!");
		return;
	}

	// Decode string
    std::string sDecodedString(CryptoGraphy::DecodeBase64(sStr));

	// Parse results with delimiter&separator
	std::vector<std::string> vResults;
	if (boost::split(vResults, sDecodedString, boost::is_any_of(GetDelimiter())); vResults.size())
	{
		for (const auto& rStr : vResults)
		{
			std::vector<std::string> vPair;
			if (boost::split(vPair, rStr, boost::is_any_of(GetSeparator())); vPair.size() >= 2)
				um_results[vPair[0]] = AutoDetectAndAssign(vPair[1]);
		}
	}
}

std::string CAuxiliaryStringWrapper::GetWrapperString()
{
	std::string sRet;
	for (const auto& [sKey, sValue] : um_results)
	{
		if (!GetValue<std::string>(sKey).has_value())
		{
			sys_err("Empty value for key: %s", sKey.c_str());
			continue;
		}

		sRet += sKey + GetSeparator() + GetValue<std::string>(sKey).value() + GetDelimiter();
	}

	if (sRet.size())
		sRet.pop_back();

	return sRet;
}