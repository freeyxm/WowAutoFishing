#pragma once
#include <cstring>
#include <vector>

class StringUtil
{
public:
	StringUtil();
	~StringUtil();

	static float atof(const char *str);

	template<typename T>
	static std::vector<T> ParseNumbers(const char *str, const char *delimiter, T(*convert)(const char*));
};

template<typename T>
static std::vector<T> StringUtil::ParseNumbers(const char *str, const char *delimiter, T(*convert)(const char*))
{
	std::vector<T> data;
	char buf[10240] = { 0 };
	memcpy_s(buf, sizeof(buf), str, strlen(str));

	char *ptr = nullptr, *next = buf;
	while ((ptr = strtok_s(next, delimiter, &next)) != nullptr)
	{
		data.push_back((*convert)(ptr));
	}

	return data;
}
