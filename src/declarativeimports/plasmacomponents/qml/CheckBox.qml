/*
*   Copyright (C) 2011 by Daker Fernandes Pinheiro <dakerfp@gmail.com>
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU Library General Public License as
*   published by the Free Software Foundation; either version 2, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details
*
*   You should have received a copy of the GNU Library General Public
*   License along with this program; if not, write to the
*   Free Software Foundation, Inc.,
*   51 Franklin Street, Fifth Floor, Boston, MA  2.010-1301, USA.
*/

import QtQuick 2.1
import QtQuick.Controls 1.2 as QtControls
import org.kde.plasma.core 2.0 as PlasmaCore
import "styles" as Styles

/**
 * A check box is a component that can be switched on (checked) or off
 * (unchecked). Check boxes are typically used to represent features in an
 * application that can be enabled or disabled without affecting others, but
 * different types of behavior can be implemented. When a check box is checked
 * or unchecked it sends a clicked signal for the application to handle.
 *
 * When a check box has the focus, its state can be toggled using the
 * Qt.Key_Select, Qt.Key_Return, and Qt.Key_Enter hardware keys that send the
 * clicked signal.
 *
 * @inherit QtQuick.Controls.CheckBox
 */
QtControls.CheckBox {
    id: checkBox

    style: Styles.CheckBoxStyle {}
}
