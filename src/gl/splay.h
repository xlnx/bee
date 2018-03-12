#pragma once

#include <vector>
#include <utility>

template <typename T>
class Splay
{
	struct Node
	{
		template <typename ...Types, typename = typename
			::std::enable_if<::std::is_constructible<T, Types...>::value>::type>
		Node(Types &&...args):
			elem(::std::forward<Types>(args)...)
		{
		}
		T elem;
		Node *left = nullptr;
		Node *right = nullptr;
	} *root = nullptr;
public:
	Splay()
	{
	}
	Splay(::std::vector<T> &s)
	{
		for (auto &e: s)
		{
			insert(e);
		}
	}
	
	void erase(const typename T::KeyType &e)
	{
		if (auto &p = findPos(e))
		{
			auto q = p;
			auto left = q->left, right = q->right;
			if (left == nullptr)
			{
				p = right;
			}
			else if (right == nullptr)
			{
				p = left;
			}
			else
			{
				auto *pmax = &left;
				while ((*pmax)->right != nullptr)
				{
					pmax = &(*pmax)->right;
				}
				p = *pmax;
				p->left = left;
				p->right = right;
				*pmax = (*pmax)->left;
			}
			delete q;
		}
	}
protected:
	T *find(const typename T::KeyType &e)
	{
		::std::vector<Node**> currs;
		auto &curr = findPos(e, currs);
		auto result = curr;
		if (curr)
		{
			splay(currs);
			return &result->elem;
		}
		return nullptr;
	}
	void insert(const T &e)
	{
		::std::vector<Node**> currs;
		auto &curr = findPos(e.key, currs);
		if (!curr)
		{
			curr = new Node(e);
		}
		splay(currs);
	}
	void splay(::std::vector<Node**> &currs)
	{
		auto iter = currs.rbegin();
		for (; iter < currs.rend() - 2; iter += 2)
		{
			auto r = **(iter + 2), m = **(iter + 1), c = **iter;
			auto &root = **(iter + 2);
			if (r->left == m)
			{
				if (m->left == c)  // LL
				{
					r->left = m->right;
					m->left = c->right;
					c->right = m;
					m->right = r;
					root = c;
				}
				else  // LR
				{
					m->right = c->left;
					r->left = c->right;
					c->left = m;
					c->right = r;
					root = c;
				}
			}
			else
			{
				if (m->left == c)  // RL
				{
					r->right = c->left;
					m->left = c->right;
					c->left = r;
					c->right = m;
					root = c;
				}
				else // RR
				{
					r->right = m->left;
					m->right = c->left;
					m->left = r;
					c->left = m;
					root = c;
				}
			}
			// print();
		}
		if (iter == currs.rend() - 2)
		{
			// std::cout << (**iter)->elem << std::endl;
			auto r = **(iter + 1), c = **iter;
			// std::cout << r->elem << " " << c->elem << std::endl;
			if (r->left == c)
			{
				r->left = c->right;
				c->right = r;
			}
			else
			{
				r->right = c->left;
				c->left = r;
			}
			root = c;
		}
	}
	Node *&findPos(const typename T::KeyType &e)
	{
		auto *curr = &root;
		while (*curr)
		{
			if ((*curr)->elem < e) // go right subtree
			{
				curr = &(*curr)->right;
			}
			else if (!((*curr)->elem == e))
			{
				curr = &(*curr)->left;
			}
			else
			{
				break;
			}
		}
		return *curr;
	}
	Node *&findPos(const typename T::KeyType &e, ::std::vector<Node**> &currs)
	{
		auto *curr = &this->root;
		while (*curr)
		{
			currs.push_back(curr);
			if ((*curr)->elem < e) // go right subtree
			{
				curr = &(*curr)->right;
			}
			else if (!((*curr)->elem == e))
			{
				curr = &(*curr)->left;
			}
			else
			{
				currs.pop_back();
				break;
			}
		}
		currs.push_back(curr);
		return *curr;
	}
};

template <typename T>
struct FloatRangedSetNode
{
	FloatRangedSetNode(float k, const T &e):
		key(k), elem(e)
	{
	}

	using KeyType = float;

	float key;
	T elem;

	bool operator < (const FloatRangedSetNode &other) const
	{
		return key < other.key;
	}
	bool operator == (const FloatRangedSetNode &other) const
	{
		return key == other.key;
	}
};

template <typename T>
class FloatRangedSet final: public Splay<FloatRangedSetNode<T>>
{
public:
	T *getLequal(float x)
	{
		if (auto ptr = find(x))
		{
			return &ptr->elem;
		}
		return nullptr;
	}
	void insert(float k, const T &e)
	{
		Splay<FloatRangedSetNode<T>>::insert(FloatRangedSetNode<T>(k, e));
	}
};