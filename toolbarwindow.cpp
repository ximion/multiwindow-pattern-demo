/*
 * Copyright (C) 2024 Matthias Klumpp <matthias@tenstral.net>
 *
 * SPDX-License-Identifier: MIT
 */

#include "toolbarwindow.h"

#include <QTimer>
#include <QApplication>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QInputDialog>
#include <QLabel>
#include <QPainter>

#include "utils.h"

// Set some initial window size constraints
static const int toolbarHeight = 115; // Thin toolbar window at the top
static const int sidebarWidth = 200;  // Long toolbar window to the left
static const int outputHeight = 100;  // "Output" window at the bottom

class RotatedLabel : public QLabel
{
public:
    RotatedLabel(const QString &text, QWidget *parent = nullptr)
        : QLabel(text, parent)
    {
        QFont font = this->font();
        font.setPointSize(24);
        setFont(font);
        setAttribute(Qt::WA_TranslucentBackground);
        setAlignment(Qt::AlignCenter);
    }

    QSize sizeHint() const override
    {
        QFontMetrics metrics(font());
        QRect rect = metrics.boundingRect(QRect(0, 0, 1000, 1000), Qt::AlignCenter | Qt::TextWordWrap, text());

        // calculate the dimensions of the rotated text
        double angle = 45 * M_PI / 180;
        double cos_angle = std::cos(angle);
        double sin_angle = std::sin(angle);

        int width = rect.width();
        int height = rect.height();

        int rotatedWidth = int(std::ceil(std::abs(width * cos_angle) + std::abs(height * sin_angle)));
        int rotatedHeight = int(std::ceil(std::abs(width * sin_angle) + std::abs(height * cos_angle)));

        return QSize(rotatedWidth, rotatedHeight);
    }

    QSize minimumSizeHint() const override
    {
        return sizeHint();
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.translate(width() / 2, height() / 2);
        painter.rotate(45);

        // recalculate the text position to ensure it's centered post-rotation
        QFontMetrics metrics(font());
        QRect rect = metrics.boundingRect(QRect(0, 0, width(), height()), Qt::AlignCenter, text());
        painter.drawText(-rect.width() / 2, -rect.height() / 2 + metrics.ascent(), text());
    }
};

