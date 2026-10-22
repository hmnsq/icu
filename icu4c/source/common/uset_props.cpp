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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_open_ਨਮੂਨਾ(ਅਚਲ char16_t* ਨਮੂਨਾ, int32_t ਨਮੂਨਾ_ਲੰਮਾਈ,
                 UError_ਕੋਡ* ec)
{
    UnicodeString pat(ਨਮੂਨਾ_ਲੰਮਾਈ==-1, ਨਮੂਨਾ, ਨਮੂਨਾ_ਲੰਮਾਈ);
    Unicode_ਸੈੱਟ* ਸੈੱਟ = ਨਵਾਂ Unicode_ਸੈੱਟ(pat, *ec);
    /* test for nullptr */
    ਜੇ (ਸੈੱਟ == ਖਾਲੀ_ਸੂਚਕ) {
        *ec = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ (U_FAILURE(*ec)) {
        ਮਿਟਾਓ ਸੈੱਟ;
        ਸੈੱਟ = ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ (USet*) ਸੈੱਟ;
}

U_CAPI USet* U_EXPORT2
uset_open_ਨਮੂਨਾ_Options(ਅਚਲ char16_t* ਨਮੂਨਾ, int32_t ਨਮੂਨਾ_ਲੰਮਾਈ,
                 uint32_t options,
                 UError_ਕੋਡ* ec)
{
    UnicodeString pat(ਨਮੂਨਾ_ਲੰਮਾਈ==-1, ਨਮੂਨਾ, ਨਮੂਨਾ_ਲੰਮਾਈ);
    Unicode_ਸੈੱਟ* ਸੈੱਟ = ਨਵਾਂ Unicode_ਸੈੱਟ(pat, options, ਖਾਲੀ_ਸੂਚਕ, *ec);
    /* test for nullptr */
    ਜੇ (ਸੈੱਟ == ਖਾਲੀ_ਸੂਚਕ) {
        *ec = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ (U_FAILURE(*ec)) {
        ਮਿਟਾਓ ਸੈੱਟ;
        ਸੈੱਟ = ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ (USet*) ਸੈੱਟ;
}


U_CAPI int32_t U_EXPORT2 
uset_apply_ਨਮੂਨਾ(USet *ਸੈੱਟ,
                  ਅਚਲ char16_t *ਨਮੂਨਾ, int32_t ਨਮੂਨਾ_ਲੰਮਾਈ,
                  uint32_t options,
                  UError_ਕੋਡ *ਸਥਿਤੀ){

    // status code needs to be checked since we 
    // dereference it
    ਜੇ(ਸਥਿਤੀ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ 0;
    }

    // check only the set paramenter
    // if pattern is nullptr or NUL terminated
    // UnicodeString constructor takes care of it
    ਜੇ(ਸੈੱਟ == ਖਾਲੀ_ਸੂਚਕ){
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    UnicodeString pat(ਨਮੂਨਾ, ਨਮੂਨਾ_ਲੰਮਾਈ);

    ParsePosition pos;
   
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->apply_ਨਮੂਨਾ(pat, pos, options, ਖਾਲੀ_ਸੂਚਕ, *ਸਥਿਤੀ);
    
    ਵਾਪਸ pos.get_ਸੂਚਕਾਂਕ();
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_apply_Int_Property_ਮੁੱਲ(USet* ਸੈੱਟ,
               UProperty prop, int32_t value, UError_ਕੋਡ* ec) {
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->apply_Int_Property_ਮੁੱਲ(prop, value, *ec);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_applyPropertyAlias(USet* ਸੈੱਟ,
                        ਅਚਲ char16_t *prop, int32_t prop_ਲੰਮਾਈ,
                        ਅਚਲ char16_t *value, int32_t ਮੁੱਲ_ਲੰਮਾਈ,
            UError_ਕੋਡ* ec) {

    UnicodeString p(prop, prop_ਲੰਮਾਈ);
    UnicodeString v(value, ਮੁੱਲ_ਲੰਮਾਈ);

    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->applyPropertyAlias(p, v, *ec);
}

U_CAPI UBool U_EXPORT2
uset_resembles_ਨਮੂਨਾ(ਅਚਲ char16_t *ਨਮੂਨਾ, int32_t ਨਮੂਨਾ_ਲੰਮਾਈ,
                      int32_t pos) {

    UnicodeString pat(ਨਮੂਨਾ, ਨਮੂਨਾ_ਲੰਮਾਈ);

    ਵਾਪਸ ((pos+1) < pat.length() &&
            pat.charAt(pos) == (char16_t)91/*[*/) ||
            Unicode_ਸੈੱਟ::resembles_ਨਮੂਨਾ(pat, pos);
}

U_CAPI int32_t U_EXPORT2
uset_ਮੰਜ਼ਿਲ_ਨਮੂਨਾ(ਅਚਲ USet* ਸੈੱਟ,
               char16_t* ਨਤੀਜਾ, int32_t ਨਤੀਜਾ_ਸਮਰੱਥਾ,
               UBool escapeUnprintable,
               UError_ਕੋਡ* ec) {
    UnicodeString pat;
    ((ਅਚਲ Unicode_ਸੈੱਟ*) ਸੈੱਟ)->ਮੰਜ਼ਿਲ_ਨਮੂਨਾ(pat, escapeUnprintable);
    ਵਾਪਸ pat.extract(ਨਤੀਜਾ, ਨਤੀਜਾ_ਸਮਰੱਥਾ, *ec);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_closeOver(USet* ਸੈੱਟ, int32_t ਗੁਣ) {
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::closeOver(ਗੁਣ);
}
