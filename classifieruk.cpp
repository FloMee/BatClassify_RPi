/*************************************************************************

  Copyright 2011-2014 Chris Scott (fbscds@gmail.com)
  Copyright 2017 Sam Duff (sam.duff@elexonelectronics.com)

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

#include "classifieruk.h"
#include <QFileInfo>
#include <time.h>
#include <queue>

ClassifierUK::ClassifierUK()
{
    Bbar.Read("/home/pi/models/Bbar.forest");
    Myotis.Read("/home/pi/models/Myotis.forest");
    NSL.Read("/home/pi/models/NSL.forest");
    Pipistrellus.Read("/home/pi/models/Pipistrellus.forest");
    Paur.Read("/home/pi/models/Paur.forest");
    Rhinolophus.Read("/home/pi/models/Rhinolophus.forest");
}

RecordingResults ClassifierUK::AutoIdFile(QString file, bool spectrograms)
{
    QFileInfo fileInfo(file);
    QString fp = fileInfo.absoluteFilePath();
    std::string filepath = fp.toLocal8Bit().constData();
    QString fpath = fileInfo.path();
    QString filename = fileInfo.baseName();

    RecordingResults recording;
    recording.filepath = fpath.toLocal8Bit().constData();
    recording.filename = filename.toLocal8Bit().constData();

    if (!audio.FileSupported(filepath)) {
        //error_log.push_back(filepath);
        return recording;
    }
    auto date_time = ReadTime(filepath);
    recording.date = date_time.date;
    recording.time = date_time.time;

    std::vector<float> samples;
    audio.ReadFile(filepath, samples);
    float duration = samples.size() / 500000.0;

    mImage spectro;
    stft.CreateSpectrogram(samples, spectro);
    samples.clear();

    spectro.Blur();
    spectro.BackgroundSubtract();
    spectro.PostMask();
    spectro.ContrastBoost();

    auto blobmap = blobfinder.Extraction(spectro);
    int k = (duration + 1) * 4;
    auto blobs = PriorityBlobs(blobmap, k);
    for (auto& blob: blobs) {
        if (blob.second.Area() > 40) {
            mImage segment;
            blobfinder.Mask(spectro, blob, segment);
            if (segment.Width() < 6) continue;
            segment.LogCompress(20.0);
            if (spectrograms) segment.Save(fpath, fileInfo.baseName());
            std::vector<float> features;
            segment.SegmentFeatures(features);

            auto probs = Bbar.Predict(features);
            std::vector<std::string> labels = {"Bbar"};
            for (auto label: labels) {
                recording.results[label] = probs[label] > recording.results[label] ? probs[label] : recording.results[label];
            }

            probs = Myotis.Predict(features);
            labels = {"Malc", "Mbec", "MbraMmys", "Mdau", "Mnat"};
            for (auto label: labels) {
                recording.results[label] = probs[label] > recording.results[label] ? probs[label] : recording.results[label];
            }

            probs = NSL.Predict(features);
            labels = {"NSL"};
            for (auto label: labels) {
                recording.results[label] = probs[label] > recording.results[label] ? probs[label] : recording.results[label];
            }

            probs = Paur.Predict(features);
            labels = {"Paur"};
            for (auto label: labels) {
                recording.results[label] = probs[label] > recording.results[label] ? probs[label] : recording.results[label];
            }

            probs = Pipistrellus.Predict(features);
            labels = {"Ppip", "Ppyg"};
            for (auto label: labels) {
                recording.results[label] = probs[label] > recording.results[label] ? probs[label] : recording.results[label];
            }

            probs = Rhinolophus.Predict(features);
            labels = {"Rfer", "Rhip"};
            for (auto label: labels) {
                recording.results[label] = probs[label] > recording.results[label] ? probs[label] : recording.results[label];
            }
        }
    }
    return recording;
}

DateTime ClassifierUK::ReadTime(std::string filename) {
    DateTime result;
    std::fstream ifs( filename.c_str(), std::ios::in | std::ios::binary);
    if (ifs.is_open() ){
        ifs.seekg( 240, std::ios::beg );// 240 for D500X - checked with HexEdit
        std::string ID("D500X");
        std::string str;
        str.resize(ID.size());
        ifs.read(&str[0], ID.size());
        if (str == ID) {
            ifs.seekg( 224, std::ios::beg ); // 224 for date and time
            char chunk[16];
            ifs.get(chunk, 16);
            int date_time[16];
            int day;
            for (int i=0; i<16; ++i) {
                std::stringstream ss;
                ss << std::hex << chunk[i];
                ss >> day;
                date_time[i] = day;
            }

            // Unix time = the number of seconds elapsed since 00:00 hours, Jan 1, 1970 UTC
            int dec[2] = {1, 10};
            int tmp = 2000 + date_time[0]*dec[1] + date_time[1]*dec[0];
            struct tm dt;
            dt.tm_year = tmp - 1900; // year - 1900 (years since 1900!)
            tmp = date_time[2]*dec[1] + date_time[3]*dec[0];
            dt.tm_mon = tmp - 1; // month - 1 (months since January 0-11)
            tmp = date_time[4]*dec[1] + date_time[5]*dec[0];
            dt.tm_mday = tmp;
            dt.tm_hour = date_time[7]*dec[1] + date_time[8]*dec[0];
            dt.tm_min = date_time[10]*dec[1] + date_time[11]*dec[0];
            dt.tm_sec = date_time[13]*dec[1] + date_time[14]*dec[0];
            dt.tm_isdst = -1; // less than zero if the information is not available
            dt.tm_wday = 0;
            dt.tm_yday = 0;
            mktime(&dt);
            ifs.close();

            std::stringstream date;
            if (dt.tm_mday < 10) date << "0";
            date << dt.tm_mday << "/";
            if (dt.tm_mon+1 < 10) date << "0";
            date << dt.tm_mon+1 << "/" << (dt.tm_year+1900);
            result.date = date.str();

            std::stringstream time;
            if (dt.tm_hour < 10) time << "0";
            time << dt.tm_hour << ":";
            if (dt.tm_min < 10) time << "0";
            time << dt.tm_min << ":";
            if (dt.tm_sec < 10) time << "0";
            time << dt.tm_sec;
            result.time = time.str();
        }
    }
    return result;
}

std::unordered_map<int, Blob> ClassifierUK::PriorityBlobs(std::unordered_map<int, Blob>& blobs, std::size_t n) {
    n = n > blobs.size() ? blobs.size() : n;
    typedef std::pair<float, int> Element;
    std::priority_queue< Element, std::vector<Element>, std::greater<Element> > priority_blobs;
    auto first = std::begin(blobs);
    auto last = std::begin(blobs);
    std::advance(last, n);
    while (first != last) {
        Element element = Element(first->second.Magnitude(), first->first);
        priority_blobs.push(element);
        ++first;
    }

    last = std::end(blobs);
    while (first != last) {
        Element element = Element(first->second.Magnitude(), first->first);
        if (priority_blobs.top() < element) {
            priority_blobs.pop();
            priority_blobs.push(element);
        }
        ++first;
    }

    std::unordered_map<int, Blob> result;
    for (std::size_t i = 0; i < n; ++i) {
        auto blob = blobs[priority_blobs.top().second];
        result[priority_blobs.top().second] = blob;
        priority_blobs.pop();
    }
    return result;
}
