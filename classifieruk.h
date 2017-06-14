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
