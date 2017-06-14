/*************************************************************************

  Copyright 2011-2014 Chris Scott (fbscds@gmail.com)

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with This program.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/

#include <iostream>
#include <fstream>
#include "decision_forest.h"

void DecisionForest::Read(std::string filename) {
    std::ifstream ifs(filename.c_str());
    if (!ifs.is_open()) {
        std::cerr << "failed to open " << filename << '\n';
        return;
    }
	std::size_t forest_size;
	ifs >> forest_size;
	forest_.clear();
	forest_.resize(forest_size);
	for (std::size_t i = 0; i < forest_size; ++i) {
	    forest_[i].Read(ifs);
	}
	ifs.close();
}

// sum posterior probability from each tree and normalise
std::unordered_map<std::string, float> DecisionForest::Predict(const std::vector<float>& data) {
	std::unordered_map<std::string, float> posterior_prob;
	for (auto& tree: forest_) {
	    tree.Predict(data, posterior_prob);
	}
	float sum = 0;
	for (auto i: posterior_prob) {
	    sum += i.second;
	}
	for (auto& i: posterior_prob) {
	    i.second /= sum;
	    i.second = i.second > 0.9999999 ? 0.9999999 : i.second;
	    i.second = i.second < 0.0000001 ? 0.0000001 : i.second;
	}
	return posterior_prob;
}
