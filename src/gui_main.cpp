#include <QAbstractItemView>
#include <QApplication>
#include <QFileDialog>
#include <QFontDatabase>
#include <QFrame>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QStatusBar>
#include <QStringList>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <cstdint>
#include <string>
#include <vector>

#include "LoadResult.hpp"
#include "StepResult.hpp"
#include "VoleMachine.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void loadProgram();
    void setAddress();
    void run();
    void singleStep();
    void clearMemory();
    void resetCpu();
    void runTick();

private:
    void loadProgramFile(const QString& path, unsigned address);
    void buildUi();
    void refresh();
    void updateScreen();
    void setRunning(bool running);
    void stopRun(const QString& message);
    unsigned loadAddress() const;

    Vole::VoleMachine machine_;
    QTableWidget* memoryTable_ = nullptr;
    QTableWidget* registerTable_ = nullptr;
    QLabel* pcLabel_ = nullptr;
    QLabel* irLabel_ = nullptr;
    QLabel* haltedLabel_ = nullptr;
    QPlainTextEdit* screenConsole_ = nullptr;
    QLineEdit* addressEdit_ = nullptr;
    QPushButton* loadButton_ = nullptr;
    QPushButton* addressButton_ = nullptr;
    QPushButton* runButton_ = nullptr;
    QPushButton* stepButton_ = nullptr;
    QPushButton* clearButton_ = nullptr;
    QPushButton* resetButton_ = nullptr;
    QTimer* runTimer_ = nullptr;
    unsigned long long stepsRun_ = 0;
};

namespace {

QString hexByte(unsigned value) {
    return QString::number(value, 16).toUpper().rightJustified(2, '0');
}

QString hexWord(unsigned value) {
    return QString::number(value, 16).toUpper().rightJustified(4, '0');
}

QWidget* makeCard(const QString& title, QLayout* content) {
    QWidget* section = new QWidget;
    QVBoxLayout* outer = new QVBoxLayout(section);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(6);

    QLabel* heading = new QLabel(title);
    heading->setObjectName("cardTitle");
    outer->addWidget(heading);

    QFrame* card = new QFrame;
    card->setObjectName("card");
    card->setFrameShape(QFrame::NoFrame);

    QVBoxLayout* inner = new QVBoxLayout(card);
    inner->setContentsMargins(12, 12, 12, 12);
    inner->setSpacing(0);
    inner->addLayout(content);

    outer->addWidget(card, 1);
    return section;
}

}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    buildUi();
    refresh();
    statusBar()->showMessage("Ready");
}

