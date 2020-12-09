//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2020
// European Synchrotron Radiation Facility
// CS40220 38043 Grenoble Cedex 9 
// FRANCE
//
// Contact: lima@esrf.fr
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

#include "XimeaInterface.h"
#include "XimeaCamera.h"


using namespace lima;
using namespace lima::Ximea;


Interface::Interface(Camera& cam) : m_cam(cam)
{
  DEB_CONSTRUCTOR();
}

Interface::~Interface()
{
  DEB_DESTRUCTOR();
}

void Interface::getCapList(CapList &cap_list) const
{
}

void Interface::reset(ResetLevel reset_level)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(reset_level);
}

void Interface::prepareAcq()
{
  DEB_MEMBER_FUNCT();
}

void Interface::startAcq()
{
  DEB_MEMBER_FUNCT();
}

void Interface::stopAcq()
{
  DEB_MEMBER_FUNCT();
}

void Interface::getStatus(StatusType& status)
{
  DEB_MEMBER_FUNCT();
}

int Interface::getNbHwAcquiredFrames()
{
  DEB_MEMBER_FUNCT();
  return 0;
}
