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

#include <ctime>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <utility>
#include <numeric>
#include <queue>
#include "utilities.h"
#include "image.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QStatusBar>

const std::vector<std::string> class_labels = {"Bbar", "Malc", "Mbec", "MbraMmys", "Mdau", "Mnat", "NSL", "Paur", "Ppip", "Ppyg", "Rfer", "Rhip"};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // add status bar message
    //statusBar()->showMessage("Extract and classify calls from recordings");

    connect(ui->source_button, SIGNAL(released()), this, SLOT(SourceButton()));
    connect(ui->destination_button, SIGNAL(released()), this, SLOT(DestinationButton()));
    connect(ui->run_button, SIGNAL(released()), this, SLOT(RunButton()));
    connect(ui->source_button_2, SIGNAL(released()), this, SLOT(SourceButton2()));
    connect(ui->destination_button_2, SIGNAL(released()), this, SLOT(DestinationButton2()));
    connect(ui->run_button_2, SIGNAL(released()), this, SLOT(RunButton2()));

    Bbar.Read("../models/Bbar.forest");
    Myotis.Read("../models/Myotis.forest");
    NSL.Read("../models/NSL.forest");
    Pipistrellus.Read("../models/Pipistrellus.forest");
    Paur.Read("../models/Paur.forest");
    Rhinolophus.Read("../models/Rhinolophus.forest");
}

void MainWindow::SourceButton() {
    QString path = QFileDialog::getExistingDirectory(0, "", QString(), QFileDialog::ShowDirsOnly);
    ui->source_edit->setText(path);
}

void MainWindow::DestinationButton() {
    QString path = QFileDialog::getExistingDirectory(0, "", QString(), QFileDialog::ShowDirsOnly);
    ui->destination_edit->setText(path);
}

void MainWindow::RunButton() {
    bool success = true;

    if (ui->source_edit->text().isEmpty()) {
        success = false;
        QMessageBox::warning(this, tr("Error"),
                                        tr("Source directory must be selected"),
                                        QMessageBox::Ok);
    }
    else {
        if (!QDir(ui->source_edit->text()).exists()) {
            success = false;
            QMessageBox::warning(this, tr("Error"),
                                            tr("Invalid source directory"),
                                            QMessageBox::Ok);
        }
    }
    if (ui->destination_edit->text().isEmpty()) {
        success = false;
        QMessageBox::warning(this, tr("Error"),
                                        tr("Destination directory must be selected"),
                                        QMessageBox::Ok);
    }
    else {
        if (!QDir(ui->destination_edit->text()).exists()) {
            success = false;
            QMessageBox::warning(this, tr("Error"),
                                            tr("Invalid destination directory"),
                                            QMessageBox::Ok);
        }
    }

    if (success) Process();
}

void MainWindow::SourceButton2() {
    QString path = QFileDialog::getExistingDirectory(0, "", QString(), QFileDialog::ShowDirsOnly);
    ui->source_edit_2->setText(path);
}

void MainWindow::DestinationButton2() {
    QString path = QFileDialog::getExistingDirectory(0, "", QString(), QFileDialog::ShowDirsOnly);
    ui->destination_edit_2->setText(path);
}

void MainWindow::RunButton2() {
    bool success = true;

    if (ui->source_edit_2->text().isEmpty()) {
        success = false;
        QMessageBox::warning(this, tr("Error"),
                                        tr("Source directory must be selected"),
                                        QMessageBox::Ok);
    }
    else {
        if (!QDir(ui->source_edit_2->text()).exists()) {
            success = false;
            QMessageBox::warning(this, tr("Error"),
                                            tr("Invalid source directory"),
                                            QMessageBox::Ok);
        }
    }
    if (ui->destination_edit_2->text().isEmpty()) {
        success = false;
        QMessageBox::warning(this, tr("Error"),
                                        tr("Destination directory must be selected"),
                                        QMessageBox::Ok);
    }
    else {
        if (!QDir(ui->destination_edit_2->text()).exists()) {
            success = false;
            QMessageBox::warning(this, tr("Error"),
                                            tr("Invalid destination directory"),
                                            QMessageBox::Ok);
        }
    }

    if (success) Process2();
}

