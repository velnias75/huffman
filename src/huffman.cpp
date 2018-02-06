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
#include <iterator>
#include <numeric>

#include "hufflib.h"

int main(int, char **) {

	HUFFMAN::ALPHABET alpha;
	HUFFMAN::CSEQ    source;

	{
		std::unordered_map<char, std::size_t> m;
		std::size_t ms = 0u;
		char i;

		while(!std::cin.read(&i, 1).eof()) {
			++m[i];
			++ms;
			source.emplace_back(i);
		}

#ifdef HAVE_RATIONAL_H
		const PROBABILITY pf(1ul, ms);
#else
		const PROBABILITY pf(1.0f/ms);
#endif

		for(const auto& mi : m) {
			alpha.emplace_back(HUFFMAN::ALPHABET_ENTRY(mi.first, pf * PROBABILITY(mi.second)));
		}
	}

	const HUFFMAN huff(alpha);
	HUFFMAN::CODE enc(huff.encode(std::begin(source), std::end(source)));

	std::cerr << "Encoded into " << enc.size() << " bits: "
		<< std::defaultfloat
		<< (float(enc.size() * 100u)/float(source.size()*sizeof(HUFFMAN::character_type)*8u))
		<< "%" << std::endl;
	std::cerr << "Dictionary has " << huff.dictionary().size() <<
		" entries with an average code of " << std::defaultfloat
		<< ((float)std::accumulate(std::begin(huff.dictionary()), std::end(huff.dictionary()),
				HUFFMAN::DICT::mapped_type::size_type(0),
				[](const HUFFMAN::DICT::mapped_type::size_type &a,
					const HUFFMAN::DICT::value_type &b)
						{ return a + b.second.size(); })/(float)huff.dictionary().size())
		<< " bits." << std::endl;

	HUFFMAN::CSEQ dec(huff.decode(std::begin(enc), std::end(enc)));

	std::cout << "Decoded: ";
	std::copy(std::begin(dec), std::end(dec),
		std::ostream_iterator<HUFFMAN::character_type>(std::cout));
	std::cout << std::endl;

	return EXIT_SUCCESS;
}