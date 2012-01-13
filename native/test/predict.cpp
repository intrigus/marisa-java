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
