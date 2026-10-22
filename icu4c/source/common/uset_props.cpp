// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2002-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uset_props.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2004aug30
*   created by: Markus W. Scherer
*
*   C wrappers around UnicodeSet functions that are implemented in
*   uniset_props.cpp, split off for modularization.
*/

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "unicode/uset.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_open_నమూనా(స్థిరరాశి char16_t* నమూనా, int32_t నమూనా_నిడివి,
                 UError_సంకేతం* ec)
{
    UnicodeString pat(నమూనా_నిడివి==-1, నమూనా, నమూనా_నిడివి);
    Unicode_సమితి* సమితి = కొత్త Unicode_సమితి(pat, *ec);
    /* test for nullptr */
    ఒకవేళ (సమితి == శూన్యసూచిక) {
        *ec = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ (U_FAILURE(*ec)) {
        తొలగించు సమితి;
        సమితి = శూన్యసూచిక;
    }
    తిరిగి (USet*) సమితి;
}

U_CAPI USet* U_EXPORT2
uset_open_నమూనా_Options(స్థిరరాశి char16_t* నమూనా, int32_t నమూనా_నిడివి,
                 uint32_t options,
                 UError_సంకేతం* ec)
{
    UnicodeString pat(నమూనా_నిడివి==-1, నమూనా, నమూనా_నిడివి);
    Unicode_సమితి* సమితి = కొత్త Unicode_సమితి(pat, options, శూన్యసూచిక, *ec);
    /* test for nullptr */
    ఒకవేళ (సమితి == శూన్యసూచిక) {
        *ec = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ (U_FAILURE(*ec)) {
        తొలగించు సమితి;
        సమితి = శూన్యసూచిక;
    }
    తిరిగి (USet*) సమితి;
}


U_CAPI int32_t U_EXPORT2 
uset_apply_నమూనా(USet *సమితి,
                  స్థిరరాశి char16_t *నమూనా, int32_t నమూనా_నిడివి,
                  uint32_t options,
                  UError_సంకేతం *స్థితి_2){

    // status code needs to be checked since we 
    // dereference it
    ఒకవేళ(స్థితి_2 == శూన్యసూచిక || U_FAILURE(*స్థితి_2)){
        తిరిగి 0;
    }

    // check only the set paramenter
    // if pattern is nullptr or NUL terminated
    // UnicodeString constructor takes care of it
    ఒకవేళ(సమితి == శూన్యసూచిక){
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    UnicodeString pat(నమూనా, నమూనా_నిడివి);

    ParsePosition pos;
   
    ((Unicode_సమితి*) సమితి)->apply_నమూనా(pat, pos, options, శూన్యసూచిక, *స్థితి_2);
    
    తిరిగి pos.get_సూచీ();
}

U_CAPI శూన్య U_EXPORT2
uset_apply_Int_Property_విలువ(USet* సమితి,
               UProperty prop, int32_t value, UError_సంకేతం* ec) {
    ((Unicode_సమితి*) సమితి)->apply_Int_Property_విలువ(prop, value, *ec);
}

U_CAPI శూన్య U_EXPORT2
uset_applyPropertyAlias(USet* సమితి,
                        స్థిరరాశి char16_t *prop, int32_t prop_నిడివి,
                        స్థిరరాశి char16_t *value, int32_t విలువ_నిడివి,
            UError_సంకేతం* ec) {

    UnicodeString p(prop, prop_నిడివి);
    UnicodeString v(value, విలువ_నిడివి);

    ((Unicode_సమితి*) సమితి)->applyPropertyAlias(p, v, *ec);
}

U_CAPI UBool U_EXPORT2
uset_resembles_నమూనా(స్థిరరాశి char16_t *నమూనా, int32_t నమూనా_నిడివి,
                      int32_t pos) {

    UnicodeString pat(నమూనా, నమూనా_నిడివి);

    తిరిగి ((pos+1) < pat.length() &&
            pat.charAt(pos) == (char16_t)91/*[*/) ||
            Unicode_సమితి::resembles_నమూనా(pat, pos);
}

U_CAPI int32_t U_EXPORT2
uset_గమ్యం_నమూనా(స్థిరరాశి USet* సమితి,
               char16_t* ఫలితం, int32_t ఫలితం_సామర్థ్యం,
               UBool escapeUnprintable,
               UError_సంకేతం* ec) {
    UnicodeString pat;
    ((స్థిరరాశి Unicode_సమితి*) సమితి)->గమ్యం_నమూనా(pat, escapeUnprintable);
    తిరిగి pat.extract(ఫలితం, ఫలితం_సామర్థ్యం, *ec);
}

U_CAPI శూన్య U_EXPORT2
uset_closeOver(USet* సమితి, int32_t లక్షణాలు) {
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::closeOver(లక్షణాలు);
}
