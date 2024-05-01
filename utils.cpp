/*
 * Copyright (C) 2024 Matthias Klumpp <matthias@tenstral.net>
 *
 * SPDX-License-Identifier: MIT
 */

#include "utils.h"

#include <QApplication>
#include <QScreen>

QRect multiWindowZone(double percentageOfMax)
{
    if (percentageOfMax > 1.0)
        percentageOfMax = 1.0;
    if (percentageOfMax < 0.05)
        percentageOfMax = 0.05;

    auto guiApp = qobject_cast<QApplication *>(QApplication::instance());
    if (guiApp == nullptr)
        return QRect();

    auto zoneRect = guiApp->primaryScreen()->availableGeometry();

    zoneRect.setX(zoneRect.width() * (1.0 - percentageOfMax));
    zoneRect.setY(zoneRect.height() * (1.0 - percentageOfMax));
    zoneRect.setWidth(zoneRect.width() * percentageOfMax);
    zoneRect.setHeight(zoneRect.height() * percentageOfMax);

    return zoneRect;
}
