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

#ifndef BLOB_FINDER_H__
#define BLOB_FINDER_H__

#include <climits>
#include <algorithm>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <utility>
#include <unordered_map>
#include "blob.h"
#include "image.h"

// Algorithm (Chang et al. 2003)
// "A linear-time component labeling algorithm using contour tracing technique"

// Code adapted from
// https://github.com/bramp/Connected-component-labelling/blob/master/connected-component-labelling.js
// Andrew Brampton (2011)

class BlobFinder {
public:
    BlobFinder() : w_(0), h_(0), max_(0), c_(0) {}
	std::unordered_map<int, Blob> Extraction(mImage& data);
	void Mask(mImage& data, std::pair<int, Blob> blob, mImage& segment);

private:
	std::pair<int,int> Tracer(mImage& data, int S, int p);
	void ContourTracing(mImage& data, int S, int C, bool external);
	void Extract(mImage& data);

	int w_, h_, max_, c_;
	std::vector<int> label_;
	std::vector<int> pos_;
	const static int UNSET;
	const static int MARKED;
	std::unordered_map<int, Blob> blobmap;
};

#endif // BLOB_FINNDER_H__
