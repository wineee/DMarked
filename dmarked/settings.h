/* -*- Mode: C++; indent-tabs-mode: nil; tab-width: 4 -*-
 * -*- coding: utf-8 -*-
 *
 * Copyright (C) 2011 ~ 2018 Deepin, Inc.
 * Copyright (C) 2021 DMarked.
 *
 * Author:     Wang Yong <wangyong@deepin.com>
 * Maintainer: Rekols    <rekols@foxmail.com>
 * Maintainer：Lu Hongxu <lhongxu@outlook.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include "dsettingsdialog.h"
#include <qsettingbackend.h>
#include <DKeySequenceEdit>
#include <DDialog>
#include <QSettings>
#include <QPointer>
#include <QKeyEvent>
#include <QDebug>
#include <DApplication>
#include <QLabel>
#include <QPushButton>


DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
DTK_USE_NAMESPACE


class Settings : public QObject
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

    //void dtkThemeWorkaround(QWidget *parent, const QString &theme);
    static QPair<QWidget*, QWidget*> createFontComBoBoxHandle(QObject *obj);
    static QPair<QWidget*, QWidget*> createKeySequenceEditHandle(QObject *obj);
    static Settings* instance();

    void setSettingDialog(DSettingsDialog *settingsDialog);

    int m_iDefaultFontSize = 12;
    int m_iMaxFontSize = 50;
    int m_iMinFontSize = 8;

    DSettings *settings;

signals:
    void sigAdjustFont(QString name);
    void sigAdjustFontSize(int fontSize);
    void sigAdjustTabSpaceNumber(int number);
    //void sigShowBlankCharacter(bool enable);
    void sigHightLightCurrentLine(bool enable);
    void sigThemeChanged(const QString &theme);
    void sigSetLineNumberShow(bool bIsShow);
    void sigChangeWindowSize(QString mode);

private:
    void updateAllKeysWithKeymap(QString keymap);
    void copyCustomizeKeysFromKeymap(QString keymap);
    bool checkShortcutValid(const QString &Name, QString Key, QString &Reason, bool &bIsConflicts);
    bool isShortcutConflict(const QString &Name, const QString &Key);
    DDialog *createDialog(const QString &title, const QString &content, const bool &bIsConflicts);

private:
    Dtk::Core::QSettingBackend *m_backend;

    bool m_bUserChangeKey = false;
    DSettingsDialog *m_pSettingsDialog;
    static Settings* s_pSetting;
    DKeySequenceEdit *m_pShortCutLineEdit;
    DDialog *m_pDialog;
};

class KeySequenceEdit : public DKeySequenceEdit
{
public:
    inline KeySequenceEdit(DTK_CORE_NAMESPACE::DSettingsOption *opt, QWidget *parent = nullptr): DKeySequenceEdit(parent)
    {
        m_pOption = opt;
        this->installEventFilter(this);
    }
	
    DTK_CORE_NAMESPACE::DSettingsOption *option()
    {
        return m_pOption;
    }

protected:
    inline bool eventFilter(QObject *object, QEvent *event)
    {
        //设置界面　回车键和空格键　切换输入 梁卫东　２０２０－０８－２１　１６：２８：３１
        if (object == this) {
            if (event->type() == QEvent::KeyPress) {
                QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
               //判断是否包含组合键　梁卫东　２０２０－０９－０２　１５：０３：５６
                Qt::KeyboardModifiers modifiers = keyEvent->modifiers();
                bool bHasModifier = (modifiers & Qt::ShiftModifier ||modifiers & Qt::ControlModifier ||
                                     modifiers & Qt::AltModifier);

                if (!bHasModifier && (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Space)) {
                    QRect rect = this->rect();
                    QList<QLabel*> childern = findChildren<QLabel*>();

                    for (int i =0; i < childern.size(); i++) {
                        QPoint pos(25,rect.height()/2);

                        QMouseEvent event0(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
                        DApplication::sendEvent(childern[i], &event0);
                    }

                    return true;
                }
            }
        }

        return DKeySequenceEdit::eventFilter(object, event);
    }

private:
    DTK_CORE_NAMESPACE::DSettingsOption *m_pOption = nullptr;
};

#endif // SETTINGS_H
