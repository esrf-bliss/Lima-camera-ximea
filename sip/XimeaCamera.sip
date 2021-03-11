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

namespace Ximea
{
	class Camera
	{
%TypeHeaderCode
#include <XimeaCamera.h>
%End

	public:
		enum Status {
			Ready, Exposure, Readout, Latency, Fault
		};

		enum TriggerPolarity {
			TriggerPolarity_Low_Falling, TriggerPolarity_High_Rising
		};

		enum GPISelector {
			GPISelector_Port_1 = XI_GPI_PORT1,
			GPISelector_Port_2 = XI_GPI_PORT2,
			GPISelector_Port_3 = XI_GPI_PORT3,
			GPISelector_Port_4 = XI_GPI_PORT4,
			GPISelector_Port_5 = XI_GPI_PORT5,
			GPISelector_Port_6 = XI_GPI_PORT6,
			GPISelector_Port_7 = XI_GPI_PORT7,
			GPISelector_Port_8 = XI_GPI_PORT8,
			GPISelector_Port_9 = XI_GPI_PORT9,
			GPISelector_Port_10 = XI_GPI_PORT10,
			GPISelector_Port_11 = XI_GPI_PORT11,
			GPISelector_Port_12 = XI_GPI_PORT12
		};

		enum GPIMode {
			GPIMode_Off = XI_GPI_OFF,
			GPIMode_Trigger = XI_GPI_TRIGGER,
			GPIMode_Ext_Event = XI_GPI_EXT_EVENT
		};

		enum GPOSelector {
			GPOSelector_Port_1 = XI_GPO_PORT1,
			GPOSelector_Port_2 = XI_GPO_PORT2,
			GPOSelector_Port_3 = XI_GPO_PORT3,
			GPOSelector_Port_4 = XI_GPO_PORT4,
			GPOSelector_Port_5 = XI_GPO_PORT5,
			GPOSelector_Port_6 = XI_GPO_PORT6,
			GPOSelector_Port_7 = XI_GPO_PORT7,
			GPOSelector_Port_8 = XI_GPO_PORT8,
			GPOSelector_Port_9 = XI_GPO_PORT9,
			GPOSelector_Port_10 = XI_GPO_PORT10,
			GPOSelector_Port_11 = XI_GPO_PORT11,
			GPOSelector_Port_12 = XI_GPO_PORT12
		};

		enum GPOMode {
			GPOMode_Off = XI_GPO_OFF,
			GPOMode_On = XI_GPO_ON,
			GPOMode_Frame_Active = XI_GPO_FRAME_ACTIVE,
			GPOMode_Neg_Frame_Active = XI_GPO_FRAME_ACTIVE_NEG,
			GPOMode_Exposure_Active = XI_GPO_EXPOSURE_ACTIVE,
			GPOMode_Neg_Exposure_Active = XI_GPO_EXPOSURE_ACTIVE_NEG,
			GPOMode_Frame_Trigger_Wait = XI_GPO_FRAME_TRIGGER_WAIT,
			GPOMode_Neg_Frame_Trigger_Wait = XI_GPO_FRAME_TRIGGER_WAIT_NEG,
			GPOMode_Exposure_Pulse = XI_GPO_EXPOSURE_PULSE,
			GPOMode_Neg_Exposure_Pulse = XI_GPO_EXPOSURE_PULSE_NEG,
			GPOMode_Busy = XI_GPO_BUSY,
			GPOMode_Neg_Busy = XI_GPO_BUSY_NEG,
			GPOMode_High_Impedance = XI_GPO_HIGH_IMPEDANCE,
			GPOMode_Frame_Buffer_Overflow = XI_GPO_FRAME_BUFFER_OVERFLOW,
			GPOMode_Exposure_Active_First_Row = XI_GPO_EXPOSURE_ACTIVE_FIRST_ROW,
			GPOMode_Neg_Exposure_Active_First_Row = XI_GPO_EXPOSURE_ACTIVE_FIRST_ROW_NEG,
			GPOMode_Exposure_Active_All_Rows = XI_GPO_EXPOSURE_ACTIVE_ALL_ROWS,
			GPOMode_Neg_Exposure_Active_All_Rows = XI_GPO_EXPOSURE_ACTIVE_ALL_ROWS_NEG
		};

		enum LEDSelector {
			LEDSelector_1 = XI_LED_SEL1,
			LEDSelector_2 = XI_LED_SEL2,
			LEDSelector_3 = XI_LED_SEL3,
			LEDSelector_4 = XI_LED_SEL4,
			LEDSelector_5 = XI_LED_SEL5
		};

		enum LEDMode {
			LEDMode_Heartbeat = XI_LED_HEARTBEAT,
			LEDMode_Trigger_Active = XI_LED_TRIGGER_ACTIVE,
			LEDMode_Ext_Event_Active = XI_LED_EXT_EVENT_ACTIVE,
			LEDMode_Link = XI_LED_LINK,
			LEDMode_Acquisition = XI_LED_ACQUISITION,
			LEDMode_Exposure_Active = XI_LED_EXPOSURE_ACTIVE,
			LEDMode_Frame_Active = XI_LED_FRAME_ACTIVE,
			LEDMode_Off = XI_LED_OFF,
			LEDMode_On = XI_LED_ON,
			LEDMode_Blink = XI_LED_BLINK
		};

		Camera(int camera_id);
		~Camera();

		void prepareAcq();
		void startAcq();
		void stopAcq();

		// DetInfoCtrlObj
		void getImageType(ImageType& type);
		void setImageType(ImageType type);

		void getDetectorType(std::string& type);
		void getDetectorModel(std::string& model);
		void getDetectorImageSize(Size& size);

		// SyncCtrlObj
		void setExpTime(double exp_time);
		void getExpTime(double& exp_time);

		void setNbFrames(int nb_frames);
		void getNbFrames(int& nb_frames);

		void getNbHwAcquiredFrames(int& nb_acq_frames);

		void getStatus(Camera::Status& status);

		// Buffer control object
		HwBufferCtrlObj* getBufferCtrlObj();

		// Trigger polarity
		void getTriggerPolarity(TriggerPolarity& p);
		void setTriggerPolarity(TriggerPolarity p);

		// Software trigger
		void getSoftwareTrigger(bool &t);
		void setSoftwareTrigger(bool t);

		// GPIO setup
		void getGpiSelector(GPISelector& s);
		void setGpiSelector(GPISelector s);
		void getGpiMode(GPIMode& m);
		void setGpiMode(GPIMode m);
		void getGpiLevel(int& s);
		void setGpiLevel(int s);
		void getGpiLevelAtExpStart(int& s);
		void setGpiLevelAtExpStart(int s);
		void getGpiLevelAtExpEnd(int& s);
		void setGpiLevelAtExpEnd(int s);
		void getGpiDebounce(bool& e);
		void setGpiDebounce(bool e);
		void getGpoSelector(GPOSelector& s);
		void setGpoSelector(GPOSelector s);
		void getGpoMode(GPOMode& m);
		void setGpoMode(GPOMode m);
		void getLedSelector(LEDSelector& s);
		void setLedSelector(LEDSelector s);
		void getLedMode(LEDMode& m);
		void setLedMode(LEDMode m);

	private:
		Camera(const Ximea::Camera&);
	};
};
