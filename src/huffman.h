#ifndef _HUFFMAN_H
#define _HUFFMAN_H

#include <algorithm>
#include <vector>
#include <deque>

namespace huffman {

template<class C, class P = double>
class huffman {
public:
	typedef C character_type;
	typedef P probability_type;

private:

	typedef class _alphabet_entry {
	public:

		_alphabet_entry(const character_type& c, const probability_type& p, bool a = false) :
			m_active(a), m_character(c), m_probability(p) {}

		void setActive(std::size_t p = 0u) {
			m_active = p;
		}

		std::size_t active() const {
			return m_active;
		}

		character_type character() const {
			return m_character;
		}

		probability_type probability() const {
			return m_probability;
		}

	private:
		std::size_t m_active;
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

		void setActive(std::size_t p) {
			m_entry.setActive(p);
		}

		std::size_t active() const {
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

	typedef struct {
		typedef std::deque<character_type> NAME;
		std::size_t row;
		NAME name;
		probability_type probability;
	} NODE;

public:

	typedef std::vector<COLUMN> COLUMNS;
	typedef std::vector<NODE> NODES;

	explicit huffman(const ALPHABET& a) : m_nodes(build_nodes(build_table(a))) {}

	NODES nodes() const {
		return m_nodes;
	}

private:
	COLUMNS build_table(const ALPHABET& a) {

		COLUMNS c { COLUMN(std::begin(a), std::end(a)) };

		auto i(c.begin());

		do {

			auto col(*i);

			typename COLUMN::value_type minima[2];

			std::partial_sort_copy(std::begin(col), std::end(col), minima, minima + 2);
			std::find(std::begin(*i), std::end(*i), minima[0])->setActive(1);
			std::find(std::begin(*i), std::end(*i), minima[1])->setActive(2);

			c.push_back(COLUMN());

			std::copy_if(std::begin(col), std::end(col), std::back_inserter(c.back()),
				[minima](const typename COLUMN::value_type& x)
					{ return x.probability() != minima[0].probability() &&
						 x.probability() != minima[1].probability(); });

			c.back().push_back(typename ALPHABET::value_type(0,
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
				if(j->active() == 1u) {
					n.back().name.push_front(j->character());
				} else if(j->active() == 2u) {
					n.back().name.push_back(j->character());
				}
			}

			pname.clear();
			pname.insert(std::end(pname), std::begin(n.back().name), std::end(n.back().name));
		}

		return n;
	}

private:
	NODES m_nodes;
};

}

#endif /* _HUFFMAN_H */
