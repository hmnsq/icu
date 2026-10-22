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

#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "umapfile.h"


#define COMMON_DATA_NAME U_ICUDATA_NAME

प्रकारनिर्देश संरचना  {
    uint16_t    headerSize;
    uint8_t     magic1;
    uint8_t     magic2;
} MappedData;


प्रकारनिर्देश संरचना  {
    MappedData  dataHeader;
    UDataInfo   info;
} DataHeader;

प्रकारनिर्देश संरचना {
    uint32_t nameOffset;
    uint32_t dataOffset;
} UDataOffsetTOCEntry;

प्रकारनिर्देश संरचना {
    uint32_t मोजणी;
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
udata_getHeaderSize(ध्रुव DataHeader *udh);

/**
 * Get the UDataInfo.size from a const UDataInfo *info.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_getInfoSize(ध्रुव UDataInfo *info);

U_CDECL_BEGIN
/*
 *  "Virtual" functions for data lookup.
 *  To call one, given a UDataMemory *p, the code looks like this:
 *     p->vFuncs.Lookup(p, tocEntryName, pErrorCode);
 *          (I sure do wish this was written in C++, not C)
 */

प्रकारनिर्देश ध्रुव DataHeader *
(U_CALLCONV * LookupFn)(ध्रुव UDataMemory *pData,
                        ध्रुव वर्ण *tocEntryName,
                        int32_t *pLength,
                        UErrorCode *pErrorCode);

प्रकारनिर्देश uint32_t
(U_CALLCONV * अंक_Entries_Fn)(ध्रुव UDataMemory *pData);

U_CDECL_END

प्रकारनिर्देश संरचना {
    LookupFn      Lookup;
    अंक_Entries_Fn  अंक_Entries; 
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
U_CFUNC शून्य udata_checkCommonData(UDataMemory *pData, UErrorCode *pErrorCode);

#endif
