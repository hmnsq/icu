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

#اگر_غیر_تعریف_شدہ __UCMNDATA_H__
#تعریف_کرو __UCMNDATA_H__

#شامل "unicode/udata.h"
#شامل "umapfile.h"


#تعریف_کرو COMMON_ڈیٹا_NAME U_ICUDATA_NAME

قسمتعریف ساخت  {
    uint16_t    سرخی_سائز;
    uint8_t     magic1;
    uint8_t     magic2;
} Mapped_ڈیٹا;


قسمتعریف ساخت  {
    Mapped_ڈیٹا  ڈیٹا_سرخی_2;
    UDataInfo   info;
} ڈیٹا_سرخی;

قسمتعریف ساخت {
    uint32_t name_انحراف;
    uint32_t ڈیٹا_انحراف;
} UData_انحراف_TOCEntry;

قسمتعریف ساخت {
    uint32_t گنتی;
    /**
     * Variable-length array declared with length 1 to disable bounds checkers.
     * The actual array length is in the count field.
     */
    UData_انحراف_TOCEntry اندراج_2[1];
} UData_انحراف_TOC;

/**
 * Get the header size from a const DataHeader *udh.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_get_سرخی_سائز(ثابت ڈیٹا_سرخی *udh);

/**
 * Get the UDataInfo.size from a const UDataInfo *info.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_get_Info_سائز(ثابت UDataInfo *info);

U_CDECL_BEGIN
/*
 *  "Virtual" functions for data lookup.
 *  To call one, given a UDataMemory *p, the code looks like this:
 *     p->vFuncs.Lookup(p, tocEntryName, pErrorCode);
 *          (I sure do wish this was written in C++, not C)
 */

قسمتعریف ثابت ڈیٹا_سرخی *
(U_CALLCONV * LookupFn)(ثابت UData_حافظہ *p_ڈیٹا,
                        ثابت حرف *toc_اندراج_Name,
                        int32_t *p_لمبائی,
                        UError_رمز *p_Error_رمز);

قسمتعریف uint32_t
(U_CALLCONV * عدد_Entries_Fn)(ثابت UData_حافظہ *p_ڈیٹا);

U_CDECL_انتہا

قسمتعریف ساخت {
    LookupFn      Lookup;
    عدد_Entries_Fn  عدد_Entries; 
} common_ڈیٹا_Funcs;


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
U_CFUNC خالی udata_check_Common_ڈیٹا(UData_حافظہ *p_ڈیٹا, UError_رمز *p_Error_رمز);

#اگر_ختم
