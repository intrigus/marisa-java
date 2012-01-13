#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <time.h>

#include "marisa.h"

void search_keys(std::istream &input, const marisa::Trie& trie) {
	marisa::Agent agent;
	std::string str;
	int line_count = 0;
	int found_count = 0;
	int not_found_count = 0;
	while (std::getline(input, str)) {
		line_count++;
		agent.set_query(str.c_str(), str.length());
		if (trie.lookup(agent)) {
			//std::cout << agent.key().id() << '\t' << str << '\n';
			found_count++;
		} else {
			//std::cout << "-1\t" << str << '\n';
			not_found_count++;
		}
	}

	std::cout << line_count << " entries is looked up, " << found_count << " entries is found, " << not_found_count << " entries is not found." << std::endl;
}

int main(int argc, const char* argv[])
{
	clock_t start_at = clock();
	std::cout << "start lookup" << std::endl;

	marisa::Trie trie;
	trie.load(argv[1]);

	if (argc <= 1) {
		search_keys(std::cin, trie);
	} else {
		std::ifstream in(argv[2], std::ios::binary | std::ios::in);
		search_keys(in, trie);
		in.close();
	}

	clock_t end_at = clock();
	std::cout << "finish lookup. elapsis: " << (double)(end_at - start_at) / (double)CLOCKS_PER_SEC << " secs" <<  std::endl;

	return 0;
}
