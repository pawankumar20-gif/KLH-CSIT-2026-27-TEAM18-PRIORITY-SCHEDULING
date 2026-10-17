#pragma once

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QProgressBar>
#include <QLabel>
#include <QTableWidget>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QTimer>
#include <memory>
#include "search_controller.h"

namespace pfs {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    void setSearchDirectory(const QString& dir);
    void setSearchPattern(const QString& pattern);
    void triggerSearch();

private slots:
    void onBrowseDirectory();
    void onStartSearch();
    void onStopSearch();
    void onClearAll();
    void onExportCsv();
    void onExportLog();
    void onClearLog();
    void onFilterTextChanged(const QString& text);
    void onTableDoubleClicked(int row, int column);
    void onTableCustomContextMenu(const QPoint& pos);

    // Controller event handlers
    void handleSearchStarted();
    void handleProgressUpdated(const pfs::SearchStats& stats);
    void handleBatchResultsFound(const std::vector<pfs::SearchResult>& results);
    void handleSearchFinished(bool cancelled, const pfs::SearchStats& stats);
    void handleLogMessage(const pfs::LogEntry& entry);
    void handleError(const QString& message);

private:
    void setupUi();
    void applyDarkTheme();
    QWidget* createHeaderWidget();
    QWidget* createSidebarWidget();
    QWidget* createDashboardWidget();
    QWidget* createResultsWidget();
    QWidget* createLogWidget();

    void updateStatCards(const pfs::SearchStats& stats);
    void setUiSearching(bool searching);
    void highlightMatchInRow(int row, const QString& keyword);

    // Search Configuration UI Elements
    QLineEdit* m_dirPathEdit{nullptr};
    QPushButton* m_browseBtn{nullptr};
    QLineEdit* m_patternEdit{nullptr};
    QComboBox* m_searchModeCombo{nullptr};
    QCheckBox* m_recursiveCheck{nullptr};
    QCheckBox* m_caseSensitiveCheck{nullptr};
    QCheckBox* m_regexCheck{nullptr};
    QSpinBox* m_threadCountSpin{nullptr};
    QPushButton* m_startBtn{nullptr};
    QPushButton* m_stopBtn{nullptr};
    QPushButton* m_clearBtn{nullptr};

    // Dashboard UI Elements
    QLabel* m_statusBadge{nullptr};
    QProgressBar* m_progressBar{nullptr};
    QLabel* m_statDiscovered{nullptr};
    QLabel* m_statScanned{nullptr};
    QLabel* m_statMatches{nullptr};
    QLabel* m_statThreads{nullptr};
    QLabel* m_statTime{nullptr};

    // Results UI Elements
    QLineEdit* m_resultFilterEdit{nullptr};
    QLabel* m_resultCountLabel{nullptr};
    QPushButton* m_exportCsvBtn{nullptr};
    QTableWidget* m_resultsTable{nullptr};
    QLabel* m_emptyStateLabel{nullptr};

    // Activity Log Elements
    QPlainTextEdit* m_logViewer{nullptr};
    QPushButton* m_clearLogBtn{nullptr};
    QPushButton* m_exportLogBtn{nullptr};

    // Search Controller
    std::unique_ptr<SearchController> m_controller;

    // Cache of current results for quick filtering
    std::vector<SearchResult> m_currentResults;
};

} // namespace pfs
