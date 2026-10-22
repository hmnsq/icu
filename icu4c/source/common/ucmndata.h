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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __UCMNDATA_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __UCMNDATA_H__

#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "umapfile.h"


#ವ್ಯಾಖ್ಯಾನಿಸು COMMON_ದತ್ತಾಂಶ_NAME U_ICUDATA_NAME

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ  {
    uint16_t    ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
    uint8_t     magic1;
    uint8_t     magic2;
} Mapped_ದತ್ತಾಂಶ;


ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ  {
    Mapped_ದತ್ತಾಂಶ  ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2;
    UDataInfo   info;
} ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    uint32_t name_ಸ್ಥಾನಾಂತರ;
    uint32_t ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ;
} UData_ಸ್ಥಾನಾಂತರ_TOCEntry;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    uint32_t ಎಣಿಕೆ_2;
    /**
     * Variable-length array declared with length 1 to disable bounds checkers.
     * The actual array length is in the count field.
     */
    UData_ಸ್ಥಾನಾಂತರ_TOCEntry ನಮೂದು[1];
} UData_ಸ್ಥಾನಾಂತರ_TOC;

/**
 * Get the header size from a const DataHeader *udh.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_get_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *udh);

/**
 * Get the UDataInfo.size from a const UDataInfo *info.
 * Handles opposite-endian data.
 *
 * @internal
 */
U_CFUNC uint16_t
udata_get_Info_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ UDataInfo *info);

U_CDECL_BEGIN
/*
 *  "Virtual" functions for data lookup.
 *  To call one, given a UDataMemory *p, the code looks like this:
 *     p->vFuncs.Lookup(p, tocEntryName, pErrorCode);
 *          (I sure do wish this was written in C++, not C)
 */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *
(U_CALLCONV * LookupFn)(ಸ್ಥಿರಾಂಕ UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *toc_ನಮೂದು_Name,
                        int32_t *p_ಉದ್ದಳತೆ,
                        UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint32_t
(U_CALLCONV * ಅಂಕೆ_Entries_Fn)(ಸ್ಥಿರಾಂಕ UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ);

U_CDECL_ಕೊನೆ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    LookupFn      Lookup;
    ಅಂಕೆ_Entries_Fn  ಅಂಕೆ_Entries; 
} common_ದತ್ತಾಂಶ_Funcs;


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
U_CFUNC ಶೂನ್ಯ udata_check_Common_ದತ್ತಾಂಶ(UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

#ಆದರೆ_ಅಂತ್ಯ
