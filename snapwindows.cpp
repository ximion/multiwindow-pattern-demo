/*
 * Copyright (C) 2024 Matthias Klumpp <matthias@tenstral.net>
 *
 * SPDX-License-Identifier: MIT
 */

#include "snapwindows.h"

#include <QVBoxLayout>
#include <QWindowStateChangeEvent>

static Qt::Edge edgeFromPointAngle(const QPoint &p1, const QPoint &p2)
{
    int deltaX = p2.x() - p1.x();
    int deltaY = p2.y() - p1.y();

    if (deltaY > 0 && std::abs(deltaX) <= std::abs(deltaY))
        return Qt::BottomEdge;
    else if (deltaY < 0 && std::abs(deltaX) <= std::abs(deltaY))
        return Qt::TopEdge;
    else if (deltaX > 0 && std::abs(deltaX) > std::abs(deltaY))
        return Qt::RightEdge;
    else
        return Qt::LeftEdge;
}

PrimarySnapWindow::PrimarySnapWindow(QWidget *parent)
    : QWidget(parent),
      m_infoLabel(new QLabel(this))
{
    resize(400, 300);
    setWindowTitle("Primary Window");

    QFont font = m_infoLabel->font();
    font.setPointSize(24);
    m_infoLabel->setFont(font);
    m_infoLabel->setText("Primary Window.");
}

void PrimarySnapWindow::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);
    if (m_secondaryWindow) {
        emit primaryWindowMoved();

        if (m_snapped)
            setSecondaryWindowSnapped(true);
    }
}

void PrimarySnapWindow::closeEvent(QCloseEvent *event)
{
    if (m_secondaryWindow)
        m_secondaryWindow->close();
    QWidget::closeEvent(event);
}

void PrimarySnapWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange) {
        auto scEvent = static_cast<QWindowStateChangeEvent *>(event);
        if (isMinimized()) {
            if (m_snapped && m_secondaryWindow)
                m_secondaryWindow->showMinimized();
        } else if (scEvent->oldState() & Qt::WindowMinimized && !isMinimized()) {
            if (m_snapped && m_secondaryWindow)
                m_secondaryWindow->showNormal();
        }
    }
    QWidget::changeEvent(event);
}

void PrimarySnapWindow::setSecondaryWindow(QWidget *secondary)
{
    m_secondaryWindow = secondary;
}

void PrimarySnapWindow::setSecondaryWindowSnapped(bool snapped)
{
    if (!snapped) {
        m_snapped = false;
        return;
    }

    QRect primaryGeometry = frameGeometry();
    QRect secondaryGeometry = m_secondaryWindow->frameGeometry();

    const auto snapEdge = edgeFromPointAngle(primaryGeometry.center(), secondaryGeometry.center());
    switch (snapEdge) {
    case Qt::LeftEdge:
        m_secondaryWindow->move(primaryGeometry.left() - secondaryGeometry.width(), primaryGeometry.top());
        break;
    case Qt::RightEdge:
        m_secondaryWindow->move(primaryGeometry.right(), primaryGeometry.top());
        break;
    case Qt::TopEdge:
        m_secondaryWindow->move(primaryGeometry.left(), primaryGeometry.top() - secondaryGeometry.height());
        break;
    case Qt::BottomEdge:
        m_secondaryWindow->move(primaryGeometry.left(), primaryGeometry.bottom());
        break;
    default:
        break;
    }
    m_snapped = true;
}

SecondarySnapWindow::SecondarySnapWindow(QWidget *parent)
    : QWidget(parent)
{
    resize(250, 200);
    setWindowTitle("Secondary Attachable Window");

    auto layout = new QVBoxLayout(this);
    m_snapButton = new QPushButton("Snap Window", this);
    m_snapButton->setCheckable(true);
    m_snapButton->move(50, 40);
    layout->addWidget(m_snapButton);
    layout->addStretch();
    setLayout(layout);

    connect(m_snapButton, &QPushButton::toggled, [this](bool checked) {
        if (!m_primaryWindow)
            return;

        if (checked) {
            m_primaryWindow->setSecondaryWindowSnapped(true);
            m_snapButton->setText("Unsnap Window");
        } else {
            m_primaryWindow->setSecondaryWindowSnapped(false);
            m_snapButton->setText("Snap Window");
        }
    });
}

void SecondarySnapWindow::setPrimaryWindow(PrimarySnapWindow *primary)
{
    m_primaryWindow = primary;

    connect(m_primaryWindow, &PrimarySnapWindow::primaryWindowMoved, [this]() {
        primaryPositionChanged(edgeFromPointAngle(m_primaryWindow->frameGeometry().center(), frameGeometry().center()));
    });
}

void SecondarySnapWindow::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);
    primaryPositionChanged(edgeFromPointAngle(m_primaryWindow->frameGeometry().center(), frameGeometry().center()));
}

void SecondarySnapWindow::primaryPositionChanged(Qt::Edge edge)
{
    switch (edge) {
    case Qt::LeftEdge:
        m_snapButton->setIcon(QIcon::fromTheme("arrow-right"));
        break;
    case Qt::RightEdge:
        m_snapButton->setIcon(QIcon::fromTheme("arrow-left"));
        break;
    case Qt::TopEdge:
        m_snapButton->setIcon(QIcon::fromTheme("arrow-down"));
        break;
    case Qt::BottomEdge:
        m_snapButton->setIcon(QIcon::fromTheme("arrow-up"));
        break;
    default:
        break;
    }
}
