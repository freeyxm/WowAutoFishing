#include "StringUtil.hpp"

namespace comm_util
{

StringUtil::StringUtil()
{
}


StringUtil::~StringUtil()
{
}

float StringUtil::atof(const char *str)
{
	return (float)::atof(str);
}

std::list<std::string> StringUtil::Split(const char *str, const char *delimiter)
{
	std::list<string> strs;
	size_t size = strlen(str);
	std::shared_ptr<char> buf(new char[size + 1], std::default_delete<char[]>());
	assert(buf.get());

	buf.get()[0] = '\0';
	memcpy_s(buf.get(), size + 1, str, size);

	char *ptr = nullptr, *next = buf.get();
	while ((ptr = strtok_s(next, delimiter, &next)) != nullptr)
	{
		strs.push_back(string(ptr));
	}

	return strs;
}

} // namespace comm_util