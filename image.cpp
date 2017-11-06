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

#include <sstream>
#include <cstring>
#include <iterator>
#include <utility>
#include <limits>
#include <functional>
//#include <png.hpp>
#include <QImage>
#include "running_stat.h"
#include "utilities.h"
#include "image.h"

const float PI = 4.0 * atan(1.0);

mImage::mImage(size_t w, size_t h) {
    width = w;
    height = h;
    N = width*height;
    pixels.resize(N);
}

mImage::mImage(size_t w, size_t h, float val) {
    width = w;
    height = h;
    N = width*height;
    pixels.resize(N, val);
}

// Copy constructor
mImage::mImage(mImage& cSource) {
    width = cSource.Width();
    height = cSource.Height();
    N = width*height;
    pixels = cSource.Pixels();
}

mImage& mImage::operator=( mImage& rhs ) {
    std::swap( width, rhs.width );
    std::swap( height, rhs.height );
    std::swap( N, rhs.N );
    std::swap( pixels, rhs.pixels );
    return *this;
}

void mImage::Create(size_t w, size_t h) {
    width = w;
    height = h;
    N = width*height;
    pixels.clear();
    pixels.resize(width*height, 0.0);
}

void mImage::Clear() {
    pixels.clear();
    width = 0;
    height = 0;
    N = 0;
}

void mImage::SegmentFeatures(std::vector<float>& result) {
	std::vector<float> power_spectrum(height, 0);
	std::vector<float> temporal_envelope(width, 0);
	std::vector<float> histogram(2*height, 0);
    float area = 0;
    int prev_peak = 0;
    for (std::size_t x = 0; x < width; ++x) {
        size_t offset = x * height;
        std::vector<float> spectrum(height, 0);
		float sum = 0;
		for (std::size_t y = 0; y < height; ++y) {
            spectrum[y] = pixels[offset+y];
            sum += spectrum[y];
        }
		temporal_envelope[x] = sum;
		int cur_peak = 0;
		if (sum > std::numeric_limits<float>::epsilon()) {
            MaskSpectrum(std::begin(spectrum), std::end(spectrum), 8);
			sum = 0;
			float centroid = 0;
			for (std::size_t y = 1; y < height-1; ++y) {
				power_spectrum[y] += spectrum[y];
				area += spectrum[y] > 0.00001 ? 1 : 0;
				sum += spectrum[y];
                centroid += spectrum[y]*y;
			}
			centroid /= sum;
            cur_peak = round(centroid);
		}
		if (cur_peak > 0 && prev_peak > 0) {
            int bin = (cur_peak-prev_peak)+height;
            //histogram[bin] += sum;
            histogram[bin] += spectrum[cur_peak];
            histogram[bin-1] += spectrum[cur_peak-1];
            histogram[bin+1] += spectrum[cur_peak+1];
		}
		prev_peak = cur_peak;
    }

    // Segment Features:
    result.push_back(area);

	// frequency
	Moments(std::begin(power_spectrum), std::end(power_spectrum), std::back_inserter(result));
	auto freq = result[0];
	auto bw = result[1] > 1.0 ? result[1] : 1.0;
	float Q = freq / bw;
	result.push_back(Q);
	result.push_back(GiniImpurity(std::begin(power_spectrum), std::end(power_spectrum)));
	Quantiles(std::begin(power_spectrum), std::end(power_spectrum), std::back_inserter(result));

	// temporal
	Moments(std::begin(temporal_envelope), std::end(temporal_envelope), std::back_inserter(result));
	result.push_back(GiniImpurity(std::begin(temporal_envelope), std::end(temporal_envelope)));

    // gradient
    Moments(std::begin(histogram), std::end(histogram), std::back_inserter(result));
	result.push_back(GiniImpurity(std::begin(histogram), std::end(histogram)));
}

// trimmed mean background subtraction
void mImage::ContrastBoost() {
    float scale = 1.0 / (height-7);
    std::vector<float> spectrum(height);
    for (size_t x=0; x<width; ++x) {
        std::size_t offset = x*height;
        pixels[offset] = pixels[offset+height-1] = 0;
        pixels[offset+1] = pixels[offset+height-2] = 0;
        pixels[offset+2] = pixels[offset+height-3] = 0;
        auto first = pixels.begin()+offset;
        auto last = first+height;
        spectrum.assign(first, last);
        float sum = std::accumulate(spectrum.begin(), spectrum.end(), 0.0);
        for (std::size_t y=3; y<height-3; ++y) {
            pixels[offset+y] -= (sum - spectrum[y] -
                                 spectrum[y-1] - spectrum[y+1] -
                                 spectrum[y+2] - spectrum[y-2] -
                                 spectrum[y+3] - spectrum[y-3]) * scale;
            pixels[offset+y] = pixels[offset+y] < 0.0 ? 0.0 : pixels[offset+y];
        }
    }
}

