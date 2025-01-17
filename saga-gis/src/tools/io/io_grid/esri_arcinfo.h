
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                        Grid_IO                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    ESRI_ArcInfo.h                     //
//                                                       //
//                 Copyright (C) 2007 by                 //
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
#ifndef HEADER_INCLUDED__ESRI_ArcInfo_H
#define HEADER_INCLUDED__ESRI_ArcInfo_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CESRI_ArcInfo_Import : public CSG_Tool
{
public:
	CESRI_ArcInfo_Import(void);

	virtual CSG_String		Get_MenuPath		(void)		{	return( _TL("Import") );	}


protected:

	virtual int				On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute			(void);


private:

	double					Read_Value			(CSG_File &Stream);

	CSG_String				Read_Header_Line	(CSG_File &Stream);
	bool					Read_Header_Value	(CSG_File &Stream, const CSG_String &sKey, int    &Value);
	bool					Read_Header_Value	(CSG_File &Stream, const CSG_String &sKey, double &Value);
	CSG_Grid *				Read_Header			(CSG_File &Stream, TSG_Data_Type Datatype = SG_DATATYPE_Float);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CESRI_ArcInfo_Export : public CSG_Tool_Grid
{
public:
	CESRI_ArcInfo_Export(void);

	virtual CSG_String		Get_MenuPath		(void)		{	return( _TL("Export") );	}


protected:

	virtual int				On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute			(void);


private:

	CSG_String				Write_Value			(double Value, int Precision, bool bComma);

	bool					Write_Header		(CSG_File &Stream, CSG_Grid *pGrid, bool bComma);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__ESRI_ArcInfo_H
