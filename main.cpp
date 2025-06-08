#include "mainwindow.h"

#include <QApplication>
#include <QIcon>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // QTranslator translator;
    // const QStringList uiLanguages = QLocale::system().uiLanguages();
    // for (const QString &locale : uiLanguages) {
    //     const QString baseName = "Gomoku_" + QLocale(locale).name();
    //     if (translator.load(":/i18n/" + baseName)) {
    //         a.installTranslator(&translator);
    //         break;
    //     }
    // }

    MainWindow w;
    w.setWindowTitle("五子棋 - by Team 请输入文本 - 2025/6/1");

    QIcon icon(":/pic/icon.svg");
    w.setWindowIcon(icon);

    w.show();
    return a.exec();
}
