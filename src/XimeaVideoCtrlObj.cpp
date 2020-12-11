//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
#include "XimeaVideoCtrlObj.h"
#include "XimeaCamera.h"
#include "XimeaInterface.h"

using namespace lima;
using namespace lima::Ximea;

VideoCtrlObj::VideoCtrlObj(Camera& cam) : m_cam(cam)
{
}

VideoCtrlObj::~VideoCtrlObj()
{
}

void VideoCtrlObj::getSupportedVideoMode(std::list<VideoMode>& aList) const
{
	DEB_MEMBER_FUNCT();
}

void VideoCtrlObj::getVideoMode(VideoMode &mode) const
{
	DEB_MEMBER_FUNCT();
}

void VideoCtrlObj::setVideoMode(VideoMode mode)
{
	DEB_MEMBER_FUNCT();
}

void VideoCtrlObj::setLive(bool flag)
{
}

void VideoCtrlObj::getLive(bool &flag) const
{
}

void VideoCtrlObj::getGain(double &aGain) const
{
}

void VideoCtrlObj::setGain(double aGain)
{
}

void VideoCtrlObj::checkBin(Bin &bin)
{
}

void VideoCtrlObj::checkRoi(const Roi&, Roi& hw_roi)
{
}

bool VideoCtrlObj::checkAutoGainMode(AutoGainMode mode) const
{
	return false;
}

void VideoCtrlObj::setHwAutoGainMode(AutoGainMode mode)
{
}
