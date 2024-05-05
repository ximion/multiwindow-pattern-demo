/*
 * Copyright (C) 2024 Matthias Klumpp <matthias@tenstral.net>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class QTextBrowser;

class PrimarySnapWindow : public QWidget
{
    Q_OBJECT
public:
    explicit PrimarySnapWindow(QWidget *parent = nullptr);
    void setSecondaryWindow(QWidget *secondary);

    void setSecondaryWindowSnapped(bool snapped);

signals:
    void primaryWindowMoved();

protected:
    void moveEvent(QMoveEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    QWidget *m_secondaryWindow = nullptr;
    QLabel *m_infoLabel;
    bool m_snapped = false;
};

class SecondarySnapWindow : public QWidget
{
    Q_OBJECT
public:
    explicit SecondarySnapWindow(QWidget *parent = nullptr);
    void setPrimaryWindow(PrimarySnapWindow *primary);

protected:
    void moveEvent(QMoveEvent *event) override;
    void primaryPositionChanged(Qt::Edge edge);

private:
    PrimarySnapWindow *m_primaryWindow = nullptr;
    QPushButton *m_snapButton;
};
