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

#ifndef DECISION_FOREST_H__
#define DECISION_FOREST_H__

#include <vector>
#include <string>
#include <unordered_map>
#include "decision_tree.h"

class DecisionForest {
  public:
	void Read(std::string filename);
	std::unordered_map<std::string, float> Predict(const std::vector<float>& data);

  protected:
	std::vector<DecisionTree> forest_;
};

#endif  // DECISION_FOREST_H__
