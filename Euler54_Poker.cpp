// Euler54_Poker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <fstream>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>

constexpr auto VERBOSE_MODE = 1;
constexpr auto FIND_COMBINATION_VERBOSE = 0;

namespace poker_utility {
	constexpr auto FILE_NAME = "poker_hands.txt";
	
	const std::map<char, std::string> suit_ch_to_str_map = { {'D',"diamonds"}, {'H',"hearts"}, {'C',"clubs"}, {'S',"spades"} };
	const std::map<char, int> rank_to_int_map = { {'2', 0}, {'3', 1},{'4', 2},{'5', 3},{'6', 4},{'7', 5},{'8', 6},{'9', 7},{'T',8}, {'J',9}, {'Q',10}, {'K',11}, {'A',12} };

	// custom comparitor
	struct Comp
	{
		bool operator()(const char& lhs, const char& rhs) const
		{
				int lhs_value = rank_to_int_map.find(lhs)->second;
				int rhs_value = rank_to_int_map.find(rhs)->second;
				return lhs_value > rhs_value;	
		}
	};

	typedef std::map<char, std::vector<std::string>, Comp> occurencies_map;	// example { {6 : {"hearts", "diamonds"}}, {2 : {"clubs"}} }

	void print_map(const occurencies_map& oc_map) {
		auto it = oc_map.cbegin();
		for (it; it != oc_map.cend(); it++) {
			std::cout << it->first << " : {";
			for (std::string suit_str : it->second) {
				std::cout << suit_str << ", ";
			}
			std::cout << "}" << std::endl;
		}
	}

	namespace combinations_finder {
		typedef char rank;

		enum class Combination_Name {
			high_card = 0,
			one_pair,
			two_pair,
			three_of_kind,
			straight,
			flush,
			full_house,
			four_of_kind,
			straight_flush,
			royal_flush,
		};

		std::map<Combination_Name, std::string> comb_to_str_map = { 
			{Combination_Name::high_card, "high card"}, 
			{Combination_Name::one_pair, "one pair"},
			{Combination_Name::two_pair, "two pairs"},
			{Combination_Name::three_of_kind, "three of kind"},
			{Combination_Name::straight, "straight"},
			{Combination_Name::flush, "flush"},
			{Combination_Name::full_house, "full house"},
			{Combination_Name::four_of_kind, "four of a kind"},
			{Combination_Name::straight_flush, "straight flush"},
			{Combination_Name::royal_flush, "royal flush"}
		};

		struct Combination {
			Combination_Name comb_name;
			rank comb_rank;
			rank secondary_comb_rank;
		};
		
		rank has_flush(const occurencies_map& oc_map) {
			if (oc_map.size() == 5) {
				auto it = oc_map.cbegin();
				const std::string suit = it->second.at(0);
				while (it != oc_map.cend()) {
					if (it->second.at(0) != suit)
						return 'z';
					it++;
				}
				return oc_map.begin()->first;
			}
			return 'z';
		}
		rank has_straight(const occurencies_map& oc_map) {
			if (oc_map.size() == 5) {
				auto begin_it = oc_map.begin();
				auto end_it = oc_map.end();

				int high_rank = rank_to_int_map.find(begin_it->first)->second;
				int low_rank = rank_to_int_map.find(std::prev(end_it)->first)->second;

				if ((high_rank - low_rank) == 4) {
					return begin_it->first;
				}
			}
			return 'z';
		}

		rank has_four(const occurencies_map& oc_map) {
			auto it = oc_map.cbegin();
			while (it != oc_map.cend()) {
				if (it->second.size() == 4) {
					return it->first;
				}
				it++;
			}
			return 'z';
		}

		rank has_pair(const occurencies_map& oc_map) {
			auto it = oc_map.cbegin();
			while (it != oc_map.cend()) {
				if (it->second.size() == 2) {
					return it->first;
				}
				it++;
			}
			return 'z';
		}

