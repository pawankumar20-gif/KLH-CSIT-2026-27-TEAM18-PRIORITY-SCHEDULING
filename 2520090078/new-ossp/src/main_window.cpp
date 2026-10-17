#include "main_window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QDesktopServices>
#include <QUrl>
#include <QClipboard>
#include <QApplication>
#include <QMenu>
#include <QFileInfo>
#include <QDateTime>
#include <thread>

namespace pfs {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      m_controller(std::make_unique<SearchController>(this)) {

    setWindowTitle(QStringLiteral("Parallel File Search Using Threads"));
    setMinimumSize(1100, 750);
    resize(1280, 820);

    setupUi();
    applyDarkTheme();

    // Wire controller signals to UI slots
    connect(m_controller.get(), &SearchController::searchStarted, this, &MainWindow::handleSearchStarted);
    connect(m_controller.get(), &SearchController::progressUpdated, this, &MainWindow::handleProgressUpdated);
    connect(m_controller.get(), &SearchController::batchResultsFound, this, &MainWindow::handleBatchResultsFound);
    connect(m_controller.get(), &SearchController::searchFinished, this, &MainWindow::handleSearchFinished);
    connect(m_controller.get(), &SearchController::logMessageReceived, this, &MainWindow::handleLogMessage);
    connect(m_controller.get(), &SearchController::errorOccurred, this, &MainWindow::handleError);

    Logger::instance().info("Application initialized successfully. Ready to search.");
}

MainWindow::~MainWindow() = default;

void MainWindow::setSearchDirectory(const QString& dir) {
    if (m_dirPathEdit) m_dirPathEdit->setText(dir);
}

void MainWindow::setSearchPattern(const QString& pattern) {
    if (m_patternEdit) m_patternEdit->setText(pattern);
}

void MainWindow::triggerSearch() {
    onStartSearch();
}

void MainWindow::applyDarkTheme() {
    setStyleSheet(R"(
        /* Global Window and Base Styles */
        QMainWindow, QWidget#centralWidget {
            background-color: #0d1117;
            color: #e6edf3;
            font-family: 'Segoe UI', -apple-system, BlinkMacSystemFont, Roboto, sans-serif;
            font-size: 13px;
        }

        /* Panels and Group Boxes */
        QGroupBox {
            background-color: #161b22;
            border: 1px solid #30363d;
            border-radius: 8px;
            margin-top: 12px;
            padding-top: 16px;
            padding-bottom: 10px;
            padding-left: 10px;
            padding-right: 10px;
            font-weight: 600;
            color: #58a6ff;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            left: 12px;
            padding: 0 6px;
            background-color: #161b22;
        }

        /* Input Controls */
        QLineEdit {
            background-color: #0d1117;
            border: 1px solid #30363d;
            border-radius: 6px;
            padding: 7px 10px;
            color: #f0f6fc;
            selection-background-color: #1f6feb;
        }
        QLineEdit:focus {
            border: 1px solid #58a6ff;
            background-color: #111822;
        }

        QComboBox {
            background-color: #0d1117;
            border: 1px solid #30363d;
            border-radius: 6px;
            padding: 6px 10px;
            color: #f0f6fc;
        }
        QComboBox:focus, QComboBox:hover {
            border: 1px solid #58a6ff;
        }
        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 20px;
            border-left: 1px solid #30363d;
        }
        QComboBox QAbstractItemView {
            background-color: #161b22;
            border: 1px solid #30363d;
            selection-background-color: #1f6feb;
            color: #f0f6fc;
        }

        QSpinBox {
            background-color: #0d1117;
            border: 1px solid #30363d;
            border-radius: 6px;
            padding: 6px 10px;
            color: #f0f6fc;
        }
        QSpinBox:focus {
            border: 1px solid #58a6ff;
        }

