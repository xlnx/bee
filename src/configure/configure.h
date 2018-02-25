#pragma once

#include "jsoncpp/json.h"
#include "runtime.h"
#include <string>
#include <map>
#include <functional>
#include <vector>

namespace bee
{

namespace exception
{

struct ConfigureError: Fatal
{
	ConfigureError(const ::std::string &info):
		Fatal(info)
	{
	}
};

}

class Configure final
{
public:
	Configure(const ::std::string &path):
		path(path)
	{
		::Json::Reader reader;
		::std::ifstream fin(path);
		if (!reader.parse(fin, root, false))
		{
			able = false;
			root = ::Json::objectValue;
			// BEE_RAISE(ConfigureError, "Failed to read configuration file: " + path);
		}
	}
	~Configure()
	{
	}

	template <typename T>
	void set(const ::std::string &key, T &&value)
	{
		operator [] (key) = ::std::forward<T>(value);
	}
	::Json::Value &operator [] (::std::string key)
	{
		auto *ref = &root;
		auto ptr = &key[0];
		for (auto p = ptr; *p; ++p)
		{
			switch (*p)
			{
			case '.': {
				*p = 0;
				if (ref->isNull())
				{
					*ref = ::Json::objectValue;
				}
				ref = &(*ref)[ptr];
				ptr = p + 1;
			} break;
			case '[': {
				*p = 0;
				auto q = p + 1;
				p = q;
				while (*p != ']' && *p)
				{
					p++;
				}
				if (!*p)
				{
					BEE_RAISE(ConfigureError, "Key error");
				}
				*p = 0;
				if (*ptr)
				{
					if (ref->isNull())
					{
						*ref = ::Json::objectValue;
					}
					ref = &(*ref)[ptr];
					if (ref->isNull())
					{
						*ref = ::Json::arrayValue;
					}
					ref = &(*ref)[::std::stoi(q)];
					ptr = p + 1;
				}
				else
				{
					if (ref->isNull())
					{
						*ref = ::Json::arrayValue;
					}
					ref = &(*ref)[::std::stoi(q)];
					ptr = p + 1;
				}
			} break;
			}
		}
		if (*ptr)
		{
			if (ref->isNull())
			{
				*ref = ::Json::objectValue;
			}
			ref = &(*ref)[ptr];
		}
		return *ref;
	}
	const ::Json::Value &get(const ::std::string &key)
	{
		return operator [] (key);
	}
	void dump()
	{
		dump(path);
		able = true;
	}
	void dump(const ::std::string &file)
	{
		::Json::StyledStreamWriter writer;
		::std::ofstream fout(file);
		if (!fout)
		{
			BEE_RAISE(ConfigureError, "Failed to write file: " + file);
		}
		writer.write(fout, root);
	}
	// operator bool() const
	// {
	// 	return able;
	// }
private:
	::Json::Value root;
	::std::string path;
	bool able = true;
};

}