		rank has_three(const occurencies_map& oc_map) {
			auto it = oc_map.cbegin();
			while (it != oc_map.cend()) {
				if (it->second.size() == 3) {
					return it->first;
				}
				it++;
			}
			return 'z';
		}

		std::pair<rank,rank> has_two_pairs(occurencies_map& oc_map) {
			rank pair_one = has_pair(oc_map);
			rank pair_two;

			if (pair_one != 'z') {
				auto it = oc_map.find(pair_one);
				auto pair_element = *it;
				oc_map.erase(it->first);
				pair_two = has_pair(oc_map);
				if (pair_two != 'z')
					return { pair_one,pair_two };
				else
					oc_map.insert(pair_element);
			}
			
			return std::make_pair('z', 'z');
		}

		Combination find_combination(occurencies_map& oc_map) {
			// from most powerfull to least powerfull
			rank flush = poker_utility::combinations_finder::has_flush(oc_map);
			rank straight = poker_utility::combinations_finder::has_straight(oc_map);
			rank four = poker_utility::combinations_finder::has_four(oc_map);
			std::pair<rank, rank> two_pairs = poker_utility::combinations_finder::has_two_pairs(oc_map);
			rank three = poker_utility::combinations_finder::has_three(oc_map);
			rank pair = poker_utility::combinations_finder::has_pair(oc_map);

			if (flush != 'z' && straight != 'z') {
				if (straight == 'A') {
					Combination combination;
					combination.comb_name = Combination_Name::royal_flush;
					combination.comb_rank = straight;
					combination.secondary_comb_rank = 'z';
					return combination;
				}
				else {
					Combination combination;
					combination.comb_name = Combination_Name::straight_flush;
					combination.comb_rank = straight;
					combination.secondary_comb_rank = 'z';
					return combination;
				}
			}
			if (flush != 'z') {
				Combination combination;
				combination.comb_name = Combination_Name::flush;
				combination.comb_rank = flush;
				combination.secondary_comb_rank = 'z';
				return combination;
			}
			else if (straight != 'z') {
				Combination combination;
				combination.comb_name = Combination_Name::straight;
				combination.comb_rank = straight;
				combination.secondary_comb_rank = 'z';
				return combination;
			}
			else if (four != 'z') {
				Combination combination;
				combination.comb_name = Combination_Name::four_of_kind;
				combination.comb_rank = four;
				auto it = oc_map.find(four);
				if (it != oc_map.begin())
					combination.secondary_comb_rank = oc_map.begin()->first;
				else {
					combination.secondary_comb_rank = std::next(it)->first;
				}
				return combination;
			}
			else if (three != 'z' && pair != 'z') {
				Combination combination;
				combination.comb_name = Combination_Name::full_house;
				combination.comb_rank = three;
				combination.secondary_comb_rank = pair;
				return combination;
			}
			
			else if (two_pairs.first != 'z' && two_pairs.second != 'z') {
				Combination combination;
				combination.comb_name = Combination_Name::two_pair;
				combination.comb_rank = two_pairs.first;
				combination.secondary_comb_rank = two_pairs.second;
				return combination;
			}
			else if (three != 'z') {
				Combination combination;
				combination.comb_name = Combination_Name::three_of_kind;
				combination.comb_rank = three;

				auto it = oc_map.find(three);
				if(it != oc_map.begin())
					combination.secondary_comb_rank = oc_map.begin()->first;
				else {
					combination.secondary_comb_rank = std::next(it)->first;
				}
				
				return combination;
			}
			else if (pair != 'z') {
				Combination combination;
				combination.comb_name = Combination_Name::one_pair;
				combination.comb_rank = pair;

				auto it = oc_map.find(pair);
				if (it != oc_map.begin())
					combination.secondary_comb_rank = oc_map.begin()->first;
				else {
					combination.secondary_comb_rank = std::next(it)->first;
				}
				return combination;
				
			}
			else {
				Combination combination;
				combination.comb_name = Combination_Name::high_card;
				combination.comb_rank = oc_map.begin()->first;
				auto it = oc_map.find(oc_map.begin()->first);
				if (it != oc_map.begin())
					combination.secondary_comb_rank = oc_map.begin()->first;
				else {
					combination.secondary_comb_rank = std::next(it)->first;
				}
				return combination;
			}
		}

