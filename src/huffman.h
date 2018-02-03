#ifndef _HUFFMAN_H
#define _HUFFMAN_H

#include <algorithm>
#include <vector>

namespace huffman {

template<class C, class P = double>
class huffman {
private:
	typedef C character_type;
	typedef P probability_type;

	typedef class _alphabet_entry {
	public:

		_alphabet_entry(const character_type& c, const probability_type& p) :
			m_character(c), m_probability(p) {}

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

		_column_entry(const column_entry_type& e) : m_entry(e) {}

		probability_type probability() const {
			return m_entry.probability();
		}

		friend bool operator<(const _column_entry& x, const _column_entry& y) {
			return x.probability() < y.probability();
		}

	private:
		column_entry_type m_entry;
	} COLUMN_ENTRY;

	typedef std::vector<COLUMN_ENTRY> COLUMN;
	typedef std::vector<COLUMN> COLUMNS;

public:
	explicit huffman(const ALPHABET& a) : m_columns(build_table(a)) {}

	COLUMNS table() const {
		return m_columns;
	}

private:
	COLUMNS build_table(const ALPHABET& a) {

		COLUMNS c { COLUMN(std::begin(a), std::end(a)) };

		auto i(c.begin());

		do {

			auto col(*i);

			typename COLUMN::value_type minima[2];

			std::partial_sort_copy(std::begin(col), std::end(col), minima, minima + 2);

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

private:
	COLUMNS m_columns;
};

}

#endif /* _HUFFMAN_H */
