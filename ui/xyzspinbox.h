/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software  Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2015 Kévin Dietrich.
 * All rights reserved.
 *
 * ***** END GPL LICENSE BLOCK *****
 *
 */

#pragma once

#include <QWidget>

class QDoubleSpinBox;
class QVBoxLayout;

class XYZSpinBox : public QWidget {
	Q_OBJECT

	QDoubleSpinBox *m_x, *m_y, *m_z;
	QVBoxLayout *m_layout;

private Q_SLOTS:
	void xValueChanged(double value);
	void yValueChanged(double value);
	void zValueChanged(double value);

Q_SIGNALS:
	void valueChanged(double value, int axis);

public:
	explicit XYZSpinBox(QWidget *parent = 0);
	~XYZSpinBox();

	void setValue(float *value);
	void getValue(float *value) const;
	void setMinMax(float min, float max) const;
};
