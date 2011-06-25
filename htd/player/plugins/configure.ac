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

AC_INIT(plugins,1.0)
AM_INIT_AUTOMAKE(plugins,1.0)
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])
PKG_CHECK_MODULES(PLAYERCORE, playercore)
AC_SUBST(PLAYERCORE_CFLAGS)
AC_SUBST(PLAYERCORE_LIBS)
PKG_CHECK_MODULES(OPENCV, opencv)
AC_SUBST(OPENCV_CFLAGS)
AC_SUBST(OPENCV_LIBS)
AC_OUTPUT(Makefile camera/Makefile motion/Makefile face/Makefile ht/Makefile)
