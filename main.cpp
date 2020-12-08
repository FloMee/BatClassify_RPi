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

#include "mainwindow.h"
#include "classifieruk.h"
#include <QApplication>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>

int main(int argc, char *argv[]) {
    
    if(argc > 1){
        QApplication a(argc, argv);
        QCoreApplication::setApplicationName("BatClassify");

        MainWindow w;
        w.setWindowTitle("BatClassify");
        w.show();

        return a.exec();
    }
    
    
    ClassifierUK classifier;
    std::string input;
    do {
	std::cout << "inputfile:";
        std::cin >> input;
        if(input != "quit") {
            QString filename = QString::fromStdString(input);
            RecordingResults result = classifier.AutoIdFile(filename, false);
            QJsonObject results;
            for (auto i = result.results.begin(); i != result.results.end(); ++i) {
                    results.insert(QString::fromStdString(i->first), QJsonValue(i->second));
            }

            std::cout << QJsonDocument(results).toJson(QJsonDocument::Compact).toStdString() << "\n";
        }
    } while(input != "quit");
    
    return 0;
}
