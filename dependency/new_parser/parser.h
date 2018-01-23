#pragma once
#include "parser_initializer.h"
#include "lexer.h"
#include <set>
#include <map>
#include <queue>
#include <vector>
#include <stack>
//#include <iostream>

namespace np
{

template <typename lexer_type>
struct parser_exception: public std::logic_error {
	parser_exception():
		std::logic_error("unexpected \'eof\'")
	{}
	parser_exception(const typename lexer_type::value_type& t):
		std::logic_error("unexpected \'" + t.value + "\'")
	{}
};

template <class AstTy, class CharTy = char>
class parser
{
	static const long long empty_elem;
	static const long long stack_bottom;
	using param_list = initializer<parser_init_element<AstTy>>;
	using ast_base = ast<typename AstTy::value_type>;
	enum action_type { a_error = 0, a_accept, a_move_in, a_reduce, a_hold };
	struct action
	{
		action_type flag;
		const parser_rule<AstTy>* rule;
		constexpr action(action_type act = a_error, const parser_rule<AstTy>* r = nullptr):
			flag(act), rule(r) {}
	};
	reflected_lexer<AstTy, CharTy>& lex;
	param_list params;
	std::set<long long> signs;
	std::vector<std::map<long long, action>> ACTION;		// [state][sign]->action->rule_id
	std::vector<std::map<long long, std::size_t>> GOTO;		// [state][sign]->state
	std::map<const parser_rule<AstTy>*, long long> parent_of;
	std::map<const parser_rule<AstTy>*, std::size_t> index_of;
	std::map<long long, parser_init_element<AstTy>*> param_of;
	std::map<long long, std::set<long long>> FIRST, FOLLOW;
	struct item
	{
		const parser_rule<AstTy>& rule;
		std::size_t position;
		bool operator < (const item& other) const
			{ return &rule < &other.rule || &rule == &other.rule && position < other.position; }
	};
	using closure = std::set<item>;
	std::vector<closure> closures;
	auto find_empty(long long value)->const parser_rule<AstTy>*
	{
		auto& param = *param_of[value];
		for (auto& rule: param)
		{
			if (rule.size() == 1 && rule[0].value == empty_elem)
			{
				return &rule;
			}
			else
			{
				bool is_empty = true;
				for (auto& elem: rule)
				{
					if (!(is_empty &= FIRST[elem.value].count(empty_elem)))
						break;
				}
				if (is_empty && rule[0].value != value)
				{
					return find_empty(rule[0].value);
				}
			}
		}
	}
	void register_sub(long long sign, const parser_rule<AstTy>* rule_ptr, std::size_t state)
	{
		if (!ACTION[state][sign].flag)
		{
			ACTION[state][sign] = action(a_hold, rule_ptr);
			GOTO[state][sign] = closures.size();
			if (auto param_ptr = param_of[sign])
			{
				for (auto &rule: *param_ptr)
				{
					for (auto &elem: rule)
					{
						register_sub(elem.value, rule_ptr, state);
						if (!FIRST[elem.value].count(empty_elem)) break;
					}
				}
			}
		}
	}
	void expand_closure(closure I, std::size_t state)
	{
		for (auto &term: closure(I))
		{	
			if (FIRST[term.rule[term.position - 1].value].count(empty_elem))
			{
				if (term.rule.size() != term.position)
				{
					register_sub(term.rule[term.position].value,
						find_empty(term.rule[term.position - 1].value), state);
				}
				else
				{
					for (auto &sign: signs)
					{
						if (!ACTION[state][sign].flag)
						{
							ACTION[state][sign] = action(
								a_hold, find_empty(term.rule[term.position - 1].value)
							);
							GOTO[state][sign] = closures.size();
						}
					}
				}
			}
		}
	}
	void gen_closure(closure I, std::size_t state)
	{
		expand_closure(I, state);
		bool gen_sub = false;
		do {
			gen_sub = false;
			for (auto& term: closure(I))		// prevent iter loss
			{
				if (term.rule.size() != term.position)
				{
					auto pos = term.position;
					do {
						if (auto param_ptr = param_of[term.rule[pos].value])
						{
							for (auto& rule: *param_ptr)
							{
								if (rule.size() != 1 || rule[0].value != empty_elem)
								{
									if (!I.count(item{rule, 0}))
									{
										I.insert(item{rule, 0});
										gen_sub = true;
									}
								}
							}
						}
					} while (FIRST[term.rule[pos].value].count(empty_elem) && 
							++pos != term.position);
				}
			}
		} while (gen_sub);
		closures.push_back(std::move(I));
		GOTO.push_back(std::map<long long, std::size_t>());
		ACTION.push_back(std::map<long long, action>());
	}
public:
	using exception_type = parser_exception<reflected_lexer<AstTy, CharTy>>;
	template <class... Args, typename = typename
			std::enable_if<
				tmp_and<
					std::is_same<
						Args, parser_init_element<AstTy>
					>::value...
				>::value
			>::type
		>
		parser(reflected_lexer<AstTy, CharTy>& engine, const Args&... args):
			lex(engine), params(args...), signs(lex.signs)
		{
			element start = params.back();
			params = params | parser_init_element<AstTy>(
				""_p = start >> make_reflect<AstTy>(
					[](AstTy& ast){return ast[0].gen();}
				)	// tag: ~0LL
			);
			for (auto& elem: lex.signs)
			{
				FIRST[elem].insert(elem);
			}
			FIRST[empty_elem].insert(empty_elem);
			FIRST[~empty_elem].insert(empty_elem);
			bool add_sub = false;
			do {
				add_sub = false;
				for (auto& param: params)
				{
					for (auto& rule: param)
					{
						//std::cout << std::endl;
						bool has_empty = false;
						for (auto& elem: rule)
						{
							/*if (param.value == -66720670770044783LL) {
								std::cout << "one of: " << elem.value << std::endl;
							} */
							//std::cout << elem.value << ": check";
							//if (param.value == -84)
    						//	std::cout << elem.value << std::endl;
							has_empty = false;
							for (auto first_elem: FIRST[elem.value])
							{
								//if (param.value == -84) 
    							//	std::cout << "\n a: " <<first_elem << " \n";
								if (first_elem == empty_elem)
								{
									has_empty = true;
								}
								else if (!FIRST[param.value].count(first_elem))
								{
									FIRST[param.value].insert(first_elem);
									add_sub = true;
								}
							}
							if (!has_empty) break;
						}
						if (has_empty && !FIRST[param.value].count(empty_elem))
						{
							FIRST[param.value].insert(empty_elem);
							add_sub = true;
						}
					}
				}
			} while (add_sub);
			/*for (auto & sign: signs) {
				std::cout << "in " << sign << std::endl;
				for (auto & v: FIRST[sign]) {
					std::cout << v << std::endl;
				}
			}*/
			
			/*for (auto& param: params)
			{
				static std::size_t index = 0;
				for (auto& rule: param)
				{
					parent_of[&rule] = param.value;
					index_of[&rule] = index++;
				}
			}*/
			
			do {
				add_sub = false;
				for (auto& param: params)
				{
					for (auto& rule: param)
					{
						for (auto i = rule.size() - 1; i != 0; --i)
						{
							if (rule[i - 1].value < 0)	// parser element
							{
								if (rule[i].value < 0)	// parser element
								{
									for (auto first_elem: FIRST[rule[i].value])
									{
										if (first_elem != empty_elem &&
											!FOLLOW[rule[i - 1].value].count(first_elem))
										{
											FOLLOW[rule[i - 1].value].insert(first_elem);
											/*if (first_elem == 5676) {
                                            	std::cout << rule[i].value << " -> " << rule[i - 1].value << " 1"<<std::endl;
                                            }*/
                                            add_sub = true;
										}
									}
									if (FIRST[rule[i].value].count(empty_elem))
									{
										for (auto follow_elem: FOLLOW[param.value])
										{
											if (!FOLLOW[rule[i - 1].value].count(follow_elem))
											{
												FOLLOW[rule[i - 1].value].insert(follow_elem);
												/*if (follow_elem == 5676) {
                                            	std::cout << param.value << " -> " << rule[i - 1].value << " 2" << std::endl;
                                            }*/
                                                add_sub = true;
											}
										}
									}
								}
								else if (!FOLLOW[rule[i - 1].value].count(rule[i].value))
								{
									FOLLOW[rule[i - 1].value].insert(rule[i].value);
									/*if (rule[i].value == 5676) {
                                          	std::cout << rule[i].value << " -> " << rule[i - 1].value << " 3" << std::endl;
                                        }*/
                                    add_sub = true;
								}
							}
						}
						if (rule.back().value < 0)
						{
							for (auto follow_elem: FOLLOW[param.value])
							{
								if (!FOLLOW[rule.back().value].count(follow_elem))
								{
									FOLLOW[rule.back().value].insert(follow_elem);
									/*if (follow_elem == 5676) {
                                        	std::cout << param.value << " -> " << rule.back().value << " 4" << std::endl;
                                        }*/
                                    add_sub = true;
								}
							}
						}
					}
				}
			} while (add_sub);
			
			for (auto& param: params)
			{
				static std::size_t index = 0;
				for (auto& rule: param)
				{
					parent_of[&rule] = param.value;
					index_of[&rule] = index++;
				}
				FOLLOW[param.value].insert(stack_bottom);
				param_of[param.value] = &param;
				signs.insert(param.value);
			}
			
			/*for (auto& item: FOLLOW[-3932122194289])
			{
				std::cout << item << std::endl;
            }
*/
			/*for (auto & sign: signs) {
				std::cout << "in " << sign << std::endl;
				for (auto & v: FOLLOW[sign]) {
					std::cout << v << std::endl;
				}
			}*/

			signs.insert(stack_bottom);
			gen_closure({item{params.back()[0], 0}}, 0);
			do {
				add_sub = false;
				for (std::size_t state = 0; state != closures.size(); ++state)
				{
					for (auto& term: closures[state])
					{
						if (term.rule.size() == term.position)
						{
							for (auto elem: FOLLOW[parent_of[&term.rule]])
							{
								if (ACTION[state][elem].flag != a_move_in) {
									ACTION[state][elem] = action(
										&term.rule == &params.back()[0] ? a_accept : a_reduce,
										&term.rule
									);
								}
							}
						}
					}
					for (auto& sign: signs)
					{
						if (!GOTO[state][sign] || ACTION[state][sign].flag == a_hold)
						{
							closure NEW;
							for (auto& term: closures[state])
							{
								if (term.position != term.rule.size() &&
									term.rule[term.position].value == sign)
								{
									NEW.insert(item{term.rule, term.position + 1});
								}
							}
							if (!NEW.empty())
							{
								bool is_sub = false;
								const parser_rule<AstTy>* rule_ptr = nullptr;
								std::size_t dest_state = 0;
								for (; dest_state != closures.size(); ++dest_state)
								{
									is_sub = true;
									for (auto& term: NEW)
									{
										if (!rule_ptr)
											rule_ptr = &term.rule;
										else if (index_of[&term.rule] < index_of[rule_ptr])
											rule_ptr = &term.rule;
										if (!closures[dest_state].count(term))
										{
											is_sub = false;
											break;
										}
									}
									if (is_sub) break;
								}
								if (is_sub)
								{
									GOTO[state][sign] = dest_state;
									ACTION[state][sign] = a_move_in;
									expand_closure(closures[dest_state], state);
								}
								else
								{
									GOTO[state][sign] = closures.size();
									gen_closure(NEW, state);
									add_sub = true;
									switch (ACTION[state][sign].flag)
									{
									case a_accept: case a_move_in: throw std::bad_cast();
									case a_error: ACTION[state][sign] = a_move_in; break;
									case a_reduce: 
                                        //std::cout << index_of[rule_ptr] << " " << index_of[ACTION[state][sign].rule] << std::endl;
                                    if (index_of[rule_ptr] > index_of[ACTION[state][sign].rule]) {
										ACTION[state][sign] = a_move_in;
										ACTION[state][sign].rule = rule_ptr;
										//std::cout << state << " " << sign << std::endl;
									}
									}
								}
							}
						}
					}
					/*if (state == 38) {
						std::cout << ">>> ";
						for (auto &v: FOLLOW[-918011105800045699LL]) {
							std::cout << v << " ";
						}
						std::cout << "]" << std::endl;
						for (auto &sign: signs) {
							std::cout << sign << ": " << ACTION[state][sign].flag << std::endl;
						}
						std::cout << " ============== \n";
					}*/
				}
			} while (add_sub);
            /*for (std::size_t i = 0; i != closures.size(); ++i) {
				std::cout << i <<std::endl;
				for (auto & t: closures[i]) {
					std::cout << parent_of[&t.rule] << " " << t.position << std::endl;
				}
			}*/
		}
	virtual ~parser() = default;
public:
	void parse(const CharTy* buffer)
	{
		std::queue<typename reflected_lexer<AstTy, CharTy>::value_type> tokens;
		std::stack<std::pair<std::shared_ptr<AstTy>, long long>> new_asts;
		lex <= buffer;
		while (!lex.empty()) {
			tokens.push(lex.next());
		}
		//std::cout << tokens.front().value;
		std::stack<std::size_t> states;
		std::vector<std::shared_ptr<AstTy>> ast_stack;
		std::vector<typename reflected_lexer<AstTy, CharTy>::value_type> term_stack;
		states.push(0);
		auto merge = [&](const parser_rule<AstTy>* rule)
		{
			std::shared_ptr<ast_base> this_ast(std::make_shared<AstTy>(ast_base(rule->ast_data)));
			//if (rule->size() > 1 || (*rule)[0].value != empty_elem)
			//{
			std::size_t ast_size = 0, term_size = 0;
			for (auto& dummy: *rule)
			{
				if (dummy.value <= 0) ast_size++;
					else term_size++;
			}
			for (auto itr = ast_stack.end() - ast_size; itr != ast_stack.end(); ++itr)
			{
				this_ast->sub_ast.push_back(*itr);
			}
			for (auto itr = term_stack.end() - term_size; itr != term_stack.end(); ++itr)
			{
				this_ast->sub_terms.push_back(lex.handlers[itr->id](itr->value));
			}
			for (auto& dummy: *rule)
			{
				states.pop(); if (dummy.value <= 0) ast_stack.pop_back();
					else term_stack.pop_back();
			}
			//}		// sub_rules
			rule->ast_data.on_parse(*this_ast);
			new_asts.push(make_pair(std::move(this_ast), parent_of[rule]));
		};
		auto match = [&](long long sign)->bool
		{
			//std::cout << states.top() << " " << sign << std::endl;
			switch (ACTION[states.top()][sign].flag)
			{
			case a_move_in: {
				//std::cout << "movein" << std::endl;
				states.push(GOTO[states.top()][sign]);
				if (sign >= 0)
					term_stack.push_back(tokens.front()), tokens.pop();
				else
					ast_stack.push_back(new_asts.top().first), new_asts.pop();
			} break;
			case a_hold: {
				//std::cout << "hold" << std::endl;
				std::size_t state = states.top();
				states.push(0);
				ast_stack.push_back(std::make_shared<AstTy>(
					ast_base(ACTION[state][sign].rule->ast_data)));
				merge(ACTION[state][sign].rule);
			} break;
			case a_accept: {
				if (ast_stack.size() == 1 && tokens.empty() && new_asts.empty()
						&& term_stack.empty()) return true;
			}
			case a_error: {
				if (tokens.empty())
					throw exception_type();//std::bad_cast();
				else
					throw exception_type(tokens.front());
			}
			case a_reduce: {
				//std::cout << "reduce" << std::endl;
				merge(ACTION[states.top()][sign].rule);
			}
			}
			return false;
		};
		do {
			if (new_asts.empty()) {
				if (match(tokens.empty() ? stack_bottom : tokens.front().id)) break;
			} else {
				if (match(new_asts.top().second)) break;
			}
		} while (1);
		ast_stack.front()->gen();
	}
};

template <class AstTy, class CharTy>
const long long parser<AstTy, CharTy>::empty_elem = 0LL;
template <class AstTy, class CharTy>
const long long parser<AstTy, CharTy>::stack_bottom = 1LL;

}
