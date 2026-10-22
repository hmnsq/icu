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

#నిర్వచితం_కాకపోతే __UCMNDATA_H__
#నిర్వచించు __UCMNDATA_H__

#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "umapfile.h"


#నిర్వచించు COMMON_దత్తాంశం_NAME U_ICUDATA_NAME

రకనిర్వచనం నిర్మాణం  {
    uint16_t    శీర్షిక_కొలత;
    uint8_t     magic1;
    uint8_t     magic2;
} Mapped_దత్తాంశం;


రకనిర్వచనం నిర్మాణం  {
    Mapped_దత్తాంశం  దత్తాంశం_శీర్షిక_2;
    UDataInfo   info;
} దత్తాంశం_శీర్షిక;

రకనిర్వచనం నిర్మాణం {
    uint32_t name_స్థానభ్రంశం;
    uint32_t దత్తాంశం_స్థానభ్రంశం;
} UData_స్థానభ్రంశం_TOCEntry;

రకనిర్వచనం నిర్మాణం {
    uint32_t లెక్క;
    /**
     * Variable-length array declared with length 1 to disable bounds checkers.
     * The actual array length is in the count field.
     */
    UData_స్థానభ్రంశం_TOCEntry నమోదు_2[1];
} UData_స్థానభ్రంశం_TOC;

/**
 * Get the header size from a const DataHeader *udh.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_get_శీర్షిక_కొలత(స్థిరరాశి దత్తాంశం_శీర్షిక *udh);

/**
 * Get the UDataInfo.size from a const UDataInfo *info.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_get_Info_కొలత(స్థిరరాశి UDataInfo *info);

U_CDECL_BEGIN
/*
 *  "Virtual" functions for data lookup.
 *  To call one, given a UDataMemory *p, the code looks like this:
 *     p->vFuncs.Lookup(p, tocEntryName, pErrorCode);
 *          (I sure do wish this was written in C++, not C)
 */

రకనిర్వచనం స్థిరరాశి దత్తాంశం_శీర్షిక *
(U_CALLCONV * LookupFn)(స్థిరరాశి UData_జ్ఞాపకం *p_దత్తాంశం,
                        స్థిరరాశి అక్షరం *toc_నమోదు_Name,
                        int32_t *p_నిడివి,
                        UError_సంకేతం *p_Error_సంకేతం);

రకనిర్వచనం uint32_t
(U_CALLCONV * అంకె_Entries_Fn)(స్థిరరాశి UData_జ్ఞాపకం *p_దత్తాంశం);

U_CDECL_చివర

రకనిర్వచనం నిర్మాణం {
    LookupFn      Lookup;
    అంకె_Entries_Fn  అంకె_Entries; 
} common_దత్తాంశం_Funcs;


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
U_CFUNC శూన్య udata_check_Common_దత్తాంశం(UData_జ్ఞాపకం *p_దత్తాంశం, UError_సంకేతం *p_Error_సంకేతం);

#అయితే_ముగింపు