void MainWindow::buildUi() {
    setWindowTitle("Vole Machine Simulator");
    resize(1280, 900);
    setMinimumSize(1120, 820);

    setStyleSheet(QStringLiteral(R"(
QMainWindow { background: #eef2f7; }
QWidget { color: #1f2937; font-size: 13px; }
QLabel#titleLabel { font-size: 24px; font-weight: 700; color: #0f172a; padding: 0 0 6px 0; }
QLabel#fieldLabel { color: #64748b; font-weight: 600; }
QLabel#valueLabel { font-family: "DejaVu Sans Mono", monospace; font-size: 14px; font-weight: 600; color: #0f172a; }
QFrame#card { background: #ffffff; border: 1px solid #e2e8f0; border-radius: 10px; }
QLabel#cardTitle { font-size: 13px; font-weight: 600; color: #475569; padding: 0 2px; }
QTableWidget { background: #ffffff; border: none; gridline-color: #eef2f7; selection-background-color: #dbeafe; selection-color: #1e3a8a; }
QHeaderView::section { background: #f8fafc; color: #475569; border: none; border-right: 1px solid #e2e8f0; border-bottom: 1px solid #e2e8f0; padding: 5px; font-weight: 600; }
QTableCornerButton::section { background: #f8fafc; border: none; }
QLineEdit { background: #ffffff; border: 1px solid #cbd5e1; border-radius: 8px; padding: 6px 10px; }
QLineEdit:focus { border: 1px solid #2563eb; }
QPushButton { background: #ffffff; border: 1px solid #cbd5e1; border-radius: 8px; padding: 8px 12px; font-weight: 600; }
QPushButton:hover { background: #f1f5f9; }
QPushButton:pressed { background: #e2e8f0; }
QPushButton:disabled { color: #94a3b8; background: #f8fafc; border-color: #e2e8f0; }
QPushButton#primaryButton { background: #2563eb; border: 1px solid #1d4ed8; color: #ffffff; }
QPushButton#primaryButton:hover { background: #1d4ed8; }
QPushButton#primaryButton:pressed { background: #1e40af; }
QPlainTextEdit { background: #0f172a; color: #e2e8f0; border: 1px solid #0f172a; border-radius: 8px; padding: 8px; selection-background-color: #1e40af; }
QStatusBar { background: #e2e8f0; color: #334155; }
QStatusBar::item { border: none; }
)"));

    QLabel* titleLabel = new QLabel("Vole Machine Simulator");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);

    QWidget* memoryBox = nullptr;
    memoryTable_ = new QTableWidget(16, 16, this);
    memoryTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    memoryTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    memoryTable_->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    memoryTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    memoryTable_->horizontalHeader()->setMinimumSectionSize(24);
    memoryTable_->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    memoryTable_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QStringList columns;
    for (unsigned column = 0; column < 16; ++column) {
        columns << QString::number(column, 16).toUpper();
    }
    memoryTable_->setHorizontalHeaderLabels(columns);

    QStringList rows;
    for (unsigned row = 0; row < 16; ++row) {
        rows << hexByte(row * 16);
    }
    memoryTable_->setVerticalHeaderLabels(rows);

    for (unsigned row = 0; row < 16; ++row) {
        for (unsigned column = 0; column < 16; ++column) {
            QTableWidgetItem* item = new QTableWidgetItem("00");
            item->setTextAlignment(Qt::AlignCenter);
            memoryTable_->setItem(static_cast<int>(row), static_cast<int>(column), item);
        }
    }

    QVBoxLayout* memoryLayout = new QVBoxLayout;
    memoryLayout->setContentsMargins(0, 0, 0, 0);
    memoryLayout->setSpacing(8);
    memoryLayout->addWidget(memoryTable_);
    memoryBox = makeCard("Memory (16x16)", memoryLayout);

    QWidget* cpuBox = nullptr;
    QGridLayout* cpuLayout = new QGridLayout;
    cpuLayout->setContentsMargins(0, 0, 0, 0);
    cpuLayout->setHorizontalSpacing(18);
    cpuLayout->setVerticalSpacing(6);
    cpuLayout->setColumnStretch(1, 1);

    QLabel* pcTitle = new QLabel("PC");
    pcTitle->setObjectName("fieldLabel");
    cpuLayout->addWidget(pcTitle, 0, 0);
    pcLabel_ = new QLabel("00");
    pcLabel_->setObjectName("valueLabel");
    cpuLayout->addWidget(pcLabel_, 0, 1);

    QLabel* irTitle = new QLabel("IR");
    irTitle->setObjectName("fieldLabel");
    cpuLayout->addWidget(irTitle, 1, 0);
    irLabel_ = new QLabel("0000");
    irLabel_->setObjectName("valueLabel");
    cpuLayout->addWidget(irLabel_, 1, 1);

    QLabel* haltedTitle = new QLabel("Halted");
    haltedTitle->setObjectName("fieldLabel");
    cpuLayout->addWidget(haltedTitle, 2, 0);
    haltedLabel_ = new QLabel("no");
    haltedLabel_->setObjectName("valueLabel");
    cpuLayout->addWidget(haltedLabel_, 2, 1);
    cpuBox = makeCard("CPU", cpuLayout);

    QWidget* registerBox = nullptr;
    registerTable_ = new QTableWidget(8, 4, this);
    registerTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    registerTable_->setSelectionMode(QAbstractItemView::NoSelection);
    registerTable_->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    registerTable_->setHorizontalHeaderLabels(QStringList() << "Register" << "Value" << "Register" << "Value");
    registerTable_->verticalHeader()->setVisible(false);
    registerTable_->verticalHeader()->setDefaultSectionSize(28);
    registerTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    registerTable_->setColumnWidth(0, 90);
    registerTable_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    registerTable_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    registerTable_->setColumnWidth(2, 90);
    registerTable_->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    registerTable_->setMinimumHeight(8 * 28 + 40);
    registerTable_->setMaximumHeight(8 * 28 + 40);
    registerTable_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    registerTable_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    for (unsigned row = 0; row < 8; ++row) {
        const unsigned first = row * 2;
        const unsigned second = first + 1;

        QTableWidgetItem* firstName = new QTableWidgetItem(QString("R") + QString::number(first, 16).toUpper());
        firstName->setTextAlignment(Qt::AlignCenter);
        registerTable_->setItem(static_cast<int>(row), 0, firstName);

        QTableWidgetItem* firstValue = new QTableWidgetItem("00");
        firstValue->setTextAlignment(Qt::AlignCenter);
        registerTable_->setItem(static_cast<int>(row), 1, firstValue);

        QTableWidgetItem* secondName = new QTableWidgetItem(QString("R") + QString::number(second, 16).toUpper());
        secondName->setTextAlignment(Qt::AlignCenter);
        registerTable_->setItem(static_cast<int>(row), 2, secondName);

        QTableWidgetItem* secondValue = new QTableWidgetItem("00");
        secondValue->setTextAlignment(Qt::AlignCenter);
        registerTable_->setItem(static_cast<int>(row), 3, secondValue);
    }

    QVBoxLayout* registerLayout = new QVBoxLayout;
    registerLayout->setContentsMargins(0, 0, 0, 0);
    registerLayout->setSpacing(8);
    registerLayout->addWidget(registerTable_);
    registerBox = makeCard("Registers", registerLayout);

    QWidget* screenBox = nullptr;
    screenConsole_ = new QPlainTextEdit;
    screenConsole_->setReadOnly(true);
    screenConsole_->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    screenConsole_->setMinimumHeight(80);
    screenConsole_->setMaximumHeight(100);
    QVBoxLayout* screenLayout = new QVBoxLayout;
    screenLayout->setContentsMargins(0, 0, 0, 0);
    screenLayout->setSpacing(8);
    screenLayout->addWidget(screenConsole_);
    screenBox = makeCard("Screen output (3RFF)", screenLayout);

    QWidget* controlBox = nullptr;
    addressEdit_ = new QLineEdit("00");
    addressEdit_->setMaxLength(2);
    addressEdit_->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9A-Fa-f]{1,2}"), this));
    addressEdit_->setAlignment(Qt::AlignCenter);

    loadButton_ = new QPushButton("Load File");
    addressButton_ = new QPushButton("Set Address");
    runButton_ = new QPushButton("Run");
    stepButton_ = new QPushButton("Single Step");
    clearButton_ = new QPushButton("Clear Memory");
    resetButton_ = new QPushButton("Reset CPU");

    runButton_->setObjectName("primaryButton");

    QGridLayout* controlLayout = new QGridLayout;
    controlLayout->setContentsMargins(0, 0, 0, 0);
    controlLayout->setHorizontalSpacing(10);
    controlLayout->setVerticalSpacing(8);
    controlLayout->setColumnStretch(0, 1);
    controlLayout->setColumnStretch(1, 1);

    QLabel* addressLabel = new QLabel("Load address (hex)");
    addressLabel->setObjectName("fieldLabel");
    controlLayout->addWidget(addressLabel, 0, 0);
    controlLayout->addWidget(addressEdit_, 0, 1);
    controlLayout->addWidget(loadButton_, 1, 0);
    controlLayout->addWidget(addressButton_, 1, 1);
    controlLayout->addWidget(runButton_, 2, 0);
    controlLayout->addWidget(stepButton_, 2, 1);
    controlLayout->addWidget(clearButton_, 3, 0);
    controlLayout->addWidget(resetButton_, 3, 1);
    controlBox = makeCard("Controls", controlLayout);

    QVBoxLayout* rightLayout = new QVBoxLayout;
    rightLayout->setSpacing(12);
    rightLayout->addWidget(cpuBox);
    rightLayout->addWidget(registerBox);
    rightLayout->addWidget(controlBox);
    rightLayout->addStretch();

    QHBoxLayout* bodyLayout = new QHBoxLayout;
    bodyLayout->setSpacing(16);
    bodyLayout->addWidget(memoryBox, 3);
    bodyLayout->addLayout(rightLayout, 2);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(18, 14, 18, 14);
    mainLayout->setSpacing(10);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(screenBox);
    mainLayout->addLayout(bodyLayout, 1);

    QWidget* central = new QWidget(this);
    central->setLayout(mainLayout);
    setCentralWidget(central);

    runTimer_ = new QTimer(this);
    runTimer_->setInterval(25);

    connect(loadButton_, &QPushButton::clicked, this, &MainWindow::loadProgram);
    connect(addressButton_, &QPushButton::clicked, this, &MainWindow::setAddress);
    connect(runButton_, &QPushButton::clicked, this, &MainWindow::run);
    connect(stepButton_, &QPushButton::clicked, this, &MainWindow::singleStep);
    connect(clearButton_, &QPushButton::clicked, this, &MainWindow::clearMemory);
    connect(resetButton_, &QPushButton::clicked, this, &MainWindow::resetCpu);
    connect(runTimer_, &QTimer::timeout, this, &MainWindow::runTick);
}

void MainWindow::loadProgram() {
    const unsigned address = loadAddress();
    if (address > 0xFEu || (address & 1u) != 0u) {
        QMessageBox::warning(this, "Invalid address", "Load address must be even, from 00 to FE.");
        return;
    }

    const QString path = QFileDialog::getOpenFileName(this, "Load Vole program", QString(),
                                                      "Vole programs (*.vol *.txt);;All files (*)");
    if (path.isEmpty()) {
        return;
    }

    loadProgramFile(path, address);
}

void MainWindow::loadProgramFile(const QString& path, unsigned address) {
    if (address > 0xFEu || (address & 1u) != 0u) {
        QMessageBox::warning(this, "Invalid address", "Load address must be even, from 00 to FE.");
        return;
    }

    const Vole::LoadResult result = machine_.loadProgram(path.toStdString(), static_cast<std::uint8_t>(address));
    if (!result.ok) {
        QMessageBox::warning(this, "Load failed", QString::fromStdString(result.err));
        return;
    }

    stepsRun_ = 0;
    refresh();
    statusBar()->showMessage("Loaded " + QString::number(result.words.size()) + " word(s) at 0x" + hexByte(address),
                             5000);
}

void MainWindow::setAddress() {
    const unsigned address = loadAddress();
    if (address > 0xFEu || (address & 1u) != 0u) {
        QMessageBox::warning(this, "Invalid address", "Load address must be even, from 00 to FE.");
        return;
    }

    addressEdit_->setText(hexByte(address));
    statusBar()->showMessage(QString("Load address set to 0x%1").arg(hexByte(address)), 5000);
}

void MainWindow::run() {
    if (runTimer_->isActive()) {
        stopRun("Stopped");
        return;
    }

    stepsRun_ = 0;
    setRunning(true);
    runTimer_->start();
}

void MainWindow::singleStep() {
    const StepResult result = machine_.step();
    refresh();
    statusBar()->showMessage(result.message.empty() ? QString("Executed") : QString::fromStdString(result.message),
                             5000);
}

void MainWindow::clearMemory() {
    for (unsigned address = 0; address < 256; ++address) {
        machine_.setMemory(static_cast<std::uint8_t>(address), 0);
    }
    refresh();
    statusBar()->showMessage("Memory cleared", 5000);
}

void MainWindow::resetCpu() {
    machine_.reset();
    stepsRun_ = 0;
    refresh();
    statusBar()->showMessage("CPU and memory reset", 5000);
}

void MainWindow::runTick() {
    if (stepsRun_ >= 1000000ULL) {
        stopRun("Step limit reached");
        return;
    }

    const StepResult result = machine_.step();
    ++stepsRun_;
    refresh();

    if (result.status != StepStatus::EXECUTED) {
        stopRun(result.message.empty() ? QString("Stopped") : QString::fromStdString(result.message));
    }
}

void MainWindow::refresh() {
    for (unsigned row = 0; row < 16; ++row) {
        for (unsigned column = 0; column < 16; ++column) {
            const unsigned address = row * 16 + column;
            memoryTable_->item(static_cast<int>(row), static_cast<int>(column))
                ->setText(hexByte(machine_.memory().read(static_cast<std::uint8_t>(address))));
        }
    }

    for (unsigned index = 0; index < 16; ++index) {
        const unsigned row = index / 2;
        const unsigned column = (index % 2 == 0) ? 1 : 3;
        registerTable_->item(static_cast<int>(row), static_cast<int>(column))
            ->setText(hexByte(machine_.cpu().getRegisters().get(static_cast<std::uint8_t>(index))));
    }

    pcLabel_->setText(hexByte(machine_.cpu().getPc()));
    irLabel_->setText(hexWord(static_cast<std::uint16_t>(machine_.cpu().getIr())));
    haltedLabel_->setText(machine_.cpu().isHalted() ? "yes" : "no");
    updateScreen();
}

void MainWindow::updateScreen() {
    const auto& bytes = machine_.screen().getBytes();
    if (bytes.empty()) {
        screenConsole_->setPlainText("(empty)");
        return;
    }

    QString hex;
    QString ascii;
    for (std::uint8_t value : bytes) {
        if (!hex.isEmpty()) {
            hex += ' ';
        }
        hex += hexByte(value);
        ascii += (value >= 32 && value <= 126) ? QChar::fromLatin1(static_cast<char>(value)) : QChar(u'.');
    }

    screenConsole_->setPlainText(hex + "\n" + ascii);
}

void MainWindow::setRunning(bool running) {
    loadButton_->setEnabled(!running);
    addressButton_->setEnabled(!running);
    stepButton_->setEnabled(!running);
    clearButton_->setEnabled(!running);
    resetButton_->setEnabled(!running);
    addressEdit_->setEnabled(!running);
    runButton_->setText(running ? "Stop" : "Run");
}

void MainWindow::stopRun(const QString& message) {
    runTimer_->stop();
    setRunning(false);
    statusBar()->showMessage(message, 5000);
}

unsigned MainWindow::loadAddress() const {
    bool ok = false;
    const unsigned value = addressEdit_->text().toUInt(&ok, 16);
    return ok ? value : 0xFFFFFFFFu;
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}

#include "gui_main.moc"
