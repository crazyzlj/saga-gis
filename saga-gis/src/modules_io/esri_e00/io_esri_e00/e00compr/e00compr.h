/**********************************************************
 * Version $Id$
 *********************************************************/
/**********************************************************************
 * $Id$
 *
 * Name:     e00compr.h
 * Project:  Compressed E00 Read/Write library
 * Language: ANSI C
 * Purpose:  Header file containing all definitions for the library.
 * Author:   Daniel Morissette, danmo@videotron.ca
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2005/08/31 14:01:00  oconrad
 * no message
 *
 * Revision 1.1.1.1  2005/08/17 08:25:16  oconrad
 * no message
 *
 * Revision 1.1.1.1  2005/08/15 13:35:12  oconrad
 * no message
 *
 * Revision 1.1  2004/04/16 13:36:45  oconrad
 * no message
 *
 * Revision 1.7  1999/02/25 18:47:40  daniel
 * Now use CPL for Error handling, Memory allocation, and File access.
 *
 * Revision 1.6  1999/01/08 17:40:33  daniel
 * Added E00Read/WriteCallbakcOpen()
 *
 * Revision 1.5  1998/11/13 15:39:45  daniel
 * Added functions for write support.
 *
 * Revision 1.4  1998/11/02 18:37:03  daniel
 * New file header, and added E00ErrorReset()
 *
 * Revision 1.1  1998/10/29 13:26:00  daniel
 * Initial revision
 *
 **********************************************************************
 * Copyright (c) 1998, 1999, Daniel Morissette
 *
 * All rights reserved.  This software may be copied or reproduced, in
 * all or in part, without the prior written consent of its author,
 * Daniel Morissette (danmo@videotron.ca).  However, any material copied
 * or reproduced must bear the original copyright notice (above), this 
 * original paragraph, and the original disclaimer (below).
 * 
 * The entire risk as to the results and performance of the software,
 * supporting text and other information contained in this file
 * (collectively called the "Software") is with the user.  Although 
 * considerable efforts have been used in preparing the Software, the 
 * author does not warrant the accuracy or completeness of the Software.
 * In no event will the author be liable for damages, including loss of
 * profits or consequential damages, arising out of the use of the 
 * Software.
 * 
 **********************************************************************/

#ifndef _E00COMPR_H_INCLUDED_
#define _E00COMPR_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>

#include "cpl_port.h"
#include "cpl_conv.h"
#include "cpl_error.h"

/*=====================================================================
                        Data types and constants
 =====================================================================*/

#define E00_READ_BUF_SIZE   256 /* E00 lines are always 80 chars or less */
                                /* for both compressed and uncompressed  */
                                /* files, except the first line (the EXP)*/
                                /* for which there is no known limit     */
                                /* We'll assume that it can't be longer  */
                                /* than 256 chars                        */

#define E00_WRITE_BUF_SIZE  256 /* This buffer must be big enough to hold*/
                                /* at least 2 lines of compressed output */
                                /* (i.e. 160 chars)... but just in case  */
                                /* compressing a line would ever result  */
                                /* in it becoming bigger than its source */
                                /* we'll set the size to 256 chars!      */

#define E00_COMPR_NONE      0   /* Compression levels to use when writing*/
#define E00_COMPR_PARTIAL   1
#define E00_COMPR_FULL      2

/*---------------------------------------------------------------------
 *                          E00ReadPtr
 *
 * A E00ReadPtr handle is used to hold information about the compressed 
 * file currently being read.
 *--------------------------------------------------------------------*/
struct _E00ReadInfo
{
    FILE    *fp;            /* Input file handle */
    int     bEOF;           /* Reached EOF?      */
    int     bIsCompressed;  /* 1 if file is compressed, 0 if not    */
    int     nInputLineNo;

    int     iInBufPtr;      /* Last character processed in szInBuf  */
    char    szInBuf[E00_READ_BUF_SIZE]; /* compressed input buffer  */
    char    szOutBuf[E00_READ_BUF_SIZE];/* uncompressed output buffer   */

    /* pRefData, pfnReadNextLine() and pfnReadRewind() are used only
     * when the file is opened with E00ReadCallbackOpen()
     * (and in this case the FILE *fp defined above is not used)
     */
    void       * pRefData;
    const char * (*pfnReadNextLine)(void *);
    void         (*pfnReadRewind)(void *);
};

typedef struct _E00ReadInfo *E00ReadPtr;

/*---------------------------------------------------------------------
 *                          E00WritePtr
 *
 * A E00WritePtr handle is used to hold information about the 
 * file currently being written.
 *--------------------------------------------------------------------*/
struct _E00WriteInfo
{
    FILE    *fp;            /* Output file handle */
    int     nComprLevel;

    int     nSrcLineNo;

    int     iOutBufPtr;     /* Current position in szOutBuf  */
    char    szOutBuf[E00_WRITE_BUF_SIZE]; /* compressed output buffer */

    /* pRefData and pfnWriteNextLine() are used only
     * when the file is opened with E00WriteCallbackOpen()
     * (and in this case the FILE *fp defined above is not used)
     */
    void    *pRefData;
    int     (*pfnWriteNextLine)(void *, const char *);
};

typedef struct _E00WriteInfo *E00WritePtr;


/*=====================================================================
                        Function prototypes
 =====================================================================*/

E00ReadPtr  E00ReadOpen(const char *pszFname);
E00ReadPtr  E00ReadCallbackOpen(void *pRefData,
                                const char * (*pfnReadNextLine)(void *),
                                void (*pfnReadRewind)(void *));
void        E00ReadClose(E00ReadPtr psInfo);

const char *E00ReadNextLine(E00ReadPtr psInfo);
void        E00ReadRewind(E00ReadPtr psInfo);

E00WritePtr E00WriteOpen(const char *pszFname, int nComprLevel);
E00WritePtr E00WriteCallbackOpen(void *pRefData,
                                 int (*pfnWriteNextLine)(void *, const char *),
                                 int nComprLevel);
void        E00WriteClose(E00WritePtr psInfo);
int         E00WriteNextLine(E00WritePtr psInfo, const char *pszLine);


#ifdef __cplusplus
}
#endif

#endif /* _E00COMPR_H_INCLUDED_ */
