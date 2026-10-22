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

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/putil.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "unicode/ures.h"
#అన్తర్భావ "uinvchar.h"
#అన్తర్భావ "ustr_cnv.h"

U_CAPI UResourceBundle * U_EXPORT2
ures_openU(స్థిరరాశి char16_t *my_మార్గం,
           స్థిరరాశి అక్షరం *localeID, 
           UError_సంకేతం *స్థితి_2)
{
    అక్షరం మార్గం_బఫరు[1024];
    int32_t length;
    అక్షరం *మార్గం = మార్గం_బఫరు;

    ఒకవేళ(స్థితి_2==శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ(my_మార్గం==శూన్యసూచిక) {
        మార్గం = శూన్యసూచిక;
    }
    లేకపోతే {
        length=u_strlen(my_మార్గం);
        ఒకవేళ(length>=(int32_t)పరిమాణం(మార్గం_బఫరు)) {
            *స్థితి_2=U_ILLEGAL_వాదం_ERROR;
            తిరిగి శూన్యసూచిక;
        } లేకపోతే ఒకవేళ(uprv_isInvariantUString(my_మార్గం, length)) {
            /*
             * the invariant converter is sufficient for package and tree names
             * and is more efficient
             */
            u_UChars_గమ్యం_Chars(my_మార్గం, మార్గం, length+1); /* length+1 to include the NUL */
        } లేకపోతే {
#ఒకవేళ !UCONFIG_NO_CONVERSION
            /* use the default converter to support variant-character paths */
            UConverter *cnv=u_getDefaultConverter(స్థితి_2);
            length=ucnv_fromUChars(cnv, మార్గం, (int32_t)పరిమాణం(మార్గం_బఫరు), my_మార్గం, length, స్థితి_2);
            u_releaseDefaultConverter(cnv);
            ఒకవేళ(U_FAILURE(*స్థితి_2)) {
                తిరిగి శూన్యసూచిక;
            }
            ఒకవేళ(length>=(int32_t)పరిమాణం(మార్గం_బఫరు)) {
                /* not NUL-terminated - path too long */
                *స్థితి_2=U_ILLEGAL_వాదం_ERROR;
                తిరిగి శూన్యసూచిక;
            }
#లేకపోతే
            /* the default converter is not available */
            *స్థితి_2=U_UNSUPPORTED_ERROR;
            తిరిగి శూన్యసూచిక;
#అయితే_ముగింపు
        }
    }

    తిరిగి ures_open(మార్గం, localeID, స్థితి_2);
}
