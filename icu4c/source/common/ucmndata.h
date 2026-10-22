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

#અન્તર્ભાવ "unicode/udata.h"
#અન્તર્ભાવ "umapfile.h"


#define COMMON_DATA_NAME U_ICUDATA_NAME

પ્રકારવ્યાખ્યા સંરચના  {
    uint16_t    headerSize;
    uint8_t     magic1;
    uint8_t     magic2;
} MappedData;


પ્રકારવ્યાખ્યા સંરચના  {
    MappedData  dataHeader;
    UDataInfo   info;
} DataHeader;

પ્રકારવ્યાખ્યા સંરચના {
    uint32_t nameOffset;
    uint32_t dataOffset;
} UDataOffsetTOCEntry;

પ્રકારવ્યાખ્યા સંરચના {
    uint32_t ગણતરી;
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
udata_getHeaderSize(અચલ DataHeader *udh);

/**
 * Get the UDataInfo.size from a const UDataInfo *info.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_getInfoSize(અચલ UDataInfo *info);

U_CDECL_BEGIN
/*
 *  "Virtual" functions for data lookup.
 *  To call one, given a UDataMemory *p, the code looks like this:
 *     p->vFuncs.Lookup(p, tocEntryName, pErrorCode);
 *          (I sure do wish this was written in C++, not C)
 */

પ્રકારવ્યાખ્યા અચલ DataHeader *
(U_CALLCONV * LookupFn)(અચલ UDataMemory *pData,
                        અચલ અક્ષર *tocEntryName,
                        int32_t *pLength,
                        UErrorCode *pErrorCode);

પ્રકારવ્યાખ્યા uint32_t
(U_CALLCONV * અંક_Entries_Fn)(અચલ UDataMemory *pData);

U_CDECL_END

પ્રકારવ્યાખ્યા સંરચના {
    LookupFn      Lookup;
    અંક_Entries_Fn  અંક_Entries; 
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
U_CFUNC શૂન્ય udata_checkCommonData(UDataMemory *pData, UErrorCode *pErrorCode);

#endif
