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

	// use timeout of 2 * exposure time
	double exp_time = 0;
	this->m_cam.getExpTime(exp_time);
	this->m_timeout = int(2 * exp_time * TIME_HW);
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
	while(!this->m_quit && this->m_cam.m_image_number < this->m_cam.m_nb_frames)
	{
		printf("\n\n----------- AcqThread: loop\n\n");

		this->m_cam._read_image(&this->m_buffer, this->m_timeout);

		printf("\n\n----------- AcqThread: read image\n\n");
		
		int nb_buffers;
		buffer_mgr.getNbBuffers(nb_buffers);
		printf("\n\n----------- AcqThread: %d buffers\n\n", nb_buffers);
					
		HwFrameInfoType frame_info;
		frame_info.acq_frame_nb = this->m_cam.m_image_number;
		// copy TmpBuffer frame to SoftBuffer frame room
		void *ptr = buffer_mgr.getFrameBufferPtr(this->m_cam.m_image_number);
		memcpy(ptr, (void*)this->m_buffer.bp, this->m_cam.m_buffer_size);

		unsigned char cam_pixel = *(unsigned char*)this->m_buffer.bp;
		unsigned char buf_pixel = *(unsigned char*)ptr;

		printf("\n\n ==================== \n");
		printf("      IMAGE INFO\n");
		printf("       = Lima =\n");
		printf("buffer size: %d\n", this->m_cam.m_buffer_size);
		printf("image number: %d\n", this->m_cam.m_image_number);
		printf("frame buffer ptr: %p\n", ptr);
		printf("first pixel: %d\n", buf_pixel);
		printf("        = XI =\n");
		printf("image size type: %d\n", this->m_buffer.size);
		printf("buffer ptr: %p\n", this->m_buffer.bp);
		printf("filled size: %d\n", this->m_buffer.bp_size);
		printf("format: %d\n", this->m_buffer.frm);
		printf("frame size: %dx%d\n", this->m_buffer.width, this->m_buffer.height);
		printf("frame number: %d\n", this->m_buffer.nframe);
		printf("acq frame number: %d\n", this->m_buffer.transport_frm);
		printf("exposure time: %d us\n", this->m_buffer.exposure_time_us);
		printf("gain: %d dB\n", this->m_buffer.gain_db);
		printf("downsampling: %dx%d\n", this->m_buffer.fDownsamplingX, this->m_buffer.fDownsamplingY);
		printf("first pixel: %d\n", cam_pixel);
		printf(" ==================== \n\n");

		continueAcq = buffer_mgr.newFrameReady(frame_info);
		printf("\n\n----------- AcqThread: new frame, continue: %d\n\n", continueAcq);
		DEB_TRACE() << DEB_VAR1(continueAcq);
		++this->m_cam.m_image_number;
		printf("\n\n----------- AcqThread: counter\n\n");
	}
	printf("\n\n----------- AcqThread: stop\n\n");
}

void AcqThread::_destroy_buffer()
{
	if(this->m_buffer)
	{
		free(this->m_buffer);
		this->m_buffer = NULL;
		printf("\n\n----------- AcqThread: buffer destroyed\n\n");
	}
}
