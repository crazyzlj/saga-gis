
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       table.h                         //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__SAGA_API__table_H
#define HEADER_INCLUDED__SAGA_API__table_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** \file table.h
* The table data container, a specialized CSG_Table class
* following a geometry object based approach.
* @see CSG_Table_Value
* @see CSG_Table_Record
* @see CSG_Table
* @see CSG_Data_Object
*/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "dataobject.h"
#include "table_value.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Table_FileType
{
	TABLE_FILETYPE_Undefined	= 0,
	TABLE_FILETYPE_Text,
	TABLE_FILETYPE_Text_NoHeadLine,
	TABLE_FILETYPE_DBase
}
TSG_Table_File_Type;

//---------------------------------------------------------
typedef enum ESG_Table_Index_Order
{
	TABLE_INDEX_None			= 0,
	TABLE_INDEX_Ascending,
	TABLE_INDEX_Descending
}
TSG_Table_Index_Order;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_TABLE_REC_FLAG_Modified		0x01
#define SG_TABLE_REC_FLAG_Selected		0x02


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Table_Record
{
	friend class CSG_Table;
	friend class CSG_PointCloud;

public:

	class CSG_Table *			Get_Table		(void)				{	return( m_pTable );	}
	int							Get_Index		(void)	const		{	return( m_Index );	}

	bool						Set_Value		(int              iField, const CSG_Bytes  &Value);
	bool						Set_Value		(const CSG_String &Field, const CSG_Bytes  &Value);
	bool						Set_Value		(int              iField, const CSG_String &Value);
	bool						Set_Value		(const CSG_String &Field, const CSG_String &Value);
	bool						Set_Value		(int              iField, double            Value);
	bool						Set_Value		(const CSG_String &Field, double            Value);
	bool						Add_Value		(int              iField, double            Value);
	bool						Add_Value		(const CSG_String &Field, double            Value);
	bool						Mul_Value		(int              iField, double            Value);
	bool						Mul_Value		(const CSG_String &Field, double            Value);

	bool						Set_NoData		(int              iField);
	bool						Set_NoData		(const CSG_String &Field);
	bool						is_NoData		(int              iField)	const;
	bool						is_NoData		(const CSG_String &Field)	const;

	const SG_Char *				asString		(int              iField, int Decimals = -99)	const;
	const SG_Char *				asString		(const CSG_String &Field, int Decimals = -99)	const;

	SG_Char						asChar			(int              iField)	const	{	return( (SG_Char)asInt   (iField) );	}
	SG_Char						asChar			(const CSG_String &Field)	const	{	return( (SG_Char)asInt   ( Field) );	}
	short						asShort			(int              iField)	const	{	return( (short  )asInt   (iField) );	}
	short						asShort			(const CSG_String &Field)	const	{	return( (short  )asInt   ( Field) );	}
	int							asInt			(int              iField)	const;
	int							asInt			(const CSG_String &Field)	const;
	sLong						asLong			(int              iField)	const;
	sLong						asLong			(const CSG_String &Field)	const;
	float						asFloat			(int              iField)	const	{	return( (float  )asDouble(iField) );	}
	float						asFloat			(const CSG_String &Field)	const	{	return( (float  )asDouble( Field) );	}
	double						asDouble		(int              iField)	const;
	double						asDouble		(const CSG_String &Field)	const;

	CSG_Table_Value *			Get_Value		(int              iField)			{	return(  m_Values[iField] );	}
	CSG_Table_Value &			operator []		(int              iField)	const	{	return( *m_Values[iField] );	}

	virtual bool				Assign			(CSG_Table_Record *pRecord);

	bool						is_Selected		(void)						const	{	return( (m_Flags & SG_TABLE_REC_FLAG_Selected) != 0 );	}
	bool						is_Modified		(void)						const	{	return( (m_Flags & SG_TABLE_REC_FLAG_Modified) != 0 );	}


protected:

	CSG_Table_Record(class CSG_Table *pTable, int Index);
	virtual ~CSG_Table_Record(void);


	char						m_Flags;

	int							m_Index;

	class CSG_Table_Value		**m_Values;

	class CSG_Table				*m_pTable;


	void						Set_Selected	(bool bOn = true);
	void						Set_Modified	(bool bOn = true);


	static CSG_Table_Value *	_Create_Value	(TSG_Data_Type Type);

	bool						_Add_Field		(int add_Field);
	bool						_Del_Field		(int del_Field);

	int							_Get_Field	 	(const CSG_String &Field)	const;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Table : public CSG_Data_Object
{
	friend class CSG_Table_Record;
	friend class CSG_TIN;

public:

	CSG_Table(void);

									CSG_Table			(const CSG_Table &Table);
	bool							Create				(const CSG_Table &Table);

									CSG_Table			(const CSG_Table *pTemplate);
	bool							Create				(const CSG_Table *pTemplate);

									CSG_Table			(const CSG_String &FileName, TSG_Table_File_Type Format = TABLE_FILETYPE_Undefined, int Encoding = SG_FILE_ENCODING_UNDEFINED);
	bool							Create				(const CSG_String &FileName, TSG_Table_File_Type Format = TABLE_FILETYPE_Undefined, int Encoding = SG_FILE_ENCODING_UNDEFINED);

									CSG_Table			(const CSG_String &FileName, TSG_Table_File_Type Format, const SG_Char Separator  , int Encoding = SG_FILE_ENCODING_UNDEFINED);
	bool							Create				(const CSG_String &FileName, TSG_Table_File_Type Format, const SG_Char Separator  , int Encoding = SG_FILE_ENCODING_UNDEFINED);

	virtual ~CSG_Table(void);

	virtual bool					Destroy				(void);

	virtual TSG_Data_Object_Type	Get_ObjectType		(void)	const			{	return( SG_DATAOBJECT_TYPE_Table );	}

	CSG_Table &						operator =			(const CSG_Table &Table);
	virtual bool					Assign				(CSG_Data_Object *pSource);

	bool							Assign_Values		(const CSG_Table  &Table);
	bool							Assign_Values		(      CSG_Table *pTable);
	bool							Assign_Values		(const SG_Char *FileName);

	bool							Load				(const CSG_String &File, int Format, SG_Char Separator, int Encoding = SG_FILE_ENCODING_UNDEFINED);

	virtual bool					Save				(const CSG_String &File, int Format, SG_Char Separator, int Encoding = SG_FILE_ENCODING_UNDEFINED);
	virtual bool					Save				(const char       *File, int Format, SG_Char Separator, int Encoding = SG_FILE_ENCODING_UNDEFINED)	{ return( Save(CSG_String(File), Format, Separator, Encoding) ); }
	virtual bool					Save				(const wchar_t    *File, int Format, SG_Char Separator, int Encoding = SG_FILE_ENCODING_UNDEFINED)	{ return( Save(CSG_String(File), Format, Separator, Encoding) ); }

	virtual bool					Save				(const CSG_String &File, int Format = 0);
	virtual bool					Save				(const char       *File, int Format = 0)	{	return( Save(CSG_String(File), Format) );	}
	virtual bool					Save				(const wchar_t    *File, int Format = 0)	{	return( Save(CSG_String(File), Format) );	}

	bool							Set_File_Encoding	(int Encoding);
	int								Get_File_Encoding	(void)	const	{	return( m_Encoding );	}

	bool							Serialize			(CSG_File &Stream, bool bSave);

	//-----------------------------------------------------
	virtual bool					is_Valid			(void)	const			{	return( m_nFields > 0 );	}

	bool							is_Compatible		(const CSG_Table  &Table, bool bExactMatch = false)	const;
	bool							is_Compatible		(      CSG_Table *pTable, bool bExactMatch = false)	const;

	//-----------------------------------------------------
	virtual const CSG_Rect &		Get_Extent			(void)					{	return( m_Extent );	}

	//-----------------------------------------------------
	virtual bool					Add_Field			(const CSG_String &Name, TSG_Data_Type Type, int Position = -1);
	virtual bool					Del_Field			(int iField);
	virtual bool					Mov_Field			(int iField, int Position);

	int								Get_Field_Count		(void)			const	{	return( m_nFields );	}
	const SG_Char *					Get_Field_Name		(int iField)	const	{	return( iField >= 0 && iField < m_nFields ? m_Field_Name[iField]->c_str() : NULL );			}
	TSG_Data_Type					Get_Field_Type		(int iField)	const	{	return( iField >= 0 && iField < m_nFields ? m_Field_Type[iField] : SG_DATATYPE_Undefined );	}
	int								Get_Field_Length	(int iField, int Encoding = SG_FILE_ENCODING_UNDEFINED)	const;	// returns the maximum number of characters for data type string and zero for all other data types.
	int								Get_Field			(const CSG_String &Name)	const;	// returns the zero based position of the field named 'Name' or '-1' if there is no field with such name.

	bool							Set_Field_Name		(int iField, const SG_Char *Name);
	virtual bool					Set_Field_Type		(int iField, TSG_Data_Type  Type);

	sLong							Get_N				(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Count   () : 0   );	}
	double							Get_Minimum			(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Minimum () : 0.0 );	}
	double							Get_Maximum			(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Maximum () : 0.0 );	}
	double							Get_Range			(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Range   () : 0.0 );	}
	double							Get_Sum				(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Sum     () : 0.0 );	}
	double							Get_Mean			(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Mean    () : 0.0 );	}
	double							Get_StdDev			(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_StdDev  () : 0.0 );	}
	double							Get_Variance		(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Variance() : 0.0 );	}
	const CSG_Simple_Statistics &	Get_Statistics		(int iField)	const	{	_Stats_Update(iField); return( *m_Field_Stats[iField] );	}

	//-----------------------------------------------------
	virtual CSG_Table_Record *		Add_Record			(             CSG_Table_Record *pCopy = NULL);
	virtual CSG_Table_Record *		Ins_Record			(int iRecord, CSG_Table_Record *pCopy = NULL);
	virtual bool					Set_Record			(int iRecord, CSG_Table_Record *pCopy       );
	virtual bool					Del_Record			(int iRecord);
	virtual bool					Del_Records			(void);

	virtual bool					Set_Count			(int nRecords);
	virtual bool					Set_Record_Count	(int nRecords);

	int								Get_Count			(void)			const	{	return( m_nRecords );	}
	int								Get_Record_Count	(void)			const	{	return( m_nRecords );	}

	virtual CSG_Table_Record *		Get_Record			(int    Index)	const	{	return( Index >= 0 && Index < m_nRecords ? m_Records[Index] : NULL );	}
	virtual CSG_Table_Record *		Get_Record			(size_t Index)	const	{	return(          (int)Index < m_nRecords ? m_Records[Index] : NULL );	}
	virtual CSG_Table_Record *		Get_Record			(sLong  Index)	const	{	return( Index >= 0 && Index < m_nRecords ? m_Records[Index] : NULL );	}

	virtual CSG_Table_Record &		operator []			(int    Index)	const	{	return( *Get_Record_byIndex(Index) );	}
	virtual CSG_Table_Record &		operator []			(size_t Index)	const	{	return( *Get_Record_byIndex(Index) );	}
	virtual CSG_Table_Record &		operator []			(sLong  Index)	const	{	return( *Get_Record_byIndex(Index) );	}

	int								Get_Index			(int    Index)	const	{	return( Index >= 0 && Index < m_nRecords ? (is_Indexed() ? m_Index[Index] : Index) : -1 );	}
	int								Get_Index			(size_t Index)	const	{	return( Get_Index((int)Index) );	}
	int								Get_Index			(sLong  Index)	const	{	return( Get_Index((int)Index) );	}

	CSG_Table_Record *				Get_Record_byIndex	(sLong  Index)	const	{	return( Get_Record_byIndex((int)Index) ); }
	CSG_Table_Record *				Get_Record_byIndex	(size_t Index)	const	{	return( Get_Record_byIndex((int)Index) ); }
	CSG_Table_Record *				Get_Record_byIndex	(int    Index)	const
	{
		return( Index >= 0 && Index < m_nRecords ? Get_Record(is_Indexed() ? m_Index[Index] : Index) : NULL );
	}

	//-----------------------------------------------------
	virtual bool					Find_Record			(int &iRecord, int iField, const CSG_String &Value, bool bCreateIndex = false);
	virtual CSG_Table_Record *		Find_Record			(              int iField, const CSG_String &Value, bool bCreateIndex = false);
	virtual bool					Find_Record			(int &iRecord, int iField, double            Value, bool bCreateIndex = false);
	virtual CSG_Table_Record *		Find_Record			(              int iField, double            Value, bool bCreateIndex = false);

	//-----------------------------------------------------
	virtual bool					Set_Value			(int iRecord, int iField, const SG_Char  *Value);
	virtual bool					Set_Value			(int iRecord, int iField, double          Value);

	virtual bool					Get_Value			(int iRecord, int iField, CSG_String     &Value)	const;
	virtual bool					Get_Value			(int iRecord, int iField, double         &Value)	const;

	virtual void					Set_Modified		(bool bModified = true);

	//-----------------------------------------------------
	size_t							Get_Selection_Count	(void)				const	{	return( m_Selection.Get_Size() );	}
	size_t							Get_Selection_Index	(size_t Index = 0)	const	{	return( Index < m_Selection.Get_Size() ? *((size_t *)m_Selection.Get_Entry(Index)) : (size_t)Get_Count() );	}
	virtual CSG_Table_Record *		Get_Selection		(size_t Index = 0)	const	{	return( Index < m_Selection.Get_Size() ? Get_Record((int)Get_Selection_Index(Index)) : NULL );	}

	virtual bool					is_Selected			(int iRecord)	const	{	return( iRecord >= 0 && iRecord < m_nRecords ? m_Records[iRecord]->is_Selected() : false );	}

	virtual bool					Select				(int iRecord						, bool bInvert = false);
	virtual bool					Select				(CSG_Table_Record *pRecord = NULL	, bool bInvert = false);

	virtual int						Del_Selection		(void);
	virtual int						Inv_Selection		(void);

	//-----------------------------------------------------
	bool							Set_Index			(CSG_Index &Index, int Field                  , bool bAscending = true);
	bool							Set_Index			(CSG_Index &Index, int Fields[], int   nFields, bool bAscending = true);
	bool							Set_Index			(CSG_Index &Index, const CSG_Array_Int &Fields, bool bAscending = true);
	bool							Del_Index			(void);
	bool							Toggle_Index		(int iField);

	bool							is_Indexed			(void)	const		{	return( m_nRecords > 0 && m_Index.Get_Count() == m_nRecords );	}

	bool							Set_Index			(int Field_1, TSG_Table_Index_Order Order_1, int Field_2 = -1, TSG_Table_Index_Order Order_2 = TABLE_INDEX_None, int Field_3 = -1, TSG_Table_Index_Order Order_3 = TABLE_INDEX_None);
	int								Get_Index_Field		(size_t i)	const	{	return( i >= m_Index_Fields.Get_Size() ? -1 : abs(m_Index_Fields[i]) - 1 );	}
	TSG_Table_Index_Order			Get_Index_Order		(size_t i)	const	{	return( i >= m_Index_Fields.Get_Size() ? TABLE_INDEX_None : m_Index_Fields[i] > 0 ? TABLE_INDEX_Ascending : TABLE_INDEX_Descending );	}


