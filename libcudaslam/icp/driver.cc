/*
 * Copyright (C) 2009 Ziyan Zhou <zhou@ziyan.info>
 * Visit http://ziyan.info/ for more information
 *
 * This file is part of libcudaslam, CUDA-based SLaM library for Player.
 *
 * libcudaslam is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libcudaslam is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libcudaslam.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "driver.h"
#include <unistd.h>

using namespace lcs;

ICPDriver::ICPDriver(ConfigFile* cf, int section) : Driver(cf, section) {
}

ICPDriver::~ICPDriver() {
}

int ICPDriver::Setup() {
	this->StartThread();
	return 0;
}

int ICPDriver::Shutdown() {
	this->StopThread();
	return 0;
}

int ICPDriver::ProcessMessage(QueuePointer &resp_queue, player_msghdr *hdr, void *data) {
	return -1;
}

void ICPDriver::Main() {
	for(;;) {
		pthread_testcancel();
		this->ProcessMessages();
		usleep(25000);
	}
}

Driver* driver_Init(ConfigFile *cf, int section) {
	return new ICPDriver(cf, section);
}

void driver_Register(DriverTable* table) {
	table->AddDriver("cudaslamicp", driver_Init);
}

extern "C" {
	int player_driver_init(DriverTable *table) {
		driver_Register(table);
		return 0;
	}
}


