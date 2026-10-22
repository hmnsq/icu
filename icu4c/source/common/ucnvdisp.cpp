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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION

#समावेश "unicode/ustring.h"
#समावेश "unicode/ures.h"
#समावेश "unicode/ucnv.h"
#समावेश "cstring.h"
#समावेश "ustr_imp.h"
#समावेश "ucnv_imp.h"
#समावेश "putilimp.h"

U_CAPI int32_t U_EXPORT2
ucnv_getDisplayName(स्थिर UConverter *cnv,
                    स्थिर अक्षर *displayLocale,
                    char16_t *displayName, int32_t displayNameCapacity,
                    UErrorCode *pErrorCode) {
    UResourceBundle *rb;
    स्थिर char16_t *name;
    int32_t length;
    UErrorCode localStatus = U_ZERO_ERROR;

    /* check arguments */
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    अगर(cnv==nullptr || displayNameCapacity<0 || (displayNameCapacity>0 && displayName==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* open the resource bundle and get the display name string */
    rb=ures_open(nullptr, displayLocale, pErrorCode);
    अगर(U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    /* use the internal name as the key */
    name=ures_get_String_By_कुंजी(rb, cnv->shared_तथ्य->static_तथ्य->name, &length, &localStatus);
    ures_close(rb);

    अगर(U_SUCCESS(localStatus)) {
        /* copy the string */
        अगर (*pErrorCode == U_ZERO_ERROR) {
            *pErrorCode = localStatus;
        }
        u_memcpy(displayName, name, uprv_min(length, displayNameCapacity)*U_SIZEOF_UCHAR);
    } अन्यथा {
        /* convert the internal name into a Unicode string */
        length=(int32_t)uprv_strlen(cnv->shared_तथ्य->static_तथ्य->name);
        u_charsToUChars(cnv->shared_तथ्य->static_तथ्य->name, displayName, uprv_min(length, displayNameCapacity));
    }
    वापस u_terminateUChars(displayName, displayNameCapacity, length, pErrorCode);
}

#पूर्ण_अगर

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
