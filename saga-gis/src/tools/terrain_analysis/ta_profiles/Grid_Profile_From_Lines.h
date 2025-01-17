/**********************************************************
 * Version $Id: Grid_Profile_From_Lines.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     ta_profiles                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               Grid_Profile_From_Lines.h               //
//                                                       //
//                 Copyright (C) 2006 by                 //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Grid_Profile_From_Lines_H
#define HEADER_INCLUDED__Grid_Profile_From_Lines_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrid_Profile_From_Lines : public CSG_Tool_Grid
{
public:
	CGrid_Profile_From_Lines(void);
	virtual ~CGrid_Profile_From_Lines(void);


protected:

	virtual bool				On_Execute(void);


private:

	CSG_Shapes					*m_pProfile, *m_pLines;

	CSG_Grid					*m_pDEM;

	CSG_Parameter_Grid_List		*m_pValues;


	bool						Init_Profile	(CSG_Shapes *pPoints, const SG_Char *Name);

	bool						Set_Profile		(int Line_ID, CSG_Shape *pLine);
	bool						Set_Profile		(int Line_ID, bool bStart, const TSG_Point &A, const TSG_Point &B, bool bLastPoint);
	bool						Add_Point		(int Line_ID, bool bStart, const TSG_Point &Point);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Grid_Profile_From_Lines_H
