/*
 * File: LMS200.java is part of info.ziyan.robot.module.
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
package info.ziyan.robot.module;

import info.ziyan.robot.Callback;
import info.ziyan.robot.LocalModule;
import info.ziyan.robot.specification.Laser;
import info.ziyan.robot.specification.LaserConfig;
import info.ziyan.robot.specification.LaserScanData;

/**
 * @author ziyan
 *
 */
public class LMS200 extends LocalModule implements Laser {

	/* (non-Javadoc)
	 * @see info.ziyan.robot.specification.Laser#getConfig()
	 */
	public LaserConfig getConfig() {
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see info.ziyan.robot.specification.Laser#getConfig(info.ziyan.robot.Callback)
	 */
	public void getConfig(Callback<LaserConfig> callback) {
		// TODO Auto-generated method stub
		
	}

	/* (non-Javadoc)
	 * @see info.ziyan.robot.specification.Laser#getScanData()
	 */
	public LaserScanData getScanData() {
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see info.ziyan.robot.specification.Laser#getScanData(info.ziyan.robot.Callback)
	 */
	public void getScanData(Callback<LaserScanData> callback) {
		// TODO Auto-generated method stub
		
	}

	/* (non-Javadoc)
	 * @see info.ziyan.robot.specification.Laser#setConfig(info.ziyan.robot.specification.LaserConfig)
	 */
	public boolean setConfig(LaserConfig config) {
		// TODO Auto-generated method stub
		return false;
	}

	/* (non-Javadoc)
	 * @see info.ziyan.robot.specification.Laser#setConfig(info.ziyan.robot.specification.LaserConfig, info.ziyan.robot.Callback)
	 */
	public void setConfig(LaserConfig config, Callback<Boolean> callback) {
		// TODO Auto-generated method stub
		
	}

}
