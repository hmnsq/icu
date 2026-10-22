// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1998-2004, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
*  ucnvdisp.c:
*  Implements APIs for the ICU's codeset conversion library display names.
*
* Modification History:
*
*   Date        Name        Description
*   04/04/99    helena      Fixed internal header inclusion.
*   05/09/00    helena      Added implementation to handle fallback mappings.
*   06/20/2000  helena      OS/400 port changes; mostly typecast.
*   09/08/2004  grhoten     split from ucnv.c
*/

#अन्तर्भाव "unicode/utypes.h"

#जर !UCONFIG_NO_CONVERSION

#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "ucnv_imp.h"
#अन्तर्भाव "putilimp.h"

U_CAPI int32_t U_EXPORT2
ucnv_getDisplayName(स्थिरांक UConverter *cnv,
                    स्थिरांक अक्षर *displayLocale,
                    char16_t *displayName, int32_t displayNameCapacity,
                    UErrorCode *pErrorCode) {
    UResourceBundle *rb;
    स्थिरांक char16_t *name;
    int32_t length;
    UErrorCode localStatus = U_ZERO_ERROR;

    /* check arguments */
    जर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        परत 0;
    }

    जर(cnv==nullptr || displayNameCapacity<0 || (displayNameCapacity>0 && displayName==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत 0;
    }

    /* open the resource bundle and get the display name string */
    rb=ures_open(nullptr, displayLocale, pErrorCode);
    जर(U_FAILURE(*pErrorCode)) {
        परत 0;
    }

    /* use the internal name as the key */
    name=ures_get_String_By_किल्ली(rb, cnv->sharedData->staticData->name, &length, &localStatus);
    ures_close(rb);

    जर(U_SUCCESS(localStatus)) {
        /* copy the string */
        जर (*pErrorCode == U_ZERO_ERROR) {
            *pErrorCode = localStatus;
        }
        u_memcpy(displayName, name, uprv_min(length, displayNameCapacity)*U_SIZEOF_UCHAR);
    } अन्यथा {
        /* convert the internal name into a Unicode string */
        length=(int32_t)uprv_strlen(cnv->sharedData->staticData->name);
        u_charsToUChars(cnv->sharedData->staticData->name, displayName, uprv_min(length, displayNameCapacity));
    }
    परत u_terminateUChars(displayName, displayNameCapacity, length, pErrorCode);
}

#endif

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
