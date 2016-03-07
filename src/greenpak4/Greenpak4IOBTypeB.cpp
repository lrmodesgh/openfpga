/***********************************************************************************************************************
 * Copyright (C) 2016 Andrew Zonenberg and contributors                                                                *
 *                                                                                                                     *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General   *
 * Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) *
 * any later version.                                                                                                  *
 *                                                                                                                     *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied  *
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for     *
 * more details.                                                                                                       *
 *                                                                                                                     *
 * You should have received a copy of the GNU Lesser General Public License along with this program; if not, you may   *
 * find one here:                                                                                                      *
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.txt                                                              *
 * or you may search the http://www.gnu.org website for the version 2.1 license, or you may write to the Free Software *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA                                      *
 **********************************************************************************************************************/
 
#include "Greenpak4.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction / destruction

Greenpak4IOBTypeB::Greenpak4IOBTypeB(
	Greenpak4Device* device,
	unsigned int matrix,
	unsigned int ibase,
	unsigned int oword,
	unsigned int cbase,
	unsigned int flags)
	: Greenpak4IOB(device, matrix, ibase, oword, cbase, flags)
{
	
}

Greenpak4IOBTypeB::~Greenpak4IOBTypeB()
{
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Bitfile metadata

unsigned int Greenpak4IOBTypeB::GetConfigLen()
{
	if(m_flags & IOB_FLAG_X4DRIVE)
		return 0;
	return 7;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Serialization

bool Greenpak4IOBTypeB::Load(bool* /*bitstream*/)
{
	//TODO
	fprintf(stderr, "Greenpak4IOBTypeB::Load not implemented\n");	
	return false;
}

bool Greenpak4IOBTypeB::Save(bool* bitstream)
{
	//See if we're an input or output.
	//Throw an error if OE isn't tied to a power rail, because we don't have runtime adjustable direction
	Greenpak4PowerRail* oe = dynamic_cast<Greenpak4PowerRail*>(m_outputEnable);
	if(oe == NULL)
	{
		fprintf(stderr, "ERROR: Tried to tie OE of a type-B IOB to something other than a power rail\n");
		return false;
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// INPUT BUS
	
	//Write the output signal (even if we don't actually have an output hooked up, there has to be something)
	if(!WriteMatrixSelector(bitstream, m_inputBaseWord, m_outputSignal))
		return false;
		
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// CONFIGURATION
		
	//MODE CONTROL 2:0. 2 is direction, 1:0 is type
	
	//OUTPUT
	if(oe->GetDigitalValue())
	{
		//always high for outputs
		bitstream[m_configBase + 2] = true;
		
		switch(m_driveType)
		{
			case DRIVE_PUSHPULL:
				bitstream[m_configBase+1] = false;
				bitstream[m_configBase+0] = false;
				break;
				
			case DRIVE_NMOS_OPENDRAIN:
				bitstream[m_configBase+1] = false;
				bitstream[m_configBase+0] = true;
				break;
				
			case DRIVE_PMOS_OPENDRAIN:
				bitstream[m_configBase+1] = true;
				bitstream[m_configBase+0] = false;
				break;
			
			//also may have NMOS open drain capability according to datasheet
			//but not currently tested with this toolchian
			case DRIVE_ANALOG:
				bitstream[m_configBase+1] = true;
				bitstream[m_configBase+0] = true;
				break;
				
			default:
				fprintf(stderr, "ERROR: Invalid IOB drive type\n");
				return false;
		}
	}
	
	//INPUT
	else
	{
		//always low for inputs
		bitstream[m_configBase + 2] = false;

		switch(m_inputThreshold)
		{ 
			case THRESHOLD_ANALOG:
				bitstream[m_configBase+0] = true;
				bitstream[m_configBase+1] = true;
				break;
				
			case THRESHOLD_LOW:
				bitstream[m_configBase+0] = false;
				bitstream[m_configBase+1] = true;
				break;
				
			case THRESHOLD_NORMAL:
				bitstream[m_configBase+1] = false;
				bitstream[m_configBase+0] = m_schmittTrigger;
				break;
				
			default:
				fprintf(stderr, "ERROR: Invalid IOB threshold\n");
				return false;
		}
	}
	
	//Pullup/down resistor strength 4:3, direction 5
	if(m_pullDirection == PULL_NONE)
	{
		bitstream[m_configBase + 4] = false;
		bitstream[m_configBase + 3] = false;
		
		//don't care, pull circuit disconnected
		bitstream[m_configBase + 5] = false;
	}
	else
	{
		switch(m_pullStrength)
		{
			case PULL_10K:
				bitstream[m_configBase + 3] = true;
				bitstream[m_configBase + 4] = false;
				break;
				
			case PULL_100K:
				bitstream[m_configBase + 3] = false;
				bitstream[m_configBase + 4] = true;
				break;
				
			case PULL_1M:
				bitstream[m_configBase + 3] = true;
				bitstream[m_configBase + 4] = true;
				break;
				
			default:
				fprintf(stderr, "ERROR: Invalid pull strength\n");
				return false;
		}
		
		switch(m_pullDirection)
		{
			case PULL_UP:
				bitstream[m_configBase + 5] = true;
				break;
			
			case PULL_DOWN:
				bitstream[m_configBase + 5] = false;
				break;
				
			default:
				fprintf(stderr, "ERROR: Invalid pull direction\n");
				return false;
		}
	}
	
	//Output drive strength 6
	switch(m_driveStrength)
	{
		case DRIVE_1X:
			bitstream[m_configBase + 6] = false;
			if(m_flags & IOB_FLAG_X4DRIVE)
				bitstream[m_configBase + 7] = false;
			break;
			
		case DRIVE_2X:
			bitstream[m_configBase + 6] = true;
			if(m_flags & IOB_FLAG_X4DRIVE)
				bitstream[m_configBase + 7] = false;
			break;
			
		case DRIVE_4X:
			if(m_flags & IOB_FLAG_X4DRIVE)
			{
				bitstream[m_configBase + 6] = true;
				bitstream[m_configBase + 7] = true;
			}
			else
			{
				fprintf(stderr, "ERROR: Asked for x4 drive strength on a pin without a super driver\n");
				return false;
			}
			break;
			
		default:
			fprintf(stderr, "ERROR: Invalid drive strength\n");
			return false;
	}
	
	return true;
}