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

#جيڪڏهن_وصف_نه_ٿيل __UCMNDATA_H__
#وصف_ڪريو __UCMNDATA_H__

#ضمن "unicode/udata.h"
#ضمن "umapfile.h"


#وصف_ڪريو COMMON_معلومات_NAME U_ICUDATA_NAME

تعريفنوع بنية  {
    uint16_t    سرو_سائيز;
    uint8_t     magic1;
    uint8_t     magic2;
} Mapped_معلومات;


تعريفنوع بنية  {
    Mapped_معلومات  معلومات_سرو_2;
    UDataInfo   info;
} معلومات_سرو;

تعريفنوع بنية {
    uint32_t name_وٿي;
    uint32_t معلومات_وٿي;
} UData_وٿي_TOCEntry;

تعريفنوع بنية {
    uint32_t ڳڻپ;
    /**
     * Variable-length array declared with length 1 to disable bounds checkers.
     * The actual array length is in the count field.
     */
    UData_وٿي_TOCEntry داخلا_2[1];
} UData_وٿي_TOC;

/**
 * Get the header size from a const DataHeader *udh.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_get_سرو_سائيز(ثابت معلومات_سرو *udh);

/**
 * Get the UDataInfo.size from a const UDataInfo *info.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_get_Info_سائيز(ثابت UDataInfo *info);

U_CDECL_BEGIN
/*
 *  "Virtual" functions for data lookup.
 *  To call one, given a UDataMemory *p, the code looks like this:
 *     p->vFuncs.Lookup(p, tocEntryName, pErrorCode);
 *          (I sure do wish this was written in C++, not C)
 */

تعريفنوع ثابت معلومات_سرو *
(U_CALLCONV * LookupFn)(ثابت UData_يادگيري *p_معلومات,
                        ثابت حرف *toc_داخلا_Name,
                        int32_t *p_ڊيگهائي,
                        UError_ڪوڊ *p_Error_ڪوڊ);

تعريفنوع uint32_t
(U_CALLCONV * انگ_Entries_Fn)(ثابت UData_يادگيري *p_معلومات);

U_CDECL_پڇاڙي

تعريفنوع بنية {
    LookupFn      Lookup;
    انگ_Entries_Fn  انگ_Entries; 
} common_معلومات_Funcs;


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
U_CFUNC فراغ udata_check_Common_معلومات(UData_يادگيري *p_معلومات, UError_ڪوڊ *p_Error_ڪوڊ);

#جيڪڏهن_پورو
