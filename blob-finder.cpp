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


#include <cmath>
#include <sstream>
#include <limits>
#include "blob-finder.h"

const int BlobFinder::UNSET  =  0;
const int BlobFinder::MARKED = -1;

std::pair<int,int> BlobFinder::Tracer(mImage& data, int S, int p) {
	for (int d=0; d<8; ++d) {
		int q = (p + d) % 8;
		int T = S + pos_[q];

		// Make sure we are inside image
		if (T < 0 || T >= max_)
			continue;

		if (data.getPixel(T) >= 0.00001)
			return std::pair<int,int>(T,q);

		label_[T] = MARKED;
	}

	// No move
	return std::pair<int,int>(S,-1);
}

std::unordered_map<int, Blob> BlobFinder::Extraction(mImage& data) {
	// width/height switched
	w_ = data.Height(), h_ = data.Width();
	max_ = w_ * h_;

    pos_.resize(8);
    pos_[0] = 1;
    pos_[1] = w_+1;
    pos_[2] = w_;
    pos_[3] = w_-1;
    pos_[4] = -1;
    pos_[5] = -w_-1;
    pos_[6] = -w_;
    pos_[7] = -w_+1;

	label_.clear();
	label_.resize(max_, 0);
	c_ = 1;// blob index
	blobmap.clear();

	// We change the border to be white. We could add a pixel around
	// but we are lazy and want to do this in place.
	// Set the outer rows/cols to background
    for (int j = 0; j < w_; ++j) {
        data.setPixel(j, 0.0);
        data.setPixel((h_-1) * w_ + j, 0.0);
    }
    for (int i = 0; i < h_; ++i) {
        data.setPixel(i * w_, 0.0);
        data.setPixel(i * w_ + (w_-1), 0.0);
    }

	Extract(data);
	return blobmap;
}

void BlobFinder::ContourTracing(mImage& data, int S, int C, bool external) {
	int p = external ? 7 : 3;

	// Find out our default next pos (from S)
	std::pair<int,int> tmp = Tracer(data, S, p);
	int T2 = tmp.first;
	int q  = tmp.second;

	label_[S] = C;
	blobmap[C].Push(S, data.getPixel(S));

	// Single pixel check
	if (T2 == S)
		return;

	int Tnext   = T2;
	int T       = T2;

	while ( T != S || Tnext != T2 ) {
		label_[Tnext] = C;
        blobmap[C].Push(Tnext, data.getPixel(Tnext));
		T = Tnext;
		p = (q + 5) % 8;
		std::pair<int,int> tmp = Tracer(data, T, p);
		Tnext = tmp.first;
		q     = tmp.second;
	}
}

void BlobFinder::Extract(mImage& data) {
	int y = 1; // We start at 1 to avoid looking above the image
	do {
		int x = 0;
		do {
			int offset = y * w_ + x;
			// We skip white pixels or previous labeled pixels
			if (data.getPixel(offset) < 0.00001)
				continue;

			//bool traced = false;

			// Step 1 - P not labelled, and above pixel is white
			if (data.getPixel(offset-w_) < 0.00001 && label_[offset] == UNSET) {
				// P must be external contour
				ContourTracing(data, offset, c_, true);
				++c_;
				//traced = true;
			}

			// Step 2 - Below pixel is white, and unmarked
			if (data.getPixel(offset+w_) < 0.00001 && label_[offset+w_] == UNSET) {
				// Use previous pixel label, unless this is already labelled
				int n = label_[offset-1];
				if (label_[offset] != UNSET)
					n = label_[offset];

				// P must be a internal contour
				ContourTracing(data, offset, n, false);
				//traced = true;
			}

			// Step 3 - Not dealt with in previous two steps
			if (label_[offset] == UNSET) {
				int n = label_[offset-1];
				// Assign P the value of N
				label_[offset] = n;
				blobmap[n].Push(offset, data.getPixel(offset));
			}

		} while (++x < w_);
	} while (++y < (h_-1)); // We end one before the end to to avoid looking below the image
}

void BlobFinder::Mask(mImage& data, std::pair<int, Blob> blob, mImage& segment) {
    int height = data.Height();
    auto mmindex = blob.second.MinMaxIndex();
    int index = height * floor(mmindex.first / (float)height);
    int frames = 1 + ceil((mmindex.second - mmindex.first) / (float)height);
    int counter = 0;
    segment.Create(frames, height);
    for (auto i=0; i<frames; ++i) {
        for (auto j=0; j<height; ++j) {
            if (label_[counter+index] == blob.first) segment.setPixel(counter, data.getPixel(counter+index));
            ++counter;
        }
    }
}
