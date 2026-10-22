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

#यदि_अपरिभाषितम् __UCMNDATA_H__
#परिभाषय __UCMNDATA_H__

#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "umapfile.h"


#परिभाषय COMMON_दत्त_NAME U_ICUDATA_NAME

प्रकारनिर्देश संरचना  {
    uint16_t    शीर्षकम्_आकार;
    uint8_t     magic1;
    uint8_t     magic2;
} Mapped_दत्त;


प्रकारनिर्देश संरचना  {
    Mapped_दत्त  दत्त_शीर्षकम्_2;
    UDataInfo   info;
} दत्त_शीर्षकम्;

प्रकारनिर्देश संरचना {
    uint32_t name_विस्थापनम्;
    uint32_t दत्त_विस्थापनम्;
} UData_विस्थापनम्_TOCEntry;

प्रकारनिर्देश संरचना {
    uint32_t गणनम्;
    /**
     * Variable-length array declared with length 1 to disable bounds checkers.
     * The actual array length is in the count field.
     */
    UData_विस्थापनम्_TOCEntry प्रविष्टिः[1];
} UData_विस्थापनम्_TOC;

/**
 * Get the header size from a const DataHeader *udh.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_get_शीर्षकम्_आकार(ध्रुव दत्त_शीर्षकम् *udh);

/**
 * Get the UDataInfo.size from a const UDataInfo *info.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_get_Info_आकार(ध्रुव UDataInfo *info);

U_CDECL_BEGIN
/*
 *  "Virtual" functions for data lookup.
 *  To call one, given a UDataMemory *p, the code looks like this:
 *     p->vFuncs.Lookup(p, tocEntryName, pErrorCode);
 *          (I sure do wish this was written in C++, not C)
 */

प्रकारनिर्देश ध्रुव दत्त_शीर्षकम् *
(U_CALLCONV * LookupFn)(ध्रुव UData_स्मृतिः *p_दत्त,
                        ध्रुव वर्ण *toc_प्रविष्टिः_Name,
                        int32_t *p_दीर्घता,
                        UError_संहिता *p_Error_संहिता);

प्रकारनिर्देश uint32_t
(U_CALLCONV * अङ्कः_Entries_Fn)(ध्रुव UData_स्मृतिः *p_दत्त);

U_CDECL_अन्तः

प्रकारनिर्देश संरचना {
    LookupFn      Lookup;
    अङ्कः_Entries_Fn  अङ्कः_Entries; 
} common_दत्त_Funcs;


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
U_CFUNC शून्य udata_check_Common_दत्त(UData_स्मृतिः *p_दत्त, UError_संहिता *p_Error_संहिता);

#यदि_अन्तः
