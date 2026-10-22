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

#अगर_अघोषित __UCMNDATA_H__
#घोषणा __UCMNDATA_H__

#समावेश "unicode/udata.h"
#समावेश "umapfile.h"


#घोषणा COMMON_तथ्य_NAME U_ICUDATA_NAME

प्रकार काष्ठा  {
    uint16_t    header_आकार;
    uint8_t     magic1;
    uint8_t     magic2;
} Mapped_तथ्य;


प्रकार काष्ठा  {
    Mapped_तथ्य  तथ्य_Header_2;
    UDataInfo   info;
} तथ्य_Header;

प्रकार काष्ठा {
    uint32_t nameOffset;
    uint32_t तथ्य_Offset;
} UDataOffsetTOCEntry;

प्रकार काष्ठा {
    uint32_t गिनती;
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
udata_get_Header_आकार(स्थिर तथ्य_Header *udh);

/**
 * Get the UDataInfo.size from a const UDataInfo *info.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_get_Info_आकार(स्थिर UDataInfo *info);

U_CDECL_BEGIN
/*
 *  "Virtual" functions for data lookup.
 *  To call one, given a UDataMemory *p, the code looks like this:
 *     p->vFuncs.Lookup(p, tocEntryName, pErrorCode);
 *          (I sure do wish this was written in C++, not C)
 */

प्रकार स्थिर तथ्य_Header *
(U_CALLCONV * LookupFn)(स्थिर UDataMemory *p_तथ्य,
                        स्थिर अक्षर *tocEntryName,
                        int32_t *p_लंबाई,
                        UErrorCode *pErrorCode);

प्रकार uint32_t
(U_CALLCONV * अंक_Entries_Fn)(स्थिर UDataMemory *p_तथ्य);

U_CDECL_END

प्रकार काष्ठा {
    LookupFn      Lookup;
    अंक_Entries_Fn  अंक_Entries; 
} common_तथ्य_Funcs;


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
U_CFUNC व्योम udata_check_Common_तथ्य(UDataMemory *p_तथ्य, UErrorCode *pErrorCode);

#पूर्ण_अगर
