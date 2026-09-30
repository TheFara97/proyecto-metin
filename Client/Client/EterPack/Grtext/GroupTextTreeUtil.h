#pragma once
#include "GroupTextTree.h"

#ifdef _WIN64
#include "../../Extern/Graphic/DirectX/Include/d3dx9math.h"
#endif

bool GetGroupProperty(const GroupTextGroup* group, const std::string& name, std::string& value);

template <typename T>
typename boost::enable_if<boost::is_integral<T>, bool>::type GetGroupProperty(const GroupTextGroup* group, std::string name, T& val)
{
	const auto& prop = group->GetProperty(name);
	if (prop.empty())
		return false;

	val = std::stoi(prop);

	return true;
}

template <typename T>
typename boost::enable_if<boost::is_floating_point<T>, bool>::type GetGroupProperty(const GroupTextGroup* group, std::string name, T& val)
{
	const auto& prop = group->GetProperty(name);
	if (prop.empty())
		return false;

	val = std::stof(prop);

	return true;
}

