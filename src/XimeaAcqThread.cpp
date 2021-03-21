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

#include "XimeaAcqThread.h"

using namespace lima;
using namespace lima::Ximea;

AcqThread::AcqThread(Camera& cam)
	: m_cam(cam),
	  m_quit(false),
	  m_timeout(0)
{
	pthread_attr_setscope(&m_thread_attr, PTHREAD_SCOPE_PROCESS);
	memset((void*)&this->m_buffer, 0, sizeof(XI_IMG));

	TrigMode t_m;
	this->m_cam.getTrigMode(t_m);
	if(t_m == IntTrig || t_m == IntTrigMult)
	{
		// use timeout of 2 * exposure time for internal trigger
		double exp_time = 0;
		this->m_cam.getExpTime(exp_time);
		this->m_timeout = int(2 * exp_time * TIME_HW / 1e3);	// convert to ms
	}
	else
		// use user provided timeout for external trigger
		this->m_timeout = this->m_cam.m_trig_timeout;
}

AcqThread::~AcqThread()
{
	this->m_quit = true;
	join();
}

void AcqThread::threadFunction()
{
	DEB_MEMBER_FUNCT();

	StdBufferCbMgr& buffer_mgr = this->m_cam.m_buffer_ctrl_obj.getBuffer();

	bool continueAcq = true;
	while(!this->m_quit && (this->m_cam.m_nb_frames == 0 || this->m_cam.m_image_number < this->m_cam.m_nb_frames))
	{
		// set up acq buffers
		this->m_buffer.bp = buffer_mgr.getFrameBufferPtr(this->m_cam.m_image_number);
		this->m_buffer.bp_size = this->m_cam.m_buffer_size;

		this->m_cam._set_status(Camera::Exposure);
		this->m_cam._read_image(&this->m_buffer, this->m_timeout);
		
		this->m_cam._set_status(Camera::Readout);
		HwFrameInfoType frame_info;
		frame_info.acq_frame_nb = this->m_cam.m_image_number;
		continueAcq = buffer_mgr.newFrameReady(frame_info);
		DEB_TRACE() << DEB_VAR1(continueAcq);
		++this->m_cam.m_image_number;

		if(continueAcq && this->m_cam.xi_status == XI_OK)
			this->m_cam._set_status(Camera::Ready);
		else
			this->m_cam._set_status(Camera::Fault);

	}
}
