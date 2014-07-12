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

#ifndef UTILITIES_H
#define UTILITIES_H

#include <cstdio>
#include <cassert>
#include <cstring>
#include <cmath>
#include <vector>
#include <iterator>
#include <algorithm>
#include <utility>
#include <numeric>
#include <functional>
#include <limits>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include "data_struct.h"

inline float PositiveHalfWaveRectify(float x) { return (x + fabs(x)) * 0.5; }

template <class InputIterator, class OutputIterator>
inline void Moments(InputIterator first, InputIterator last, OutputIterator result) {
	std::vector<float> index(std::distance(first, last));
	std::iota(std::begin(index), std::end(index), 1);

	std::vector<float> x;
	x.assign(first, last);
	auto sum = std::accumulate(first, last, 0.0);
	auto epsilon = std::numeric_limits<float>::epsilon();
	sum = sum > epsilon ? sum : epsilon;
	std::for_each(std::begin(x), std::end(x), [sum] (float& i) { i /= sum; });

	float centroid = std::inner_product(std::begin(x), std::end(x), std::begin(index), 0.0);
	float bandwidth = 0; float skew = 0; float kurtosis = 0;
	for (auto i = 0; i < x.size(); ++i) {
		float delta = index[i] - centroid;
		float tmp = delta * delta * x[i];
		bandwidth += tmp;
		tmp *= delta;
		skew += tmp;
		tmp *= delta;
		kurtosis += tmp;
	}
	bandwidth = sqrt(bandwidth);
	skew = (bandwidth > epsilon) ? (skew / pow(bandwidth, 3.0)) : 0.0;
	kurtosis = (bandwidth > epsilon) ? (kurtosis / pow(bandwidth, 4.0)) : 3.0;
	kurtosis -= 3.0;

	*result = centroid;
	*++result = bandwidth;
	*++result = skew;
	*++result = kurtosis;
}

template <class InputIterator>
inline float AveragePool(InputIterator first, InputIterator last) {
    return std::accumulate(first, last, 0.0) / std::distance(first, last);
}

template <class InputIterator>
inline float MaxPool(InputIterator first, InputIterator last) {
    return *std::max_element(first, last);
}

template <class InputIterator>
inline float L2Pool(InputIterator first, InputIterator last) {
    return sqrt(std::inner_product(first, last, first, 0.0));
}

template <class InputIterator, class OutputIterator>
inline void Quantiles(InputIterator first, InputIterator last, OutputIterator result) {
	std::vector<float> tmp(std::distance(first, last));
	std::partial_sum(first, last, std::begin(tmp));
	auto sum = tmp.back();
	std::vector<float> quantiles = {0.025, 0.25, 0.5, 0.75, 0.975};
	std::vector<float> freq;
	for (auto& q: quantiles) {
        float threshold = q * sum;
        auto it = std::find_if(std::begin(tmp), std::end(tmp), [threshold] (float i)
                               { return i >= threshold; });
        freq.push_back(std::distance(std::begin(tmp), it));
	}
	for (auto i: freq) {
        *result = i;
        ++result;
	}
	*result = freq[4] - freq[0];// 95% CI
	++result;
	*result = freq[3] - freq[1];// IQR
	++result;
}

template <class InputIterator>
inline float GiniImpurity(InputIterator first, InputIterator last) {
	float sum = std::accumulate(first, last, 0.0);
	float gini = 1;
	if (sum > std::numeric_limits<float>::epsilon()) {
	    while (first != last) {
            float p = *first / sum;
		    gini -= p * p;
		    ++first;
        }
	}
    return gini;
}

template <class InputIterator>
void ExpSmoothSpectrum(InputIterator first, InputIterator last, float gain) {
    float alpha = 1 - gain;
	float beta = gain;

	auto N = std::distance(first, last);
	auto it = last;
    float prev = *--it;
	for (auto i = 0; i < N-1; ++i) {
        --it;
        *it = (alpha * *it) + (beta * prev);
        prev = *it;
	}
    it = first;
	prev = *it;
    for (auto i = 1; i < N; ++i) {
        ++it;
        *it = (alpha * *it) + (beta * prev);
        prev = *it;
	}
}

template <class InputIterator>
void MaskSpectrum(InputIterator first, InputIterator last, int width) {
    auto max_it = std::max_element(first, last);
	if (max_it == last) return;
	auto dist = std::distance(max_it, last);
	if (dist > width) {
	    auto it = max_it;
		std::advance(it, width);
		while (++it != last) {
            *it = 0;
		}
	}
	dist = std::distance(first, max_it);
	if (dist > width) {
	    auto it = max_it;
		std::advance(it, -width);
		while (--it != first) {
            *it = 0;
		}
	}
}

template <class T>
inline T Median(std::vector<T> x) {
    assert(!x.empty());
    auto mid = x.size() / 2;
    std::nth_element(x.begin(), x.begin() + mid, x.end());
    return x.size() % 2 == 0 ? (x[mid] + x[mid-1]) / 2 : x[mid];
}

std::string NumberToString(int num);

void LogTransform(std::vector<float>& x);
void SaveFeatures(std::vector< std::vector<float> > x, std::string filename);
void SaveToFile(std::vector<float> x, std::string filename = "output.txt");
void AppendToFile(const RowData& x, const std::string& file_name);
void AppendToFile(const std::vector<float>& x, const std::string& file_name);
void AppendSparseFormat(const std::vector<float>& x, std::string y, std::string filename);

#endif // UTILITIES_H
