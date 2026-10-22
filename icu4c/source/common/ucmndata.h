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

#جے_بنا_تعریف __UCMNDATA_H__
#تعریف_کرو __UCMNDATA_H__

#ضمن "unicode/udata.h"
#ضمن "umapfile.h"


#تعریف_کرو COMMON_انکڑے_NAME U_ICUDATA_NAME

تعريفنوع بنية  {
    uint16_t    سرخی_ناپ;
    uint8_t     magic1;
    uint8_t     magic2;
} Mapped_انکڑے;


تعريفنوع بنية  {
    Mapped_انکڑے  انکڑے_سرخی_2;
    UDataInfo   info;
} انکڑے_سرخی;

تعريفنوع بنية {
    uint32_t name_وتھ;
    uint32_t انکڑے_وتھ;
} UData_وتھ_TOCEntry;

تعريفنوع بنية {
    uint32_t گنتی;
    /**
     * Variable-length array declared with length 1 to disable bounds checkers.
     * The actual array length is in the count field.
     */
    UData_وتھ_TOCEntry اندراج[1];
} UData_وتھ_TOC;

/**
 * Get the header size from a const DataHeader *udh.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_get_سرخی_ناپ(ثابت انکڑے_سرخی *udh);

/**
 * Get the UDataInfo.size from a const UDataInfo *info.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_get_Info_ناپ(ثابت UDataInfo *info);

U_CDECL_BEGIN
/*
 *  "Virtual" functions for data lookup.
 *  To call one, given a UDataMemory *p, the code looks like this:
 *     p->vFuncs.Lookup(p, tocEntryName, pErrorCode);
 *          (I sure do wish this was written in C++, not C)
 */

تعريفنوع ثابت انکڑے_سرخی *
(U_CALLCONV * LookupFn)(ثابت UData_حافظہ *p_انکڑے,
                        ثابت حرف *toc_اندراج_Name,
                        int32_t *p_لمائی,
                        UError_کوڈ *p_Error_کوڈ);

تعريفنوع uint32_t
(U_CALLCONV * عدد_Entries_Fn)(ثابت UData_حافظہ *p_انکڑے);

U_CDECL_سرا

تعريفنوع بنية {
    LookupFn      Lookup;
    عدد_Entries_Fn  عدد_Entries; 
} common_انکڑے_Funcs;


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
U_CFUNC فراغ udata_check_Common_انکڑے(UData_حافظہ *p_انکڑے, UError_کوڈ *p_Error_کوڈ);

#جے_انت
