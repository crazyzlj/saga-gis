
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       Lectures                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Exercise_07.cpp                     //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Exercise_07.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExercise_07::CExercise_07(void)
{
	Set_Name		(_TL("07: Extended neighbourhoods - catchment areas (trace flow)"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Extended Neighbourhoods - Catchment areas."
	));

	Add_Reference("Conrad, O.", "2007",
		"SAGA - Entwurf, Funktionsumfang und Anwendung eines Systems f�r Automatisierte Geowissenschaftliche Analysen",
		"ediss.uni-goettingen.de.", SG_T("http://hdl.handle.net/11858/00-1735-0000-0006-B26C-6"), SG_T("Online")
	);


	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "ELEVATION"	, _TL("Elevation grid"),
		_TL("This must be your input data of type grid."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "AREA"		, _TL("Catchment area"),
		_TL("This will contain your output data of type grid."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		"", "LENGTH"		, _TL("Flow Path Length"),
		_TL("This will contain your output data of type grid."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Int(
		"", "STEP"		, _TL("Step"),
		_TL("Skip cells (step value)."),
		1, 1, true
	);

	Parameters.Add_Choice(
		"", "METHOD"	, _TL("Method"),
		_TL("Choose a method"),
		CSG_String::Format("%s|%s",
			_TL("Needs less memory, but is slow"),
			_TL("Needs more memory, but is quicker")
		)
	);

	Parameters.Add_Bool(
		"", "ONECELL"	, _TL("Single Cell"),
		_TL("Set \'false\' to execute the calculation for each point in grid. Set \'true\' tu calculate the flow of one single cell."),
		false
	);

	Parameters.Add_Int(
		"ONECELL", "XCELL"	, _TL("X Position"),
		_TL("")
	);

	Parameters.Add_Int(
		"ONECELL", "YCELL"	, _TL("Y Position"),
		_TL("")
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_07::On_Execute(void)
{
	bool		bCell;
	int			x, y, Step, xCell, yCell;
	CSG_Grid	*pLength;

	//-----------------------------------------------------
	// Get parameter settings...

	m_pDTM			= Parameters("ELEVATION")->asGrid();
	m_pArea			= Parameters("AREA"     )->asGrid();
	pLength			= Parameters("LENGTH"   )->asGrid();

	Step			= Parameters("STEP"     )->asInt();

	m_Method		= Parameters("METHOD"   )->asInt();

	bCell			= Parameters("ONECELL"  )->asBool();
	xCell			= Parameters("XCELL"    )->asInt();
	yCell			= Parameters("YCELL"    )->asInt();


	//-----------------------------------------------------
	// General initialisations...

	m_pArea			->Assign(0.0);
	m_pArea			->Set_Unit(SG_T("m\xc2\xb2"));
	DataObject_Set_Colors(m_pArea, 100, SG_COLORS_WHITE_BLUE);

	pLength			->Assign(0.0);
	pLength			->Set_Unit(SG_T("m"));

	m_Area_of_Cell	= Get_Cellsize() * Get_Cellsize();


	//-----------------------------------------------------
	// Special initialisations...

	switch( m_Method )
	{
	case 0: default:	// Needs less memory, but is slow...
		m_pDir	= NULL;
		break;

	case 1:				// Needs more memory, but is quicker...
		m_pDir	= new CSG_Grid(m_pDTM, SG_DATATYPE_Char);

		for(y=0; y<Get_NY() && Set_Progress(y); y+=Step)
		{
			for(x=0; x<Get_NX(); x+=Step)
			{
				m_pDir->Set_Value(x, y, Get_FlowDirection(x, y));
			}
		}
		break;
	}

	//-----------------------------------------------------
	// Execute calculation...

	if( bCell )
	{
		pLength->Set_Value(xCell, yCell, Trace_Flow(xCell, yCell));
	}
	else
	{
		for(y=0; y<Get_NY() && Set_Progress(y); y+=Step)
		{
			for(x=0; x<Get_NX(); x+=Step)
			{
				pLength->Set_Value(x, y, Trace_Flow(x, y));
			}
		}
	}


	//-----------------------------------------------------
	// Special finalisations...

	if( m_pDir != NULL )
	{
		delete(m_pDir);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CExercise_07::Get_FlowDirection(int x, int y)
{
	int		i, ix, iy, iMax;
	double	z, dz, dzMax;

	iMax	= -1;

	if( is_InGrid(x, y) && !m_pDTM->is_NoData(x, y) )
	{
		z		= m_pDTM->asDouble(x, y);
		dzMax	= 0.0;

		for(i=0; i<8; i++)
		{
			ix		= Get_xTo(i, x);
			iy		= Get_yTo(i, y);

			if( is_InGrid(ix, iy) && !m_pDTM->is_NoData(ix, iy) )
			{
				dz		= (z - m_pDTM->asDouble(ix, iy)) / Get_Length(i);

				if( dz > dzMax )
				{
					dzMax	= dz;
					iMax	= i;
				}
			}
		}
	}

	return( iMax );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CExercise_07::Trace_Flow(int x, int y)
{
	int		Direction;
	double	Length;

	Length	= 0.0;

	if( is_InGrid(x, y) && !m_pDTM->is_NoData(x, y) )
	{
		do
		{
			m_pArea->Add_Value(x, y, m_Area_of_Cell);

			Direction	= m_pDir != NULL
						? m_pDir->asInt(x, y)
						: Get_FlowDirection(x, y);

			if( Direction >= 0 )
			{
				x		= Get_xTo(Direction, x);
				y		= Get_yTo(Direction, y);

				Length	+= Get_Length(Direction);
			}
		}
		while( Direction >= 0 );
	}

	return( Length );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
