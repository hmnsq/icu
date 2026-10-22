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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __UCMNDATA_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __UCMNDATA_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umapfile.h"


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COMMON_ਅੰਕੜੇ_NAME U_ICUDATA_NAME

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ  {
    uint16_t    ਸਿਰਲੇਖ_ਮਾਪ;
    uint8_t     magic1;
    uint8_t     magic2;
} Mapped_ਅੰਕੜੇ;


ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ  {
    Mapped_ਅੰਕੜੇ  ਅੰਕੜੇ_ਸਿਰਲੇਖ_2;
    UDataInfo   info;
} ਅੰਕੜੇ_ਸਿਰਲੇਖ;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    uint32_t name_ਵਿੱਥ;
    uint32_t ਅੰਕੜੇ_ਵਿੱਥ;
} UData_ਵਿੱਥ_TOCEntry;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    uint32_t ਗਿਣਤੀ_2;
    /**
     * Variable-length array declared with length 1 to disable bounds checkers.
     * The actual array length is in the count field.
     */
    UData_ਵਿੱਥ_TOCEntry ਇੰਦਰਾਜ[1];
} UData_ਵਿੱਥ_TOC;

/**
 * Get the header size from a const DataHeader *udh.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_get_ਸਿਰਲੇਖ_ਮਾਪ(ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ *udh);

/**
 * Get the UDataInfo.size from a const UDataInfo *info.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_get_Info_ਮਾਪ(ਅਚਲ UDataInfo *info);

U_CDECL_BEGIN
/*
 *  "Virtual" functions for data lookup.
 *  To call one, given a UDataMemory *p, the code looks like this:
 *     p->vFuncs.Lookup(p, tocEntryName, pErrorCode);
 *          (I sure do wish this was written in C++, not C)
 */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ *
(U_CALLCONV * LookupFn)(ਅਚਲ UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ,
                        ਅਚਲ ਅੱਖਰ *toc_ਇੰਦਰਾਜ_Name,
                        int32_t *p_ਲੰਮਾਈ,
                        UError_ਕੋਡ *p_Error_ਕੋਡ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint32_t
(U_CALLCONV * ਅੰਕ_Entries_Fn)(ਅਚਲ UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ);

U_CDECL_ਸਿਰਾ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    LookupFn      Lookup;
    ਅੰਕ_Entries_Fn  ਅੰਕ_Entries; 
} common_ਅੰਕੜੇ_Funcs;


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
U_CFUNC ਖਾਲੀ udata_check_Common_ਅੰਕੜੇ(UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ, UError_ਕੋਡ *p_Error_ਕੋਡ);

#ਜੇ_ਅੰਤ
