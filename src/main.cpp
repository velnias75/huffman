#include <cstdlib>

#include <iostream>

#include <algorithm>
#include <vector>

template<class C, class P>
class _alphabet_entry {
public:

	typedef C character_type;
	typedef P probability_type;

	_alphabet_entry(const character_type& c, const probability_type& p) :
		m_character(c), m_probability(p) {}

	probability_type probability() const {
		return m_probability;
	}

private:
	character_type m_character;
	probability_type m_probability;
};

template<class C, class P = double>
class _column_entry {
public:

	typedef _alphabet_entry<C, P> column_entry_type;

	_column_entry() : m_ae(typename column_entry_type::character_type(0),
		typename column_entry_type::probability_type(0)) {}

	_column_entry(const column_entry_type& ae) : m_ae(ae) {}

	typename column_entry_type::probability_type probability() const {
		return m_ae.probability();
	}

	friend bool operator<(const _column_entry& x, const _column_entry& y) {
		return x.probability() < y.probability();
	}

private:
	column_entry_type m_ae;
};

template<class C, class P = double>
using _alphabet = std::vector<_alphabet_entry<C, P>>;

template<class C, class P = double>
using _column = std::vector<_column_entry<C, P>>;

typedef _alphabet<char> VOWELLISH;
typedef _column<VOWELLISH::value_type::character_type> COLUMN;
typedef std::vector<COLUMN> COLUMNS;

COLUMNS build_huffman_table(const VOWELLISH& a) {

	// TODO: check for accumulated sum of a to be 1.0

	typedef COLUMN::value_type::column_entry_type::probability_type PROBABILITY;

	COLUMNS c { COLUMN(std::begin(a), std::end(a)) };

	auto i(c.begin());
	PROBABILITY p;

	do {

		auto col(*i);

		COLUMN::value_type minima[2];

		std::partial_sort_copy(std::begin(col), std::end(col), minima, minima + 2);

		p = minima[0].probability() + minima[1].probability();

		c.push_back(COLUMN());

		std::copy_if(std::begin(col), std::end(col), std::back_inserter(c.back()),
			[minima](const COLUMN::value_type& x)
				{ return x.probability() != minima[0].probability() &&
					 x.probability() != minima[1].probability(); });

		c.back().push_back(VOWELLISH::value_type(0, p));

		i = --end(c);

	} while(p != PROBABILITY(1));

	return c;
}

int main(int, char **) {

	std::cout << "Stages: " << build_huffman_table({
		VOWELLISH::value_type('A', 0.12),
		VOWELLISH::value_type('E', 0.42),
		VOWELLISH::value_type('I', 0.09),
		VOWELLISH::value_type('O', 0.30),
		VOWELLISH::value_type('U', 0.07)
	}).size() << std::endl;

	return EXIT_SUCCESS;
}
