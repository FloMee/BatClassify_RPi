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

#include <iostream>
#include "audiodecoder.h"

bool AudioDecoder::FileSupported(std::string filename) {
    bool flag = true;
    SndfileHandle snd_file = SndfileHandle(filename.c_str(), SFM_READ);
    if (snd_file.error() == 0) {
        auto duration = snd_file.frames() / snd_file.samplerate();
        if (duration > 90) flag = false;
        if (snd_file.channels() != 1) flag = false;
    }
    else {
        flag = false;
    }

    return flag;
}

void AudioDecoder::ReadFile(std::string filename, std::vector<float>& audio) {
	SndfileHandle snd_file = SndfileHandle(filename.c_str(), SFM_READ);
	try {
		audio.resize(snd_file.frames());
	}
	catch (std::bad_alloc const&) {
		std::cerr << "memory allocation fail: " << filename << '\n';
	}
	snd_file.read(audio.data(), snd_file.frames());
    std::size_t samplerate = snd_file.samplerate();
	if (samplerate < 192000) samplerate *= 10;// assume time-expanded file
	std::size_t internal_rate = 500000;// process all files at 500 kHz sampling rate
	if (samplerate != internal_rate) {
		float ratio = static_cast<float>(internal_rate) / samplerate;
		Resample(audio, ratio);
	}
}

void AudioDecoder::Resample(std::vector<float>& audio, float ratio) {
    int converter = SRC_SINC_FASTEST;
    std::vector<float> original = audio;
    long output_frames = (original.size() * ratio) + 1;
	audio.resize(output_frames);

    SRC_DATA src_data;
    src_data.data_in = original.data();
    src_data.input_frames = original.size();
    src_data.data_out = audio.data();
    src_data.output_frames = output_frames;
    src_data.src_ratio = ratio;
    src_simple(&src_data, converter, 1);
}
