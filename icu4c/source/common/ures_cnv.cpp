// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 1997-2006, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  ures_cnv.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2004aug25
*   created by: Markus W. Scherer
*
*   Character conversion functions moved here from uresbund.c
*/

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/putil.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/ucnv.h"
#અન્તર્ભાવ "unicode/ures.h"
#અન્તર્ભાવ "uinvchar.h"
#અન્તર્ભાવ "ustr_cnv.h"

U_CAPI UResourceBundle * U_EXPORT2
ures_openU(અચલ char16_t *myPath,
           અચલ અક્ષર *localeID, 
           UErrorCode *status)
{
    અક્ષર pathBuffer[1024];
    int32_t length;
    અક્ષર *path = pathBuffer;

    જો(status==nullptr || U_FAILURE(*status)) {
        પાછા nullptr;
    }
    જો(myPath==nullptr) {
        path = nullptr;
    }
    નહીંતર {
        length=u_strlen(myPath);
        જો(length>=(int32_t)કદ(pathBuffer)) {
            *status=U_ILLEGAL_ARGUMENT_ERROR;
            પાછા nullptr;
        } નહીંતર જો(uprv_isInvariantUString(myPath, length)) {
            /*
             * the invariant converter is sufficient for package and tree names
             * and is more efficient
             */
            u_UCharsToChars(myPath, path, length+1); /* length+1 to include the NUL */
        } નહીંતર {
#જો !UCONFIG_NO_CONVERSION
            /* use the default converter to support variant-character paths */
            UConverter *cnv=u_getDefaultConverter(status);
            length=ucnv_fromUChars(cnv, path, (int32_t)કદ(pathBuffer), myPath, length, status);
            u_releaseDefaultConverter(cnv);
            જો(U_FAILURE(*status)) {
                પાછા nullptr;
            }
            જો(length>=(int32_t)કદ(pathBuffer)) {
                /* not NUL-terminated - path too long */
                *status=U_ILLEGAL_ARGUMENT_ERROR;
                પાછા nullptr;
            }
#નહીંતર
            /* the default converter is not available */
            *status=U_UNSUPPORTED_ERROR;
            પાછા nullptr;
#endif
        }
    }

    પાછા ures_open(path, localeID, status);
}
