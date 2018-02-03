#include <cstdlib>
#include <iterator>
#include <iostream>

#include "huffman.h"

int main(int, char **) {

	typedef huffman::huffman<char> HUFFMAN;

	HUFFMAN::NODES n(HUFFMAN({
		HUFFMAN::value_type('A', 0.12),
		HUFFMAN::value_type('E', 0.42),
		HUFFMAN::value_type('I', 0.09),
		HUFFMAN::value_type('O', 0.30),
		HUFFMAN::value_type('U', 0.07)
	}).nodes());

	std::cout << "Nodes: " << n.size() << std::endl;

	for(auto i(std::begin(n)); i != std::end(n); ++i) {

		std::cout << (i->row + 1u) << "; ";
		std::copy(std::begin(i->name), std::end(i->name),
              std::ostream_iterator<HUFFMAN::character_type>(std::cout, ""));
        std::cout << "; " << i->probability << std::endl;
	}

	return EXIT_SUCCESS;
}
