#pragma once			// parser_initializer.h
#include "lexer_initializer.h"
#include <functional>
#include <memory>

namespace np
{

template <typename RetTy>
struct ast;

template <typename AstTy, typename = typename
	std::enable_if<std::is_class<AstTy>::value>::type>
struct parser_ast_data;

template <template <typename> class AstTy, typename RetTy>
struct parser_ast_data<AstTy<RetTy>>
{
	using ast_type = AstTy<RetTy>;
	typename ast_type::on_exec_reflection on_exec;
	typename ast_type::on_parse_reflection on_parse;
public:
	constexpr parser_ast_data(
			std::function<RetTy(ast_type&)> rexec =
				[](ast_type&)->RetTy{}, 
			std::function<void(ast_type&)> rparse =
				[](ast_type&){}):
		on_exec(rexec),
		on_parse(rparse) {}
};
template <typename T>
using make_reflect = parser_ast_data<T>;

template <typename AstTy, typename CharTy>
class parser;

template <typename RetTy>
struct ast
{
	using value_type = RetTy;
	using on_exec_reflection = std::function<RetTy(ast<RetTy>&)>;
	using on_parse_reflection = std::function<void(ast<RetTy>&)>;
private:
	template <typename AstTy, typename CharTy>
	friend class parser;
	std::vector<std::shared_ptr<ast<RetTy>>> sub_ast;
	std::vector<RetTy> sub_terms;
	const parser_ast_data<ast>& ast_data;
public:
	class iterator {
		using iter_ty = typename std::vector<std::shared_ptr<ast<RetTy>>>::iterator;
		iter_ty iter;
	public:
		iterator(const iter_ty& it): iter(it) {}
		iterator& operator ++ ()
			{ ++iter; return *this; }
		iterator operator ++ (int)
			{ ++iter; return iterator(iter - 1); }
		iterator& operator -- ()
			{ --iter; return *this; }
		iterator operator -- (int)
			{ --iter; return iterator(iter + 1); }
		ast<RetTy>& operator * ()
			{ return **iter; }
		bool operator != (const iterator& it) const
			{ return iter != it.iter; }
		bool operator == (const iterator& it) const
			{ return iter == it.iter; }
	};
	ast(const parser_ast_data<ast>& ast_node):
		ast_data(ast_node) {}
	ast<RetTy>& sub(std::size_t i)
		{ return *sub_ast[i]; }
	ast<RetTy>& operator [] (std::size_t i)
		{ return *sub_ast[i]; }
	iterator begin()
		{ return iterator(sub_ast.begin()); }
	iterator end()
		{ return iterator(sub_ast.end()); }
	std::size_t size() const
		{ return sub_ast.size(); }
	std::size_t term_size() const
		{ return sub_terms.size(); }
	RetTy& term(std::size_t i)
		{ return sub_terms[i]; }
	RetTy gen()
		{ return ast_data.on_exec(*this); }
};


template <typename AstTy>
struct parser_rule
{
	element_list params;
	parser_ast_data<AstTy> ast_data;
	using iterator = typename element_list::const_iterator;
	const element& operator [](std::size_t idx) const
		{ return params[idx]; }
	std::size_t size() const 
		{ return params.size(); }
	const element& back() const
		{ return params.back(); }
	iterator begin() const
		{ return params.begin(); }
	iterator end() const
		{ return params.end(); }
public:
	constexpr parser_rule(const element& elem):
		// element(~element_count++),
		params({elem}) {}
	constexpr parser_rule(
			const element_list& rules,
			const parser_ast_data<AstTy>& data =
				parser_ast_data<AstTy>()):
		// element(~element_count++),
		params(rules),
		ast_data(data) {}
};
// private:
	// static unsigned element_count;
// template <typename AstTy>
// unsigned parser_element<AstTy>::element_count = 0;

template <typename T, typename AstTy, typename = typename
		std::enable_if<
			std::is_constructible<
				element_list, const T&
			>::value
		>::type
	>
constexpr inline parser_rule<AstTy> operator >> (const T& list, const parser_ast_data<AstTy>& f)
{
	return parser_rule<AstTy>(element_list(list), f);
}

template <class AstTy>
using parser_initializer = initializer<parser_rule<AstTy>>;

template <class AstTy>
class parser_init_element;

struct parser_element: element
{
	constexpr parser_element(const char s[]):
		element(~str_hash_64(s)) {}
	template <class AstTy>
		constexpr parser_init_element<AstTy>
			operator = (const parser_initializer<AstTy>& list) const;
	template <class AstTy>
		constexpr parser_init_element<AstTy>
			operator = (const parser_rule<AstTy>& list) const;
};

template <class AstTy>
class parser_init_element: public element
{
	static long long element_count;
public:
	parser_initializer<AstTy> rules;
	using iterator = typename parser_initializer<AstTy>::const_iterator;
	const parser_rule<AstTy>& operator [](std::size_t idx) const
		{ return rules[idx]; }
	std::size_t size() const 
		{ return rules.size(); }
	const parser_rule<AstTy>& back() const
		{ return rules.back(); }
	iterator begin() const
		{ return rules.begin(); }
	iterator end() const
		{ return rules.end(); }
public:
	constexpr parser_init_element(const parser_element& elem, const parser_rule<AstTy>& rule):
		element(elem), 
		rules(parser_initializer<AstTy>(rule)) {}
	template <class T, typename = typename
			std::enable_if<
				!std::is_same<
					parser_rule<AstTy>, T
				>::value &&
				std::is_constructible<
					parser_rule<AstTy>, const T&
				>::value
			>::type
		>
		constexpr parser_init_element(const parser_element& elem, const T& arg):
			element(elem),
			rules(parser_initializer<AstTy>(
					parser_rule<AstTy>(arg)
				)) {}
	constexpr parser_init_element(const parser_element& elem, const parser_initializer<AstTy>& list):
		element(elem), rules(list) {}
};

template <class AstTy>
	constexpr parser_init_element<AstTy>
		parser_element::operator = (const parser_initializer<AstTy>& list) const
	{
		return parser_init_element<AstTy>(*this, list);
	}
template <class AstTy>
	constexpr parser_init_element<AstTy>
		parser_element::operator = (const parser_rule<AstTy>& list) const
	{
		return parser_init_element<AstTy>(*this, parser_initializer<AstTy>(list));
	}
constexpr parser_element operator ""_p (const char s[], std::size_t)
{
	return parser_element(s);//asdasdasdasdasdasdsad
}

}
