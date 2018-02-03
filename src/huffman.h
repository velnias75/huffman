#ifndef _HUFFMAN_H
#define _HUFFMAN_H

#include <algorithm>
#include <vector>
#include <deque>

namespace huffman {

template<class C, class P = double>
class huffman {

	huffman(const huffman&);
	huffman& operator=(const huffman&);

public:
	typedef C character_type;
	typedef P probability_type;
	typedef std::vector<character_type> CSEQ;

private:

	typedef class _alphabet_entry {
	public:

		explicit _alphabet_entry(const probability_type& p) : m_node(true), m_active(0u),
			m_character(0), m_probability(p) {}

		_alphabet_entry(const character_type& c, const probability_type& p, unsigned char a = 0u) :
			m_node(false), m_active(a), m_character(c), m_probability(p) {}

		void setActive(unsigned char p = 0u) {
			m_active = p;
		}

		unsigned char active() const {
			return m_active;
		}

		character_type character() const {
			return m_character;
		}

		probability_type probability() const {
			return m_probability;
		}

		bool node() const {
			return m_node;
		}

	private:
		bool m_node;
		unsigned char m_active;
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

		_column_entry(const column_entry_type& e) : m_entry(e) {}

		bool node() const {
			return m_entry.node();
		}

		void setActive(unsigned char p) {
			m_entry.setActive(p);
		}

		unsigned char active() const {
			return m_entry.active();
		}

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
		typedef std::deque<character_type> NAME;

		std::size_t row;
		NAME name;
		probability_type probability;

		friend bool operator==(const _node &x, const _node &y) {
			return x.row == y.row;
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

	TREE tree() const {
		return *m_tree;
	}

	CSEQ decode(unsigned int c, std::size_t nbits) {

		CSEQ n;
		std::size_t bit(1);
		const TREE *p = m_tree;

		do {

			p = !!(c & (1 << bit)) ? p->right : p->left;

			if(!(p->left || p->right)) {
				n.push_back(p->node->name.front());
				p = m_tree;
			}

			++bit;

		} while(bit <= nbits);

		return n;
	}

private:

	void delete_tree(_tree_node *n) {

		if(n->left) delete_tree(n->left);
		if(n->right) delete_tree(n->right);

		delete n;
	}

	TREE *build_tree(const NODES &n) {

		TREE *tp = new TREE(), *t = tp;

		tp->node = &n.front();

		for(auto i(std::begin(n) + 1u); i != std::end(n); ++i) {
			tp->right = new TREE_NODE();
			tp->right->node = &(*i);
			tp->left  = new TREE_NODE();
			tp->left->node = &(*(++i));
			tp = tp->right->node->name.size() == 1u ? tp->left : tp->right;
		}

		return t;
	}

	COLUMNS build_table(const ALPHABET& a) {

		COLUMNS c { COLUMN(std::begin(a), std::end(a)) };

		auto i(c.begin());

		do {

			auto col(*i);

			typename COLUMN::value_type minima[2];

			std::partial_sort_copy(std::begin(col), std::end(col), minima, minima + 2);
			std::find(std::begin(*i), std::end(*i), minima[0])->setActive(1u);
			std::find(std::begin(*i), std::end(*i), minima[1])->setActive(2u);

			c.push_back(COLUMN());

			std::copy_if(std::begin(col), std::end(col), std::back_inserter(c.back()),
				[minima](const typename COLUMN::value_type& x)
					{ return x.probability() != minima[0].probability() &&
						 x.probability() != minima[1].probability(); });

			c.back().push_back(typename ALPHABET::value_type(
				minima[0].probability() + minima[1].probability()));

			i = --end(c);

		} while(c.back().size() != 1u);

		return c;
	}

	NODES build_nodes(const COLUMNS &t) {

		NODES n;
		std::size_t p = 0u;

		for(auto i(std::begin(t.front())); i != std::end(t.front()); ++i, ++p) {
			n.push_back(NODE { p, typename NODE::NAME { i->character() },
				probability_type(i->probability()) });
		}

		typename NODE::NAME pname;

		for(auto i(std::begin(t)); i != std::end(t) - 1u; ++i, ++p) {

			n.push_back(NODE { p, typename NODE::NAME(std::begin(pname), std::end(pname)),
				probability_type((i + 1u)->back().probability()) });

			for(auto j(std::begin(*i)); j != std::end(*i); ++j) {

				const character_type &c(j->character());

				if(!j->node()) {
					if(j->active() == 1u) {
						n.back().name.push_front(c);
					} else if(j->active() == 2u) {
						n.back().name.push_back(c);
					}
				}
			}

			pname.clear();
			pname.insert(std::end(pname), std::begin(n.back().name), std::end(n.back().name));
		}

		std::sort(std::begin(n), std::end(n));

		return n;
	}

private:
	NODES m_nodes;
	TREE  *m_tree;
};

}

#endif /* _HUFFMAN_H */
