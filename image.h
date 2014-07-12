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

#ifndef MIMAGE_H
#define MIMAGE_H

#include <cmath>
#include <vector>
#include <string>
#include <QString>
#include <iostream>
#include <fstream>
#include <sstream>

class mImage {
    private:
        std::size_t width, height, N;
        std::vector<float> pixels;

    public:
        mImage(size_t w = 0.0, size_t h = 0.0);
        mImage(size_t w, size_t h, float val);
        mImage(mImage& cSource);
        mImage& operator=( mImage& rhs );
        void Create(size_t w, size_t h);
        void Clear();
        void SegmentFeatures(std::vector<float>& result);
        mImage Extract(size_t from, size_t to);
        void Load(std::string filename);
        void Save(QString filepath, QString filename);
        void AppendToFile(std::string filename);
        void VerticalMask(size_t length = 7);
        void ContrastBoost();
        void LogCompress(float compression = 20);
        void BackgroundSubtract();
        void Blur();
        void PostMask();

        float getPixel(int x, int y) { return pixels[x*height + y]; };
        float getPixel(int index) { return pixels[index]; };
        void setPixel(int x, int y, float value) { pixels[x*height + y] = value; };
        void setPixel(int index, float value) { pixels[index] = value; };
        size_t Width() { return width; };
        size_t Height() { return height; };
        size_t Size() { return pixels.size(); };
        std::vector<float> Pixels() { return pixels; };
};

#endif
