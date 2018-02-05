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

#include <unordered_map>
#include <algorithm>
#include <queue>

namespace huffman {

template<class C, class P = double, class BitSeq = std::vector<bool>>
class huffman {

	huffman(const huffman&);
	huffman& operator=(const huffman&);

public:
	typedef C character_type;
	typedef P probability_type;
	typedef BitSeq bitsequence_type;
	typedef std::vector<character_type> CSEQ;
	typedef bitsequence_type CODE;

	typedef class _alphabet_entry {
	public:
		explicit _alphabet_entry(const probability_type& p) : m_character(0), m_probability(p) {}

		_alphabet_entry(const character_type& c, const probability_type& p)
			: m_character(c), m_probability(p) {}

		const character_type &character() const {
			return m_character;
		}

		const probability_type &probability() const {
			return m_probability;
		}

	private:
		const character_type m_character;
		const probability_type m_probability;
	} ALPHABET_ENTRY;

	typedef std::vector<ALPHABET_ENTRY> ALPHABET;
	typedef typename ALPHABET::value_type value_type;

private:
	typedef class _tree_node {

		_tree_node(const _tree_node&);
		_tree_node& operator=(const _tree_node&);

	public:
		_tree_node() : m_probability(0), m_name(0), m_leaf(true), m_left(0L), m_right(0L) {}
		_tree_node(const probability_type& p, _tree_node *l, _tree_node *r) : m_probability(p),
			m_name(0), m_leaf(false), m_left(l), m_right(r) {}
		_tree_node(const probability_type& p, const character_type &c) : m_probability(p),
			m_name(c), m_leaf(true), m_left(0L), m_right(0L) {}

		const probability_type &probability() const {
			return m_probability;
		}

		const character_type &name() const {
			return m_name;
		}

		bool leaf() const {
			return m_leaf;
		}

		_tree_node * left() const {
			return m_left;
		}

		_tree_node *right() const {
			return m_right;
		}

	private:
		const probability_type m_probability;
		const character_type m_name;
		const bool m_leaf;

		_tree_node * const m_left;
		_tree_node * const m_right;
	} TREE_NODE;

public:
	typedef std::unordered_map<character_type, CODE> DICT;
	typedef _tree_node TREE;

	explicit huffman(const ALPHABET& a) : m_tree(build_tree(a)),
		m_dictionary(build_dictionary(a)) {}

	~huffman() {
		delete_tree(m_tree);
	}

	CODE encode(const typename CSEQ::iterator &b, const typename CSEQ::iterator &e) const {

		CODE code;
		code.reserve(4096u);

		for(auto it(b); it != e; ++it) {

			const auto &r(m_dictionary.find(*it));
			const auto &bits(r->second);

			if(r != std::end(m_dictionary)) {
				for(const auto &bit : bits) code.emplace_back(bit);
			} else break;
		}

		return code;
	}

	CSEQ decode(const CODE &c) const {

		CSEQ n;
		const TREE *p = m_tree;

		for(bool bit : c) {

			p = bit ? p->right() : p->left();

			if(!(p->left() || p->right())) {
				n.emplace_back(p->name());
				p = m_tree;
			}
		}

		n.shrink_to_fit();

		return n;
	}

	DICT dictionary() const {
		return m_dictionary;
	}

	const TREE *tree() const {
		return m_tree;
	}

private:
	void delete_tree(_tree_node *n) {

		if(n->left()) delete_tree(n->left());
		if(n->right()) delete_tree(n->right());

		delete n;
	}

	bool lookup(const character_type& c, CODE &code, _tree_node *n) const {

		if(!n) return false;

		if(!n->leaf() || n->name() != c) {

			if(n->left()) {
				code.emplace_back(false);
				if(lookup(c, code, n->left())) return true;
			}

			if(n->right()) {
				code.emplace_back(true);
				if(lookup(c, code, n->right())) return true;
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
				d.emplace(std::make_pair(c.character(), code));
			}
		}

		return d;
	}

	struct _node_cmp {
		constexpr bool operator()(const TREE_NODE *x, const TREE_NODE *y) const {
			return x->probability() > y->probability();
		}
	};

	TREE *build_tree(const ALPHABET &n) {

		std::priority_queue<TREE_NODE *, std::vector<TREE_NODE *>, _node_cmp> pq;

		for(const ALPHABET_ENTRY &a : n) {
			pq.push(new TREE_NODE(a.probability(), a.character()));
		}

		while(pq.size() > 1u) {

			TREE_NODE *min[2];

			min[0] = pq.top();
			pq.pop();

			min[1] = pq.top();
			pq.pop();

			pq.push(new TREE_NODE(min[0]->probability() + min[1]->probability(), min[0], min[1]));
		}

		return pq.top();
	}

private:
	TREE * const m_tree;
	DICT   m_dictionary;
};

}

#endif /* _HUFFMAN_H */
