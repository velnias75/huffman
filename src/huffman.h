/*
 * Copyright 2018 by Heiko Schäfer <heiko@rangun.de>
 *
 * This file is part of huffman.
 *
 * huffman is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * huffman is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with huffman.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _HUFFMAN_H
#define _HUFFMAN_H

#include <algorithm>
#include <queue>
#include <map>

namespace huffman {

template<class C, class P = double>
class huffman {

	huffman(const huffman&);
	huffman& operator=(const huffman&);

public:
	typedef C character_type;
	typedef P probability_type;
	typedef std::vector<character_type> CSEQ;
	typedef std::vector<bool> CODE;

	typedef class _alphabet_entry {
	public:
		explicit _alphabet_entry(const probability_type& p) : m_character(0), m_probability(p) {}

		_alphabet_entry(const character_type& c, const probability_type& p)
			: m_character(c), m_probability(p) {}

		character_type character() const {
			return m_character;
		}

		probability_type probability() const {
			return m_probability;
		}

	private:
		character_type m_character;
		probability_type m_probability;
	} ALPHABET_ENTRY;

	typedef std::vector<ALPHABET_ENTRY> ALPHABET;
	typedef typename ALPHABET::value_type value_type;

private:
	typedef class _tree_node {

		_tree_node(const _tree_node&);
		_tree_node& operator=(const _tree_node&);

	public:
		_tree_node() : probability(0), name(0), leaf(true), left(0L), right(0L) {}

		probability_type probability;
		character_type name;
		bool leaf;

		_tree_node *left;
		_tree_node *right;
	} TREE_NODE;

	typedef std::map<character_type, CODE> DICT;

public:
	typedef _tree_node TREE;

	explicit huffman(const ALPHABET& a) : m_tree(build_tree(a)),
		m_dictionary(build_dictionary(a)) {}

	~huffman() {
		delete_tree(m_tree);
	}

	CODE encode(const CSEQ &n) const {

		CODE code;
		code.reserve(4096u);

		for(const typename CSEQ::value_type &i : n) {

			const auto &r(m_dictionary.find(i));

			if(r != std::end(m_dictionary)) {
				code.insert(std::end(code), std::begin(r->second), std::end(r->second));
			} else break;
		}

		return code;
	}

	CSEQ decode(const CODE &c) const {

		CSEQ n;
		const TREE *p = m_tree;

		for(bool bit : c) {

			p = bit ? p->right : p->left;

			if(!(p->left || p->right)) {
				n.push_back(p->name);
				p = m_tree;
			}
		}

		n.shrink_to_fit();

		return n;
	}

private:
	void delete_tree(_tree_node *n) {

		if(n->left) delete_tree(n->left);
		if(n->right) delete_tree(n->right);

		delete n;
	}

	bool lookup(const character_type& c, CODE &code, _tree_node *n) const {

		if(!n) return false;

		if(n->name != c || !n->leaf) {

			if(n->left) {
				code.push_back(false);
				if(lookup(c, code, n->left)) return true;
			}

			if(n->right) {
				code.push_back(true);
				if(lookup(c, code, n->right)) return true;
			}

			code.pop_back();

		} else return true;

		return false;
	}

	DICT build_dictionary(const ALPHABET& a) {

		DICT d;

		for(const ALPHABET_ENTRY &c : a) {

			CODE code;

			if(lookup(c.character(), code, m_tree)) {
				d.insert(std::make_pair(c.character(), code));
			}
		}

		return d;
	}

	struct _node_cmp {
		constexpr bool operator()(const TREE_NODE *x, const TREE_NODE *y) const {
			return x->probability > y->probability;
		}
	};

	TREE *build_tree(const ALPHABET &n) {

		std::priority_queue<TREE_NODE *, std::vector<TREE_NODE *>, _node_cmp> pq;

		for(const ALPHABET_ENTRY &a : n) {
			TREE_NODE *tn = new TREE_NODE();
			tn->probability = a.probability();
			tn->name = a.character();
			pq.push(tn);
		}

		while(pq.size() > 1u) {

			TREE_NODE *min1(pq.top());
			pq.pop();
			TREE_NODE *min2(pq.top());
			pq.pop();

			TREE_NODE *tn = new TREE_NODE();
			tn->probability = min1->probability + min2->probability;
			tn->left  = min1;
			tn->right = min2;

			pq.push(tn);
		}

		return pq.top();
	}

private:
	TREE * const m_tree;
	DICT   m_dictionary;
};

}

#endif /* _HUFFMAN_H */