ToolbarWindow::ToolbarWindow(QWidget *parent)
    : QWidget{parent}
{
    // Get the available space for us to place windows in
    m_zoneRect = multiWindowZone();
    qDebug() << "Multiwindow Zone: " << m_zoneRect;

    setGeometry(0, 0, m_zoneRect.width(), toolbarHeight);
    move(m_zoneRect.x(), m_zoneRect.y());
    setWindowTitle("Toolbar Window");
    show();

    m_sidebarWindow = std::make_unique<QWidget>();
    m_mainWindow = std::make_unique<QWidget>();
    m_outputWindow = std::make_unique<QWidget>();
    m_btnResponseWindow = std::make_unique<QWidget>();

    // create output text widget
    m_logWidget = new QTextBrowser(m_outputWindow.get());
    auto outputLayout = new QVBoxLayout(m_outputWindow.get());
    outputLayout->addWidget(m_logWidget);
    m_outputWindow->setLayout(outputLayout);
    m_logWidget->setFontFamily(QStringLiteral("Monospace"));
    logText(QStringLiteral("Multiwindow Zone: x=%1, y=%2, width=%3, height=%4")
                .arg(m_zoneRect.x())
                .arg(m_zoneRect.y())
                .arg(m_zoneRect.width())
                .arg(m_zoneRect.height()));

    // Create toolbar widgets
    auto tbLayout = new QHBoxLayout(this);

    auto profilesGroup = new QGroupBox("Profiles", this);
    auto profilesLayout = new QVBoxLayout(profilesGroup);
    m_profilesComboBox = new QComboBox(profilesGroup);
    auto saveButton = new QPushButton("Save Profile", profilesGroup);
    auto loadButton = new QPushButton("Load Profile", profilesGroup);

    profilesLayout->addWidget(m_profilesComboBox);
    profilesLayout->addWidget(saveButton);
    profilesLayout->addWidget(loadButton);
    profilesLayout->addStretch(1);
    profilesGroup->setLayout(profilesLayout);

    auto expGroup = new QGroupBox("Experiments", this);
    auto expLayout = new QVBoxLayout(expGroup);
    auto otherButton = new QPushButton("Magnetic Windows", expGroup);

    expLayout->addWidget(otherButton);
    expLayout->addStretch(1);
    expGroup->setLayout(expLayout);

    tbLayout->addWidget(profilesGroup);
    tbLayout->addWidget(expGroup);
    tbLayout->addStretch(1);

    setLayout(tbLayout);

    // create sidebar widgets
    auto sbLayout = new QVBoxLayout(m_sidebarWindow.get());

    auto winButton = new QPushButton("A window will appear\nnext to the cursor...\nClick me!", m_sidebarWindow.get());
    sbLayout->addWidget(winButton);
    auto pinButton = new QPushButton("Float always on top", m_sidebarWindow.get());
    pinButton->setCheckable(true);
    sbLayout->addWidget(winButton);
    sbLayout->addWidget(pinButton);
    sbLayout->addStretch(1);
    m_sidebarWindow->setLayout(sbLayout);

    // configure button response window
    m_btnResponseWindow->setWindowTitle("Wow!");
    m_btnResponseWindow->setGeometry(0, 0, 140, 120);
    auto btnWinLayout = new QVBoxLayout(m_btnResponseWindow.get());
    auto btnWinLabel = new RotatedLabel("WOW!", m_btnResponseWindow.get());
    btnWinLayout->addWidget(btnWinLabel, 0, Qt::AlignCenter);

    connect(winButton, &QPushButton::clicked, this, [this] {
        auto cursorPos = QCursor::pos();
        m_btnResponseWindow->move(cursorPos.x(), cursorPos.y() - (m_btnResponseWindow->height() / 2.0));
        m_btnResponseWindow->show();
        m_btnResponseWindow->raise();
        logText(QStringLiteral("Button clicked at %1, %2").arg(cursorPos.x()).arg(cursorPos.y()));
    });

    connect(pinButton, &QPushButton::toggled, this, [this](bool checked) {
        if (checked) {
            m_sidebarWindow->setWindowFlags(m_sidebarWindow->windowFlags() | Qt::WindowStaysOnTopHint);
            logText(QStringLiteral("Sidebar is now floating on top"));
        } else {
            m_sidebarWindow->setWindowFlags(m_sidebarWindow->windowFlags() & ~Qt::WindowStaysOnTopHint);
            logText(QStringLiteral("Sidebar returned to normal behavior."));
        }
        m_sidebarWindow->show();
    });

    // connect profile button clicks
    connect(saveButton, &QPushButton::clicked, this, [this] {
        bool ok;
        const auto profileName = QInputDialog::getText(
            this,
            "Save current window positions",
            "Layout Profile Name:",
            QLineEdit::Normal,
            "New" + m_profilesComboBox->currentText(),
            &ok);
        if (!ok || profileName.isEmpty())
            return;

        for (int i = 0; i < m_profilesComboBox->count(); ++i) {
            if (m_profilesComboBox->itemText(i) == profileName) {
                logText(QStringLiteral("Profile '%1' already exists!").arg(profileName));
                return;
            }
        }

        m_profilesComboBox->addItem(profileName, QVariant::fromValue(currentWindowPositions()));
        m_profilesComboBox->setCurrentIndex(m_profilesComboBox->count() - 1);
        logText(QStringLiteral("Saved new profile: %1").arg(profileName));
    });

    connect(m_profilesComboBox, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
        loadSelectedWinPositionProfile();
    });

    connect(loadButton, &QPushButton::clicked, this, [this] {
        loadSelectedWinPositionProfile();
    });

    // Add some label texts
    auto mainLayout = new QVBoxLayout(m_mainWindow.get());
    auto label = new RotatedLabel("Some very important content", m_mainWindow.get());
    mainLayout->addWidget(label, 0, Qt::AlignCenter);

    // On X11 with client-side decoration, we have no idea when our window will be decorated,
    // so we use a silly timer to get a good chance of having the right dimensions.
    // On Wayland, this issue can be easily resolved by using xdg-decoration and ext-placement,
    // which do not replicate this issue.
    QTimer::singleShot(50, this, [this] {
        const auto toolbarBottom = frameGeometry().y() + frameGeometry().height();

        // Position the sidebar window
        m_sidebarWindow->setGeometry(0, 0, sidebarWidth, m_zoneRect.height() - frameGeometry().height() - outputHeight);
        m_sidebarWindow->move(m_zoneRect.x(), toolbarBottom);
        m_sidebarWindow->setWindowTitle("Sidebar Window");
        m_sidebarWindow->show();

        // Update the position for the next window
        const auto mainHeight = m_sidebarWindow->frameGeometry().height();

        // Position the main window
        m_mainWindow->setGeometry(0, 0, m_zoneRect.width() - sidebarWidth, mainHeight);
        m_mainWindow->move(m_sidebarWindow->frameGeometry().right() + 1, toolbarBottom);
        m_mainWindow->setWindowTitle("Main Window");
        m_mainWindow->show();

        QTimer::singleShot(50, this, [this] {
            // Position the output window
            m_outputWindow->setGeometry(0, 0, m_zoneRect.width(), outputHeight);
            m_outputWindow->move(m_zoneRect.x(), m_mainWindow->frameGeometry().bottom() + 1);
            m_outputWindow->setWindowTitle("Output Window");
            m_outputWindow->show();

            // create our default window position profile
            m_profilesComboBox->addItem("Default", QVariant::fromValue(currentWindowPositions()));
        });
    });
}

void ToolbarWindow::logText(const QString &msg)
{
    m_logWidget->append(msg);
}

void ToolbarWindow::showEvent(QShowEvent *ev)
{
    QWidget::showEvent(ev);
}

void ToolbarWindow::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    QApplication::quit();
}

QHash<QWidget *, QRect> ToolbarWindow::currentWindowPositions()
{
    QHash<QWidget *, QRect> winPositions;
    winPositions[this] = this->geometry();
    winPositions[m_mainWindow.get()] = m_mainWindow->geometry();
    winPositions[m_sidebarWindow.get()] = m_sidebarWindow->geometry();
    winPositions[m_outputWindow.get()] = m_outputWindow->geometry();

    return winPositions;
}

void ToolbarWindow::loadSelectedWinPositionProfile()
{
    const auto profileData = m_profilesComboBox->currentData().value<QHash<QWidget *, QRect>>();
    for (auto it = profileData.constBegin(); it != profileData.constEnd(); ++it)
        it.key()->setGeometry(it.value());

    logText(QStringLiteral("Loaded profile: %1").arg(m_profilesComboBox->currentText()));
}
