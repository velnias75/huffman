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

#ifndef _HUFFLIB_H
#define _HUFFLIB_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_RATIONAL_H
#include <rational/rational.h>
#endif

#include "huffman.h"

#ifdef HAVE_RATIONAL_H
typedef Commons::Math::Rational<unsigned long> PROBABILITY;

extern template class Commons::Math::Rational<unsigned long>;

#else
typedef float PROBABILITY;
#endif

extern template class huffman::huffman<char, PROBABILITY, std::vector<uint_fast8_t>>;

typedef huffman::huffman<char, PROBABILITY, std::vector<uint_fast8_t>> HUFFMAN;

namespace huffman {

typedef struct {
	const uint8_t magic[4] = { 'H', 'u', 'F', 'f' }; // "HuFf"
	const uint32_t version = 0x20180214; // new version: today (2018-02-14)
	uint32_t dict_entry_size = 0u;
	uint32_t dict_entries = 0u;
} HEADER;

HUFFMAN huffread(HUFFMAN::CSEQ &source, const std::string &fname, bool isFile,
	std::size_t max = 0u);

}

#endif /* _HUFFLIB_H */
