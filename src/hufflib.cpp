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
#include <fstream>

#include "hufflib.h"

#ifdef HAVE_RATIONAL_H
typedef Commons::Math::Rational<unsigned long> PROBABILITY;

template class Commons::Math::Rational<unsigned long>;

#else
typedef float PROBABILITY;
#endif

template class huffman::huffman<char, PROBABILITY, std::vector<unsigned char>>;

HUFFMAN huffman::huffread(HUFFMAN::CSEQ &source, const std::string &fname, bool isFile,
	std::size_t max) {

	HUFFMAN::ALPHABET alpha;
	std::istream *in;

	if(isFile) {
		in = new std::ifstream(fname, std::ios::in|std::ios::binary);
	} else {
		in = &std::cin;
	}

	std::unordered_map<char, std::size_t> m;
	std::size_t ms = 0u;
	char i;

	while(!in->read(&i, 1).eof()) {
		++m[i];

		if(max && ms < max) {
			source.emplace_back(i);
		} else source.emplace_back(i);

		++ms;
	}

	if(isFile) {
		delete in;
	}

#ifdef HAVE_RATIONAL_H
	const PROBABILITY pf(1ul, ms);
#else
	const PROBABILITY pf(1.0f/ms);
#endif

	for(const auto& mi : m) {
		alpha.emplace_back(HUFFMAN::ALPHABET_ENTRY(mi.first, pf * PROBABILITY(mi.second)));
	}

	return HUFFMAN(alpha);
}
