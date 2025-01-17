/**********************************************************
 * Version $Id: SlopeLength.cpp 1081 2011-06-08 08:05:26Z reklov_w $
 *********************************************************/
/*******************************************************************************
    SlopeLength.cpp
    Copyright (C) Victor Olaya

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "SlopeLength.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSlopeLength::CSlopeLength(void)
{
	Set_Name		(_TL("Slope Length"));

	Set_Author		(SG_T("V.Olaya (c) 2004"));

	Set_Description	(_TW(
		"The tool allows one to calculate a special version of slope (or flowpath) length. "
        "In a first processing step, the slope of each cell is calculated with the method of "
        "Zevenbergen & Thorne (1986). The slope length values are then accumulated downslope "
        "using the Deterministic 8 (D8) single flow direction algorithm and a threshold "
        "criterion: the slope length is only accumulated downslope if the "
        "slope of the receiving cell is steeper than half of the slope of the contributing "
        "cell (i.e. the accumulation stops if the slope profile gets abruptly flatter).\n"
        "If several cells are contributing to a cell, the maximum of the "
        "accumulated slope length values found in these cells is passed to the receiving "
        "cell and then routed further downslope.\n"
	));

	Parameters.Add_Grid(
		NULL	, "DEM"		, _TL("Elevation"),
		_TL("The digital elevation model."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "LENGTH"	, _TL("Slope Length"),
		_TL("The maximum slope (flowpath) length."),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSlopeLength::On_Execute(void)
{
	int		x, y;

	//-----------------------------------------------------
	m_pDEM		= Parameters("DEM"   )->asGrid();
	m_pLength	= Parameters("LENGTH")->asGrid();

	if( !m_pDEM->Set_Index() )
	{
		Error_Set(_TL("index creation failed"));

		return( false );
	}

	//-----------------------------------------------------
	m_Slope.Create(Get_System());

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for private(x)
		for(x=0; x<Get_NX(); x++)
		{
			double	Slope, Aspect;

			if( m_pDEM->Get_Gradient(x, y, Slope, Aspect) )
			{
				m_Slope   .Set_Value(x, y, Slope);
				m_pLength->Set_Value(x, y, 0.0);
			}
			else
			{
				m_Slope   .Set_NoData(x, y);
				m_pLength->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	for(sLong n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		if( m_pDEM->Get_Sorted(n, x, y) )
		{
			Get_Length(x, y);
		}
	}

	//-----------------------------------------------------
	m_Slope.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSlopeLength::Get_Length(int x, int y)
{
	int		i, ix, iy;

	if( m_Slope.is_InGrid(x, y) && (i = m_pDEM->Get_Gradient_NeighborDir(x, y)) >= 0 )
	{
		ix	= Get_xTo(i, x);
		iy	= Get_yTo(i, y);

		if( m_Slope.is_InGrid(ix, iy) )
		{
			if( m_Slope.asDouble(ix, iy) > 0.5 * m_Slope.asDouble(x, y) )	// ???
			{
				double	Length	= m_pLength->asDouble(x, y) + CSG_Tool_Grid::Get_Length(i);
			
				if( Length > m_pLength->asDouble(ix, iy) )
				{
					m_pLength->Set_Value(ix, iy, Length);
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
