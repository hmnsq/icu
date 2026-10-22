// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************/


/*----------------------------------------------------------------------------------
 *
 *   UCommonData   An abstract interface for dealing with ICU Common Data Files.
 *                 ICU Common Data Files are a grouping of a number of individual
 *                 data items (resources, converters, tables, anything) into a
 *                 single file or dll.  The combined format includes a table of
 *                 contents for locating the individual items by name.
 *
 *                 Two formats for the table of contents are supported, which is
 *                 why there is an abstract interface involved.
 *
 *                 These functions are part of the ICU internal implementation, and
 *                 are not intended to be used directly by applications.
 */

#ifndef __UCMNDATA_H__
#define __UCMNDATA_H__

#അന്തര്ഭാവ "unicode/udata.h"
#അന്തര്ഭാവ "umapfile.h"


#define COMMON_DATA_NAME U_ICUDATA_NAME

തരനിർവചനം ഘടന  {
    uint16_t    headerSize;
    uint8_t     magic1;
    uint8_t     magic2;
} MappedData;


തരനിർവചനം ഘടന  {
    MappedData  dataHeader;
    UDataInfo   info;
} DataHeader;

തരനിർവചനം ഘടന {
    uint32_t nameOffset;
    uint32_t dataOffset;
} UDataOffsetTOCEntry;

തരനിർവചനം ഘടന {
    uint32_t എണ്ണം;
    /**
     * Variable-length array declared with length 1 to disable bounds checkers.
     * The actual array length is in the count field.
     */
    UDataOffsetTOCEntry entry[1];
} UDataOffsetTOC;

/**
 * Get the header size from a const DataHeader *udh.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_getHeaderSize(സ്ഥിരം DataHeader *udh);

/**
 * Get the UDataInfo.size from a const UDataInfo *info.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_getInfoSize(സ്ഥിരം UDataInfo *info);

U_CDECL_BEGIN
/*
 *  "Virtual" functions for data lookup.
 *  To call one, given a UDataMemory *p, the code looks like this:
 *     p->vFuncs.Lookup(p, tocEntryName, pErrorCode);
 *          (I sure do wish this was written in C++, not C)
 */

തരനിർവചനം സ്ഥിരം DataHeader *
(U_CALLCONV * LookupFn)(സ്ഥിരം UDataMemory *pData,
                        സ്ഥിരം അക്ഷരം *tocEntryName,
                        int32_t *pLength,
                        UErrorCode *pErrorCode);

തരനിർവചനം uint32_t
(U_CALLCONV * അക്കം_Entries_Fn)(സ്ഥിരം UDataMemory *pData);

U_CDECL_END

തരനിർവചനം ഘടന {
    LookupFn      Lookup;
    അക്കം_Entries_Fn  അക്കം_Entries; 
} commonDataFuncs;


/*
 *  Functions to check whether a UDataMemory refers to memory containing 
 *     a recognizable header and table of contents a Common Data Format
 *
 *     If a valid header and TOC are found,
 *         set the CommonDataFuncs function dispatch vector in the UDataMemory
 *             to point to the right functions for the TOC type.
 *     otherwise
 *         set an errorcode.
 */
U_CFUNC ശൂന്യം udata_checkCommonData(UDataMemory *pData, UErrorCode *pErrorCode);

#endif
