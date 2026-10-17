#include "search_controller.h"
#include <QMetaObject>

namespace pfs {

SearchController::SearchController(QObject* parent)
    : QObject(parent),
      m_engine(std::make_unique<SearchEngine>()) {

    // Register types for queued cross-thread signal emission
    qRegisterMetaType<pfs::SearchStats>("pfs::SearchStats");
    qRegisterMetaType<pfs::SearchResult>("pfs::SearchResult");
    qRegisterMetaType<std::vector<pfs::SearchResult>>("std::vector<pfs::SearchResult>");
    qRegisterMetaType<pfs::LogEntry>("pfs::LogEntry");

    // Connect Logger callbacks to SearchController signals
    Logger::instance().setCallback([this](const LogEntry& entry) {
        QMetaObject::invokeMethod(this, [this, entry]() {
            emit logMessageReceived(entry);
        }, Qt::QueuedConnection);
    });

    // Connect SearchEngine callbacks to SearchController signals
    m_engine->setProgressCallback([this](const SearchStats& stats) {
        QMetaObject::invokeMethod(this, [this, stats]() {
            emit progressUpdated(stats);
        }, Qt::QueuedConnection);
    });

    m_engine->setBatchResultCallback([this](const std::vector<SearchResult>& results) {
        QMetaObject::invokeMethod(this, [this, results]() {
            emit batchResultsFound(results);
        }, Qt::QueuedConnection);
    });

    m_engine->setResultCallback([this](const SearchResult& res) {
        QMetaObject::invokeMethod(this, [this, res]() {
            emit singleResultFound(res);
        }, Qt::QueuedConnection);
    });

    m_engine->setFinishedCallback([this](bool cancelled, const SearchStats& stats) {
        QMetaObject::invokeMethod(this, [this, cancelled, stats]() {
            emit searchFinished(cancelled, stats);
        }, Qt::QueuedConnection);
    });
}

SearchController::~SearchController() {
    if (m_engine) {
        m_engine->cancel();
    }
}

bool SearchController::isRunning() const {
    return m_engine && m_engine->isRunning();
}

SearchStats SearchController::currentStats() const {
    return m_engine ? m_engine->getStats() : SearchStats{};
}

std::vector<SearchResult> SearchController::allResults() const {
    return m_engine ? m_engine->getAllResults() : std::vector<SearchResult>{};
}

void SearchController::startSearch(const pfs::SearchOptions& options) {
    std::string errMsg;
    if (!SearchEngine::validateOptions(options, errMsg)) {
        emit errorOccurred(QString::fromStdString(errMsg));
        return;
    }

    emit searchStarted();
    if (!m_engine->startAsync(options)) {
        emit errorOccurred(QStringLiteral("Failed to initiate search engine thread."));
    }
}

void SearchController::cancelSearch() {
    if (m_engine) {
        m_engine->cancel();
    }
}

void SearchController::clearSearch() {
    if (m_engine) {
        m_engine->clear();
    }
}

bool SearchController::exportResultsToCsv(const QString& filePath, QString& outError) {
    if (!m_engine) {
        outError = QStringLiteral("Search engine not initialized.");
        return false;
    }

    auto results = m_engine->getAllResults();
    std::string errorStr;
    bool ok = ExportManager::exportToCsv(filePath.toStdString(), results, errorStr);
    if (!ok) {
        outError = QString::fromStdString(errorStr);
        return false;
    }
    return true;
}

bool SearchController::exportLogsToFile(const QString& filePath, QString& outError) {
    bool ok = Logger::instance().exportToFile(filePath.toStdString());
    if (!ok) {
        outError = QStringLiteral("Failed to write activity logs to file.");
        return false;
    }
    return true;
}

} // namespace pfs
