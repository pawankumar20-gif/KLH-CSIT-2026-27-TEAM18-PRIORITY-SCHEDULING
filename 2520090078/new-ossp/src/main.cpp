#include <QApplication>
#include <QStyleFactory>
#include <QIcon>
#include "main_window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName(QStringLiteral("Parallel File Search"));
    app.setApplicationVersion(QStringLiteral("1.0.0"));
    app.setOrganizationName(QStringLiteral("ParallelSystems"));
    app.setOrganizationDomain(QStringLiteral("parallelsystems.org"));

    // Set modern Fusion base style before dark theme stylesheet
    app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    pfs::MainWindow window;

    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--screenshot" && i + 1 < argc) {
            window.show();
            app.processEvents();
            QPixmap pixmap = window.grab();
            pixmap.save(QString::fromLocal8Bit(argv[i + 1]));
            return 0;
        }
        if (std::string(argv[i]) == "--demo-search" && i + 3 < argc) {
            QString searchDir = QString::fromLocal8Bit(argv[i + 1]);
            QString searchPattern = QString::fromLocal8Bit(argv[i + 2]);
            QString outScreenshot = QString::fromLocal8Bit(argv[i + 3]);

            window.show();
            app.processEvents();

            window.setSearchDirectory(searchDir);
            window.setSearchPattern(searchPattern);
            window.triggerSearch();

            // Wait for search workers and results to populate
            for (int step = 0; step < 20; ++step) {
                app.processEvents();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            app.processEvents();
            QPixmap pixmap = window.grab();
            pixmap.save(outScreenshot);
            return 0;
        }
    }

    window.show();
    return app.exec();
}
