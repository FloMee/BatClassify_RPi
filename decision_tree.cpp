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

#include "decision_tree.h"

void DecisionTree::Read(std::ifstream& ifs) {
    int N, num_non_null;
    ifs >> N >> num_non_null;
    nodes_.resize(num_non_null);
    index_lut_.clear();
    for (auto i = 0; i < num_non_null; ++i) {
		int index;
		Node node;
		ifs >> index >> node.col >> node.threshold >> node.leaf;
        index_lut_[index] = i;
        nodes_[i] = node;
        if (!node.leaf) continue;

		int num_labels;
		ifs >> num_labels;

		for (auto j = 0; j < num_labels; ++j) {
            std::string label;
            float p;
            ifs >> label >> p;
            nodes_[i].probability[label] = p;
        }
    }
}

int DecisionTree::Search(const std::vector<float>& data) {
    int index = 0, node_index = 0;
    for (std::size_t i = 0; i < nodes_.size(); ++i) {
        node_index = index_lut_[index];
        if (nodes_[node_index].leaf) break;
        index = data[nodes_[node_index].col] < nodes_[node_index].threshold ? (index*2+1) : (index*2+2);
    }
    return node_index;
}

void DecisionTree::Predict(const std::vector<float>& data, std::unordered_map<std::string, float>& result) {
    auto probs = nodes_[Search(data)].probability;
	for (auto p: probs) {
		result[p.first] += p.second;
	}
}
