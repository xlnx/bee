#pragma once			// lexer.h
#include "lexer_initializer.h"
#include <set>
#include <map>

namespace np
{

template <typename CharT = char>
class lexer
{
	lexer_initializer rules;
	using iterator = const CharT*;
	using string_type = std::basic_string<CharT>;
	string_type str;
	iterator iter;
	static const string_type spaces;
public:
	struct token;
	using value_type = token;
	using position_type = std::pair<unsigned, unsigned>;
	struct token
	{
		long long id;
		string_type value;
	};
	std::set<long long> signs;
private:
	lexer(const lexer_initializer& list):
		rules(list), iter(&str[0])
	{ for (auto& elem: list){
		signs.insert(elem.value);
	} }
public:
	template <class... T, typename = typename
			std::enable_if<
				tmp_and<
					std::is_constructible<
						lexer_init_element, T
					>::value...
				>::value
			>::type
		>
		lexer(const T&... args):
			lexer(lexer_initializer(args...)) {}
	virtual ~lexer() = default;
public:
	void reset() { str = ""; iter = &str[0]; }
	bool empty() const { return !*iter; }
	lexer& operator << (string_type&& src)
	{
		auto diff = iter - &str[0];
		str += std::forward<string_type>(src) + "\n";
		iter = &str[0] + diff;
		return *this;
	}
	void operator <= (string_type&& src)
	{
		str = std::forward<string_type>(src);
		iter = &str[0];
		while (*iter && spaces.find_first_of(*iter) != string_type::npos)
				++iter;
	}
	value_type next()
	{
		if (*iter)
		{
			std::match_results<const CharT*> result;
			for (const auto& rule: rules)
			{
				if (std::regex_search(iter, result, rule.mode, std::regex_constants::match_continuous))
				{
					while (iter != result.suffix().first)
							++iter;
					while (*iter && spaces.find_first_of(*iter) != string_type::npos)
							++iter;
					string_type res = result[0];
					return {rule.value, res};
				}
			}
			iter = &str[str.length()];
			throw std::bad_cast();
		}
	}
};
template <typename CharT>
const typename lexer<CharT>::string_type lexer<CharT>::spaces = " \t\r\n";

template <typename AstTy, typename CharT = char>
class reflected_lexer: public lexer<CharT>
{
	using reflected_lexer_initializer = initializer<
		reflected_lexer_init_element<AstTy, CharT>>;
	reflected_lexer_initializer list;
public:
	std::map<long long, lexer_callback<AstTy, CharT>> handlers;
	template <class... T, typename = typename
			std::enable_if<
				tmp_and<
					std::is_constructible<
						reflected_lexer_init_element<AstTy, CharT>, T
					>::value...
				>::value
			>::type
		>
		reflected_lexer(const T&... args):
			lexer<CharT>(reflected_lexer_init_element<AstTy, CharT>(args).elem...),
			list({reflected_lexer_init_element<AstTy, CharT>(args)...})
		{ for (auto& reflect: list){
			handlers[reflect.elem.value] = reflect.handler;
		}}
};

}
