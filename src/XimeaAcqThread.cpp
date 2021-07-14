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

AcqThread::AcqThread(Camera& cam, int timeout)
	: m_cam(cam),
	  m_quit(false),
	  m_timeout(timeout),
	  m_thread_started(false)
{
	pthread_attr_setscope(&m_thread_attr, PTHREAD_SCOPE_PROCESS);
	memset((void*)&this->m_buffer, 0, sizeof(XI_IMG));
}

AcqThread::~AcqThread()
{
	this->m_quit = true;
}

void AcqThread::threadFunction()
{
	DEB_MEMBER_FUNCT();
	this->m_thread_started = true;

	StdBufferCbMgr& buffer_mgr = this->m_cam.m_buffer_ctrl_obj.getBuffer();

	bool continueAcq = true;
	while(!this->m_quit && (this->m_cam.m_nb_frames == 0 || this->m_cam.m_image_number < this->m_cam.m_nb_frames))
	{
		// set up acq buffers
		this->m_buffer.bp = buffer_mgr.getFrameBufferPtr(this->m_cam.m_image_number);
		this->m_buffer.bp_size = this->m_cam.m_buffer_size;

		if(this->m_cam.m_trigger_mode == IntTrigMult)
		{
			// for software trigger, wait for trigger before setting camera
			// mode to Exposure, otherwise startAcq will fail on CtControl level
			bool do_break = false;
			while(!this->m_cam._soft_trigger_issued())
				if(this->m_quit)
				{
					do_break = true;
					break;
				}
			if(do_break || this->m_quit)
				break;
		}
		
		this->m_cam._set_status(Camera::Exposure);
		this->m_cam._read_image(&this->m_buffer, this->m_timeout);
		
		this->m_cam._set_status(Camera::Readout);
		HwFrameInfoType frame_info;
		frame_info.acq_frame_nb = this->m_cam.m_image_number;
		continueAcq = buffer_mgr.newFrameReady(frame_info);
		DEB_TRACE() << DEB_VAR1(continueAcq);
		++this->m_cam.m_image_number;

		if(!continueAcq)
		{
			this->m_cam._set_status(Camera::Fault);
			Exception e = LIMA_CTL_EXC(Error, "Frame not ready");
			this->m_cam.reportException(e, "Ximea/AcqThread/newFrameReady");
			break;
		}
		if(this->m_cam.xi_status != XI_OK)
		{
			this->m_cam._set_status(Camera::Fault);
			Exception e = LIMA_HW_EXC(Error, "Image read failed, status: " + std::to_string(this->m_cam.xi_status));
			this->m_cam.reportException(e, "Ximea/Camera/_read_image");
			continue;
		}
		this->m_cam._set_status(Camera::Ready);
	}
	// when leaving the thread stop acqusition no matter what
	xiStopAcquisition(this->m_cam.xiH);
}