        QCheckBox {
            color: #c9d1d9;
            spacing: 8px;
        }
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border-radius: 4px;
            border: 1px solid #30363d;
            background-color: #0d1117;
        }
        QCheckBox::indicator:checked {
            background-color: #238636;
            border: 1px solid #2ea043;
        }

        /* Buttons */
        QPushButton {
            background-color: #21262d;
            border: 1px solid #363b42;
            border-radius: 6px;
            padding: 8px 14px;
            color: #c9d1d9;
            font-weight: 600;
        }
        QPushButton:hover {
            background-color: #30363d;
            border-color: #8b949e;
        }
        QPushButton:pressed {
            background-color: #161b22;
        }
        QPushButton:disabled {
            background-color: #161b22;
            border-color: #21262d;
            color: #484f58;
        }

        QPushButton#primaryBtn {
            background-color: #238636;
            border: 1px solid #2ea043;
            color: #ffffff;
        }
        QPushButton#primaryBtn:hover {
            background-color: #2ea043;
        }
        QPushButton#primaryBtn:pressed {
            background-color: #1a6327;
        }
        QPushButton#primaryBtn:disabled {
            background-color: #161b22;
            border-color: #21262d;
            color: #484f58;
        }

        QPushButton#stopBtn {
            background-color: #da3633;
            border: 1px solid #f85149;
            color: #ffffff;
        }
        QPushButton#stopBtn:hover {
            background-color: #f85149;
        }
        QPushButton#stopBtn:pressed {
            background-color: #b62324;
        }
        QPushButton#stopBtn:disabled {
            background-color: #161b22;
            border-color: #21262d;
            color: #484f58;
        }

        /* Progress Bar */
        QProgressBar {
            background-color: #161b22;
            border: 1px solid #30363d;
            border-radius: 6px;
            text-align: center;
            color: #f0f6fc;
            font-weight: 600;
            height: 18px;
        }
        QProgressBar::chunk {
            background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #1f6feb, stop:1 #388bfd);
            border-radius: 5px;
        }

        /* Results Table */
        QTableWidget {
            background-color: #0d1117;
            border: 1px solid #30363d;
            border-radius: 8px;
            gridline-color: #21262d;
            color: #e6edf3;
            selection-background-color: #1f6feb;
            selection-color: #ffffff;
        }
        QTableWidget::item {
            padding: 5px 8px;
            border-bottom: 1px solid #161b22;
        }
        QTableWidget::item:selected {
            background-color: #1f6feb;
            color: #ffffff;
        }
        QHeaderView::section {
            background-color: #161b22;
            color: #8b949e;
            font-weight: 600;
            padding: 6px 8px;
            border: none;
            border-right: 1px solid #21262d;
            border-bottom: 2px solid #30363d;
        }

        /* Plain Text Edit for Logs */
        QPlainTextEdit {
            background-color: #090d13;
            border: 1px solid #30363d;
            border-radius: 6px;
            color: #8b949e;
            font-family: 'Consolas', 'Fira Code', 'Cascadia Code', monospace;
            font-size: 12px;
            padding: 8px;
        }

        /* Scrollbars */
        QScrollBar:vertical {
            background: #0d1117;
            width: 10px;
            margin: 0px;
        }
        QScrollBar::handle:vertical {
            background: #30363d;
            min-height: 20px;
            border-radius: 4px;
        }
        QScrollBar::handle:vertical:hover {
            background: #58a6ff;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        QScrollBar:horizontal {
            background: #0d1117;
            height: 10px;
            margin: 0px;
        }
        QScrollBar::handle:horizontal {
            background: #30363d;
            min-width: 20px;
            border-radius: 4px;
        }
        QScrollBar::handle:horizontal:hover {
            background: #58a6ff;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0px;
        }

        /* Splitter */
        QSplitter::handle {
            background-color: #21262d;
            height: 2px;
        }
    )");
}

void MainWindow::setupUi() {
    auto centralWidget = new QWidget(this);
    centralWidget->setObjectName(QStringLiteral("centralWidget"));
    setCentralWidget(centralWidget);

    auto mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(14, 12, 14, 12);
    mainLayout->setSpacing(10);

    // 1. Header
    mainLayout->addWidget(createHeaderWidget());

    // 2. Middle area with Sidebar (Left) and Content (Right)
    auto middleSplitter = new QSplitter(Qt::Horizontal, this);

    auto sidebarWidget = createSidebarWidget();
    sidebarWidget->setMinimumWidth(320);
    sidebarWidget->setMaximumWidth(400);
    middleSplitter->addWidget(sidebarWidget);

    // Right side: Dashboard + Results + Log
    auto rightContent = new QWidget(this);
    auto rightLayout = new QVBoxLayout(rightContent);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(10);

    rightLayout->addWidget(createDashboardWidget());

    // Vertical splitter between Results table and Activity Log
    auto contentSplitter = new QSplitter(Qt::Vertical, rightContent);
    contentSplitter->addWidget(createResultsWidget());
    contentSplitter->addWidget(createLogWidget());
    contentSplitter->setStretchFactor(0, 3);
    contentSplitter->setStretchFactor(1, 1);

    rightLayout->addWidget(contentSplitter, 1);
    middleSplitter->addWidget(rightContent);
    middleSplitter->setStretchFactor(0, 0);
    middleSplitter->setStretchFactor(1, 1);

    mainLayout->addWidget(middleSplitter, 1);

    // Set initial state
    setUiSearching(false);
}