		bool player1_is_winner(const Combination& c1, const Combination& c2) {
			if (c1.comb_name > c2.comb_name) {
				return true;
			}
			else if (c1.comb_name < c2.comb_name) {
				return false;
			}
			else {
				if (rank_to_int_map.find(c1.comb_rank)->second > rank_to_int_map.find(c2.comb_rank)->second) {
					return true;
				}
				else if (rank_to_int_map.find(c1.comb_rank)->second < rank_to_int_map.find(c2.comb_rank)->second) {
					return false;
				}
				else {
					if (rank_to_int_map.find(c1.secondary_comb_rank)->second > rank_to_int_map.find(c2.secondary_comb_rank)->second) {
						return true;
					}
					else {
						return false;
					}
				}
			}
		}
	}
}


void parse_file(const std::string& _FILE_NAME) {
	std::string buffer;
	std::ifstream in(_FILE_NAME);
	if (in.is_open()) {
		int hands = 0;
		int player1_wins = 0;
		int player2_wins = 0;
		while (std::getline(in, buffer)) {
			hands++;
			poker_utility::occurencies_map oc_map_p1;
			poker_utility::occurencies_map oc_map_p2;

			std::istringstream iss(buffer);
			int card_num = 0;
			while (iss)
			{
				std::string rank_suit;
				iss >> rank_suit;
				if (!rank_suit.empty()) {
					char rank = rank_suit[0];
					char suit = rank_suit[1];
					std::string suit_str = poker_utility::suit_ch_to_str_map.find(suit)->second;

					if(card_num < 5)
						oc_map_p1[rank].push_back(suit_str);
					else 
						oc_map_p2[rank].push_back(suit_str);
					card_num++;
				}
			}

			// Determine winner
			poker_utility::combinations_finder::Combination comb_1 = poker_utility::combinations_finder::find_combination(oc_map_p1);
			poker_utility::combinations_finder::Combination comb_2 = poker_utility::combinations_finder::find_combination(oc_map_p2);

			if (poker_utility::combinations_finder::player1_is_winner(comb_1, comb_2)) {
				player1_wins++;
			}
			else {
				player2_wins++;
			}

			/** *****************************DEBUG******************************************* */
			// For nice visual debug please set VERBOSE_MODE and FIND_COMBINATION_VERBOSE at the top of this file to 1
			if (VERBOSE_MODE) {
				using namespace poker_utility;
				using namespace poker_utility::combinations_finder;

				std::cout << "Hand #" << hands << ": " << std::endl;
				
				std::cout << "Player 1: \n";
				poker_utility::print_map(oc_map_p1);

				if (FIND_COMBINATION_VERBOSE) {
					poker_utility::combinations_finder::Combination comb = poker_utility::combinations_finder::find_combination(oc_map_p1);
					std::cout << "Has combination ( " << comb_to_str_map.find(comb.comb_name)->second << ") with the main card of rank <" << comb.comb_rank << "> and side card of rank <" << comb.secondary_comb_rank << ">\n";
				}
	
				std::cout << "Player 2: \n";
				poker_utility::print_map(oc_map_p2);
				if (FIND_COMBINATION_VERBOSE) {
					poker_utility::combinations_finder::Combination comb = poker_utility::combinations_finder::find_combination(oc_map_p2);
					std::cout << "Has combination ( " << comb_to_str_map.find(comb.comb_name)->second << ") with the main card of rank <" << comb.comb_rank << "> and side card of rank <" << comb.secondary_comb_rank << ">\n";
				}

				std::cout << "-----------------------------------------" << std::endl;
			}
			/** ****************************************************************************** */
		}
		std::cout << "P1 WINS: " << player1_wins << std::endl;
		in.close();
	}
}

int main()
{
	parse_file(poker_utility::FILE_NAME);
}