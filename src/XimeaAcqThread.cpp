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
	  m_quit(false)
{
	pthread_attr_setscope(&m_thread_attr, PTHREAD_SCOPE_PROCESS);
}

AcqThread::~AcqThread()
{
	this->m_quit = true;
	join();
}

void AcqThread::threadFunction()
{
	DEB_MEMBER_FUNCT();

	while(!this->m_quit)
	{
		printf("\n\n----------- AcqThread: loop\n\n");
		XI_IMG image = this->m_cam._read_image(5000);
		printf("\n\n----------- AcqThread: read image\n\n");
		VideoMode mode;
		switch(image.frm)
		{
			case XI_MONO8:
				mode = Y8;
				break;
			case XI_MONO16:
				mode = Y16;
				break;
			case XI_RGB24:
				mode = RGB24;
				break;
			case XI_RGB32:
				mode = RGB32;
				break;

			case XI_RGB_PLANAR:
			case XI_RAW8:
			case XI_RAW16:
			case XI_FRM_TRANSPORT_DATA:
			case XI_RGB64:
			case XI_RGB48:
			case XI_RGB16_PLANAR:
			case XI_RAW8X2:
			case XI_RAW8X4:
			case XI_RAW16X2:
			case XI_RAW16X4:
			case XI_RAW32:
			case XI_RAW32FLOAT:
				DEB_ERROR() << "Image type not known to Lima";
				return;

			default:
				THROW_HW_ERROR(Error) << "Invalid image type";
				return;
		}
		printf("\n\n----------- AcqThread: video mode\n\n");
		this->m_cam.m_video->callNewImage(
			(char*)image.bp,
			image.width,
			image.height,
			mode
		);
		printf("\n\n----------- AcqThread: new frame\n\n");
		++this->m_cam.m_image_number;
		printf("\n\n----------- AcqThread: counter\n\n");
	}
}
