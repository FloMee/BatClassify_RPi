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

#include <cstdlib>
#include <sstream>
#include <fstream>
#include "utilities.h"

std::string NumberToString(int num)
{
    std::ostringstream ss;
    ss << num;
    return ss.str();
}

void AppendToFile(const RowData& x, const std::string& file_name)
{
    std::fstream file;
    file.open (file_name.c_str(), std::fstream::out | std::fstream::app);
    if (!file.is_open())
    {
        std::cerr << "Error opening output file: " << file_name << std::endl;
        exit (EXIT_FAILURE);
    }
    file << x.label << '\t';
    file << x.factor << '\t';
    for (auto i: x.values)
    {
        file << i << '\t';
    }
    file << '\n';
    file.close();
}

void AppendToFile(const std::vector<float>& x, const std::string& file_name)
{
    std::fstream file;
    file.open (file_name.c_str(), std::fstream::out | std::fstream::app);
    if (!file.is_open())
    {
        std::cerr << "Error opening output file: " << file_name << std::endl;
        exit (EXIT_FAILURE);
    }
    for (auto i: x)
    {
        file << i << '\t';
    }
    file << '\n';
    file.close();
}
