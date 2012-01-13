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

