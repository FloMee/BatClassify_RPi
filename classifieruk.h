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

#ifndef CLASSIFIERUK_H
#define CLASSIFIERUK_H

#include "blob.h"
#include "decision_forest.h"
#include "audiodecoder.h"
#include "STFT.h"
#include "blob-finder.h"

struct DateTime {
    std::string date;
    std::string time;
    DateTime() : date("NA"), time("NA") {}
};

struct RecordingResults {
    std::string filepath, filename, date, time;
    std::unordered_map<std::string, float> results;
    RecordingResults() : filepath("NA"), filename("NA"), date("NA"), time("NA") {}
};

class ClassifierUK
{
public:
    ClassifierUK();

    RecordingResults AutoIdFile(QString file, bool spectrograms);

private:
    static DateTime ReadTime(std::string filename);
    std::unordered_map<int, Blob> PriorityBlobs(std::unordered_map<int, Blob>& blobs, std::size_t n = 4);

    DecisionForest Bbar, Myotis, NSL, Pipistrellus, Paur, Rhinolophus;
    AudioDecoder audio;
    BlobFinder blobfinder;
    STFT stft;
};

#endif // CLASSIFIERUK_H
