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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/putil.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/ucnv.h"
#অন্তর্ভাব "unicode/ures.h"
#অন্তর্ভাব "uinvchar.h"
#অন্তর্ভাব "ustr_cnv.h"

U_CAPI UResourceBundle * U_EXPORT2
ures_openU(ধ্রুবক char16_t *myPath,
           ধ্রুবক অক্ষর *localeID, 
           UErrorCode *status)
{
    অক্ষর path_বাফার[1024];
    int32_t length;
    অক্ষর *path = path_বাফার;

    যদি(status==nullptr || U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    যদি(myPath==nullptr) {
        path = nullptr;
    }
    অন্যথা {
        length=u_strlen(myPath);
        যদি(length>=(int32_t)আকার(path_বাফার)) {
            *status=U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত nullptr;
        } অন্যথা যদি(uprv_isInvariantUString(myPath, length)) {
            /*
             * the invariant converter is sufficient for package and tree names
             * and is more efficient
             */
            u_UCharsToChars(myPath, path, length+1); /* length+1 to include the NUL */
        } অন্যথা {
#যদি !UCONFIG_NO_CONVERSION
            /* use the default converter to support variant-character paths */
            UConverter *cnv=u_getDefaultConverter(status);
            length=ucnv_fromUChars(cnv, path, (int32_t)আকার(path_বাফার), myPath, length, status);
            u_releaseDefaultConverter(cnv);
            যদি(U_FAILURE(*status)) {
                ফেরত nullptr;
            }
            যদি(length>=(int32_t)আকার(path_বাফার)) {
                /* not NUL-terminated - path too long */
                *status=U_ILLEGAL_ARGUMENT_ERROR;
                ফেরত nullptr;
            }
#অন্যথা
            /* the default converter is not available */
            *status=U_UNSUPPORTED_ERROR;
            ফেরত nullptr;
#endif
        }
    }

    ফেরত ures_open(path, localeID, status);
}
