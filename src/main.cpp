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
#include <cstdlib>
#include <numeric>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_RATIONAL_H
#include <rational/rational.h>
#endif

#include "huffman.h"

int main(int, char **) {

#ifdef HAVE_RATIONAL_H
	typedef Commons::Math::Rational<unsigned long> PROBABILITY;
#else
	typedef float PROBABILITY;
#endif

	typedef huffman::huffman<char, PROBABILITY> HUFFMAN;

	HUFFMAN::ALPHABET alpha;
	HUFFMAN::CSEQ    source;

	{
		std::map<char, std::size_t> m;
		std::size_t ms = 0u;
		char i;

		while((i = std::cin.get()) != std::istream::traits_type::eof()) {

			++m[i];
			++ms;
			source.push_back(i);
		}

#ifdef HAVE_RATIONAL_H
		const PROBABILITY pf(1ul, ms);
#else
		const PROBABILITY pf(1.0f/ms);
#endif

		for(const auto& mi : m) {
			alpha.push_back(HUFFMAN::ALPHABET_ENTRY(mi.first, pf * PROBABILITY(mi.second)));
		}
	}

	HUFFMAN huff(alpha);
	HUFFMAN::CODE enc(huff.encode(source));

	const float ratio = (enc.size() * 100u)/(source.size()*sizeof(HUFFMAN::character_type)*8u);

	std::cout << "Encoded into " << enc.size() << " bits: " << ratio << "%" << std::endl;

	HUFFMAN::CSEQ dec(huff.decode(enc));

	std::cout << "Decoded: ";
	std::copy(std::begin(dec), std::end(dec),
		std::ostream_iterator<HUFFMAN::character_type>(std::cout));
	std::cout << std::endl;

	return EXIT_SUCCESS;
}
