/********************************************************************************
*   Copyright 2014 David Edmundson <davidedmundson@kde.org>                     *
*                                                                               *
*   This library is free software; you can redistribute it and/or               *
*   modify it under the terms of the GNU Library General Public                 *
*   License as published by the Free Software Foundation; either                *
*   version 2 of the License, or (at your option) any later version.            *
*                                                                               *
*   This library is distributed in the hope that it will be useful,             *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of              *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU            *
*   Library General Public License for more details.                            *
*                                                                               *
*   You should have received a copy of the GNU Library General Public License   *
*   along with this library; see the file COPYING.LIB.  If not, write to        *
*   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,        *
*   Boston, MA 02110-1301, USA.                                                 *
*********************************************************************************/

#include "dialogqmltest.h"

#include "plasmaquick/dialog.h"
#include <plasma.h>

#include <QQmlEngine>
#include <QQmlContext>

#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>


//this test checks that we don't set visible to true until after we set the window flags
void DialogQmlTest::loadAndShow()
{
    QQmlEngine engine;

    QByteArray dialogQml =
"import QtQuick 2.0\n"
"import org.kde.plasma.core 2.0 as PlasmaCore\n"
"\n"
"PlasmaCore.Dialog {\n"
"    id: root\n"
"\n"
"    location: true && PlasmaCore.Types.TopEdge\n"
"    visible: true && true\n"
"    type: true && PlasmaCore.Dialog.Notification\n"
"\n"
"    mainItem: Rectangle {\n"
"        width: 200\n"
"        height: 200\n"
"    }\n"
"}\n";

    //we use true && Value to force it to be a complex binding, which won't be evaluated in
    //component.beginCreate
    //the bug still appears without this, but we need to delay it in this test
    //so we can connect to the visibleChanged signal


    QQmlComponent component(&engine);

    QSignalSpy spy(&component, SIGNAL(statusChanged(QQmlComponent::Status)));
    component.setData(dialogQml, QUrl("test://dialogTest"));
    spy.wait();

    PlasmaQuick::Dialog *dialog = qobject_cast< PlasmaQuick::Dialog* >(component.beginCreate(engine.rootContext()));
    qDebug() << component.errorString();
    Q_ASSERT(dialog);

    m_dialogShown = false;

    //this will be called during component.completeCreate
    auto c = connect(dialog, &QWindow::visibleChanged, [=]() {
        m_dialogShown = true;
        QCOMPARE(dialog->type(), PlasmaQuick::Dialog::Notification);
        QCOMPARE(dialog->location(), Plasma::Types::TopEdge);
    });

    component.completeCreate();
    QCOMPARE(m_dialogShown, true);

    //disconnect on visible changed before we delete the dialog
    disconnect(c);

    delete dialog;
}



QTEST_MAIN(DialogQmlTest)