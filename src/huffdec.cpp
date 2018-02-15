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

int main(int, char **) {

	huffman::HEADER header;

	std::cin.read(reinterpret_cast<char *>(&header), sizeof(huffman::HEADER));

	if(header.version == HUFFVER) {

		HUFFMAN::DICT d;

		for(uint32_t i = 0u; i < header.dict_entries; ++i) {

			uint8_t  cl = 0u;
			uint8_t  ch = 0u;
			uint64_t co = 0u;

			std::cin.read(reinterpret_cast<char *>(&ch), sizeof(uint8_t));
			std::cin.read(reinterpret_cast<char *>(&cl), sizeof(uint8_t));

			if(header.dict_entry_size < 25u) {
				std::cin.read(reinterpret_cast<char *>(&co), sizeof(uint8_t));
			} else if(header.dict_entry_size < 33u) {
				std::cin.read(reinterpret_cast<char *>(&co), sizeof(uint16_t));
			} else if(header.dict_entry_size < 49u) {
				std::cin.read(reinterpret_cast<char *>(&co), sizeof(uint32_t));
			} else {
				std::cin.read(reinterpret_cast<char *>(&co), sizeof(uint64_t));
			}

			d[HUFFMAN::DICT_KEY {cl, co}] = ch;
		}

		char ic;
		HUFFMAN::CSEQ source;

		while(!std::cin.read(&ic, 1).eof()) {
			for(uint8_t bit = 0u; bit < 8u; ++bit) {
				source.emplace_back(static_cast<uint8_t>(ic) & 1u << bit ? true : false);
			}
		}

		const HUFFMAN huff(d);
		const HUFFMAN::CSEQ dec(huff.decode(std::begin(source), std::end(source),
			header.bit_length));

		for(const auto &di : dec) {
			std::cout.put(di);
		}

		std::cout.flush();

		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}
