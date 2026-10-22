// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 * Copyright (C) 2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pluralmap.h"

U_NAMESPACE_BEGIN

ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ gPluralForms[] = {
        "other", "zero", "one", "two", "few", "many"};

Plural_Map_ਅਧਾਰ::Category
Plural_Map_ਅਧਾਰ::ਮੰਜ਼ਿਲ_Category(ਅਚਲ ਅੱਖਰ *pluralForm) {
    ਲਈ (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        ਜੇ (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            ਵਾਪਸ static_cast<Category>(i);
        }
    }
    ਵਾਪਸ NONE;
}

Plural_Map_ਅਧਾਰ::Category
Plural_Map_ਅਧਾਰ::ਮੰਜ਼ਿਲ_Category(ਅਚਲ UnicodeString &pluralForm) {
    CharString cCategory;
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, ਸਥਿਤੀ);    
    ਵਾਪਸ U_FAILURE(ਸਥਿਤੀ) ? NONE : ਮੰਜ਼ਿਲ_Category(cCategory.ਅੰਕੜੇ());
}

ਅਚਲ ਅੱਖਰ *Plural_Map_ਅਧਾਰ::getCategoryName(Category c) {
    int32_t ਸੂਚਕਾਂਕ = c;
    ਵਾਪਸ (ਸੂਚਕਾਂਕ < 0 || ਸੂਚਕਾਂਕ >= UPRV_LENGTHOF(gPluralForms)) ?
            ਖਾਲੀ_ਸੂਚਕ : gPluralForms[ਸੂਚਕਾਂਕ];
}


U_NAMESPACE_ਸਿਰਾ

