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

#include "STFT.h"

STFT::STFT() : fftSize_(512), stepSize_(128) {
    fft.SetSize(fftSize_);
}

STFT::STFT(std::size_t fftSize, std::size_t stepSize) {
    fft.SetSize(fftSize);
    fftSize_ = fftSize;
    stepSize_ = stepSize;
}

void STFT::CreateSpectrogram(const std::vector<float>& samples, mImage& spectro) {
	std::size_t height = (fftSize_ * 0.25);
	std::size_t width = ceil(samples.size() / stepSize_);
	spectro.Create(width, height);
    std::vector<float> spectrum(height);
    int index = 0;
	for (std::size_t x = 0; x < width; ++x, index += stepSize_) {
		spectrum = fft.Process(samples, index);
		for (int y=0; y<12; ++y) spectro.setPixel(x, y, 0.0);
		for (std::size_t y=12; y<height; ++y) {
		    spectro.setPixel(x, y, spectrum[y]);
		}
	}
}
