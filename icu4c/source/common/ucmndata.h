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

#که_نه_تعریف_شوی __UCMNDATA_H__
#تعریف_کړه __UCMNDATA_H__

#ضمن "unicode/udata.h"
#ضمن "umapfile.h"


#تعریف_کړه COMMON_معلومات_NAME U_ICUDATA_NAME

تعريفنوع بنية  {
    uint16_t    سرلیک_اندازه;
    uint8_t     magic1;
    uint8_t     magic2;
} Mapped_معلومات;


تعريفنوع بنية  {
    Mapped_معلومات  معلومات_سرلیک_2;
    UDataInfo   info;
} معلومات_سرلیک;

تعريفنوع بنية {
    uint32_t name_دننه_واټن;
    uint32_t معلومات_دننه_واټن;
} UData_دننه_واټن_TOCEntry;

تعريفنوع بنية {
    uint32_t شمېر;
    /**
     * Variable-length array declared with length 1 to disable bounds checkers.
     * The actual array length is in the count field.
     */
    UData_دننه_واټن_TOCEntry ثبت[1];
} UData_دننه_واټن_TOC;

/**
 * Get the header size from a const DataHeader *udh.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_get_سرلیک_اندازه(ثابت معلومات_سرلیک *udh);

/**
 * Get the UDataInfo.size from a const UDataInfo *info.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_get_Info_اندازه(ثابت UDataInfo *info);

U_CDECL_BEGIN
/*
 *  "Virtual" functions for data lookup.
 *  To call one, given a UDataMemory *p, the code looks like this:
 *     p->vFuncs.Lookup(p, tocEntryName, pErrorCode);
 *          (I sure do wish this was written in C++, not C)
 */

تعريفنوع ثابت معلومات_سرلیک *
(U_CALLCONV * LookupFn)(ثابت UData_حافظه *p_معلومات,
                        ثابت حرف *toc_ثبت_Name,
                        int32_t *p_اوږدتیا,
                        UError_کوډ *p_Error_کوډ);

تعريفنوع uint32_t
(U_CALLCONV * عدد_Entries_Fn)(ثابت UData_حافظه *p_معلومات);

U_CDECL_پای_ټکی

تعريفنوع بنية {
    LookupFn      Lookup;
    عدد_Entries_Fn  عدد_Entries; 
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
U_CFUNC فراغ udata_check_Common_معلومات(UData_حافظه *p_معلومات, UError_کوډ *p_Error_کوډ);

#د_که_پای
