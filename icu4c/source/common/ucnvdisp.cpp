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

#ضمن "unicode/utypes.h"

#𐤀𐤌 !UCONFIG_NO_CONVERSION

#ضمن "unicode/ustring.h"
#ضمن "unicode/ures.h"
#ضمن "unicode/ucnv.h"
#ضمن "cstring.h"
#ضمن "ustr_imp.h"
#ضمن "ucnv_imp.h"
#ضمن "putilimp.h"

U_CAPI int32_t U_EXPORT2
ucnv_getDisplayName(ثابت UConverter *cnv,
                    ثابت حرف *displayLocale,
                    char16_t *displayName, int32_t displayNameCapacity,
                    UErrorCode *pErrorCode) {
    UResourceBundle *rb;
    ثابت char16_t *name;
    int32_t length;
    UErrorCode localStatus = U_ZERO_ERROR;

    /* check arguments */
    𐤀𐤌(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }

    𐤀𐤌(cnv==nullptr || displayNameCapacity<0 || (displayNameCapacity>0 && displayName==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    /* open the resource bundle and get the display name string */
    rb=ures_open(nullptr, displayLocale, pErrorCode);
    𐤀𐤌(U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }

    /* use the internal name as the key */
    name=ures_getStringByKey(rb, cnv->sharedData->staticData->name, &length, &localStatus);
    ures_close(rb);

    𐤀𐤌(U_SUCCESS(localStatus)) {
        /* copy the string */
        𐤀𐤌 (*pErrorCode == U_ZERO_ERROR) {
            *pErrorCode = localStatus;
        }
        u_memcpy(displayName, name, uprv_min(length, displayNameCapacity)*U_SIZEOF_UCHAR);
    } وإلا {
        /* convert the internal name into a Unicode string */
        length=(int32_t)uprv_strlen(cnv->sharedData->staticData->name);
        u_charsToUChars(cnv->sharedData->staticData->name, displayName, uprv_min(length, displayNameCapacity));
    }
    أرجع u_terminateUChars(displayName, displayNameCapacity, length, pErrorCode);
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
