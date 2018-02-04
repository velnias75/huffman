#include <cstdlib>
#include <iterator>
#include <iostream>
#include <numeric>

#include "huffman.h"

int main(int, char **) {

	typedef huffman::huffman<char, float> HUFFMAN;

	HUFFMAN vowellish({
		HUFFMAN::value_type('A', 0.12),
		HUFFMAN::value_type('E', 0.42),
		HUFFMAN::value_type('I', 0.09),
		HUFFMAN::value_type('O', 0.30),
		HUFFMAN::value_type('U', 0.07)
	});

	std::vector<bool> code(vowellish.encode(HUFFMAN::CSEQ { 'I', 'O', 'U' }));

	unsigned int x;

	std::cout << "Encoded: " <<
		(x = std::accumulate(std::begin(code), std::end(code), 0u,
			[](unsigned int x, unsigned int y) { return (x << 1u) + y; }))
		<< " (" << code.size() <<" bits)" << std::endl;

	HUFFMAN::CSEQ n(vowellish.decode(x, code.size()));

	std::cout << "Decoded: ";
	std::copy(std::begin(n), std::end(n),
		std::ostream_iterator<HUFFMAN::character_type>(std::cout));
	std::cout << std::endl;

	return EXIT_SUCCESS;
}
