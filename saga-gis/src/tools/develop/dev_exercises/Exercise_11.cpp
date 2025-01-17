
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
//                   Exercise_11.cpp                     //
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
#include "Exercise_11.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExercise_11::CExercise_11(void)
{
	Set_Name		(_TL("11: Dynamic Simulation - Soil Nitrogen Dynamics"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Spatially Distributed Simulation of Soil Nitrogen Dynamics."
	));

	Add_Reference("Conrad, O.", "2007",
		"SAGA - Entwurf, Funktionsumfang und Anwendung eines Systems f�r Automatisierte Geowissenschaftliche Analysen",
		"ediss.uni-goettingen.de.", SG_T("http://hdl.handle.net/11858/00-1735-0000-0006-B26C-6"), SG_T("Online")
	);

	Add_Reference("Hugget, R.J.", "1993",
		"Modelling the Human Impact on Nature",
		"Oxford University Press."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "NSTORE"		, _TL("Soil Nitrogen"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Double(
		"", "TIME_SPAN"	, _TL("Time Span [a]"),
		_TL(""),
		100.0, 0.0, true
	);

	Parameters.Add_Double(
		"", "TIME_STEP"	, _TL("Time Interval [a]"),
		_TL(""),
		0.1, 0.0, true
	);

	Parameters.Add_Bool(
		"", "UPDATE"		, _TL("Update View"),
		_TL(""),
		true
	);

	//-----------------------------------------------------
	Parameters.Add_Double(
		"", "NINIT"		, _TL("Initial Nitrogen Content [kg/ha]"),
		_TL(""),
		5000.0, 0.0, true
	);

	Parameters.Add_Double(
		"", "NRAIN"		, _TL("Nitrogen in Rainfall [kg/ha/a]"),
		_TL(""),
		16.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_11::On_Execute(void)
{
	bool		bUpdate;
	int			iStep, nSteps;
	double		sTime, dTime, N_Init, N_Rain;
	CSG_Grid	*pDEM;

	//-----------------------------------------------------
	sTime	= Parameters("TIME_SPAN")->asDouble();
	dTime	= Parameters("TIME_STEP")->asDouble();
	nSteps	= (int)(sTime / dTime);

	bUpdate	= Parameters("UPDATE"   )->asBool();

	N_Init	= Parameters("NINIT"    )->asDouble();
	N_Rain	= Parameters("NRAIN"    )->asDouble();

	pDEM	= Parameters("DEM"      )->asGrid();

	m_pN	= Parameters("NSTORE"   )->asGrid();
	m_pN->Assign(N_Init);
	DataObject_Set_Colors(m_pN, 100, SG_COLORS_YELLOW_GREEN);

	Initialize(pDEM);

	//-----------------------------------------------------
	for(iStep=0; iStep<=nSteps && Set_Progress(iStep, nSteps); iStep++)
	{
		Process_Set_Text("Time [a]: %f (%f)", dTime * iStep, sTime);

		if( bUpdate )
		{
			DataObject_Update(m_pN, m_pN->Get_Min(), m_pN->Get_Max(), true);
		}

		Next_Step(N_Rain, dTime);
	}

	//-----------------------------------------------------
	Finalize();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_11::Initialize(CSG_Grid *pDEM)
{
	int		x, y, i, ix, iy;
	double	z, dz, dzSum;

	//-----------------------------------------------------
	m_Next	.Create(pDEM, SG_DATATYPE_Float);
	m_dzSum	.Create(pDEM, SG_DATATYPE_Float);

	for(i=0; i<8; i++)
	{
		m_dz[i].Create(pDEM, SG_DATATYPE_Float);
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			dzSum	= 0.0;

			if( !pDEM->is_NoData(x, y) )
			{
				z		= pDEM->asDouble(x, y);

				for(i=0; i<8; i++)
				{
					ix		= Get_xTo(i, x);
					iy		= Get_yTo(i, y);

					if( pDEM->is_InGrid(ix, iy) && (dz = (z - pDEM->asDouble(ix, iy)) / Get_Length(i)) > 0.0 )
					{
						m_dz[i].Set_Value(x, y, dz);

						dzSum	+= dz;
					}
					else
					{
						m_dz[i].Set_Value(x, y, 0.0);
					}
				}

				if( dzSum > 0.0 )
				{
					for(i=0; i<8; i++)
					{
						m_dz[i].Mul_Value(x, y, 1.0 / dzSum);
					}
				}
			}

			m_dzSum.Set_Value(x, y, dzSum);
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CExercise_11::Finalize(void)
{
	int		i;

	m_Next	.Destroy();
	m_dzSum	.Destroy();

	for(i=0; i<8; i++)
	{
		m_dz[i].Destroy();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_11::Next_Step(double N_Rain, double dTime)
{
	int		x, y, i;
	double	dz, dzSum, N, dN;

	m_Next.Assign(0.0);

	N_Rain	*= dTime;

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Process_Get_Okay(false); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			N		= m_pN->asDouble(x, y);

			if( (dzSum = m_dzSum.asDouble(x, y)) > 0.0 )
			{
				dN		= N * dzSum * dTime;

				if( dN > N )
				{
					dN		= N;
				}

				for(i=0; i<8; i++)
				{
					if( (dz = m_dz[i].asDouble(x, y)) > 0.0 )
					{
						m_Next.Add_Value(Get_xTo(i, x), Get_yTo(i, y), dz * dN);
					}
				}
			}
			else
			{
				dN		= 0.0;
			}

			m_Next.Add_Value(x, y, N - dN + N_Rain);
		}
	}

	if( is_Progress() )
	{
		m_pN->Assign(&m_Next);
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
