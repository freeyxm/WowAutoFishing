#pragma once
#include <cstring>
#include <string>
#include <vector>
#include <list>
#include <memory>
#include <assert.h>

namespace comm_util
{

using namespace std;

class StringUtil
{
public:
	StringUtil();
	~StringUtil();

	static float atof(const char *str);

	static std::list<std::string> split(const char *str, const char *delimiter);

	template<typename T>
	static std::vector<T> parseValues(const char *str, const char *delimiter, T(*convert)(const char*));
};

template<typename T>
static std::vector<T> StringUtil::parseValues(const char *str, const char *delimiter, T(*convert)(const char*))
{
	std::vector<T> data;
	size_t size = strlen(str);
	std::shared_ptr<char> buf(new char[size + 1], std::default_delete<char[]>());
	assert(buf.get());

	buf.get()[0] = '\0';
	memcpy_s(buf.get(), size + 1, str, size);

	char *ptr = nullptr, *next = buf.get();
	while ((ptr = strtok_s(next, delimiter, &next)) != nullptr)
	{
		data.push_back((*convert)(ptr));
	}

	return data;
}

} // namespace comm_util