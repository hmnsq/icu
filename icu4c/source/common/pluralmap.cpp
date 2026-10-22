// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 * Copyright (C) 2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 */

#ضمن "unicode/unistr.h"
#ضمن "charstr.h"
#ضمن "cstring.h"
#ضمن "pluralmap.h"

U_NAMESPACE_BEGIN

ساكن ثابت حرف * ثابت gPluralForms[] = {
        "other", "zero", "one", "two", "few", "many"};

Plural_Map_بنسټ::Category
Plural_Map_بنسټ::منزل_Category(ثابت حرف *pluralForm) {
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        إذا (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            أرجع static_cast<Category>(i);
        }
    }
    أرجع NONE;
}

Plural_Map_بنسټ::Category
Plural_Map_بنسټ::منزل_Category(ثابت UnicodeString &pluralForm) {
    CharString cCategory;
    UError_کوډ حالت = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, حالت);    
    أرجع U_FAILURE(حالت) ? NONE : منزل_Category(cCategory.معلومات());
}

ثابت حرف *Plural_Map_بنسټ::getCategoryName(Category c) {
    int32_t شاخص = c;
    أرجع (شاخص < 0 || شاخص >= UPRV_LENGTHOF(gPluralForms)) ?
            تش_اشاره : gPluralForms[شاخص];
}


U_NAMESPACE_پای_ټکی

