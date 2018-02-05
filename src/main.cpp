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

#ifdef HAVE_RATIONAL_H
typedef Commons::Math::Rational<unsigned long> PROBABILITY;
#else
typedef float PROBABILITY;
#endif

typedef huffman::huffman<char, PROBABILITY, std::vector<unsigned char>> HUFFMAN;

std::string nodeLabel(const HUFFMAN::TREE * const n) {

	std::ostringstream os, hex, nor;

	if(n->leaf()) {
		hex << "0x" << std::hex << (((uint16_t)n->name()) & 0xff);
		nor << "\'" << (n->name() == '\"' ? "\\\"" : std::string(1, n->name())) << "\'";
		os << " [label=\"" << (std::isprint(n->name()) ? nor.str() : hex.str())
			<< " (" << std::defaultfloat << n->probability()
				<< ")\",shape=ellipse,style=filled,fillcolor=darkolivegreen,fontcolor=white]";
	} else {
		os << " [label=\"" << std::defaultfloat << n->probability()
			<< "\",shape=box,style=filled,fillcolor=beige]";
	}

	return os.str();
}

void tree2dot(const HUFFMAN::TREE * const n) {

	if(n->left()) {
		std::cout << "N" << std::hex << n->left() << nodeLabel(n->left()) << ";" << std::endl;
		std::cout << "N" << std::hex << n << " -> " << "N" << n->left()
			<< "[label=0,fontcolor=red,labeldistance=3];" << std::endl;
		tree2dot(n->left());
	}

	if(n->right()) {
		std::cout << "N" << std::hex << n->right() << nodeLabel(n->right()) << ";" << std::endl;
		std::cout << "N" << std::hex << n << " -> " << "N" << n->right()
			<< "[label=1,fontcolor=blue,labeldistance=3];" << std::endl;
		tree2dot(n->right());
	}
}

int main(int argc, char **argv) {

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

	HUFFMAN huff(alpha);

	if(argc != 2) {

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

		HUFFMAN::CSEQ dec(huff.decode(enc));

		std::cout << "Decoded: ";
		std::copy(std::begin(dec), std::end(dec),
			std::ostream_iterator<HUFFMAN::character_type>(std::cout));
		std::cout << std::endl;

	} else if(std::string(argv[1]) == "-t") {
		std::cout << "digraph {" << std::endl;
		std::cout << "fontname=\"Courier\"; fontnames=\"ps\"; rank=same; " <<
			"rankdir=RL; center=true; splines=false;" << std::endl;
		std::cout << "\nN" << std::hex << huff.tree() << nodeLabel(huff.tree()) << ";" << std::endl;
		tree2dot(huff.tree());
		std::cout << "}" << std::endl;

	} else {
		std::cerr << "unknown option" << std::endl;
	}

	return EXIT_SUCCESS;
}
