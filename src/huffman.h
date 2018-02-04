#ifndef _HUFFMAN_H
#define _HUFFMAN_H

#include <algorithm>
#include <vector>

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

private:

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

public:
	typedef std::vector<ALPHABET_ENTRY> ALPHABET;
	typedef typename ALPHABET::value_type value_type;

private:
	typedef class _column_entry {
	public:
		typedef ALPHABET_ENTRY column_entry_type;

		_column_entry() : m_entry(character_type(0), probability_type(0)) {}

		explicit _column_entry(const column_entry_type& e) : m_entry(e) {}

		character_type character() const {
			return m_entry.character();
		}

		probability_type probability() const {
			return m_entry.probability();
		}

		friend bool operator==(const _column_entry& a, const _column_entry& b) {
			return a.character() == b.character();
		}

		friend bool operator<(const _column_entry& x, const _column_entry& y) {
			return x.probability() < y.probability();
		}

	private:
		column_entry_type m_entry;
	} COLUMN_ENTRY;

	typedef std::vector<COLUMN_ENTRY> COLUMN;

	typedef struct _node {

		probability_type probability;
		character_type name;
		bool leaf;

		friend bool operator==(const _node &x, const _node &y) {
			return x.name == y.name && x.probability == x.probability && x.leaf == x.leaf;
		}

		friend bool operator<(const _node& x, const _node& y) {
			return x.probability > y.probability;
		}

	} NODE;

	typedef std::vector<COLUMN> COLUMNS;

	typedef struct _tree_node {
		_tree_node() : node(0L), left(0L), right(0L) {}
		const NODE *node;
		_tree_node *left;
		_tree_node *right;
	} TREE_NODE;

	typedef std::vector<NODE> NODES;

public:
	typedef _tree_node TREE;

	explicit huffman(const ALPHABET& a) : m_nodes(build_nodes(build_table(a))),
		m_tree(build_tree(m_nodes)) {}

	~huffman() {
		delete_tree(m_tree);
	}

	CODE encode(const CSEQ &n) const {

		CODE code;
		code.reserve(4096u);

		for(const typename CSEQ::value_type &i : n) {
			if(!lookup(i, code, m_tree)) break;
		}

		return code;
	}

	CSEQ decode(const CODE &c) const {

		CSEQ n;
		const TREE *p = m_tree;

		for(bool bit : c) {

			p = bit ? p->right : p->left;

			if(!(p->left || p->right)) {
				n.push_back(p->node->name);
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

		if(n->node->name != c || !n->node->leaf) {

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

	TREE *build_tree(const NODES &n) {

		TREE *tp = new TREE(), *t = tp;

		tp->node = &n.front();

		for(auto i(std::begin(n) + 1u); i != std::end(n); ++i) {
			tp->right = new TREE_NODE();
			tp->right->node = &(*i);
			tp->left  = new TREE_NODE();
			tp->left->node = &(*(++i));
			tp = tp->right->node->leaf ? tp->left : tp->right;
		}

		return t;
	}

	COLUMNS build_table(const ALPHABET& a) {

		COLUMNS c { COLUMN(std::begin(a), std::end(a)) };

		c.reserve(a.size());

		auto i(c.begin());

		do {

			auto col(*i);

			typename COLUMN::value_type minima[2];

			std::partial_sort_copy(std::begin(col), std::end(col), minima, minima + 2);

			c.push_back(COLUMN());
			c.back().reserve(col.size() - 1u);

			std::copy_if(std::begin(col), std::end(col), std::back_inserter(c.back()),
				[minima](const typename COLUMN::value_type& x)
					{ return x.probability() != minima[0].probability() &&
						 x.probability() != minima[1].probability(); });

			c.back().push_back(COLUMN_ENTRY(typename ALPHABET::value_type(
				minima[0].probability() + minima[1].probability())));

			c.back().shrink_to_fit();

			i = --end(c);

		} while(c.back().size() != 1u);

		c.shrink_to_fit();

		return c;
	}

	static NODES build_nodes(const COLUMNS &t) {

		NODES n;

		n.reserve((t.front().size() * 2u) - 1u);

		for(auto i(std::begin(t.front())); i != std::end(t.front()); ++i) {
			n.push_back(NODE { probability_type(i->probability()), i->character(), true });
		}

		for(auto i(std::begin(t)); i != std::end(t) - 1u; ++i) {
			n.push_back(NODE { probability_type((i + 1u)->back().probability()), 0, false });
		}

		std::sort(std::begin(n), std::end(n));

		n.shrink_to_fit();

		return n;
	}

private:
	const NODES m_nodes;
	TREE * const m_tree;
};

}

#endif /* _HUFFMAN_H */
