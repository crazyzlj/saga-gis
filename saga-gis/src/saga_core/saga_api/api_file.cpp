/**********************************************************
 * Version $Id$
 *********************************************************/

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
//                    api_file.cpp                       //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/utils.h>
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/wxcrtvararg.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

#include "api_core.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_File::CSG_File(void)
{
	m_pStream	= NULL;
	m_Encoding	= SG_FILE_ENCODING_CHAR;
}

//---------------------------------------------------------
CSG_File::CSG_File(const CSG_String &FileName, int Mode, bool bBinary, int Encoding)
{
	m_pStream	= NULL;

	Open(FileName, Mode, bBinary, Encoding);
}

//---------------------------------------------------------
CSG_File::~CSG_File(void)
{
	Close();
}

//---------------------------------------------------------
bool CSG_File::Open(const CSG_String &FileName, int Mode, bool bBinary, int Encoding)
{
	Close();

	m_Mode		= Mode;
	m_Encoding	= Encoding;
	m_FileName	= FileName;

	wxString	sEncoding;

	switch( Encoding )
	{
	default                      :	sEncoding	= ""             ;	break;
	case SG_FILE_ENCODING_UNICODE:	sEncoding	= ", ccs=UNICODE";	break;
	case SG_FILE_ENCODING_UTF8   :	sEncoding	= ", ccs=UTF-8"  ;	break;
	case SG_FILE_ENCODING_UTF16  :	sEncoding	= ", ccs=UTF-16" ;	break;
	}

	if( Mode == SG_FILE_W )
	{
		wxString	sMode	= bBinary ? "wb" : "w";

		m_pStream	= new wxFFileOutputStream(FileName.c_str(), sMode + sEncoding);
	}
	else if( Mode == SG_FILE_R && SG_File_Exists(FileName) )
	{
		wxString	sMode	= bBinary ? "rb" : "r";

		m_pStream	= new wxFFileInputStream (FileName.c_str(), sMode + sEncoding);
	}
	else if( Mode == SG_FILE_RW && SG_File_Exists(FileName) )
	{
		wxString	sMode	= bBinary ? "r+b" : "r+";

		m_pStream	= (wxInputStream *)new wxFFileStream(FileName.c_str(), sMode + sEncoding);
	}

	if( !m_pStream || !m_pStream->IsOk() )
	{
		Close();

		return( false );
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_File::Close(void)
{
	if( m_pStream )
	{
		delete(m_pStream);

		m_pStream	= NULL;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
sLong CSG_File::Length(void) const
{
	return( m_pStream ? m_pStream->GetLength() : -1 );
}

//---------------------------------------------------------
bool CSG_File::is_EOF(void)	const
{
	return( is_Reading() && ((wxInputStream *)m_pStream)->Eof() );
}

//---------------------------------------------------------
bool CSG_File::Seek(sLong Offset, int Origin) const
{
	if( is_Reading() )
	{
		switch( Origin )
		{
		default             : return( ((wxInputStream  *)m_pStream)->SeekI(Offset, wxFromStart  ) != wxInvalidOffset );
		case SG_FILE_CURRENT: return( ((wxInputStream  *)m_pStream)->SeekI(Offset, wxFromCurrent) != wxInvalidOffset );
		case SG_FILE_END    : return( ((wxInputStream  *)m_pStream)->SeekI(Offset, wxFromEnd    ) != wxInvalidOffset );
		}
	}

	if( is_Writing() )
	{
		switch( Origin )
		{
		default             : return( ((wxOutputStream *)m_pStream)->SeekO(Offset, wxFromStart  ) != wxInvalidOffset );
		case SG_FILE_CURRENT: return( ((wxOutputStream *)m_pStream)->SeekO(Offset, wxFromCurrent) != wxInvalidOffset );
		case SG_FILE_END    : return( ((wxOutputStream *)m_pStream)->SeekO(Offset, wxFromEnd    ) != wxInvalidOffset );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_File::Seek_Start(void) const	{	return( Seek(0, SEEK_SET) );	}
bool CSG_File::Seek_End  (void) const	{	return( Seek(0, SEEK_END) );	}

//---------------------------------------------------------
sLong CSG_File::Tell(void) const
{
	if( is_Reading() )
	{
		return( ((wxInputStream  *)m_pStream)->TellI() );
	}

	if( is_Writing() )
	{
		return( ((wxOutputStream *)m_pStream)->TellO() );
	}

	return( -1 );
}

//---------------------------------------------------------
int CSG_File::Printf(const char *Format, ...)
{
	if( !is_Writing() )
	{
		return( 0 );
	}

	wxString	String;

#ifdef _SAGA_LINUX
	wxString _Format(Format); _Format.Replace("%s", "%ls");	// workaround as we only use wide characters since wx 2.9.4 so interpret strings as multibyte
	va_list	argptr; va_start(argptr, _Format);
	int	Result	= String.PrintfV(_Format, argptr);
#else
	va_list	argptr; va_start(argptr, Format);
	int	Result	= String.PrintfV(Format, argptr);
#endif
	va_end(argptr);

	Write(&String);

	return( Result );
}

//---------------------------------------------------------
int CSG_File::Printf(const wchar_t *Format, ...)
{
	if( !is_Writing() )
	{
		return( 0 );
	}

	wxString	String;

#ifdef _SAGA_LINUX
	wxString _Format(Format); _Format.Replace("%s", "%ls");	// workaround as we only use wide characters since wx 2.9.4 so interpret strings as multibyte
	va_list	argptr; va_start(argptr, _Format);
	int	Result	= String.PrintfV(_Format, argptr);
#else
	va_list	argptr; va_start(argptr, Format);
	int	Result	= String.PrintfV(Format, argptr);
#endif

	va_end(argptr);

	Write(&String);

	return( Result );
}

//---------------------------------------------------------
size_t CSG_File::Read(void *Buffer, size_t Size, size_t Count) const
{
	return( is_Reading() && Size > 0 ? ((wxInputStream *)m_pStream)->Read(Buffer, Size * Count).LastRead() / Size : 0 );
}

size_t CSG_File::Read(CSG_String &Buffer, size_t Size) const
{
	if( is_Reading() )
	{
		CSG_Array	b(1, Size);

		size_t	 i	= Read(b.Get_Array(), b.Get_Value_Size(), Size);

		Buffer	= (const char *)b.Get_Array();

		return( i );
	}

	return( 0 );
}

//---------------------------------------------------------
size_t CSG_File::Write(void *Buffer, size_t Size, size_t Count) const
{
	return( is_Writing() ? ((wxOutputStream *)m_pStream)->Write(Buffer, Size * Count).LastWrite() : 0 );
}

size_t CSG_File::Write(const CSG_String &Buffer) const
{
	return( Write((void *)Buffer.b_str(), sizeof(char), strlen(Buffer.b_str())) );
}

//---------------------------------------------------------
bool CSG_File::Read_Line(CSG_String &sLine)	const
{
	if( is_Reading() && !is_EOF() )
	{
		sLine.Clear();

		int		c;

		while( !is_EOF() && (c = ((wxInputStream *)m_pStream)->GetC()) != 0x0A && c != EOF )
		{
			if( c != 0x0D )
			{
				sLine.Append((char)c);
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
int CSG_File::Read_Char(void) const
{
	return( is_Reading() ? ((wxInputStream *)m_pStream)->GetC() : 0 );
}

//---------------------------------------------------------
int CSG_File::Read_Int(bool bByteOrderBig) const
{
	int		Value	= 0;

	if( Read(&Value, sizeof(Value)) == 1 )
	{
		if( bByteOrderBig )
		{
			SG_Swap_Bytes(&Value, sizeof(Value));
		}
	}

	return( Value );
}

bool CSG_File::Write_Int(int Value, bool bByteOrderBig)
{
	if( bByteOrderBig )
	{
		SG_Swap_Bytes(&Value, sizeof(Value));
	}

	return( Write(&Value, sizeof(Value)) == sizeof(Value) );
}

//---------------------------------------------------------
double CSG_File::Read_Double(bool bByteOrderBig) const
{
	double	Value	= 0;

	if( Read(&Value, sizeof(Value)) == 1 )
	{
		if( bByteOrderBig )
		{
			SG_Swap_Bytes(&Value, sizeof(Value));
		}
	}

	return( Value );
}

bool CSG_File::Write_Double(double Value, bool bByteOrderBig)
{
	if( bByteOrderBig )
	{
		SG_Swap_Bytes(&Value, sizeof(Value));
	}

	return( Write(&Value, sizeof(Value)) == sizeof(Value) );
}

//---------------------------------------------------------
bool CSG_File::Scan(int &Value) const
{
	if( is_Reading() )
	{
		int		c;

		while( !is_EOF() && isspace(c = Read_Char()) );	// remove leading white space

		if( isdigit(c) || strchr("-+", c) )
		{
			CSG_String	s	= (char)c;

			while( !is_EOF() && isdigit(c = Read_Char()) )
			{
				s	+= (char)c;
			}

			return( s.asInt(Value) );
		}
	}

	return( false );
}

bool CSG_File::Scan(double &Value) const
{
	if( is_Reading() )
	{
		int		c;

		while( !is_EOF() && isspace(c = Read_Char()) );	// remove leading white space

		if( isdigit(c) || strchr("-+.,eE", c) )
		{
			CSG_String	s	= (char)c;

			while( !is_EOF() && (isdigit(c = Read_Char()) || strchr("", c)) )
			{
				s	+= (char)c;
			}

			return( s.asDouble(Value) );
		}
	}

	return( false );
}

bool CSG_File::Scan(CSG_String &Value, SG_Char Separator) const
{
	if( is_Reading() && !is_EOF() )
	{
		Value.Clear();

		int		c;

		while( !is_EOF() && (c = ((wxInputStream *)m_pStream)->GetC()) != Separator && c != EOF )
		{
			Value	+= (char)c;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
int CSG_File::Scan_Int(void) const
{
	int		Value;

	return( Scan(Value) ? Value : 0 );
}

double CSG_File::Scan_Double(void) const
{
	double	Value;

	return( Scan(Value) ? Value : 0.0 );
}

CSG_String CSG_File::Scan_String(SG_Char Separator) const
{
	CSG_String	Value;

	Scan(Value, Separator);

	return( Value );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_File_Zip::CSG_File_Zip(void)
{
}

//---------------------------------------------------------
CSG_File_Zip::~CSG_File_Zip(void)
{
	Close();
}

//---------------------------------------------------------
CSG_File_Zip::CSG_File_Zip(const CSG_String &FileName, int Mode)
{
	Open(FileName, Mode);
}

//---------------------------------------------------------
bool CSG_File_Zip::Open(const CSG_String &FileName, int Mode)
{
	Close();

	m_Mode	= Mode;

	if( Mode == SG_FILE_W )
	{
		m_pStream	= new wxZipOutputStream(new wxFileOutputStream(FileName.c_str()));
	}
	else if( Mode == SG_FILE_R && SG_File_Exists(FileName) )
	{
		m_pStream	= new wxZipInputStream(new wxFileInputStream(FileName.c_str()));
	}

	if( !m_pStream || !m_pStream->IsOk() )
	{
		Close();

		return( false );
	}

	if( is_Reading() )
	{
		wxZipEntry	*pEntry;

		while( (pEntry = ((wxZipInputStream *)m_pStream)->GetNextEntry()) != NULL )
		{
			m_Files	+= pEntry;
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_File_Zip::Close(void)
{
	for(size_t i=0; i<m_Files.Get_Size(); i++)
	{
		delete((wxZipEntry *)m_Files[i]);
	}

	m_Files.Set_Array(0);

	return( CSG_File::Close() );
}

//---------------------------------------------------------
bool CSG_File_Zip::Add_Directory(const CSG_String &Name)
{
	return( is_Writing() && ((wxZipOutputStream *)m_pStream)->PutNextDirEntry(Name.c_str()) );
}

//---------------------------------------------------------
bool CSG_File_Zip::Add_File(const CSG_String &Name, bool bBinary)
{
	if( is_Writing() )
	{
		wxZipEntry	*pEntry	= new wxZipEntry(Name.c_str());

		pEntry->SetIsText(bBinary == false);

		if( ((wxZipOutputStream *)m_pStream)->PutNextEntry(pEntry) )
		{
			m_FileName	= Name;

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_File_Zip::is_Directory(size_t Index)
{
	if( is_Reading() && m_Files[Index] )
	{
		return( ((wxZipEntry *)m_Files[Index])->IsDir() );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_File_Zip::Get_File(size_t Index)
{
	if( is_Reading() && m_Files[Index] )
	{
		if( ((wxZipInputStream *)m_pStream)->OpenEntry(*(wxZipEntry *)m_Files[Index]) )
		{
			m_FileName	= Get_File_Name(Index);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_File_Zip::Get_File(const CSG_String &Name)
{
	if( is_Reading() )
	{
		for(size_t i=0; i<m_Files.Get_Size(); i++)
		{
			if( !Name.Cmp(&((wxZipEntry *)m_Files[i])->GetName()) )
			{
				return( Get_File(i) );
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
CSG_String CSG_File_Zip::Get_File_Name(size_t Index)
{
	CSG_String	s;

	if( is_Reading() && m_Files[Index] )
	{
		s	= &((wxZipEntry *)m_Files[Index])->GetName();
	}

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool			SG_Dir_Exists(const CSG_String &Directory)
{
	return( wxFileName::DirExists(Directory.c_str()) );
}

//---------------------------------------------------------
bool			SG_Dir_Create(const CSG_String &Directory)
{
	if( SG_Dir_Exists(Directory) )
	{
		return( true );
	}

	return( wxFileName::Mkdir(Directory.c_str()) );
}

//---------------------------------------------------------
CSG_String		SG_Dir_Get_Current(void)
{
	wxString cwd = wxFileName::GetCwd();

	return( CSG_String(&cwd) );
}

//---------------------------------------------------------
CSG_String		SG_Dir_Get_Temp(void)
{
	wxString fname = wxFileName::GetTempDir();

	return( CSG_String(&fname) );
}

//---------------------------------------------------------
bool			SG_Dir_List_Subdirectories	(CSG_Strings &List, const CSG_String &Directory)
{
	List.Clear();

	wxDir	Dir;

	if( Dir.Open(Directory.c_str()) )
	{
		wxString	FileName;

		if( Dir.GetFirst(&FileName, wxEmptyString, wxDIR_DIRS) )
		{
			do
			{
				List	+= SG_File_Make_Path(Directory, &FileName);
			}
			while( Dir.GetNext(&FileName) );
		}
	}

	return( List.Get_Count() > 0 );
}

//---------------------------------------------------------
bool			SG_Dir_List_Files			(CSG_Strings &List, const CSG_String &Directory)
{
	return( SG_Dir_List_Files(List, Directory, "") );
}

bool			SG_Dir_List_Files			(CSG_Strings &List, const CSG_String &Directory, const CSG_String &Extension)
{
	List.Clear();

	wxDir	Dir;

	if( Dir.Open(Directory.c_str()) )
	{
		wxString	FileName;

		if( Dir.GetFirst(&FileName, wxEmptyString, wxDIR_FILES) )
		{
			do
			{
				if( Extension.is_Empty() || SG_File_Cmp_Extension(&FileName, Extension) )
				{
					List	+= SG_File_Make_Path(Directory, &FileName);
				}
			}
			while( Dir.GetNext(&FileName) );
		}
	}

	return( List.Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool			SG_File_Exists(const CSG_String &FileName)
{
	return( wxFileExists(FileName.c_str()) );
}

//---------------------------------------------------------
bool			SG_File_Delete(const CSG_String &FileName)
{
	return( SG_File_Exists(FileName) && wxRemoveFile(FileName.c_str()) );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Name_Temp(const CSG_String &Prefix)
{
	return( SG_File_Get_Name_Temp(Prefix, "") );
}

CSG_String		SG_File_Get_Name_Temp(const CSG_String &Prefix, const CSG_String &Directory)
{
	if( !SG_Dir_Exists(Directory) )
	{
		return( CSG_String(wxFileName::CreateTempFileName(Prefix.c_str()).wc_str()) );
	}

	return( CSG_String(wxFileName::CreateTempFileName(SG_File_Make_Path(Directory, Prefix).w_str()).wc_str()) );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Name(const CSG_String &full_Path, bool bExtension)
{
	wxFileName	fn(full_Path.c_str());

	if( bExtension )
	{
		return( CSG_String(&fn.GetFullName()) );
	}

	return( CSG_String(&fn.GetName()) );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Path(const CSG_String &full_Path)
{
	wxFileName	fn(full_Path.c_str());

	return( CSG_String(&fn.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR)) );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Path_Absolute(const CSG_String &full_Path)
{
	wxFileName	fn(full_Path.c_str());

	fn.MakeAbsolute();

	return( CSG_String(&fn.GetFullPath()) );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Path_Relative(const CSG_String &Directory, const CSG_String &full_Path)
{
	wxFileName	fn(full_Path.c_str());

	fn.MakeRelativeTo(Directory.c_str());

	return( CSG_String(&fn.GetFullPath()) );
}

//---------------------------------------------------------
CSG_String		SG_File_Make_Path(const CSG_String &Directory, const CSG_String &Name)
{
	return( SG_File_Make_Path(Directory, Name, "") );
}

CSG_String		SG_File_Make_Path(const CSG_String &Directory, const CSG_String &Name, const CSG_String &Extension)
{
	wxFileName	fn;

	fn.AssignDir(!Directory.is_Empty() ? Directory.c_str() : SG_File_Get_Path(Name).c_str());

	if( !Extension.is_Empty() )
	{
		fn.SetName		(SG_File_Get_Name(Name, false).c_str());
		fn.SetExt		(Extension.c_str());
	}
	else
	{
		fn.SetFullName	(SG_File_Get_Name(Name,  true).c_str());
	}

	return( CSG_String(&fn.GetFullPath()) );
}

//---------------------------------------------------------
bool			SG_File_Cmp_Extension(const CSG_String &FileName, const CSG_String &Extension)
{
	return( SG_File_Get_Extension(FileName).CmpNoCase(Extension) == 0 );
}

//---------------------------------------------------------
bool			SG_File_Set_Extension(CSG_String &FileName, const CSG_String &Extension)
{
	if( FileName.Length() > 0 && Extension.Length() > 0 )
	{
		wxFileName	fn(FileName.c_str());

		fn.SetExt(Extension.c_str());

		FileName	= &fn.GetFullPath();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
CSG_String		SG_File_Get_Extension(const CSG_String &FileName)
{
	wxFileName	fn(FileName.c_str());

	return( CSG_String(&fn.GetExt()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool			SG_Get_Environment(const CSG_String &Variable, CSG_String *Value)
{
	if( Value == NULL )
	{
		return( wxGetEnv(Variable.w_str(), NULL) );
	}

	wxString	s;

	if( wxGetEnv(Variable.w_str(), &s) )
	{
		*Value	= s.wc_str();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool			SG_Set_Environment(const CSG_String &Variable, const CSG_String &Value)
{
	return( wxSetEnv(Variable.w_str(), Value.w_str()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