protected:

	int								m_nFields, m_nRecords, m_nBuffer, m_Encoding;

	TSG_Data_Type					*m_Field_Type;

	CSG_String						**m_Field_Name;

	CSG_Simple_Statistics			**m_Field_Stats;

	CSG_Array						m_Selection;

	CSG_Rect						m_Extent;


	virtual void					_On_Construction	(void);

	virtual CSG_Table_Record *		_Get_New_Record		(int Index);

	bool							_Add_Selection		(size_t iRecord);
	bool							_Set_Selection		(size_t iRecord, size_t Index);
	bool							_Del_Selection		(size_t iRecord);

	bool							_Stats_Invalidate	(void)			const;
	bool							_Stats_Invalidate	(int iField)	const;
	virtual bool					_Stats_Update		(int iField)	const;

	virtual bool					On_NoData_Changed	(void);
	virtual bool					On_Update			(void);

	virtual bool					On_Reload			(void);
	virtual bool					On_Delete			(void);


private:

	CSG_Index						m_Index;

	CSG_Array_Int					m_Index_Fields;

	CSG_Table_Record				**m_Records;


	bool							_Destroy_Selection	(void);

	bool							_Inc_Array			(void);
	bool							_Dec_Array			(void);

	size_t							_Load_Text_Trim		(      CSG_String &Text, const SG_Char Separator);
	size_t							_Load_Text_EndQuote	(const CSG_String &Text, const SG_Char Separator);

	bool							_Load_Text			(const CSG_String &FileName, bool bHeadline, const SG_Char Separator);
	bool							_Save_Text			(const CSG_String &FileName, bool bHeadline, const SG_Char Separator);

	bool							_Load_DBase			(const CSG_String &FileName);
	bool							_Save_DBase			(const CSG_String &FileName);

	void							_Index_Update		(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** Safe table construction */
SAGA_API_DLL_EXPORT CSG_Table *	SG_Create_Table	(void);

/** Safe table construction */
SAGA_API_DLL_EXPORT CSG_Table *	SG_Create_Table	(const CSG_Table &Table);

/** Safe table construction */
SAGA_API_DLL_EXPORT CSG_Table *	SG_Create_Table	(CSG_Table *pTemplate);

/** Safe table construction */
SAGA_API_DLL_EXPORT CSG_Table *	SG_Create_Table	(const CSG_String &FileName, TSG_Table_File_Type Format = TABLE_FILETYPE_Undefined, int Encoding = SG_FILE_ENCODING_UNDEFINED);

/** Safe table construction */
SAGA_API_DLL_EXPORT CSG_Table *	SG_Create_Table	(const CSG_String &FileName, TSG_Table_File_Type Format, const SG_Char Separator  , int Encoding = SG_FILE_ENCODING_UNDEFINED);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__table_H
