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

#ifndef FFT_H
#define FFT_H

#include <cmath>
#include <vector>
#include "fftw3.h"

class FFT {
 public:
  FFT();
  ~FFT();
  void SetSize(const size_t &fftSize);
  std::vector<float> Process(const std::vector<float>& samples, size_t index);
  size_t Size() const;
  std::vector<float> Spectrum() const;

 private:
  float BlackmanHarris(std::vector<float>& data);
  std::vector<float> window_function_;
  size_t fft_size_, fft_bins_;
  std::vector<float>  original_;
  std::vector<float>  transformed_;
  std::vector<float> spectral_magnitude_;
  float normalise_;
  fftwf_plan plan_;
};

#endif // FFT_H
