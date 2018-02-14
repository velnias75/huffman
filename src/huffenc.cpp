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

#include <iostream>

#include "hufflib.h"

int main(int argc, char **argv) {

	HUFFMAN::CSEQ source;

	const HUFFMAN huff(huffman::huffread(source, argc == 2 ? argv[1] : "",
		argc == 2 && argv[1][0] != '-'));

	HUFFMAN::CODE enc(huff.encode(std::begin(source), std::end(source)));

	std::vector<uint8_t> dst;
	dst.reserve((enc.size()/8u) + 1u);

	uint8_t     c = 0u;
	std::size_t b = 0u;

	for(auto i : enc) {

		if(i) c |= 1u << b;

		++b;

		if(b > 7u) {

			dst.push_back(c);

			b = 0u;
			c = 0u;
		}
	}

	if(b < 8u) dst.push_back(c);

	std::for_each(std::begin(dst), std::end(dst), [](uint8_t x) -> void { std::cout.put(x); });
	std::cout.flush();

	return EXIT_SUCCESS;
}
