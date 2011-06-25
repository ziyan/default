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

#include <model/shape.h>

#include <model/light.h>
#include <model/world.h>
#include <common/utils.h>

Shape::Shape(Object *parent, const material_t material, const point3d_t p, const point3d_t d) :
	Object(parent, p, d), material(material) {
}

Shape::~Shape() {
}

void Shape::setMaterial(const material_t material) {
	this->material = material;
}

material_t Shape::getMaterial() const {
	return material;
}

color_t Shape::phong(const point3d_t p, const point2d_t tc, const point3d_t n, const ray_t *ray) const {
	const World *world = this->world();
	color_t texture = material.texture == 0 ? material.color : material.texture(tc);
	if(!world) return texture;
	boost::numeric::ublas::vector<double> color = vector3d(0.0, 0.0, 0.0);

	boost::numeric::ublas::vector<double> vn = vector3d(n.x, n.y, n.z);
	boost::numeric::ublas::vector<double> vv = vector3d(-ray->d.x, -ray->d.y, -ray->d.z);
	double dvn = dot_prod(vv, vn);
	double nit = 1.0 / material.n;
	if (dvn < 0.0) {
		nit = material.n / 1.0;
		vn = -vn;
		dvn = dot_prod(vv, vn);
	}

	boost::numeric::ublas::vector<double> texture_color = vector3d(texture.r, texture.g, texture.b);
	boost::numeric::ublas::vector<double> specular_color = vector3d(material.specular.r, material.specular.g, material.specular.b);
	boost::numeric::ublas::vector<double> ambient_color = vector3d(world->ambient.r, world->ambient.g, world->ambient.b);

	// ambient
	color += material.ka * vector3d(ambient_color(0) * texture_color(0),
				 ambient_color(1) * texture_color(1),
				 ambient_color(2) * texture_color(2));

	double kr = material.kr;

	// transmission
	if(ray->ttl > 0 && material.kt > 0.0 && material.n > 0.0) {
		double l = 1.0 + nit*nit*(dvn*dvn - 1.0);
		if(l >= 0.0) {
			boost::numeric::ublas::vector<double> d = -nit * vv;
			d += (nit*dvn - sqrt(l)) * vn;
			d = normalize(d);
			ray_t transmission_ray;
			transmission_ray.p = p;
			transmission_ray.d = POINT3D(d(0), d(1), d(2));
			transmission_ray.near = 0.000000001;
			transmission_ray.far = DOUBLE_INF;
			transmission_ray.ttl = ray->ttl-1;
			color_t transmission_color = world->trace(&transmission_ray);
			color += material.kt * vector3d(transmission_color.r, transmission_color.g, transmission_color.b);
		} else
			kr += material.kt;
	}


	// reflection
	if(ray->ttl > 0 && kr > 0.0) {
		boost::numeric::ublas::vector<double> vr = 2.0 * dot_prod(vv, vn) * vn - vv;
		ray_t reflection_ray;
		reflection_ray.p = p;
		reflection_ray.d = POINT3D(vr(0), vr(1), vr(2));
		reflection_ray.near = 0.000001;
		reflection_ray.far = DOUBLE_INF;
		reflection_ray.ttl = ray->ttl-1;
		color_t reflection_color = world->trace(&reflection_ray);
		color += kr * vector3d(reflection_color.r, reflection_color.g, reflection_color.b);
	}

	// lights
	for(std::vector<Light*>::const_iterator i = world->lights.begin(); i != world->lights.end(); i++) {
		// shadow ray
		point3d_t s = (*i)->getSource();
		boost::numeric::ublas::vector<double> vs = vector3d(s.x - p.x, s.y - p.y, s.z - p.z);
		double light_distance = sqrt(dot_prod(vs, vs));
		vs = normalize(vs);
		ray_t shadow_ray;
		shadow_ray.p = p;
		shadow_ray.d = POINT3D(vs(0), vs(1), vs(2));
		shadow_ray.near = 0.00000001;
		shadow_ray.far = DOUBLE_INF;
		shadow_ray.ttl = 1;
		if(world->hit(&shadow_ray).t < light_distance) continue;

		light_t light = (*i)->getLight();
		boost::numeric::ublas::vector<double> light_color = vector3d(light.color.r, light.color.g, light.color.b);
		// diffuse
		color += material.kd * dot_prod(vs, vn) * vector3d(light_color(0) * texture_color(0),
								light_color(1) * texture_color(1),
								light_color(2) * texture_color(2));
		// specular
		boost::numeric::ublas::vector<double> vr = 2.0 * dot_prod(vs, vn) * vn - vs;
		double vra = dot_prod(vr, vv);
		if(vra >= 0.0) color += material.ks * pow(vra, material.ke) * vector3d(light_color(0) * specular_color(0),
							light_color(1) * specular_color(1),
							light_color(2) * specular_color(2));
	}

	// keep color in range
	if(color(0) > 1.0) color(0) = 1.0;
	if(color(0) < 0.0) color(0) = 0.0;
	if(color(1) > 1.0) color(1) = 1.0;
	if(color(1) < 0.0) color(1) = 0.0;
	if(color(2) > 1.0) color(2) = 1.0;
	if(color(2) < 0.0) color(2) = 0.0;
	return COLOR(color(0), color(1), color(2));
}
