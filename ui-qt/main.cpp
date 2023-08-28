#include <QApplication>
#include <QFileInfo>
#include <QPushButton>

#include <QBoxLayout>
#include <QLabel>
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

#include "../lib/jack-backend/audiobackend.h"
#include "../lib/jack-backend/chord.h"

static constexpr const size_t numLabels = 4;

static std::atomic<Chord> actChord;

void consumeThread() {
    while (true) {
        static Chord oldChord{0, 0, 0};
        if (oldChord != actChord) {
            oldChord = actChord;
            std::cout << actChord;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

void ChordArrived(Chord newChord) { actChord = newChord; }


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QString appFilePath = QCoreApplication::applicationFilePath();
    QString appFileName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();

    std::thread consume(consumeThread);

    registerChordListener(&ChordArrived);

    std::string clientName(argv[0]);
    std::thread connector(&connectAudioBackend, appFileName.toStdString());

    auto* window = new QWidget();
    auto* box = new QHBoxLayout(window);

    std::vector<QLabel*> chordLabels;

    QFont defaultFont;
    defaultFont.setPointSize(35);

    for (size_t i = 0; i < numLabels; ++i){
        auto* newLabel = new QLabel(QString::number(i));
        newLabel->setFont(defaultFont);
        box->addWidget(newLabel);
    }

    window->show();

    int ret = QApplication::exec();

    delete window;
    delete box;

    for (size_t i = 0; i < numLabels; ++i) {
        delete chordLabels.at(i);
    }
    chordLabels.clear();
}
