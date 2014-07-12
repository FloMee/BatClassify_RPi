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

#ifndef STFT_H
#define STFT_H

#include <vector>
#include "fft.h"
#include "image.h"

class STFT {

public:
    STFT();
    STFT(std::size_t fftSize, std::size_t stepSize);
    void CreateSpectrogram(const std::vector<float>& samples, mImage& spectro);

private:
	FFT fft;
	std::size_t fftSize_, stepSize_;
};

#endif // STFT_H
