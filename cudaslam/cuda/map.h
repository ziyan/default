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

#ifndef SLAM_CUDA_MAP_H
#define SLAM_CUDA_MAP_H

void cuda_map_create(void**, const int, const int);
void cuda_map_free(void**);

void cuda_map_sync_to_cpu(const void*, char*, const int, const int);
void cuda_map_sync_to_gpu(void*, const char*, const int, const int);

void cuda_map_laser(void*, const int, const int, const float, const float x, const float y, const float yaw, const float ranges[361]);

#endif //SLAM_CUDA_MAP_H

