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

#ifndef DECISION_TREE_H__
#define DECISION_TREE_H__

#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>

struct Node {
    int col;
    float threshold;
    std::unordered_map<std::string, float> probability;
    bool leaf, null;
    Node() : col(0), threshold(0), leaf(false), null(true) {}
};

class DecisionTree {
  public:
	void Read(std::ifstream& ifs);
	void Predict(const std::vector<float>& data, std::unordered_map<std::string, float>& result);

  protected:
	int Search(const std::vector<float>& data);
	std::vector<Node> nodes_;
	std::unordered_map<int, int> index_lut_;
};

#endif  // DECISION_TREE_H__
