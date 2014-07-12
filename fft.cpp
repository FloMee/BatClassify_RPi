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

#include <cassert>
#include <complex>
#include "fft.h"

FFT::FFT() : fft_size_(0), fft_bins_(0), normalise_(0), plan_(0) {}

FFT::~FFT() {
    fftwf_destroy_plan(plan_);
}

// 4-term Blackman-Harris
float FFT::BlackmanHarris(std::vector<float>& data) {
    std::size_t N = data.size();
    float arg = 8.0 * atan(1.0) / (N-1);
    float sum = 0;
    for (std::size_t i = 0; i < N; ++i) {
        data[i] = 0.35875 - 0.48829 * cos(arg * i) + 0.14128 * cos(2 * arg *i) - \
                  0.01168 * cos(3 * arg * i);
        sum += data[i];
    }
    return sum;
}

// set up fftw plan
void FFT::SetSize(const size_t &fft_size) {
    fft_size_ = fft_size;
    window_function_.resize(fft_size_);
    float window_sum = BlackmanHarris(window_function_);
    normalise_ = 1.0 / window_sum;
    fft_bins_ = fft_size_ * 0.5;
    original_.resize(fft_size_);
    transformed_.resize(fft_size_);
    spectral_magnitude_.resize(fft_bins_);
    spectral_magnitude_.at(0) = 0.0;
    plan_ = fftwf_plan_r2r_1d( fft_size_, &original_[0], &transformed_[0], FFTW_R2HC , FFTW_PATIENT );
}

std::vector<float> FFT::Process(const std::vector<float>& audio_samples, size_t index) {
	size_t N = audio_samples.size();
	for (std::size_t i = 0; i < fft_size_; ++i, ++index) {
        original_[i] = (index < N) ? audio_samples[index] : 0.0;
        original_[i] *= window_function_[i];
    }

    fftwf_execute (plan_);
    index = fft_size_;
    for (std::size_t i = 1; i < fft_bins_; ++i) {
        spectral_magnitude_[i] = std::abs(std::complex<float>(transformed_[i], transformed_[--index])) * normalise_;
    }
    return spectral_magnitude_;
}

size_t FFT::Size() const {
    return fft_size_;
}

std::vector<float> FFT::Spectrum() const {
    return spectral_magnitude_;
}
