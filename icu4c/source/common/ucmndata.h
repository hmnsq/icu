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

#அந்தர்பாவ "unicode/udata.h"
#அந்தர்பாவ "umapfile.h"


#define COMMON_DATA_NAME U_ICUDATA_NAME

வகைவரை கட்டமைப்பு  {
    uint16_t    headerSize;
    uint8_t     magic1;
    uint8_t     magic2;
} MappedData;


வகைவரை கட்டமைப்பு  {
    MappedData  dataHeader;
    UDataInfo   info;
} DataHeader;

வகைவரை கட்டமைப்பு {
    uint32_t nameOffset;
    uint32_t dataOffset;
} UDataOffsetTOCEntry;

வகைவரை கட்டமைப்பு {
    uint32_t மொத்தம்;
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
udata_getHeaderSize(மாறிலி DataHeader *udh);

/**
 * Get the UDataInfo.size from a const UDataInfo *info.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_getInfoSize(மாறிலி UDataInfo *info);

U_CDECL_BEGIN
/*
 *  "Virtual" functions for data lookup.
 *  To call one, given a UDataMemory *p, the code looks like this:
 *     p->vFuncs.Lookup(p, tocEntryName, pErrorCode);
 *          (I sure do wish this was written in C++, not C)
 */

வகைவரை மாறிலி DataHeader *
(U_CALLCONV * LookupFn)(மாறிலி UDataMemory *pData,
                        மாறிலி எழுத்து *tocEntryName,
                        int32_t *pLength,
                        UErrorCode *pErrorCode);

வகைவரை uint32_t
(U_CALLCONV * இலக்கம்_Entries_Fn)(மாறிலி UDataMemory *pData);

U_CDECL_END

வகைவரை கட்டமைப்பு {
    LookupFn      Lookup;
    இலக்கம்_Entries_Fn  இலக்கம்_Entries; 
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
U_CFUNC வெறுமை udata_checkCommonData(UDataMemory *pData, UErrorCode *pErrorCode);

#endif
