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

#include <limits>
#include "blob.h"

Blob::Blob() {
  Clear();
}

Blob::Blob(std::size_t id) {
  Clear();
  id_ = id;
}

void Blob::Clear() {
  id_ = 0;
  index_.first = std::numeric_limits<std::size_t>::max();
  index_.second = 0;
  area_ = 0;
  magnitude_ = 0.0;
}

void Blob::Push(std::size_t index, float mag) {
  index_.first = index < index_.first ? index : index_.first;
  index_.second = index > index_.second ? index : index_.second;
  ++area_;
  magnitude_ += mag;
}
