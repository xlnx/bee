#pragma once			// lexer_initializer.h
#include <cstring>
#include <string>
#include <cstddef>
#include <regex>
#include <functional>
#include "initializer.h"

namespace np
{

inline constexpr long long str_hash_64(const char *str)
{
	long long seed = 131;
	long long hash = 0;
	while (*str)
		hash = hash * seed + (*str++);
	return hash & ((unsigned long long)(-1LL) >> 1);
}

struct lexer_rule
{
	const char* src_str;
	bool whole_word;
	constexpr lexer_rule(const char s[], bool w = false):
		src_str(s), whole_word(w) {}
};
inline constexpr lexer_rule operator ""_rw (const char s[], std::size_t)
{
	return lexer_rule(s, true);
}
inline constexpr lexer_rule operator ""_r (const char s[], std::size_t)
{
	return lexer_rule(s);
}

template <typename AstTy, typename CharT = char>
using lexer_callback = std::function<
	typename AstTy::value_type(const std::basic_string<CharT>&)>;

template <typename AstTy, typename CharT = char>
struct lexer_reflect
{
	using callback = lexer_callback<AstTy, CharT>;
	callback handler;
	constexpr lexer_reflect(const callback& func):
		handler(func) {}
};

template <typename AstTy, typename CharT = char>
struct reflected_lexer_rule
{
	lexer_rule rule;
	using callback_type = typename lexer_reflect<AstTy, CharT>::callback;
	callback_type handler;
	constexpr reflected_lexer_rule(const lexer_rule& rule, const lexer_reflect<AstTy, CharT>& r):
		rule(rule), handler(r.handler) {}
};

template <typename AstTy, typename CharT>
constexpr reflected_lexer_rule<AstTy, CharT>
	operator >> (const lexer_rule& rule, const lexer_reflect<AstTy, CharT>& r)
{
	return reflected_lexer_rule<AstTy, CharT>(rule, r);
}

struct lexer_init_element;
template <typename AstTy, typename CharT>
struct reflected_lexer_init_element;

struct lexer_element: element
{
	const char* src_str;
	constexpr lexer_element(const char s[]):
		element(str_hash_64(s)), src_str(s) {}
	lexer_init_element operator = (const lexer_rule&) const;
	template <typename AstTy, typename CharT>
	inline reflected_lexer_init_element<AstTy, CharT>
		operator = (const reflected_lexer_rule<AstTy, CharT>&) const;
};
inline constexpr lexer_element operator ""_t (const char s[], std::size_t)
{
	return lexer_element(s);
}

struct lexer_init_element: element
{
	std::regex mode;
	lexer_init_element(const lexer_element& lex):
		element(lex), mode(std::regex([](const char* p){
			static const auto escape_lst = "*.?+-$^[](){}|\\/";
			std::string res;
			while (*p)
			{
				if (strchr(escape_lst, *p)) res += "\\";
				res += *p++;
			}
			return res;
		}(lex.src_str),
			std::regex::nosubs | std::regex::optimize)) {}
	lexer_init_element(const lexer_element& lex, const lexer_rule& rule):
		element(lex), mode(std::regex(*rule.src_str ?
				std::string(rule.src_str) + (rule.whole_word ? "\\b" : "") : "$^",
			std::regex::nosubs | std::regex::optimize)) {}
};
inline lexer_init_element 
	lexer_element::operator = (const lexer_rule& rule) const
{
	return lexer_init_element(*this, rule);
}

template <typename AstTy, typename CharT>
struct reflected_lexer_init_element
{
	lexer_init_element elem;
	using callback_type = typename lexer_reflect<AstTy, CharT>::callback;
	callback_type handler;
	reflected_lexer_init_element(
			const lexer_element& e, 
			const reflected_lexer_rule<AstTy, CharT>& reflect
		):
		elem(e, reflect.rule), handler(reflect.handler) {}
	reflected_lexer_init_element(const lexer_init_element& e):
		elem(e), handler([](const std::basic_string<CharT>&){
				return typename AstTy::value_type(); }) {}
	reflected_lexer_init_element(
			const lexer_element& e,
			const lexer_reflect<AstTy, CharT>& r
		):
		elem(e), handler(r.handler) {}
};
template <typename AstTy, typename CharT>
inline reflected_lexer_init_element<AstTy, CharT>
	lexer_element::operator = (const reflected_lexer_rule<AstTy, CharT>& reflect) const
{ return reflected_lexer_init_element<AstTy, CharT>(*this, reflect); }
template <typename AstTy, typename CharT>
inline reflected_lexer_init_element<AstTy, CharT>
	operator >> (const lexer_element& e, const lexer_reflect<AstTy, CharT>& fn)
{ return reflected_lexer_init_element<AstTy, CharT>(e, fn.handler); }

using lexer_initializer = initializer<lexer_init_element>;

}