void mImage::LogCompress(float compression) {
    //float scale = 1.0 / log10(compression * 1.0 + 1.0);
    size_t i;
    for (i=0; i<N; ++i) {
        pixels[i] = log10(compression * pixels[i] + 1);// * scale;
    }
}

void mImage::BackgroundSubtract() {
    std::vector<RunningStat> stats(height);
    float C = 1.5;
    for (size_t x=0; x<width; ++x) {
        std::size_t offset = x*height;
        for (std::size_t y=0; y<height; ++y) {
            float tmp = pixels[offset+y];
            pixels[offset+y] = PositiveHalfWaveRectify(tmp - C * stats[y].Mean());
            stats[y].Push(tmp);
        }
    }
}

void mImage::Blur() {
    std::vector<float> spectrum(height, 0);
    for (size_t x=0; x<width; ++x) {
        std::size_t offset = x*height;
        for (std::size_t y=0; y<height; ++y) {
            spectrum[y] = pixels[offset+y];
        }
        for (std::size_t y=1; y<height-1; ++y) {
            pixels[offset+y] = 2 * spectrum[y] + spectrum[y-1] + spectrum[y+1];
        }
    }
}

void mImage::PostMask() {
    float alpha = 0.9;
    float beta = 1 - alpha;
    std::vector<float> threshold(height, 0);
    for (size_t x=0; x<width; ++x) {
        std::size_t offset = x*height;
        for (std::size_t y=0; y<height; ++y) {
            float tmp = pixels[offset+y];
            float decayed = alpha * threshold[y] + beta * tmp;
            threshold[y] = tmp > decayed ? tmp : decayed;
            pixels[offset+y] = (2.0 * tmp) < threshold[y] ? 0.0 : tmp;
        }
    }
}

mImage mImage::Extract(size_t from, size_t to) {
    mImage roi(1+to-from, height);
    size_t index = 0;
    for (size_t x=from; x<=to; ++x) {
        size_t offset = x*height;
        for (size_t y=0; y<height; ++y) {
            roi.setPixel(index, pixels[offset+y]);
            ++index;
        }
    }
    return roi;
}

void mImage::Load(std::string filename) {
    //png::image< png::rgb_pixel > image(filename);
    //width = image.get_width();
    //height = image.get_height();
    //N = width*height;
    //pixels.resize(N);
    //for (size_t x=0; x<width; ++x) {
    //    int row = height-1;
    //    for (size_t y=0; y<height; ++y, --row) {
    //        pixels[x*height+y] = image[row][x].red / 255.0;
    //    }
    //}
}

void mImage::Save(QString filepath, QString filename) {
    // counter for file naming
    static int count = 0;

    // find peak magnitude to normalise image
    float peak = 0;
    for (size_t x=0; x<width; ++x) {
        for (size_t y=0; y<height; ++y) {
            peak = pixels[x*height+y] > peak ? pixels[x*height+y] : peak;
        }
    }
    peak = peak > 0.000001 ? 1.0 / peak : 1.0;
    peak *= 255;

    QImage image(width, height, QImage::Format_RGB32);
    QRgb value;
    for (size_t x=0; x<width; ++x) {
        int row = height - 1;
        for (size_t y=0; y<height; ++y, --row) {
            float mag = pixels[x*height+y] * peak;
            value = qRgb(mag, mag, mag);
            image.setPixel(x, row, value);
        }
    }

    // unique name for each image
    QString qs = filepath;
    qs.append("/");
    qs.append(filename);
    qs.append("--");
    qs.append(QString("%1").arg(++count));
    qs.append(".jpg");
    image.save(qs);
}

void mImage::AppendToFile(std::string filename) {
    FILE* fout = fopen(filename.c_str(), "a");
    size_t N = height*0.5;
    for (size_t x=0; x<width; ++x) {
        size_t offset = x*height;
        for (size_t y=0; y<N; ++y) {
            fprintf(fout, "%f", pixels[offset+y]);
            if (y < (N-1)) fprintf(fout, "\t");
        }
        fprintf(fout, "\n");
    }
    fclose(fout);
}
