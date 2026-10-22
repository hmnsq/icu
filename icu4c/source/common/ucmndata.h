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

#অন্তর্ভাৱ "unicode/udata.h"
#অন্তর্ভাৱ "umapfile.h"


#define COMMON_DATA_NAME U_ICUDATA_NAME

প্রকারনির্দেশ সংরচনা  {
    uint16_t    headerSize;
    uint8_t     magic1;
    uint8_t     magic2;
} MappedData;


প্রকারনির্দেশ সংরচনা  {
    MappedData  dataHeader;
    UDataInfo   info;
} DataHeader;

প্রকারনির্দেশ সংরচনা {
    uint32_t nameOffset;
    uint32_t dataOffset;
} UDataOffsetTOCEntry;

প্রকারনির্দেশ সংরচনা {
    uint32_t গুনতি;
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
udata_getHeaderSize(ধ্রুৱ DataHeader *udh);

/**
 * Get the UDataInfo.size from a const UDataInfo *info.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_getInfoSize(ধ্রুৱ UDataInfo *info);

U_CDECL_BEGIN
/*
 *  "Virtual" functions for data lookup.
 *  To call one, given a UDataMemory *p, the code looks like this:
 *     p->vFuncs.Lookup(p, tocEntryName, pErrorCode);
 *          (I sure do wish this was written in C++, not C)
 */

প্রকারনির্দেশ ধ্রুৱ DataHeader *
(U_CALLCONV * LookupFn)(ধ্রুৱ UDataMemory *pData,
                        ধ্রুৱ ৱর্ণ *tocEntryName,
                        int32_t *pLength,
                        UErrorCode *pErrorCode);

প্রকারনির্দেশ uint32_t
(U_CALLCONV * অঙ্ক_Entries_Fn)(ধ্রুৱ UDataMemory *pData);

U_CDECL_END

প্রকারনির্দেশ সংরচনা {
    LookupFn      Lookup;
    অঙ্ক_Entries_Fn  অঙ্ক_Entries; 
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
U_CFUNC শূন্য udata_checkCommonData(UDataMemory *pData, UErrorCode *pErrorCode);

#endif