QWidget* MainWindow::createHeaderWidget() {
    auto header = new QWidget(this);
    header->setStyleSheet(R"(
        QWidget {
            background-color: #161b22;
            border: 1px solid #30363d;
            border-radius: 8px;
        }
    )");

    auto layout = new QHBoxLayout(header);
    layout->setContentsMargins(16, 10, 16, 10);

    auto titleLayout = new QVBoxLayout();
    titleLayout->setSpacing(2);

    auto titleLabel = new QLabel(QStringLiteral("Parallel File Search Using Threads"), header);
    titleLabel->setStyleSheet(QStringLiteral("font-size: 18px; font-weight: bold; color: #58a6ff; border: none;"));

    auto subtitleLabel = new QLabel(
        QStringLiteral("High-performance multithreaded directory traversal and file content pattern matching"), 
        header
    );
    subtitleLabel->setStyleSheet(QStringLiteral("font-size: 12px; color: #8b949e; border: none;"));

    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(subtitleLabel);
    layout->addLayout(titleLayout, 1);

    m_statusBadge = new QLabel(QStringLiteral("  IDLE  "), header);
    m_statusBadge->setStyleSheet(R"(
        QLabel {
            background-color: #21262d;
            color: #8b949e;
            border: 1px solid #30363d;
            border-radius: 12px;
            font-size: 11px;
            font-weight: bold;
            padding: 4px 12px;
        }
    )");
    layout->addWidget(m_statusBadge);

    return header;
}

QWidget* MainWindow::createSidebarWidget() {
    auto sidebar = new QWidget(this);
    auto layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    auto configGroup = new QGroupBox(QStringLiteral("Search Configuration"), sidebar);
    auto groupLayout = new QVBoxLayout(configGroup);
    groupLayout->setSpacing(10);

    // Target Directory
    groupLayout->addWidget(new QLabel(QStringLiteral("Search Directory:"), configGroup));
    auto dirLayout = new QHBoxLayout();
    m_dirPathEdit = new QLineEdit(configGroup);
    m_dirPathEdit->setPlaceholderText(QStringLiteral("Enter directory path..."));
    m_browseBtn = new QPushButton(QStringLiteral("Browse..."), configGroup);
    connect(m_browseBtn, &QPushButton::clicked, this, &MainWindow::onBrowseDirectory);
    dirLayout->addWidget(m_dirPathEdit, 1);
    dirLayout->addWidget(m_browseBtn);
    groupLayout->addLayout(dirLayout);

    // Search Pattern
    groupLayout->addWidget(new QLabel(QStringLiteral("Search Pattern:"), configGroup));
    m_patternEdit = new QLineEdit(configGroup);
    m_patternEdit->setPlaceholderText(QStringLiteral("Filename or text to search for..."));
    connect(m_patternEdit, &QLineEdit::returnPressed, this, &MainWindow::onStartSearch);
    groupLayout->addWidget(m_patternEdit);

    // Search Mode
    groupLayout->addWidget(new QLabel(QStringLiteral("Search Mode:"), configGroup));
    m_searchModeCombo = new QComboBox(configGroup);
    m_searchModeCombo->addItem(QStringLiteral("Both Filenames & File Contents"), 0);
    m_searchModeCombo->addItem(QStringLiteral("Filenames Only"), 1);
    m_searchModeCombo->addItem(QStringLiteral("File Contents Only"), 2);
    groupLayout->addWidget(m_searchModeCombo);

    // Search Options
    auto optionsGroup = new QWidget(configGroup);
    auto optLayout = new QVBoxLayout(optionsGroup);
    optLayout->setContentsMargins(0, 4, 0, 4);
    optLayout->setSpacing(6);

    m_recursiveCheck = new QCheckBox(QStringLiteral("Include subdirectories (Recursive)"), optionsGroup);
    m_recursiveCheck->setChecked(true);

    m_caseSensitiveCheck = new QCheckBox(QStringLiteral("Case sensitive"), optionsGroup);
    m_caseSensitiveCheck->setChecked(false);

    m_regexCheck = new QCheckBox(QStringLiteral("Regular expression (std::regex)"), optionsGroup);
    m_regexCheck->setChecked(false);

    optLayout->addWidget(m_recursiveCheck);
    optLayout->addWidget(m_caseSensitiveCheck);
    optLayout->addWidget(m_regexCheck);
    groupLayout->addWidget(optionsGroup);

    // Thread Count Selector
    auto threadLayout = new QHBoxLayout();
    auto threadLabel = new QLabel(QStringLiteral("Worker Threads:"), configGroup);
    m_threadCountSpin = new QSpinBox(configGroup);
    m_threadCountSpin->setRange(1, 64);
    unsigned int hwThreads = std::thread::hardware_concurrency();
    m_threadCountSpin->setValue(hwThreads > 0 ? hwThreads : 4);
    m_threadCountSpin->setToolTip(QStringLiteral("Number of concurrent worker threads searching files"));
    threadLayout->addWidget(threadLabel);
    threadLayout->addWidget(m_threadCountSpin);
    groupLayout->addLayout(threadLayout);

    groupLayout->addSpacing(8);

    // Action Buttons
    m_startBtn = new QPushButton(QStringLiteral("Start Search"), configGroup);
    m_startBtn->setObjectName(QStringLiteral("primaryBtn"));
    connect(m_startBtn, &QPushButton::clicked, this, &MainWindow::onStartSearch);

    m_stopBtn = new QPushButton(QStringLiteral("Stop Search"), configGroup);
    m_stopBtn->setObjectName(QStringLiteral("stopBtn"));
    m_stopBtn->setEnabled(false);
    connect(m_stopBtn, &QPushButton::clicked, this, &MainWindow::onStopSearch);

    m_clearBtn = new QPushButton(QStringLiteral("Clear Results"), configGroup);
    connect(m_clearBtn, &QPushButton::clicked, this, &MainWindow::onClearAll);

    groupLayout->addWidget(m_startBtn);
    groupLayout->addWidget(m_stopBtn);
    groupLayout->addWidget(m_clearBtn);
    groupLayout->addStretch(1);

    layout->addWidget(configGroup, 1);
    return sidebar;
}

