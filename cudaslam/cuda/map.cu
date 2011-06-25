/*
 * Copyright (C) 2009 Ziyan Zhou <zhou@ziyan.info>
 * Visit http://ziyan.info/tag/ritcg2slam/ for more information
 *
 * This file is part of SLAM Visualization for Player.
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "map.h"

#include <cuda.h>
#include <stdio.h>
#include <math.h>

#define MAP_TRAVERSABLE -100
#define MAP_INTRAVERSABLE 100
#define MAP_UNKNOWN 0
#define MAP_UNEXPLORED 120

#define LASER_RANGE_MAX 8.0
#define LASER_RESOLUTION (M_PI / 360.0)

__device__ float __cuda_map_normalize(float a) {
	while(a < -M_PI) a += M_PI + M_PI;
	while(a >= M_PI) a -= M_PI + M_PI;
	return a;
}


#define D1 0.2
#define D2 0.25
#define D3 0.3

__device__ float __cuda_map_laser_model(float range, float a, float d) {
	float g =  -0.005 * (range > LASER_RANGE_MAX ? LASER_RANGE_MAX : range) + 0.05;
	float n = 1.0 / (0.05 * 2.0 * M_PI) * exp(a*a/-2.0);
	float s = g * n;
	if ( d < range - D1 ) {
		return -s;
	} else if ( d < range + D1 ) {
		return -s + s / D1 * ( d - range + D1 );
	} else if ( d < range + D2 ) {
		return s;
	} else if ( d < range + D3 ) {
		return s - s / (D3 - D2) * ( d - range - D2 );
	} else {
		return 0.0;
	}
}

__global__ void __cuda_map_laser(char* map, int width, int height, float mpp, float x, float y, float bearing, float *ranges, int top, int left)
{	
	int w = blockIdx.x + left;
	int h = threadIdx.x + top;
	// convert to map coords
	int ww = w + width / 2;
	int hh = h + height / 2;
	if (ww < 0 || hh < 0 || ww >= width || hh >= height) return;
	
	
	float dw = mpp * w;
	float dh = mpp * h;
	float t = sqrt((x-dw)*(x-dw)+(y-dh)*(y-dh));
	if (t > LASER_RANGE_MAX) return;



	float da = __cuda_map_normalize(atan2(dh - y, dw - x));
	int i = (int)ceil(__cuda_map_normalize(da - bearing - LASER_RESOLUTION / 2.0) / LASER_RESOLUTION) + 180;
	if(i < 0 || i >= 361) return;

	float change = __cuda_map_laser_model(ranges[i], __cuda_map_normalize(da - (bearing + LASER_RESOLUTION * (i - 180))), t);
	if(change == 0) return;

	char &value = map[hh * width + ww];
	if(value == MAP_UNEXPLORED) value = MAP_UNKNOWN;
	
	float prob = (float)value / 100.0 + change;
	if(prob > 1.0) prob = 1.0;
	if(prob < -1.0) prob = -1.0;
	value = 100 * prob;
	if(value < MAP_TRAVERSABLE) value = MAP_TRAVERSABLE;
	if(value > MAP_INTRAVERSABLE) value  = MAP_INTRAVERSABLE;
}

float normalize(const float a) {
	float angle = a;
	while(angle < -M_PI) angle += M_PI + M_PI;
	while(angle >= M_PI) angle -= M_PI + M_PI;
	return angle;
}

void cuda_map_create(void **map, const int width, const int height)
{
	cudaMalloc(map, sizeof(char) * width * height);
}

void cuda_map_free(void **map)
{
	cudaFree(*map);
	*map = 0;
}

void cuda_map_sync_to_cpu(const void *src, char *dst, const int width, const int height)
{
	cudaMemcpy(dst, src, sizeof(char) * width * height, cudaMemcpyDeviceToHost);
}

void cuda_map_sync_to_gpu(void *dst, const char *src, const int width, const int height)
{
	cudaMemcpy(dst, src, sizeof(char) * width * height, cudaMemcpyHostToDevice);
}



void cuda_map_laser(void *map, const int width, const int height, const float mpp, const float x, const float y, const float yaw, const float ranges[361])
{
	float bearing = normalize(yaw);

	// calculate bounding box
	int left = (int)floor((x - LASER_RANGE_MAX) / mpp);
	int right = (int)ceil((x + LASER_RANGE_MAX) / mpp);
	int top = (int)floor((y - LASER_RANGE_MAX) / mpp);
	int bottom = (int)ceil((y + LASER_RANGE_MAX) / mpp);

	float *r = 0;
	cudaMalloc(&r, sizeof(float) * 361);
	cudaMemcpy(r, ranges, sizeof(float) * 361, cudaMemcpyHostToDevice);

	__cuda_map_laser<<<right-left+1, bottom-top+1>>>((char*)map, width, height, mpp, x, y, bearing, r, top, left);
	cudaFree(r);
}


