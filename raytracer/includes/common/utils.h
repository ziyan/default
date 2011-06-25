/*
 * Copyright (C) 2009 Ziyan Zhou <zhou@ziyan.info>
 * Visit http://ziyan.info/tag/ritcg2/ for more information
 *
 * This file is part of Raytracer.
 *
 * Raytracer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Raytracer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Raytracer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ZIYAN_RAYTRACER_COMMON_UTILS_H
#define ZIYAN_RAYTRACER_COMMON_UTILS_H

#if HAVE_CONFIG_H
#include <config.h>
#endif

//#include <limits>
//#define DOUBLE_NAN std::numeric_limits<double>::quiet_NaN()
//#define DOUBLE_INF std::numeric_limits<double>::infinity()

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

boost::numeric::ublas::vector<double> vector3d(double x, double y, double z);
boost::numeric::ublas::vector<double> cross_prod(boost::numeric::ublas::vector<double> a, boost::numeric::ublas::vector<double> b);
double dot_prod(boost::numeric::ublas::vector<double> a, boost::numeric::ublas::vector<double> b);
boost::numeric::ublas::vector<double> normalize(boost::numeric::ublas::vector<double> a);

#endif

