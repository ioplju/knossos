/*
 *  This file is a part of KNOSSOS.
 *
 *  (C) Copyright 2007-2018
 *  Max-Planck-Gesellschaft zur Foerderung der Wissenschaften e.V.
 *
 *  KNOSSOS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 of
 *  the License as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *  For further information, visit https://knossos.app
 *  or contact knossosteam@gmail.com
 */
#pragma once

#include "GuiConstants.h"

#include <QDialog>
#include <QSettings>

class DialogVisibilityNotify : public QDialog {
    Q_OBJECT
    const QString settingsPrefix;
public:
    DialogVisibilityNotify(const QString & settingsPrefix, QWidget * parent) : QDialog{parent}, settingsPrefix{settingsPrefix} {
        setWindowFlag(Qt::WindowContextHelpButtonHint, false);
#ifdef Q_OS_MACOS// workaround for child dialogs not being on top of their parent on macOS
        setWindowFlag(Qt::WindowStaysOnTopHint);
#endif//Q_OS_MACOS
    }
signals:
    void visibilityChanged(bool);
protected:
    virtual void showEvent(QShowEvent *event) override {
        QDialog::showEvent(event);
        emit visibilityChanged(true);
    }
    virtual void hideEvent(QHideEvent *event) override {
        QSettings{}.setValue(settingsPrefix + '/' + GEOMETRY, saveGeometry());// saving geometry only works for visible widgets
        QDialog::hideEvent(event);
        emit visibilityChanged(false);
    }
public:
    virtual void loadSettings() {
        QSettings settings;
        settings.beginGroup(settingsPrefix);
        restoreGeometry(settings.value(GEOMETRY).toByteArray());
    }
    virtual void saveSettings() {
        QSettings{}.setValue(settingsPrefix + '/' + VISIBLE, isVisible());
    }
};
