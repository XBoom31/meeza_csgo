#pragma once
#include "options.hpp"

#include <typeinfo>

#include <fstream>

namespace configs {

	void load(std::string filename);
	void save(std::string filename);
	void killed();
	int get_st(int weapon_index);
	void save_st();
	void load_st();

}