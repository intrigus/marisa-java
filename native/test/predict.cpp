/*--------------------------------------------------------------------------
 * Copyright (c) 2012, NAKANO Hideo
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * 
 * - Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 
 * - Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. 
 * - Neither the name of the <ORGANIZATION> nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 *
 *THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *--------------------------------------------------------------------------*/
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <time.h>

#include "marisa.h"

void predictive_search(std::istream &input, const marisa::Trie& trie) {
	marisa::Agent agent;
	marisa::Keyset keyset;
	std::string str;
	int line_count = 0;
	int found_count = 0;
	int not_found_count = 0;

	while (std::getline(input, str)) {
		line_count++;

		//std::cout << "search: " << str << "*" << std::endl;

		agent.set_query(str.c_str(), str.length());
		while (trie.predictive_search(agent)) {
			keyset.push_back(agent.key());
		}

		if (keyset.empty()) {
			//std::cout << "not found" << std::endl;
			not_found_count++;
		} else {
			//std::cout << keyset.size() << " found" << std::endl;
			found_count++;

			const std::size_t end = keyset.size();
			for (std::size_t i = 0; i < end; ++i) {
				const marisa::Key& key = keyset[i];
				key.id();
				key.ptr();
				key.length();

				//std::cout << keyset[i].id() << '\t';
				//std::cout.write(keyset[i].ptr(), keyset[i].length()) << '\t';
				//std::cout << str << '\n';
			}
		}

		keyset.reset();
	}

	std::cout << line_count << " entries is predicted, " << found_count << " entries is found, " << not_found_count << " entries is not found." << std::endl;
}

int main(int argc, const char* argv[])
{
	clock_t start_at = clock();
	std::cout << "start predictive search" << std::endl;

	marisa::Trie trie;
	trie.load(argv[1]);

	if (argc <= 1) {
		predictive_search(std::cin, trie);
	} else {
		std::ifstream in(argv[2], std::ios::binary | std::ios::in);
		predictive_search(in, trie);
		in.close();
	}

	clock_t end_at = clock();
	std::cout << "finish predictive search. elapsis: " << (double)(end_at - start_at) / (double)CLOCKS_PER_SEC << " secs" <<  std::endl;

	return 0;
}
