/**
 * @file main.cpp
 * @brief Entry point for Defense-Grade Modular Radar Health Monitoring Editor
 * 
 * This application provides a node-based editor for radar system designers
 * to visually construct and monitor radar subsystem architectures.
 * 
 * @author Radar Systems Engineering Team
 * @version 1.0.0
 */

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application metadata
    QApplication::setApplicationName("Radar Health Monitoring System");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("Defense Systems");
    QApplication::setOrganizationDomain("defense.mil");
    
    // Apply dark theme stylesheet for defense-grade UI
    QFile styleFile(":/styles/radar_theme.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&styleFile);
        app.setStyleSheet(stream.readAll());
        styleFile.close();
    } else {
        qWarning() << "Could not load stylesheet";
    }
    
    // Create and show main window
    MainWindow mainWindow;
    mainWindow.setWindowTitle("Radar Health Monitoring Editor - Defense Grade");
    mainWindow.showMaximized();
    
    qInfo() << "Radar Health Monitoring System started";
    qInfo() << "Qt Version:" << QT_VERSION_STR;
    
    return app.exec();
}
