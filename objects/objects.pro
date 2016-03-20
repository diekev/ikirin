# ***** BEGIN GPL LICENSE BLOCK *****
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# The Original Code is Copyright (C) 2015 Kévin Dietrich.
# All rights reserved.
#
# ***** END GPL LICENSE BLOCK *****

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = objects
TEMPLATE = lib

CONFIG += staticlib

include(../global.pri)

INCLUDEPATH += $$PWD/../
INCLUDEPATH += /opt/lib/ego/include
INCLUDEPATH += /opt/lib/kamikaze/include
INCLUDEPATH += /opt/lib/openvdb/include
INCLUDEPATH += /opt/lib/openexr/include

DEFINES += GLM_FORCE_RADIANS

SOURCES += \
	cube.cc \
	grid.cc \
    object_ops.cc \
    undo.cc

HEADERS += \
	cube.h \
	grid.h \
    object_ops.h \
    undo.h \
    factory.h
