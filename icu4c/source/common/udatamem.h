// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1999-2010, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************/


/*----------------------------------------------------------------------------------
 *
 *  UDataMemory     A class-like struct that serves as a handle to a piece of memory
 *                  that contains some ICU data (resource, converters, whatever.)
 *
 *                  When an application opens ICU data (with udata_open, for example,
 *                  a UDataMemory * is returned.
 *
 *----------------------------------------------------------------------------------*/
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __UDATAMEM_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __UDATAMEM_H__

#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "ucmndata.h"

ರಚನೆ UData_ಸ್ಮರಣೆ {
    ಸ್ಥಿರಾಂಕ common_ದತ್ತಾಂಶ_Funcs  *vFuncs;      /* Function Pointers for accessing TOC             */

    ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *p_ಶೀರ್ಷಿಕೆ;     /* Header of the memory being described by this    */
                                   /*   UDataMemory object.                           */
    ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ       *toc;         /* For common memory, table of contents for        */
                                   /*   the pieces within.                            */
    UBool             ರಾಶಿ_Allocated;  /* True if this UDataMemory Object is on the    */
                                   /*  heap and thus needs to be deleted when closed. */

    ಶೂನ್ಯ             *mapAddr;     /* For mapped or allocated memory, the start addr. */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data.                           */
    ಶೂನ್ಯ             *map;         /* Handle, or other data, OS dependent.            */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data, and additional info       */
                                   /*   beyond the mapAddr is needed to do that.      */
    int32_t           ಉದ್ದಳತೆ;      /* Length of the data in bytes; -1 if unknown.     */
};

U_CAPI  UData_ಸ್ಮರಣೆ* U_EXPORT2 UData_ಸ್ಮರಣೆ_create_New_Instance(UError_ಸಂಕೇತ *pErr);
U_CFUNC ಶೂನ್ಯ         UDatamemory_ನಿಯೋಜಿಸು  (UData_ಸ್ಮರಣೆ *dest, UData_ಸ್ಮರಣೆ *ಮೂಲ);
U_CFUNC ಶೂನ್ಯ         UData_ಸ್ಮರಣೆ_init    (UData_ಸ್ಮರಣೆ *This);
U_CFUNC UBool        UData_ಸ್ಮರಣೆ_is_Loaded(ಸ್ಥಿರಾಂಕ UData_ಸ್ಮರಣೆ *This);
U_CFUNC ಶೂನ್ಯ         UData_ಸ್ಮರಣೆ_ಗಣ_ದತ್ತಾಂಶ (UData_ಸ್ಮರಣೆ *This, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ_Addr);

U_CFUNC ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ* UData_ಸ್ಮರಣೆ_normalize_ದತ್ತಾಂಶ_ಸೂಚಕ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* p U_LIFETIME_BOUND);

U_CAPI int32_t U_EXPORT2
udata_get_ಉದ್ದಳತೆ(ಸ್ಥಿರಾಂಕ UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ);

U_CAPI ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* U_EXPORT2
udata_get_Raw_ಸ್ಮರಣೆ(ಸ್ಥಿರಾಂಕ UData_ಸ್ಮರಣೆ* p_ದತ್ತಾಂಶ U_LIFETIME_BOUND);

#ಆದರೆ_ಅಂತ್ಯ
