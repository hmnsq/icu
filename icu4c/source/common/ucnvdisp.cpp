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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_CONVERSION

#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/ures.h"
#অন্তর্ভাব "unicode/ucnv.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "ustr_imp.h"
#অন্তর্ভাব "ucnv_imp.h"
#অন্তর্ভাব "putilimp.h"

U_CAPI int32_t U_EXPORT2
ucnv_getDisplayName(ধ্রুবক UConverter *cnv,
                    ধ্রুবক অক্ষর *displayLocale,
                    char16_t *displayName, int32_t displayNameCapacity,
                    UErrorCode *pErrorCode) {
    UResourceBundle *rb;
    ধ্রুবক char16_t *name;
    int32_t length;
    UErrorCode localStatus = U_ZERO_ERROR;

    /* check arguments */
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    যদি(cnv==nullptr || displayNameCapacity<0 || (displayNameCapacity>0 && displayName==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    /* open the resource bundle and get the display name string */
    rb=ures_open(nullptr, displayLocale, pErrorCode);
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    /* use the internal name as the key */
    name=ures_get_String_By_চাবি(rb, cnv->shared_তথ্য->static_তথ্য->name, &length, &localStatus);
    ures_close(rb);

    যদি(U_SUCCESS(localStatus)) {
        /* copy the string */
        যদি (*pErrorCode == U_ZERO_ERROR) {
            *pErrorCode = localStatus;
        }
        u_memcpy(displayName, name, uprv_min(length, displayNameCapacity)*U_SIZEOF_UCHAR);
    } অন্যথা {
        /* convert the internal name into a Unicode string */
        length=(int32_t)uprv_strlen(cnv->shared_তথ্য->static_তথ্য->name);
        u_charsToUChars(cnv->shared_তথ্য->static_তথ্য->name, displayName, uprv_min(length, displayNameCapacity));
    }
    ফেরত u_terminateUChars(displayName, displayNameCapacity, length, pErrorCode);
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
