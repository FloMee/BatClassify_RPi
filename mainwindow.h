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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "blob.h"
#include "decision_forest.h"
#include "audiodecoder.h"
#include "STFT.h"
#include "blob-finder.h"
#include <QMainWindow>

struct Audio {
    std::vector<float> samples;
    std::size_t format, channels, samplerate;
    Audio() : format(0), channels(1), samplerate(0) {}
};

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

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void SourceButton();
    void SourceButton2();
    void DestinationButton();
    void DestinationButton2();
    void RunButton();
    void RunButton2();

private:
    void Process();
    void Process2();
    float SumSquare(const std::vector<float>& x);
    void DeInterleave(std::vector<float>& stereo, std::size_t channel, std::vector<float>& mono);
    void WriteAudio(Audio audio, std::string file, std::string timestamp);
    void AppendResults(RecordingResults& recording, std::fstream& ofs);
    void WriteErrors(const std::vector<std::string>& error_log, std::string outfile);
    DateTime ReadTime(std::string filename);
    std::string Timestamp(float seconds);
    std::unordered_map<int, Blob> PriorityBlobs(std::unordered_map<int, Blob>& blobs, std::size_t n = 4);

    Ui::MainWindow *ui;
    DecisionForest Bbar, Myotis, NSL, Pipistrellus, Paur, Rhinolophus;
    AudioDecoder audio;
    BlobFinder blobfinder;
    STFT stft;
};

#endif // MAINWINDOW_H
