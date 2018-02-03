#include <cstdlib>
#include <iostream>

#include "huffman.h"

int main(int, char **) {

	std::cout << "Stages: " <<
		huffman::huffman<char>({
		huffman::huffman<char>::value_type('A', 0.12),
		huffman::huffman<char>::value_type('E', 0.42),
		huffman::huffman<char>::value_type('I', 0.09),
		huffman::huffman<char>::value_type('O', 0.30),
		huffman::huffman<char>::value_type('U', 0.07)
	}).table().size() << std::endl;

	return EXIT_SUCCESS;
}
