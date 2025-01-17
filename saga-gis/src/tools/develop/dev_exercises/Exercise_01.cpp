
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
//                   Exercise_01.cpp                     //
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
#include "Exercise_01.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExercise_01::CExercise_01(void)
{
	//-----------------------------------------------------
	// Give some information about your tool...

	Set_Name		(_TL("01: My first tool"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"My first tool."
	));

	Add_Reference("Conrad, O.", "2007",
		"SAGA - Entwurf, Funktionsumfang und Anwendung eines Systems f�r Automatisierte Geowissenschaftliche Analysen",
		"ediss.uni-goettingen.de.", SG_T("http://hdl.handle.net/11858/00-1735-0000-0006-B26C-6"), SG_T("Online")
	);


	//-----------------------------------------------------
	// Define your parameters list...

	Parameters.Add_Grid(
		"", "INPUT"	, _TL("Input"),
		_TL("This must be your input data of type grid."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "OUTPUT"	, _TL("Output"),
		_TL("This will contain your output data of type grid."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double(
		"", "FACTOR"	, _TL("Factor"),
		_TL("Type in a value that shall be used for the chosen mathematical operation."),
		1.
	);

	Parameters.Add_Choice(
		"", "METHOD"	, _TL("Method"),
		_TL("Choose a method"),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Addition"),
			_TL("Subtraction"),
			_TL("Multiplication"),
			_TL("Division")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_01::On_Execute(void)
{
	int		x, y, Method;
	double	Factor, Value_Input, Value_Output;
	CSG_Grid	*pInput, *pOutput;

	//-----------------------------------------------------
	// Get parameter settings...

	pInput		= Parameters("INPUT" )->asGrid();
	pOutput		= Parameters("OUTPUT")->asGrid();
	Factor		= Parameters("FACTOR")->asDouble();
	Method		= Parameters("METHOD")->asInt();


	//-----------------------------------------------------
	// Check for valid parameter settings...

	if( Method == 3 && Factor == 0.0 )
	{
		Message_Add(_TL("Division by zero is not allowed !!!"));
		Message_Dlg(_TL("Division by zero is not allowed !!!"));

		return( false );	// prevent a division by zero!!!
	}


	//-----------------------------------------------------
	// Execute calculation...

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			Value_Input	= pInput->asDouble(x, y);

			switch( Method )
			{
			case 0:	// Addition...
				Value_Output	= Value_Input + Factor;
				break;

			case 1:	// Subtraction...
				Value_Output	= Value_Input - Factor;
				break;

			case 2:	// Multiplication...
				Value_Output	= Value_Input * Factor;
				break;

			case 3:	// Division...
				Value_Output	= Value_Input / Factor;
				break;
			}

			pOutput->Set_Value(x, y, Value_Output);
		}
	}


	//-----------------------------------------------------
	// Return 'true' if everything went okay...

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
