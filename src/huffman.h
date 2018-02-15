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

template<class CharType, class PropType = double, class BitSeq = std::vector<bool>>
class huffman {

	huffman& operator=(const huffman&);

public:
	typedef CharType character_type;
	typedef PropType probability_type;
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
		_tree_node() : m_probability(0), m_name(0), m_leaf(true), m_height(0u),
			m_left(0L), m_right(0L) {}
		_tree_node(const probability_type& p, _tree_node *l, _tree_node *r, std::size_t h = 0u)
			: m_probability(p), m_name(0), m_leaf(false), m_height(h), m_left(l), m_right(r) {}
		_tree_node(const probability_type& p, const character_type &c) : m_probability(p),
			m_name(c), m_leaf(true), m_height(0u), m_left(0L), m_right(0L) {}

		explicit _tree_node(const _tree_node *o) : m_probability(o->m_probability),
			m_name(o->m_name), m_leaf(o->m_leaf), m_height(o->m_height),
			m_left(o->m_left ? new _tree_node(o->m_right) : 0L),
			m_right(o->m_right ? new _tree_node(o->m_right) : 0L) {}

		const probability_type &probability() const {
			return m_probability;
		}

		const character_type &name() const {
			return m_name;
		}

		bool leaf() const {
			return m_leaf;
		}

		std::size_t height() const {
			return m_height;
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
		const std::size_t m_height;

		_tree_node * const m_left;
		_tree_node * const m_right;
	} TREE_NODE;

	struct _dict_key;

	typedef struct {
		std::size_t operator()(const _dict_key &k) const {
			return (k.length + k.bcode) % static_cast<uint64_t>(8589934591u);
		}
	} DICT_KEY_HASH;

public:
	typedef struct _dict_key {
		uint8_t length;
		uint64_t bcode;

		friend bool operator==(const _dict_key &a, const _dict_key &b) {
			return a.length == b.length && a.bcode == b.bcode;
		}

	} DICT_KEY;

	typedef std::unordered_map<DICT_KEY, character_type, DICT_KEY_HASH> DICT;
	typedef _tree_node TREE;

	huffman(const huffman &o) : m_tree(new TREE(o.m_tree)), m_dictionary(o.m_dictionary) {}

	explicit huffman(const DICT &d) : m_tree(0l), m_dictionary(d) {}

	explicit huffman(const ALPHABET &a) : m_tree(build_tree(a)),
		m_dictionary(build_dictionary(a)) {}

	~huffman() {
		delete_tree(m_tree);
	}

	template<class IIter>
	CODE encode(IIter b, IIter e) const {

		CODE code;

		for(auto it(b); it != e; ++it) {

			CODE pcode;

			if(lookup(*it, pcode, m_tree)) {
				code.insert(std::end(code), std::begin(pcode), std::end(pcode));
			} else break;
		}

		return code;
	}

	template<class IIter>
	CSEQ decode(IIter b, IIter e, uint64_t len = 0u) const {

		CSEQ n;

		if(!m_tree) {

			for(auto it(b); it < e;) {

				for(const auto &di : m_dictionary) {

					uint64_t ch = 0u;

					for(uint8_t ci = 0; ci < di.first.length; ++ci) {
						if((it + ci) < e && *(it + ci)) ch |= 1u << ci;
					}

					if(ch == di.first.bcode) {
						n.emplace_back(di.second);
						it += di.first.length;
						break;
					}
				}

				if(static_cast<uint64_t>(std::distance(b, it)) >= len) break;
			}

		} else {

			const TREE *p = m_tree;

			for(auto it(b); it != e; ++it) {

				p = *it ? p->right() : p->left();

				if(!(p->left() || p->right())) {
					n.emplace_back(p->name());
					p = m_tree;
				}
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
	void delete_tree(_tree_node *n) const {

		if(n && n->left()) delete_tree(n->left());
		if(n && n->right()) delete_tree(n->right());

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

	DICT build_dictionary(const ALPHABET& a) const {

		DICT d;

		for(const ALPHABET_ENTRY &c : a) {

			CODE code;

			if(lookup(c.character(), code, m_tree)) {

				uint64_t cc = 0u;
				uint8_t bit = 0u;

				for(const auto &i : code) {
					if(i) cc |= 1u << bit;
					++bit;
				}

				d.emplace(std::make_pair(DICT_KEY { static_cast<uint8_t>(code.size()), cc },
					c.character()));
			}
		}

		return d;
	}

	struct _node_cmp {
		constexpr bool operator()(const TREE_NODE *x, const TREE_NODE *y) const {
			return x->probability() > y->probability();
		}
	};

	typedef std::priority_queue<TREE_NODE *, std::vector<TREE_NODE *>, _node_cmp> PQ;

	TREE *build_tree(const ALPHABET &n) const {

		PQ pq;

		for(const ALPHABET_ENTRY &a : n) {
			pq.push(new TREE_NODE(a.probability(), a.character()));
		}

		while(pq.size() > 1u) {

			TREE_NODE *min[2];

			min[0] = pq.top();
			pq.pop();

			min[1] = pq.top();
			pq.pop();

			pq.push(new TREE_NODE(min[0]->probability() + min[1]->probability(),
				min[0], min[1], std::max(min[0]->height(), min[1]->height()) + 1u));
		}

		return pq.top();
	}

private:
	TREE * const m_tree;
	DICT   m_dictionary;
};

}

#endif /* _HUFFMAN_H */