void MainWindow::Process() {
    bool spectrograms = ui->save_spectrograms->isChecked();

    QString save_path = ui->destination_edit->text();
    std::string outfile = save_path.toLocal8Bit().constData();
    outfile.append("/Results.csv");
    std::fstream ofs(outfile.c_str(), std::ios_base::in|std::ios_base::out|std::ios_base::trunc);
    if (!ofs.is_open()) {
        std::cerr << "failed to open output file" << '\n';
    }

    ofs << "FilePath,FileName,Date,Time,";
    auto label = std::begin(class_labels);
    ofs << *label;
    while (++label != std::end(class_labels)) {
        ofs << "," << *label;
    }
    ofs << '\n';
    ofs.precision(2);

    QDir dir = (ui->source_edit->text());
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();
    QProgressDialog progress("Analysing files...", "Cancel", 0, list.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    for (int i = 0; i < list.size(); ++i) {
        progress.setValue(i);
        if (progress.wasCanceled())
            break;
        QFileInfo fileInfo = list[i];
        QString fp = fileInfo.absoluteFilePath();
        std::string filepath = fp.toLocal8Bit().constData();
        QString fpath = fileInfo.path();
        QString filename = fileInfo.baseName();

        RecordingResults recording;
        recording.filepath = fpath.toLocal8Bit().constData();
        recording.filename = filename.toLocal8Bit().constData();

        if (!audio.FileSupported(filepath)) {
            //error_log.push_back(filepath);
            continue;
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
                if (spectrograms) segment.Save(ui->destination_edit->text(), fileInfo.baseName());
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
        AppendResults(recording, ofs);
    }
    ofs.close();
    progress.setValue(list.size());

    QMessageBox msgBox;
    msgBox.setText("Finished!");
    msgBox.exec();
}

void MainWindow::Process2() {
    QDir dir = (ui->source_edit_2->text());
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();
    QProgressDialog progress("Splitting files...", "Cancel", 0, list.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    for (int i = 0; i < list.size(); ++i) {
        progress.setValue(i);
        if (progress.wasCanceled())
            break;
        QFileInfo fileInfo = list[i];
        QString fp = fileInfo.absoluteFilePath();
        std::string filepath = fp.toLocal8Bit().constData();
        QString filename = fileInfo.baseName();
        std::string file = filename.toLocal8Bit().constData();

        SndfileHandle sndfile;
        sndfile = SndfileHandle(filepath.c_str());
        bool stereo = true;
        if (sndfile.channels() == 1) stereo = false;
        else if (sndfile.channels() > 2) continue;//unsupported

        std::size_t start_sample = 0;
        bool triggered = false;
        std::size_t samplecount = 0;
        const std::size_t nframes = 512;
        float threshold = nframes / 10000000.0;
        std::vector<float> buffer(nframes*sndfile.channels(), 0);
        std::vector<float> monobuffer;

        Audio audio;
        audio.format = sndfile.format();
        audio.samplerate = sndfile.samplerate();

        sf_count_t nread = 0;
        while ((nread = sndfile.readf(buffer.data(), nframes)) > 0) {
            samplecount += nread;
            if (stereo) DeInterleave(buffer, 2, monobuffer);
            else monobuffer = buffer;
            auto sum_sq = SumSquare(monobuffer);
            bool signal = sum_sq > threshold ? true : false;
            if (!signal && triggered) {
                float duration = (float)(samplecount-start_sample) / audio.samplerate;
                if (duration > 0.1) {
                    float seconds = (float)start_sample / audio.samplerate;
                    std::string timestamp = Timestamp(seconds);
                    WriteAudio(audio, file, timestamp);
                    audio.samples.clear();
                }
                triggered = false;
            }
            else if (signal) {
                if (!triggered) {
                    start_sample = samplecount;
                    triggered = true;
                }
                for (auto i = 0; i < nread; ++i) {
                    audio.samples.push_back(monobuffer[i]);
                }
            }
        }
        if (!audio.samples.empty()) {
            float seconds = (float)start_sample / audio.samplerate;
            std::string timestamp = Timestamp(seconds);
            WriteAudio(audio, file, timestamp);
            audio.samples.clear();
        }
    }
    progress.setValue(list.size());

    QMessageBox msgBox;
    msgBox.setText("Finished!");
    msgBox.exec();
}

MainWindow::~MainWindow() {
    delete ui;
}

float MainWindow::SumSquare(const std::vector<float>& x) {
    auto N = x.size();
    float mean = std::accumulate(std::begin(x), std::end(x), 0.0) / N;
    float sum_sq = 0;
    for (std::size_t i = 0; i < N; ++i) {
        float diff = x[i] - mean;
        sum_sq += diff * diff;
    }
    return sum_sq;
}

void MainWindow::DeInterleave(std::vector<float>& stereo, std::size_t channel, std::vector<float>& mono) {
    auto N = stereo.size();
    mono.clear();
    mono.reserve(N/2);
    channel -= 1;
    for (auto i = channel; i < N; i+=2) {
        mono.push_back(stereo[i]);
    }
}

void MainWindow::WriteAudio(Audio audio, std::string file, std::string timestamp) {
    QString save_path = ui->destination_edit_2->text();
    std::string destination_dir = save_path.toLocal8Bit().constData();
    std::stringstream ss;
    ss << destination_dir << "/" << file << "--" << timestamp << ".wav";
    SndfileHandle sndfile = SndfileHandle(ss.str().c_str(), SFM_WRITE, (SF_FORMAT_WAV | SF_FORMAT_PCM_16), 1, audio.samplerate);
    sndfile.write(audio.samples.data(), audio.samples.size());
}

void MainWindow::AppendResults(RecordingResults& recording, std::fstream& ofs) {
    ofs << recording.filepath << "," << recording.filename << "," << recording.date << "," << recording.time << ",";
    auto label = std::begin(class_labels);

    ofs << std::fixed << recording.results[*label];
    while (++label != std::end(class_labels)) {
        ofs << "," << std::fixed << recording.results[*label];
    }
    ofs << '\n';
}

void MainWindow::WriteErrors(const std::vector<std::string>& error_log, std::string outfile) {
    std::ofstream ofs(outfile.c_str(), std::ios_base::in|std::ios_base::out|std::ios_base::trunc);
    if (!ofs.is_open()) {
        std::cerr << "failed to open " << outfile << '\n';
    }
    ofs << "The following files could not be read: " << '\n';
    for (auto file: error_log) {
        ofs << file << '\n';
    }
    ofs.close();
}

DateTime MainWindow::ReadTime(std::string filename) {
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

std::string MainWindow::Timestamp(float seconds) {
    size_t s = static_cast<size_t>(seconds);
    size_t ms = 1000 * (seconds - s);//one thousandth of a second
    size_t secRem = s % 60;
    size_t min = s / 60;
    size_t minRem = min % 60;
    size_t hours = min / 60;

    std::stringstream ss;
    if(hours < 10) ss << 0;
    ss << hours << "-";
    if(minRem < 10) ss << 0;
    ss << minRem << "-";
    if(secRem < 10) ss << 0;
    ss << secRem << "-";
    if(ms < 10) ss << 0 << 0;
    else if(ms < 100) ss << 0;
    ss << ms;
    return ss.str();
}

std::unordered_map<int, Blob> MainWindow::PriorityBlobs(std::unordered_map<int, Blob>& blobs, std::size_t n) {
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
