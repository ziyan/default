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

#include <common/utils.h>

#include <boost/numeric/ublas/matrix.hpp>

boost::numeric::ublas::vector<double> cross_prod(boost::numeric::ublas::vector<double> a, boost::numeric::ublas::vector<double> b) {
	boost::numeric::ublas::matrix<double> m(3, 3);
	m(0,0) = 0.0;   m(0,1) = -a(2); m(0,2) = a(1);
	m(1,0) = a(2);  m(1,1) = 0.0;   m(1,2) = -a(0);
	m(2,0) = -a(1); m(2,1) = a(0);  m(2,2) = 0.0;
	return prod(m, b);
}

double dot_prod(boost::numeric::ublas::vector<double> a, boost::numeric::ublas::vector<double> b) {
	return a(0)*b(0) + a(1)*b(1) + a(2)*b(2);
}

boost::numeric::ublas::vector<double> normalize(boost::numeric::ublas::vector<double> a) {
	return a / sqrt(a(0)*a(0) + a(1)*a(1) + a(2)*a(2));	
}

boost::numeric::ublas::vector<double> vector3d(double x, double y, double z) {
	boost::numeric::ublas::vector<double> v(3);
	v(0) = x; v(1) = y; v(2) = z;
	return v;
}

