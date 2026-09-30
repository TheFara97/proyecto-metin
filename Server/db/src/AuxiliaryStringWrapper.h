#pragma once
#include <typeindex>
#include <variant>
#include <optional>
#include <sstream>
#include <iomanip>
#include <unordered_map>

#include <boost/lexical_cast.hpp>
#include "../../libthecore/include/crypto.h"

class CAuxiliaryStringWrapper
{
	public:
		CAuxiliaryStringWrapper() { InitializeVerificators(); };
		virtual ~CAuxiliaryStringWrapper() {};

	public:
		using SSupportVariants = std::variant<int, float, std::string>;
		virtual void ParseString(const std::string& sStr);
		template <class T>
		std::optional<T> GetValue(const std::string& sKey);
		template <class T>
		bool SetValue(const std::string& sKey, const T& rValue);
		void DeleteValue(const std::string& sKey) { um_results.erase(sKey); }
		operator std::string()
		{
			return EncodeBase64(GetWrapperString());
		}

	private:
		SSupportVariants AutoDetectAndAssign(const std::string& sValue);
		void GetValueByType(std::ostringstream& sStream, const SSupportVariants & rVariant);
		std::string GetWrapperString();
		virtual std::string GetSeparator() { return ";"; }
		virtual std::string GetDelimiter() { return "|"; }
		virtual void InitializeVerificators();

	private:
		std::unordered_map<std::string, SSupportVariants> um_results;
		std::list<std::pair<std::type_index, std::function<bool(const std::string&, SSupportVariants & rVariant)>>> l_verificators;
};

template <class T>
inline std::optional<T> CAuxiliaryStringWrapper::GetValue(const std::string& sKey)
{
	auto fIt = um_results.find(sKey);
	if (fIt == um_results.end())
		return {};

	try
	{
		return std::get<T>(fIt->second);
	}
	catch (...)
	{
		// Try lexical cast before giving up
		try
		{
			return boost::lexical_cast<T>(std::get<0>(fIt->second));
		}
		catch (...)
		{
			printf("Some error occured when trying to parse following key: %s\n", fIt->first.c_str());
		}
	}

	return {};
}

template<>
inline std::optional<std::string> CAuxiliaryStringWrapper::GetValue<std::string>(const std::string& sKey)
{
	auto fIt = um_results.find(sKey);
	if (fIt == um_results.end())
		return {};

	try
	{
		return std::get<std::string>(fIt->second);
	}
	catch (...)
	{
		// Treat string differently
		// Use stringstream to handle float numbers
		try
		{
			std::ostringstream ss;
			GetValueByType(ss, fIt->second);
			return ss.str();
		}
		catch (...)
		{
			printf("Some error occured when trying to parse following key: %s\n", fIt->first.c_str());
		}
	}

	return {};
}

template<typename T, typename VARIANT_T>
struct isVariantMember;

template<typename T, typename... ALL_T>
struct isVariantMember<T, std::variant<ALL_T...>> : std::bool_constant<(std::is_same_v<T, ALL_T> || ...)>
{};

template <class T>
inline bool CAuxiliaryStringWrapper::SetValue(const std::string& sKey, const T& rValue)
{
	auto fIt = um_results.find(sKey);
	if (fIt == um_results.end())
		// If it doesn't exist, create a new record
		fIt = um_results.emplace(sKey, SSupportVariants{}).first;

	if (!isVariantMember<T, SSupportVariants>::value)
	{
		printf("Following type is not supported by this variant: %s\n", typeid(T).name());
		return false;
	}

	fIt->second = rValue;
	return true;
}

