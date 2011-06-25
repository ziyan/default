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

libdir = $(prefix)/share/player/plugins/
lib_LTLIBRARIES = libcudaslamicp.la

AM_CPPFLAGS = -Wall -g -ggdb
AM_CPPFLAGS += $(PLAYERCORE_CFLAGS)
AM_CPPFLAGS += $(CUDA_CFLAGS)

.cu.lo:
	/bin/bash ${top_srcdir}/libtool --tag=CC --mode=compile $(NVCC) -prefer-non-pic --compiler-options -fPIC,-DPIC $(NVCCFLAGS) -c -o $@ $<
	/bin/bash ${top_srcdir}/libtool --tag=CC --mode=compile $(NVCC) -static $(NVCCFLAGS) -c -o $@ $<

libcudaslamicp_la_SOURCES = driver.cc test.cu
libcudaslamicp_la_LDFLAGS = $(PLAYERCORE_LIBS) $(CUDA_LIBS)
