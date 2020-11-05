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
#ifndef MYCAMERAINTERFACE_H
#define MYCAMERAINTERFACE_H

#include <mycamera_export.h>

#include "lima/HwInterface.h"

namespace lima
{
namespace MyCamera
{
  class MYCAMERA_EXPORT Interface : public HwInterface
  {
    DEB_CLASS_NAMESPC(DebModCamera, "MyCameraInterface", "MyCamera");
    
  public:
    Interface(Camera&);
    virtual ~Interface();
    //- From HwInterface
    virtual void	getCapList(CapList&) const;
    virtual void	reset(ResetLevel reset_level);
    virtual void	prepareAcq();
    virtual void	startAcq();
    virtual void	stopAcq();
    virtual void	getStatus(StatusType& status);
    virtual int	        getNbHwAcquiredFrames();

  private:
    Camera&		m_cam;
  };
  
} // namespace MyCamera
} // namespace lima

#endif // MYCAMERAINTERFACE_H
