/*
 * File: MotorController.java is part of info.ziyan.robot.specification.
 * Author: Ziyan Zhou <zhou@ziyan.info> http://ziyan.info
 *
 * This package is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * To obtain a copy of the GNU General Public License,
 * see <http://www.gnu.org/licenses/>.
 *
 */
package info.ziyan.robot.specification;

import info.ziyan.robot.Callback;
import info.ziyan.robot.Module;

/**
 * @author ziyan
 *
 */
public interface Laser extends Module {
	LaserScanData getScanData();
	void getScanData(Callback<LaserScanData> callback);
	
	LaserConfig getConfig();
	void getConfig(Callback<LaserConfig> callback);
	
	boolean setConfig(LaserConfig config);
	void setConfig(LaserConfig config, Callback<Boolean> callback);
}
