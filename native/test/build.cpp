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

void read_keys(std::istream &input, marisa::Keyset *keyset) {
	std::string line;
	while (std::getline(input, line)) {
		float weight = 1.0F;
		keyset->push_back(line.c_str(), line.length(), weight);
	}
}

int main(int argc, const char* argv[])
{
	clock_t start_at = clock();
	std::cout << "start build" << std::endl;

	marisa::Keyset keyset;
	if (argc <= 1) {
		read_keys(std::cin, &keyset);
	} else {
		std::ifstream in(argv[2], std::ios::binary | std::ios::in);
		read_keys(in, &keyset);
		in.close();
	}
	std::cout << keyset.size() << " entries loaded." << std::endl;

	marisa::Trie trie;
	trie.build(keyset, MARISA_DEFAULT_NUM_TRIES | MARISA_DEFAULT_TAIL | MARISA_DEFAULT_ORDER | MARISA_DEFAULT_CACHE);

	std::cout << argv[1] << " saving" << std::endl;
	trie.save(argv[1]);

	clock_t end_at = clock();
	std::cout << "finish build. elapsis: " << (double)(end_at - start_at) / (double)CLOCKS_PER_SEC << " secs" <<  std::endl;

	return 0;
}

