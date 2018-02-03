#include <cstdlib>
#include <iterator>
#include <iostream>

#include "huffman.h"

int main(int, char **) {

	typedef huffman::huffman<char> HUFFMAN;

	HUFFMAN vowellish({
		HUFFMAN::value_type('A', 0.12),
		HUFFMAN::value_type('E', 0.42),
		HUFFMAN::value_type('I', 0.09),
		HUFFMAN::value_type('O', 0.30),
		HUFFMAN::value_type('U', 0.07)
	});

	HUFFMAN::NAME n(vowellish.decode(762u, 10u));

	std::cout << "Decoded: ";
	std::copy(std::begin(n), std::end(n),
		std::ostream_iterator<HUFFMAN::character_type>(std::cout));
	std::cout << std::endl;

	return EXIT_SUCCESS;
}
