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
 
#ifndef Greenpak4LUTPgen_h
#define Greenpak4LUTPgen_h

/**
	@brief A single LUT with pattern-generator mode
 */ 
class Greenpak4LUTPgen : public Greenpak4LUT
{
public:

	//Construction / destruction
	Greenpak4LUTPgen(
		Greenpak4Device* device,
		unsigned int lutnum,
		unsigned int matrix,
		unsigned int ibase,
		unsigned int oword,
		unsigned int cbase,
		unsigned int order);
	virtual ~Greenpak4LUTPgen();
	
	//Serialization
	virtual bool Load(bool* bitstream);
	virtual bool Save(bool* bitstream);
	
protected:
};

#endif
