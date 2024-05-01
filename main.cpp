/*
 * Copyright (C) 2024 Matthias Klumpp <matthias@tenstral.net>
 *
 * SPDX-License-Identifier: MIT
 */

#include <QApplication>

#include "toolbarwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ToolbarWindow w;
    w.show();

    return app.exec();
}
