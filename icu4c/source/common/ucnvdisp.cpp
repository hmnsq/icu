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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_CONVERSION

#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/ures.h"
#അന്തര്ഭാവ "unicode/ucnv.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "ustr_imp.h"
#അന്തര്ഭാവ "ucnv_imp.h"
#അന്തര്ഭാവ "putilimp.h"

U_CAPI int32_t U_EXPORT2
ucnv_getDisplayName(സ്ഥിരം UConverter *cnv,
                    സ്ഥിരം അക്ഷരം *displayLocale,
                    char16_t *displayName, int32_t displayNameCapacity,
                    UErrorCode *pErrorCode) {
    UResourceBundle *rb;
    സ്ഥിരം char16_t *name;
    int32_t length;
    UErrorCode localStatus = U_ZERO_ERROR;

    /* check arguments */
    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }

    എങ്കിൽ(cnv==nullptr || displayNameCapacity<0 || (displayNameCapacity>0 && displayName==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    /* open the resource bundle and get the display name string */
    rb=ures_open(nullptr, displayLocale, pErrorCode);
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }

    /* use the internal name as the key */
    name=ures_get_String_By_താക്കോൽ(rb, cnv->sharedData->staticData->name, &length, &localStatus);
    ures_close(rb);

    എങ്കിൽ(U_SUCCESS(localStatus)) {
        /* copy the string */
        എങ്കിൽ (*pErrorCode == U_ZERO_ERROR) {
            *pErrorCode = localStatus;
        }
        u_memcpy(displayName, name, uprv_min(length, displayNameCapacity)*U_SIZEOF_UCHAR);
    } അല്ലെങ്കിൽ {
        /* convert the internal name into a Unicode string */
        length=(int32_t)uprv_strlen(cnv->sharedData->staticData->name);
        u_charsToUChars(cnv->sharedData->staticData->name, displayName, uprv_min(length, displayNameCapacity));
    }
    തിരികെ u_terminateUChars(displayName, displayNameCapacity, length, pErrorCode);
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
