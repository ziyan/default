#
# Copyright (C) 2009 Ziyan Zhou <zhou@ziyan.info>
# Visit http://ziyan.info/tag/head-tracking/ for more information
#
# This file is part of Head Tracking Display.
#
# Head Tracking Display is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Head Tracking Display is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Head Tracking Display.  If not, see <http://www.gnu.org/licenses/>.
#

bindir = $(prefix)/bin/
bin_PROGRAMS = htd

AM_CPPFLAGS = -Wall -g -ggdb
AM_CPPFLAGS += $(PLAYERCC_CFLAGS)
AM_CPPFLAGS += $(GL_CFLAGS)

htd_SOURCES = htd.cpp \
		../common/glm.c \
		display.h \
		display.c \
		texture.h \
		screen.h \
		screen.c \
		grid.h \
		grid.c \
		target.h \
		target.c \
		ball.h \
		ball.c
htd_LDFLAGS = $(PLAYERCC_LIBS) -lglut

