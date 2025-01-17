
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     shapes_lines                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Lines_From_Points.cpp                 //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
#include "Lines_From_Points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLines_From_Points::CLines_From_Points(void)
{
	Set_Name		(_TL("Convert Points to Line(s)"));

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"Converts points to line(s)."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("", "POINTS", _TL("Points"), _TL(""), PARAMETER_INPUT , SHAPE_TYPE_Point);
	Parameters.Add_Shapes("", "LINES" , _TL("Lines" ), _TL(""), PARAMETER_OUTPUT, SHAPE_TYPE_Line );

	Parameters.Add_Table_Field("POINTS", "ORDER"    , _TL("Order by..."   ), _TL(""), true);
	Parameters.Add_Table_Field("POINTS", "SEPARATE" , _TL("Separate by..."), _TL(""), true);
	Parameters.Add_Table_Field("POINTS", "ELEVATION", _TL("Elevation"     ), _TL(""), true);

	Parameters.Add_Double("",
		"MAXDIST"	, _TL("Maximum Distance"),
		_TL("Maximum distance allowed to connect two consecutive points. Ignored if set to zero (default)."),
		0., 0., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CLines_From_Points::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLines_From_Points::On_Execute(void)
{
	CSG_Shapes *pPoints = Parameters("POINTS")->asShapes();

	if(	pPoints->Get_Count() < 2 )
	{
		Error_Set(_TL("nothing to do, less than 2 input points"));

		return( false );
	}

	int Order    = Parameters("ORDER"    )->asInt();
	int Separate = Parameters("SEPARATE" )->asInt();
	int Z        = Parameters("ELEVATION")->asInt();

	double maxDist = Parameters("MAXDIST")->asDouble();

	//-------------------------------------------------
	CSG_Shapes *pLines = Parameters("LINES")->asShapes();

	pLines->Create(SHAPE_TYPE_Line, pPoints->Get_Name(), NULL, Z >= 0 ? SG_VERTEX_TYPE_XYZ : SG_VERTEX_TYPE_XY);

	pLines->Add_Field("ID", SG_DATATYPE_Int);

	//-------------------------------------------------
	CSG_Index Index; int Fields[2], nFields = 0;

	if( Separate >= 0 )
	{
		pLines->Add_Field(pPoints->Get_Field_Name(Separate), pPoints->Get_Field_Type(Separate));

		Fields[nFields++] = Separate;
	}

	if( Order >= 0 )
	{
		Fields[nFields++] = Order;
	}

	if( nFields > 0 )
	{
		pPoints->Set_Index(Index, Fields, nFields);
	}

	//-------------------------------------------------
	CSG_Shape *pLine = NULL; CSG_String SeparateID;

	for(int iPoint=0; iPoint<pPoints->Get_Count(); iPoint++)
	{
		CSG_Shape *pPoint = pPoints->Get_Shape(Index.is_Okay() ? Index[iPoint] : iPoint);

		if( pLine == NULL || (Separate >= 0 && SeparateID.Cmp(pPoint->asString(Separate)))
		|| (maxDist > 0. && maxDist <= SG_Get_Distance(pPoint->Get_Point(0), pLine->Get_Point(pLine->Get_Point_Count() - 1))) )
		{
			if( pLine && pLine->Get_Point_Count() < 2 )
			{
				pLine->Del_Parts();
			}
			else
			{
				pLine = pLines->Add_Shape();

				pLine->Set_Value(0, pLines->Get_Count());
			}

			if( Separate >= 0 )
			{
				pLine->Set_Value(1, SeparateID = pPoint->asString(Separate));
			}
		}

		pLine->Add_Point(pPoint->Get_Point(0));

		if( Z >= 0 )
		{
			pLine->Set_Z(pPoint->asDouble(Z), pLine->Get_Point_Count() - 1);
		}
	}

	if( pLine && pLine->Get_Point_Count() < 2 )
	{
		pLines->Del_Shape(pLine);
	}

	return( pLines->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
