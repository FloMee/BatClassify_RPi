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

#ifndef BLOB_H__
#define BLOB_H__

#include <utility>

class Blob {
 public:
  Blob();
  Blob(std::size_t id);

  inline std::size_t ID() const { return id_; }
  inline std::pair<std::size_t, std::size_t> MinMaxIndex() const { return index_; }
  inline std::size_t Area() const { return area_; }
  inline float Magnitude() const { return magnitude_; }

  void Clear();
  void Push(std::size_t index, float mag = 0.0);

 private:
  std::size_t id_;
  std::pair<std::size_t, std::size_t> index_;
  std::size_t area_;
  float magnitude_;
};

#endif // BLOB_H__
