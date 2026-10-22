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
 *  UDataMemory     A class-like struct that serves as a handle to a piece of memory
 *                  that contains some ICU data (resource, converters, whatever.)
 *
 *                  When an application opens ICU data (with udata_open, for example,
 *                  a UDataMemory * is returned.
 *
 *----------------------------------------------------------------------------------*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"

#ಅನ್ತರ್ಭಾವ "udatamem.h"

U_CFUNC ಶೂನ್ಯ UData_ಸ್ಮರಣೆ_init(UData_ಸ್ಮರಣೆ *This) {
    uprv_memset(This, 0, ಗಾತ್ರ(UData_ಸ್ಮರಣೆ));
    This->length=-1;
}


U_CFUNC ಶೂನ್ಯ UDatamemory_ನಿಯೋಜಿಸು(UData_ಸ್ಮರಣೆ *dest, UData_ಸ್ಮರಣೆ *ಮೂಲ) {
    /* UDataMemory Assignment.  Destination UDataMemory must be initialized first.  */
    UBool mallocedFlag = dest->ರಾಶಿ_Allocated;
    uprv_memcpy(dest, ಮೂಲ, ಗಾತ್ರ(UData_ಸ್ಮರಣೆ));
    dest->ರಾಶಿ_Allocated = mallocedFlag;
}

U_CFUNC UData_ಸ್ಮರಣೆ *UData_ಸ್ಮರಣೆ_create_New_Instance(UError_ಸಂಕೇತ *pErr) {
    UData_ಸ್ಮರಣೆ *This;

    ಆದರೆ (U_FAILURE(*pErr)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    This = (UData_ಸ್ಮರಣೆ *)uprv_malloc(ಗಾತ್ರ(UData_ಸ್ಮರಣೆ));
    ಆದರೆ (This == ಶೂನ್ಯಸೂಚಕ) {
        *pErr = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR; }
    ಇಲ್ಲದಿದ್ದರೆ {
        UData_ಸ್ಮರಣೆ_init(This);
        This->ರಾಶಿ_Allocated = ನಿಜ;
    }
    ಹಿಂತಿರುಗಿ This;
}

U_CFUNC ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ*
UData_ಸ್ಮರಣೆ_normalize_ದತ್ತಾಂಶ_ಸೂಚಕ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* p U_LIFETIME_BOUND) {
    /* allow the data to be optionally prepended with an alignment-forcing double value */
    ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *pdh = (ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *)p;
    ಆದರೆ(pdh==ಶೂನ್ಯಸೂಚಕ || (pdh->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.magic1==0xda && pdh->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.magic2==0x27)) {
        ಹಿಂತಿರುಗಿ pdh;
    } ಇಲ್ಲದಿದ್ದರೆ {
#ಆದರೆ U_PLATFORM == U_PF_OS400
        /*
        TODO: Fix this once the compiler implements this feature. Keep in sync with genccode.c

        This is here because this platform can't currently put
        const data into the read-only pages of an object or
        shared library (service program). Only strings are allowed in read-only
        pages, so we use char * strings to store the data.

        In order to prevent the beginning of the data from ever matching the
        magic numbers we must skip the initial double.
        [grhoten 4/24/2003]
        */
        ಹಿಂತಿರುಗಿ (ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *)*((ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ **)p+1);
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ (ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *)((ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ *)p+1);
#ಆದರೆ_ಅಂತ್ಯ
    }
}


U_CFUNC ಶೂನ್ಯ UData_ಸ್ಮರಣೆ_ಗಣ_ದತ್ತಾಂಶ (UData_ಸ್ಮರಣೆ *This, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ_Addr) {
    This->p_ಶೀರ್ಷಿಕೆ = UData_ಸ್ಮರಣೆ_normalize_ದತ್ತಾಂಶ_ಸೂಚಕ(ದತ್ತಾಂಶ_Addr);
}


U_CAPI ಶೂನ್ಯ U_EXPORT2
udata_close(UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ) {
    ಆದರೆ(p_ದತ್ತಾಂಶ!=ಶೂನ್ಯಸೂಚಕ) {
        uprv_unmapFile(p_ದತ್ತಾಂಶ);
        ಆದರೆ(p_ದತ್ತಾಂಶ->ರಾಶಿ_Allocated ) {
            uprv_free(p_ದತ್ತಾಂಶ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            UData_ಸ್ಮರಣೆ_init(p_ದತ್ತಾಂಶ);
        }
    }
}

U_CAPI ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* U_EXPORT2
udata_get_ಸ್ಮರಣೆ(UData_ಸ್ಮರಣೆ* p_ದತ್ತಾಂಶ U_LIFETIME_BOUND) {
    ಆದರೆ(p_ದತ್ತಾಂಶ!=ಶೂನ್ಯಸೂಚಕ && p_ದತ್ತಾಂಶ->p_ಶೀರ್ಷಿಕೆ!=ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ *)(p_ದತ್ತಾಂಶ->p_ಶೀರ್ಷಿಕೆ)+udata_get_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ(p_ದತ್ತಾಂಶ->p_ಶೀರ್ಷಿಕೆ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
}

/**
 * Get the length of the data item if possible.
 * The length may be up to 15 bytes larger than the actual data.
 *
 * TODO Consider making this function public.
 * It would have to return the actual length in more cases.
 * For example, the length of the last item in a .dat package could be
 * computed from the size of the whole .dat package minus the offset of the
 * last item.
 * The size of a file that was directly memory-mapped could be determined
 * using some system API.
 *
 * In order to get perfect values for all data items, we may have to add a
 * length field to UDataInfo, but that complicates data generation
 * and may be overkill.
 *
 * @param pData The data item.
 * @return the length of the data item, or -1 if not known
 * @internal Currently used only in cintltst/udatatst.c
 */
U_CAPI int32_t U_EXPORT2
udata_get_ಉದ್ದಳತೆ(ಸ್ಥಿರಾಂಕ UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ) {
    ಆದರೆ(p_ದತ್ತಾಂಶ!=ಶೂನ್ಯಸೂಚಕ && p_ದತ್ತಾಂಶ->p_ಶೀರ್ಷಿಕೆ!=ಶೂನ್ಯಸೂಚಕ && p_ದತ್ತಾಂಶ->length>=0) {
        /*
         * subtract the header size,
         * return only the size of the actual data starting at udata_getMemory()
         */
        ಹಿಂತಿರುಗಿ p_ದತ್ತಾಂಶ->length-udata_get_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ(p_ದತ್ತಾಂಶ->p_ಶೀರ್ಷಿಕೆ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ -1;
    }
}

/**
 * Get the memory including the data header.
 * Used in cintltst/udatatst.c
 * @internal
 */
U_CAPI ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* U_EXPORT2
udata_get_Raw_ಸ್ಮರಣೆ(ಸ್ಥಿರಾಂಕ UData_ಸ್ಮರಣೆ* p_ದತ್ತಾಂಶ U_LIFETIME_BOUND) {
    ಆದರೆ(p_ದತ್ತಾಂಶ!=ಶೂನ್ಯಸೂಚಕ && p_ದತ್ತಾಂಶ->p_ಶೀರ್ಷಿಕೆ!=ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ p_ದತ್ತಾಂಶ->p_ಶೀರ್ಷಿಕೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
}

U_CFUNC UBool UData_ಸ್ಮರಣೆ_is_Loaded(ಸ್ಥಿರಾಂಕ UData_ಸ್ಮರಣೆ *This) {
    ಹಿಂತಿರುಗಿ This->p_ಶೀರ್ಷಿಕೆ != ಶೂನ್ಯಸೂಚಕ;
}
