#
# Copyright (C) 2009 Ziyan Zhou <zhou@ziyan.info>
# Visit http://ziyan.info/tag/ritcg2/ for more information
#
# This file is part of Raytracer.
#
# Raytracer is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Raytracer is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Raytracer.  If not, see <http://www.gnu.org/licenses/>.
#

bindir = $(prefix)/bin/
bin_PROGRAMS = raytracer tonereproduction

AM_CPPFLAGS = -Wall -g -ggdb -I./includes/

raytracer_SOURCES = raytracer.cpp \
			includes/common/utils.h \
			includes/common/color.h \
			includes/common/point3d.h \
			includes/common/point2d.h \
			includes/common/ray.h \
			includes/projection/camera.h \
			includes/projection/image.h \
			includes/model/object.h \
			includes/model/world.h \
			includes/model/light.h \
			includes/model/shape.h \
			includes/shapes/cone.h \
			includes/shapes/cube.h \
			includes/shapes/sphere.h \
			includes/shapes/triangle.h \
			common/utils.cpp \
			projection/camera.cpp \
			projection/image.cpp \
			model/object.cpp \
			model/world.cpp \
			model/light.cpp \
			model/shape.cpp \
			shapes/cone.cpp \
			shapes/cube.cpp \
			shapes/sphere.cpp \
			shapes/triangle.cpp
tonereproduction_SOURCES = tonereproduction.cpp \
			includes/common/color.h \
			includes/projection/image.h \
			projection/image.cpp

