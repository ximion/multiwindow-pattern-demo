/*
 * Copyright (C) 2024 Matthias Klumpp <matthias@tenstral.net>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <QWidget>
#include <QShowEvent>
#include <QPushButton>
#include <QComboBox>
#include <QHash>
#include <QVector>

class QTextBrowser;
class PrimarySnapWindow;
class SecondarySnapWindow;

class ToolbarWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ToolbarWindow(QWidget *parent = nullptr);
    ~ToolbarWindow() override;

    void logText(const QString &msg);

signals:

protected:
    void showEvent(QShowEvent *ev) override;
    void closeEvent(QCloseEvent *event) override;

private:
    QHash<QWidget *, QRect> currentWindowPositions();
    void loadSelectedWinPositionProfile();
    void setupTopSideLayout();
    void setupBottomLayout();

    bool m_windowsPositioned{false};
    QRect m_zoneRect;

    std::unique_ptr<QWidget> m_sidebarWindow;
    std::unique_ptr<QWidget> m_mainWindow;
    std::unique_ptr<QWidget> m_outputWindow;
    std::unique_ptr<QWidget> m_btnResponseWindow;

    QTextBrowser *m_logWidget;
    QComboBox *m_profilesComboBox;

    PrimarySnapWindow *m_snapWinPrim;
    SecondarySnapWindow *m_snapWinSec;
};
