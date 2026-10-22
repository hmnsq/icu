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

#اگر_تعریف_نشده __UCMNDATA_H__
#تعریف_کن __UCMNDATA_H__

#ضمن "unicode/udata.h"
#ضمن "umapfile.h"


#تعریف_کن COMMON_داده_NAME U_ICUDATA_NAME

تعريفنوع بنية  {
    uint16_t    سرآیند_اندازه;
    uint8_t     magic1;
    uint8_t     magic2;
} Mapped_داده;


تعريفنوع بنية  {
    Mapped_داده  داده_سرآیند_2;
    UDataInfo   info;
} داده_سرآیند;

تعريفنوع بنية {
    uint32_t name_جابه_جایی;
    uint32_t داده_جابه_جایی;
} UData_جابه_جایی_TOCEntry;

تعريفنوع بنية {
    uint32_t شمار;
    /**
     * Variable-length array declared with length 1 to disable bounds checkers.
     * The actual array length is in the count field.
     */
    UData_جابه_جایی_TOCEntry مدخل[1];
} UData_جابه_جایی_TOC;

/**
 * Get the header size from a const DataHeader *udh.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_get_سرآیند_اندازه(ثابت داده_سرآیند *udh);

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

تعريفنوع ثابت داده_سرآیند *
(U_CALLCONV * LookupFn)(ثابت UData_حافظه *p_داده,
                        ثابت حرف *toc_مدخل_Name,
                        int32_t *p_درازی,
                        UError_کد *p_Error_کد);

تعريفنوع uint32_t
(U_CALLCONV * رقم_Entries_Fn)(ثابت UData_حافظه *p_داده);

U_CDECL_انتها

تعريفنوع بنية {
    LookupFn      Lookup;
    رقم_Entries_Fn  رقم_Entries; 
} common_داده_Funcs;


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
U_CFUNC فراغ udata_check_Common_داده(UData_حافظه *p_داده, UError_کد *p_Error_کد);

#پایان_اگر