QWidget* MainWindow::createDashboardWidget() {
    auto dashboard = new QWidget(this);
    auto layout = new QVBoxLayout(dashboard);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    // Stat Cards
    auto statsRow = new QHBoxLayout();
    statsRow->setSpacing(8);

    auto makeStatCard = [](const QString& labelText, const QString& accentColor, QLabel*& outValueLabel, QWidget* parent) {
        auto card = new QWidget(parent);
        card->setStyleSheet(QString(R"(
            QWidget {
                background-color: #161b22;
                border: 1px solid #30363d;
                border-radius: 8px;
            }
        )"));

        auto l = new QVBoxLayout(card);
        l->setContentsMargins(12, 8, 12, 8);
        l->setSpacing(2);

        auto lbl = new QLabel(labelText, card);
        lbl->setStyleSheet(QStringLiteral("color: #8b949e; font-size: 11px; font-weight: 600; border: none;"));

        outValueLabel = new QLabel(QStringLiteral("0"), card);
        outValueLabel->setStyleSheet(QString("color: %1; font-size: 18px; font-weight: bold; border: none;").arg(accentColor));

        l->addWidget(lbl);
        l->addWidget(outValueLabel);
        return card;
    };

    statsRow->addWidget(makeStatCard(QStringLiteral("DISCOVERED"), QStringLiteral("#58a6ff"), m_statDiscovered, dashboard));
    statsRow->addWidget(makeStatCard(QStringLiteral("SCANNED"), QStringLiteral("#38bdf8"), m_statScanned, dashboard));
    statsRow->addWidget(makeStatCard(QStringLiteral("MATCHES FOUND"), QStringLiteral("#3fb950"), m_statMatches, dashboard));
    statsRow->addWidget(makeStatCard(QStringLiteral("ACTIVE THREADS"), QStringLiteral("#bc8cff"), m_statThreads, dashboard));
    statsRow->addWidget(makeStatCard(QStringLiteral("ELAPSED TIME"), QStringLiteral("#d29922"), m_statTime, dashboard));
    m_statTime->setText(QStringLiteral("0.0 s"));

    layout->addLayout(statsRow);

    // Progress Bar
    m_progressBar = new QProgressBar(dashboard);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setFormat(QStringLiteral("Ready (%p%)"));
    layout->addWidget(m_progressBar);

    return dashboard;
}

QWidget* MainWindow::createResultsWidget() {
    auto widget = new QWidget(this);
    auto layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    // Filter & Export toolbar
    auto toolBar = new QHBoxLayout();
    toolBar->setSpacing(8);

    m_resultCountLabel = new QLabel(QStringLiteral("0 matches"), widget);
    m_resultCountLabel->setStyleSheet(QStringLiteral("font-weight: bold; color: #58a6ff; font-size: 13px;"));
    toolBar->addWidget(m_resultCountLabel);

    toolBar->addSpacing(12);

    m_resultFilterEdit = new QLineEdit(widget);
    m_resultFilterEdit->setPlaceholderText(QStringLiteral("Quick filter results..."));
    connect(m_resultFilterEdit, &QLineEdit::textChanged, this, &MainWindow::onFilterTextChanged);
    toolBar->addWidget(m_resultFilterEdit, 1);

    m_exportCsvBtn = new QPushButton(QStringLiteral("Export CSV"), widget);
    connect(m_exportCsvBtn, &QPushButton::clicked, this, &MainWindow::onExportCsv);
    toolBar->addWidget(m_exportCsvBtn);

    layout->addLayout(toolBar);

    // Results Table
    m_resultsTable = new QTableWidget(0, 7, widget);
    m_resultsTable->setHorizontalHeaderLabels({
        QStringLiteral("#"),
        QStringLiteral("Filename"),
        QStringLiteral("Path"),
        QStringLiteral("Type"),
        QStringLiteral("Line"),
        QStringLiteral("Matching Text"),
        QStringLiteral("Status")
    });

    m_resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_resultsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_resultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_resultsTable->setSortingEnabled(true);
    m_resultsTable->setContextMenuPolicy(Qt::CustomContextMenu);

    auto header = m_resultsTable->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(2, QHeaderView::Interactive);
    m_resultsTable->setColumnWidth(2, 280);
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(5, QHeaderView::Stretch);
    header->setSectionResizeMode(6, QHeaderView::ResizeToContents);

    connect(m_resultsTable, &QTableWidget::cellDoubleClicked, this, &MainWindow::onTableDoubleClicked);
    connect(m_resultsTable, &QTableWidget::customContextMenuRequested, this, &MainWindow::onTableCustomContextMenu);

    layout->addWidget(m_resultsTable, 1);
    return widget;
}

QWidget* MainWindow::createLogWidget() {
    auto widget = new QWidget(this);
    auto layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    auto headerLayout = new QHBoxLayout();
    auto title = new QLabel(QStringLiteral("Activity Log"), widget);
    title->setStyleSheet(QStringLiteral("font-weight: 600; color: #8b949e;"));
    headerLayout->addWidget(title);
    headerLayout->addStretch(1);

    m_clearLogBtn = new QPushButton(QStringLiteral("Clear"), widget);
    m_clearLogBtn->setFixedHeight(24);
    connect(m_clearLogBtn, &QPushButton::clicked, this, &MainWindow::onClearLog);
    headerLayout->addWidget(m_clearLogBtn);

    m_exportLogBtn = new QPushButton(QStringLiteral("Export Log"), widget);
    m_exportLogBtn->setFixedHeight(24);
    connect(m_exportLogBtn, &QPushButton::clicked, this, &MainWindow::onExportLog);
    headerLayout->addWidget(m_exportLogBtn);

    layout->addLayout(headerLayout);

    m_logViewer = new QPlainTextEdit(widget);
    m_logViewer->setReadOnly(true);
    m_logViewer->setMaximumBlockCount(2000);
    layout->addWidget(m_logViewer);

    return widget;
}

void MainWindow::setUiSearching(bool searching) {
    m_startBtn->setEnabled(!searching);
    m_stopBtn->setEnabled(searching);
    m_dirPathEdit->setEnabled(!searching);
    m_browseBtn->setEnabled(!searching);
    m_patternEdit->setEnabled(!searching);
    m_searchModeCombo->setEnabled(!searching);
    m_recursiveCheck->setEnabled(!searching);
    m_caseSensitiveCheck->setEnabled(!searching);
    m_regexCheck->setEnabled(!searching);
    m_threadCountSpin->setEnabled(!searching);

    if (searching) {
        m_statusBadge->setText(QStringLiteral("  SEARCHING...  "));
        m_statusBadge->setStyleSheet(R"(
            QLabel {
                background-color: #1f6feb;
                color: #ffffff;
                border: 1px solid #388bfd;
                border-radius: 12px;
                font-size: 11px;
                font-weight: bold;
                padding: 4px 12px;
            }
        )");
    }
}

void MainWindow::onBrowseDirectory() {
    QString current = m_dirPathEdit->text();
    QString dir = QFileDialog::getExistingDirectory(
        this,
        QStringLiteral("Select Directory to Search"),
        current.isEmpty() ? QStringLiteral(".") : current,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    if (!dir.isEmpty()) {
        m_dirPathEdit->setText(dir);
    }
}

void MainWindow::onStartSearch() {
    QString dir = m_dirPathEdit->text().trimmed();
    if (dir.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Missing Directory"), QStringLiteral("Please select or enter a valid directory to search."));
        return;
    }

    QString pattern = m_patternEdit->text();

    SearchOptions options;
    options.rootPath = dir.toStdString();
    options.pattern = pattern.toStdString();

    int modeIdx = m_searchModeCombo->currentIndex();
    if (modeIdx == 1) {
        options.mode = SearchMode::FilenamesOnly;
    } else if (modeIdx == 2) {
        options.mode = SearchMode::ContentOnly;
    } else {
        options.mode = SearchMode::Both;
    }

    options.recursive = m_recursiveCheck->isChecked();
    options.caseSensitive = m_caseSensitiveCheck->isChecked();
    options.isRegex = m_regexCheck->isChecked();
    options.threadCount = static_cast<unsigned int>(m_threadCountSpin->value());

    // Clear previous results from UI
    m_resultsTable->setSortingEnabled(false);
    m_resultsTable->setRowCount(0);
    m_currentResults.clear();
    m_resultCountLabel->setText(QStringLiteral("0 matches"));
    m_progressBar->setValue(0);
    m_progressBar->setFormat(QStringLiteral("Scanning... (%p%)"));

    m_controller->startSearch(options);
}

void MainWindow::onStopSearch() {
    m_controller->cancelSearch();
    m_stopBtn->setEnabled(false);
    Logger::instance().warn("Cancellation requested by user. Waiting for active workers to finish current files...");
}

void MainWindow::onClearAll() {
    if (m_controller->isRunning()) {
        m_controller->cancelSearch();
    }
    m_controller->clearSearch();
    m_currentResults.clear();
    m_resultsTable->setSortingEnabled(false);
    m_resultsTable->setRowCount(0);
    m_resultCountLabel->setText(QStringLiteral("0 matches"));
    m_resultFilterEdit->clear();

    m_statDiscovered->setText(QStringLiteral("0"));
    m_statScanned->setText(QStringLiteral("0"));
    m_statMatches->setText(QStringLiteral("0"));
    m_statThreads->setText(QStringLiteral("0"));
    m_statTime->setText(QStringLiteral("0.0 s"));
    m_progressBar->setValue(0);
    m_progressBar->setFormat(QStringLiteral("Ready (%p%)"));

    m_statusBadge->setText(QStringLiteral("  IDLE  "));
    m_statusBadge->setStyleSheet(R"(
        QLabel {
            background-color: #21262d;
            color: #8b949e;
            border: 1px solid #30363d;
            border-radius: 12px;
            font-size: 11px;
            font-weight: bold;
            padding: 4px 12px;
        }
    )");
    setUiSearching(false);
}

void MainWindow::onExportCsv() {
    if (m_currentResults.empty()) {
        QMessageBox::information(this, QStringLiteral("No Results"), QStringLiteral("There are no search results to export."));
        return;
    }

    QString defaultName = QString("search_results_%1.csv").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    QString filePath = QFileDialog::getSaveFileName(
        this,
        QStringLiteral("Export Results to CSV"),
        defaultName,
        QStringLiteral("CSV Files (*.csv);;All Files (*.*)")
    );

    if (filePath.isEmpty()) return;

    QString errorMsg;
    if (m_controller->exportResultsToCsv(filePath, errorMsg)) {
        QMessageBox::information(this, QStringLiteral("Export Succeeded"), QString("Successfully exported %1 matches to:\n%2").arg(m_currentResults.size()).arg(filePath));
        Logger::instance().success("Exported results to " + filePath.toStdString());
    } else {
        QMessageBox::critical(this, QStringLiteral("Export Failed"), errorMsg);
        Logger::instance().error("Failed to export results: " + errorMsg.toStdString());
    }
}

void MainWindow::onExportLog() {
    QString defaultName = QString("activity_log_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    QString filePath = QFileDialog::getSaveFileName(
        this,
        QStringLiteral("Export Activity Log"),
        defaultName,
        QStringLiteral("Text Files (*.txt);;All Files (*.*)")
    );

    if (filePath.isEmpty()) return;

    QString errorMsg;
    if (m_controller->exportLogsToFile(filePath, errorMsg)) {
        QMessageBox::information(this, QStringLiteral("Log Exported"), QStringLiteral("Activity log successfully saved."));
    } else {
        QMessageBox::critical(this, QStringLiteral("Log Export Failed"), errorMsg);
    }
}

void MainWindow::onClearLog() {
    m_logViewer->clear();
    Logger::instance().clear();
}

void MainWindow::handleSearchStarted() {
    setUiSearching(true);
}

void MainWindow::handleProgressUpdated(const pfs::SearchStats& stats) {
    updateStatCards(stats);
}

void MainWindow::updateStatCards(const pfs::SearchStats& stats) {
    m_statDiscovered->setText(QString::number(stats.filesDiscovered));
    m_statScanned->setText(QString::number(stats.filesScanned));
    m_statMatches->setText(QString::number(stats.matchesFound));
    m_statThreads->setText(QString::number(stats.activeThreads));

    double seconds = static_cast<double>(stats.elapsedMilliseconds) / 1000.0;
    m_statTime->setText(QString("%1 s").arg(seconds, 0, 'f', 2));

    int pct = static_cast<int>(stats.progressPercentage);
    m_progressBar->setValue(pct);
    m_progressBar->setFormat(QString("%1% (%2 / %3 files)")
        .arg(pct)
        .arg(stats.filesScanned)
        .arg(stats.filesDiscovered));
}

void MainWindow::handleBatchResultsFound(const std::vector<pfs::SearchResult>& results) {
    m_resultsTable->setSortingEnabled(false);

    int startRow = m_resultsTable->rowCount();
    m_resultsTable->setRowCount(startRow + static_cast<int>(results.size()));

    for (size_t i = 0; i < results.size(); ++i) {
        const auto& res = results[i];
        m_currentResults.push_back(res);

        int row = startRow + static_cast<int>(i);

        // ID Item
        auto idItem = new QTableWidgetItem();
        idItem->setData(Qt::DisplayRole, static_cast<qulonglong>(res.id));
        idItem->setTextAlignment(Qt::AlignCenter);

        // Filename Item
        auto fnItem = new QTableWidgetItem(QString::fromStdString(res.fileName));
        fnItem->setForeground(QBrush(QColor(88, 166, 255))); // Accent blue

        // Path Item
        auto pathItem = new QTableWidgetItem(QString::fromStdString(res.filePath));

        // Type Item
        auto typeItem = new QTableWidgetItem(QString::fromStdString(matchTypeToString(res.matchType)));
        if (res.matchType == MatchType::Filename) {
            typeItem->setForeground(QBrush(QColor(188, 140, 255))); // Purple
        } else {
            typeItem->setForeground(QBrush(QColor(63, 185, 80))); // Green
        }
        typeItem->setTextAlignment(Qt::AlignCenter);

        // Line Item
        auto lineItem = new QTableWidgetItem();
        if (res.lineNumber > 0) {
            lineItem->setData(Qt::DisplayRole, res.lineNumber);
        } else {
            lineItem->setText(QStringLiteral("-"));
        }
        lineItem->setTextAlignment(Qt::AlignCenter);

        // Matching Text Item
        auto textItem = new QTableWidgetItem(QString::fromStdString(res.matchedText));

        // Status Item
        auto statusItem = new QTableWidgetItem(QString::fromStdString(res.status));

        m_resultsTable->setItem(row, 0, idItem);
        m_resultsTable->setItem(row, 1, fnItem);
        m_resultsTable->setItem(row, 2, pathItem);
        m_resultsTable->setItem(row, 3, typeItem);
        m_resultsTable->setItem(row, 4, lineItem);
        m_resultsTable->setItem(row, 5, textItem);
        m_resultsTable->setItem(row, 6, statusItem);
    }

    m_resultCountLabel->setText(QString("%1 matches found").arg(m_currentResults.size()));
    m_resultsTable->setSortingEnabled(true);
}

void MainWindow::handleSearchFinished(bool cancelled, const pfs::SearchStats& stats) {
    updateStatCards(stats);
    setUiSearching(false);
    m_resultsTable->setSortingEnabled(true);

    if (cancelled) {
        m_statusBadge->setText(QStringLiteral("  CANCELLED  "));
        m_statusBadge->setStyleSheet(R"(
            QLabel {
                background-color: #da3633;
                color: #ffffff;
                border: 1px solid #f85149;
                border-radius: 12px;
                font-size: 11px;
                font-weight: bold;
                padding: 4px 12px;
            }
        )");
        m_progressBar->setFormat(QStringLiteral("Cancelled (%p%)"));
    } else {
        m_statusBadge->setText(QStringLiteral("  COMPLETED  "));
        m_statusBadge->setStyleSheet(R"(
            QLabel {
                background-color: #238636;
                color: #ffffff;
                border: 1px solid #2ea043;
                border-radius: 12px;
                font-size: 11px;
                font-weight: bold;
                padding: 4px 12px;
            }
        )");
        m_progressBar->setValue(100);
        m_progressBar->setFormat(QString("Completed: %1 matches found").arg(stats.matchesFound));

        if (stats.matchesFound == 0 && stats.filesScanned > 0) {
            QMessageBox::information(
                this, 
                QStringLiteral("Search Finished"), 
                QString("Scanned %1 files. No matches found for pattern \"%2\".")
                    .arg(stats.filesScanned)
                    .arg(m_patternEdit->text())
            );
        }
    }
}

void MainWindow::handleLogMessage(const pfs::LogEntry& entry) {
    QString timeStr = QString::fromStdString(entry.formattedTime());
    QString msg = QString::fromStdString(entry.message);

    QString colorHex = "#8b949e";
    switch (entry.level) {
        case LogLevel::Info:    colorHex = "#58a6ff"; break;
        case LogLevel::Success: colorHex = "#3fb950"; break;
        case LogLevel::Warning: colorHex = "#d29922"; break;
        case LogLevel::Error:   colorHex = "#f85149"; break;
    }

    QString formattedHtml = QString(
        R"(<span style="color: #484f58;">[%1]</span> <span style="color: %2; font-weight: bold;">[%3]</span> <span style="color: #e6edf3;">%4</span>)"
    ).arg(timeStr, colorHex, QString::fromStdString(entry.levelString()), msg.toHtmlEscaped());

    m_logViewer->appendHtml(formattedHtml);
}

void MainWindow::handleError(const QString& message) {
    setUiSearching(false);
    QMessageBox::critical(this, QStringLiteral("Search Error"), message);
}

void MainWindow::onFilterTextChanged(const QString& text) {
    QString filter = text.trimmed().toLower();
    int rowCount = m_resultsTable->rowCount();

    int visibleCount = 0;
    for (int row = 0; row < rowCount; ++row) {
        if (filter.isEmpty()) {
            m_resultsTable->setRowHidden(row, false);
            visibleCount++;
        } else {
            bool match = false;
            for (int col = 0; col < m_resultsTable->columnCount(); ++col) {
                auto item = m_resultsTable->item(row, col);
                if (item && item->text().toLower().contains(filter)) {
                    match = true;
                    break;
                }
            }
            m_resultsTable->setRowHidden(row, !match);
            if (match) visibleCount++;
        }
    }

    if (filter.isEmpty()) {
        m_resultCountLabel->setText(QString("%1 matches found").arg(m_currentResults.size()));
    } else {
        m_resultCountLabel->setText(QString("%1 / %2 matches visible").arg(visibleCount).arg(m_currentResults.size()));
    }
}

void MainWindow::onTableDoubleClicked(int row, int /*column*/) {
    auto pathItem = m_resultsTable->item(row, 2);
    if (!pathItem) return;

    QString filePath = pathItem->text();
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void MainWindow::onTableCustomContextMenu(const QPoint& pos) {
    auto item = m_resultsTable->itemAt(pos);
    if (!item) return;

    int row = item->row();
    auto pathItem = m_resultsTable->item(row, 2);
    auto textItem = m_resultsTable->item(row, 5);

    if (!pathItem) return;
    QString filePath = pathItem->text();
    QString matchedText = textItem ? textItem->text() : QString();

    QMenu menu(this);
    menu.setStyleSheet(R"(
        QMenu {
            background-color: #161b22;
            border: 1px solid #30363d;
            border-radius: 6px;
            padding: 4px;
            color: #e6edf3;
        }
        QMenu::item {
            padding: 6px 16px;
            border-radius: 4px;
        }
        QMenu::item:selected {
            background-color: #1f6feb;
            color: #ffffff;
        }
    )");

    auto openFileAction = menu.addAction(QStringLiteral("Open File"));
    auto openFolderAction = menu.addAction(QStringLiteral("Open Containing Folder"));
    menu.addSeparator();
    auto copyPathAction = menu.addAction(QStringLiteral("Copy Full Path"));
    auto copyTextAction = menu.addAction(QStringLiteral("Copy Matching Text"));

    auto selected = menu.exec(m_resultsTable->viewport()->mapToGlobal(pos));

    if (selected == openFileAction) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    } else if (selected == openFolderAction) {
        QFileInfo fi(filePath);
        QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absolutePath()));
    } else if (selected == copyPathAction) {
        QApplication::clipboard()->setText(filePath);
    } else if (selected == copyTextAction) {
        QApplication::clipboard()->setText(matchedText);
    }
}

} // namespace pfs
