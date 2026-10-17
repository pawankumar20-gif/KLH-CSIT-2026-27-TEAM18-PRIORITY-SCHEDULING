#pragma once

#include <QObject>
#include <QString>
#include <memory>
#include <vector>
#include "search_result.h"
#include "search_engine.h"
#include "logger.h"
#include "export_manager.h"

// Register custom types for Qt Signal/Slot cross-thread meta-system
Q_DECLARE_METATYPE(pfs::SearchStats)
Q_DECLARE_METATYPE(pfs::SearchResult)
Q_DECLARE_METATYPE(std::vector<pfs::SearchResult>)
Q_DECLARE_METATYPE(pfs::LogEntry)

namespace pfs {

class SearchController : public QObject {
    Q_OBJECT

public:
    explicit SearchController(QObject* parent = nullptr);
    ~SearchController() override;

    bool isRunning() const;
    SearchStats currentStats() const;
    std::vector<SearchResult> allResults() const;

public slots:
    void startSearch(const pfs::SearchOptions& options);
    void cancelSearch();
    void clearSearch();
    bool exportResultsToCsv(const QString& filePath, QString& outError);
    bool exportLogsToFile(const QString& filePath, QString& outError);

signals:
    void searchStarted();
    void progressUpdated(const pfs::SearchStats& stats);
    void batchResultsFound(const std::vector<pfs::SearchResult>& results);
    void singleResultFound(const pfs::SearchResult& result);
    void searchFinished(bool cancelled, const pfs::SearchStats& stats);
    void logMessageReceived(const pfs::LogEntry& entry);
    void errorOccurred(const QString& errorMessage);

private:
    std::unique_ptr<SearchEngine> m_engine;
};

} // namespace pfs
