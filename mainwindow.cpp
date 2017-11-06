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
        RecordingResults recording = classifier.AutoIdFile(list.at(i).filePath(), spectrograms);
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

