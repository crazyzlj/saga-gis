
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    Lectures_Shapes                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Exercise_14.cpp                     //
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
#include "Exercise_14.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define NOCHANNEL	0
#define SPRING		1
#define CHANNEL		2
#define MOUTH		3


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExercise_14::CExercise_14(void)
{
	Set_Name		(_TL("14: Vectorising channel lines"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Vectorising channel lines."
	));

	Add_Reference("Conrad, O.", "2007",
		"SAGA - Entwurf, Funktionsumfang und Anwendung eines Systems f�r Automatisierte Geowissenschaftliche Analysen",
		"ediss.uni-goettingen.de.", SG_T("http://hdl.handle.net/11858/00-1735-0000-0006-B26C-6"), SG_T("Online")
	);


	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"ELEVATION"	, _TL("Elevation grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"CHANNELS"	, _TL("Channels (Raster)"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes("",
		"SHAPES"	, _TL("Channels (Vector)"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double("",
		"THRESHOLD"	, _TL("Threshold"),
		_TL(""),
		4, 0., true, 8., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_14::On_Execute(void)
{
	//-----------------------------------------------------
	m_pDTM    = Parameters("ELEVATION")->asGrid();
	m_pChnl   = Parameters("CHANNELS" )->asGrid();
	m_pShapes = Parameters("SHAPES"   )->asShapes();

	//-----------------------------------------------------
	m_pDir    = NULL;

	if( Initialise(Parameters("THRESHOLD")->asInt()) )
	{
		Find_Channels();

		Vectorise();
	}

	//-----------------------------------------------------
	if( m_pDir )
	{
		delete(m_pDir);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_14::Initialise(int Threshold)
{
	int		x, y, i, ix, iy, Dir;
	double	z, dz, dzMax;
	CSG_Colors	Colors;

	//-----------------------------------------------------
	m_pDir	= new CSG_Grid(m_pDTM, SG_DATATYPE_Char);

	m_pChnl->Assign();

	Colors.Set_Count(4);
	Colors.Set_Color(0, 192, 192, 192);	// NOCHANNEL
	Colors.Set_Color(1,   0, 255,   0);	// SPRING
	Colors.Set_Color(2,   0,   0, 255);	// CHANNEL
	Colors.Set_Color(3, 255,   0,   0);	// MOUTH
	DataObject_Set_Colors(m_pChnl, Colors);

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			Dir		= -1;

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
							Dir		= i;
						}
					}
				}
			}

			//---------------------------------------------
			m_pDir->Set_Value(x, y, Dir);

			if( Dir >= 0 )
			{
				m_pChnl->Add_Value(Get_xTo(Dir, x), Get_yTo(Dir, y), 1);
			}
		}
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			m_pChnl->Set_Value(x, y, m_pChnl->asInt(x, y) >= Threshold ? SPRING : NOCHANNEL);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CExercise_14::Find_Channels(void)
{
	int		x, y;

	for(sLong n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		if( m_pDTM->Get_Sorted(n, x, y, true) && m_pChnl->asInt(x, y) == SPRING )
		{
			m_pChnl	->Set_Value(x, y, SPRING);

			Find_Channels(x, y);
		}
	}
}

//---------------------------------------------------------
void CExercise_14::Find_Channels(int x, int y)
{
	int		Dir, ix, iy;

	if( (Dir = m_pDir->asInt(x, y)) >= 0 )
	{
		ix	= Get_xTo(Dir, x);
		iy	= Get_yTo(Dir, y);

		switch( m_pChnl->asInt(ix, iy) )
		{
		case NOCHANNEL: case SPRING:
			m_pChnl	->Set_Value(ix, iy, CHANNEL);
			Find_Channels(ix, iy);	// recursive function call...
			break;

		case CHANNEL:
			m_pChnl	->Set_Value(ix, iy, MOUTH);
			break;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CExercise_14::Vectorise(void)
{
	int			x, y, Segment_ID;
	double		Length;
	CSG_Shape	*pSegment;

	m_pShapes->Create(SHAPE_TYPE_Line, _TL("Channels"));

	m_pShapes->Add_Field("SEGMENT_ID"	, SG_DATATYPE_Int);
	m_pShapes->Add_Field("LENGTH"		, SG_DATATYPE_Double);

	//-----------------------------------------------------
	for(y=0, Segment_ID=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			switch( m_pChnl->asInt(x, y) )
			{
			case SPRING: case MOUTH:
				pSegment	= m_pShapes->Add_Shape();

				Length		= Vectorise(x, y, pSegment);

				if( Length > 0.0 )
				{
					pSegment->Set_Value(0, ++Segment_ID);
					pSegment->Set_Value(1, Length);
				}
				else
				{
					m_pShapes->Del_Shape(pSegment);
				}

				break;
			}
		}
	}
}

//---------------------------------------------------------
double CExercise_14::Vectorise(int x, int y, CSG_Shape *pSegment)
{
	int		Dir, ix, iy;
	double	Length;

	Length	= 0.0;

	pSegment->Add_Point(Get_XMin() + x * Get_Cellsize(), Get_YMin() + y * Get_Cellsize());

	if( (Dir = m_pDir->asInt(x, y)) >= 0 )
	{
		Length	= Get_Length(Dir);

		ix		= Get_xTo(Dir, x);
		iy		= Get_yTo(Dir, y);

		switch( m_pChnl->asInt(ix, iy) )
		{
		case CHANNEL:
			Length	+= Vectorise(ix, iy, pSegment);	// recursive function call...
			break;

		case MOUTH:
			Length	+= Get_Length(Dir);
			pSegment->Add_Point(Get_XMin() + ix * Get_Cellsize(), Get_YMin() + iy * Get_Cellsize());
			break;
		}
	}

	return( Length );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
