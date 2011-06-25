#
# Copyright (C) 2009 Ziyan Zhou <zhou@ziyan.info>
# Visit http://ziyan.info/ for more information
#
# This file is part of libcudaslam, CUDA-based SLaM library for Player.
#
# libcudaslam is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# libcudaslam is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with libcudaslam.  If not, see <http://www.gnu.org/licenses/>.
#

AC_INIT(libcudaslam, 1.0, zhou@ziyan.info)
AM_INIT_AUTOMAKE(libcudaslam, 1.0, no-define)

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_LIBTOOL
AM_PROG_LIBTOOL

AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])

# detect player

PKG_CHECK_MODULES(PLAYERCORE, playercore)
AC_SUBST(PLAYERCORE_CFLAGS)
AC_SUBST(PLAYERCORE_LIBS)


# detect cuda

AC_ARG_WITH([cuda],
   [AS_HELP_STRING([--with-cuda=PATH],[prefix where cuda is installed @<:@default=/usr/local/cuda@:>@])],
   [],
   [with_cuda=/usr/local/cuda])

NVCC=/usr/local/cuda/bin/nvcc
CUDA_CFLAGS="-I$with_cuda/include"
CUDA_LIBS="-L$with_cuda/lib -lcuda"
AC_PATH_PROG([NVCC],[nvcc],[/usr/local/cuda/bin/nvcc],[$PATH:$with_cuda/bin])
AC_SUBST(CUDA_CFLAGS)
AC_SUBST(CUDA_LIBS)
AC_SUBST(NVCC)


AC_ARG_ENABLE([emu],
	[  --enable-emu    Turn on device emulation for CUDA],
		[case "${enableval}" in
			yes) EMULATION=true;;
			no)  EMULATION=false;;
			*) AC_MSG_ERROR([bad value ${enableval} for --enable-emu]);;
	esac],
	[EMULATION=false]
)


if test "$DEBUG" = "true"; then
	NVCCFLAGS="-g"
else
	NVCCFLAGS="-O3"
fi

if test "$EMULATION" = "true"; then
	NVCCFLAGS+=" -deviceemu"
fi
AC_SUBST(NVCCFLAGS)


AC_OUTPUT(Makefile icp/Makefile)

