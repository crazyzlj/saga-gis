
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
//                   Exercise_03.cpp                     //
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
#include "Exercise_03.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExercise_03::CExercise_03(void)
{
	Set_Name		(_TL("03: Direct neighbours"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Simple neighbourhood analysis for grid cells."
	));

	Add_Reference("Conrad, O.", "2007",
		"SAGA - Entwurf, Funktionsumfang und Anwendung eines Systems f�r Automatisierte Geowissenschaftliche Analysen",
		"ediss.uni-goettingen.de.", SG_T("http://hdl.handle.net/11858/00-1735-0000-0006-B26C-6"), SG_T("Online")
	);


	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "INPUT"	, _TL("Input grid"),
		_TL("This must be your input data of type grid."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "OUTPUT"	, _TL("Output"),
		_TL("This will contain your output data of type grid."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		"", "METHOD"	, _TL("Method"),
		_TL("Choose a method"),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Difference to left neighbour"),
			_TL("Difference to left neighbour (using a while loop)"),
			_TL("Slope gradient to left neighbour [%%]"),
			_TL("Slope gradient to left neighbour [Degree]")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_03::On_Execute(void)
{
	bool	bResult;
	int		Method;

	//-----------------------------------------------------
	// Get parameter settings...

	m_pInput	= Parameters("INPUT" )->asGrid();
	m_pOutput	= Parameters("OUTPUT")->asGrid();

	Method		= Parameters("METHOD")->asInt();


	//-----------------------------------------------------
	// Execute calculation...

	switch( Method )
	{
	case 0:
		bResult	= Method_01();
		break;

	case 1:
		bResult	= Method_02();
		break;

	case 2:
		bResult	= Method_03();
		break;

	case 3:
		bResult	= Method_04();
		break;

	default:
		bResult	= false;
	}


	//-----------------------------------------------------
	// Return 'true' if everything went okay...

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_03::Method_01(void)
{
	int		x, y, ix;
	double	a, b, c;

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX()-1; x++)
		{
			ix	= x + 1;

			if( m_pInput->is_NoData(x, y) || m_pInput->is_NoData(ix, y) )	// don't work with 'no data'...
			{
				m_pOutput->Set_NoData(x, y);
			}
			else
			{
				a	= m_pInput->asDouble( x, y);
				b	= m_pInput->asDouble(ix, y);

				c	= a - b;

				m_pOutput->Set_Value(x, y, c);
			}
		}

		m_pOutput->Set_NoData(Get_NX() - 1, y);	// what shall we do with the last cell in a row ??!!
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_03::Method_02(void)
{
	int		x, y, ix;
	double	a, b, c;

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		x	= 0;	// initialize x...
		
		while( x < Get_NX() - 1 )	// condition for x...
		{
			ix	= x + 1;

			if( m_pInput->is_NoData(x, y) || m_pInput->is_NoData(ix, y) )	// don't work with 'no data'...
			{
				m_pOutput->Set_NoData(x, y);
			}
			else
			{
				a	= m_pInput->asDouble( x, y);
				b	= m_pInput->asDouble(ix, y);

				c	= a - b;

				m_pOutput->Set_Value(x, y, c);
			}

			x++;	// increment x...
		}

		m_pOutput->Set_NoData(Get_NX() - 1, y);	// what shall we do with the last cell in a row ??!!
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_03::Method_03(void)
{
	int		x, y, ix;
	double	a, b, c;

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0, ix=1; x<Get_NX()-1; x++, ix++)
		{
			if( m_pInput->is_NoData(x, y) || m_pInput->is_NoData(ix, y) )	// don't work with 'no data'...
			{
				m_pOutput->Set_NoData(x, y);
			}
			else
			{
				a	= m_pInput->asDouble( x, y);
				b	= m_pInput->asDouble(ix, y);

				c	= 100.0 * (a - b) / Get_Cellsize();

				m_pOutput->Set_Value(x, y, c);
			}
		}

		m_pOutput->Set_NoData(x, y);	// what shall we do with the last cell in a row ??!!
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_03::Method_04(void)
{
	int		x, y, ix;
	double	a, b, c;

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0, ix=1; x<Get_NX()-1; x++, ix++)
		{
			if( m_pInput->is_NoData(x, y) || m_pInput->is_NoData(ix, y) )	// don't work with 'no data'...
			{
				m_pOutput->Set_NoData(x, y);
			}
			else
			{
				a	= m_pInput->asDouble( x, y);
				b	= m_pInput->asDouble(ix, y);

				c	= atan((a - b) / Get_Cellsize()) * 180.0 / M_PI;

				m_pOutput->Set_Value(x, y, c);
			}
		}

		m_pOutput->Set_NoData(x, y);	// what shall we do with the last cell in a row ??!!
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
