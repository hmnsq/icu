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

Plural_Map_پایه::Category
Plural_Map_پایه::مقصد_Category(ثابت حرف *pluralForm) {
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        إذا (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            أرجع static_cast<Category>(i);
        }
    }
    أرجع NONE;
}

Plural_Map_پایه::Category
Plural_Map_پایه::مقصد_Category(ثابت UnicodeString &pluralForm) {
    CharString cCategory;
    UError_کد وضعیت = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, وضعیت);    
    أرجع U_FAILURE(وضعیت) ? NONE : مقصد_Category(cCategory.داده());
}

ثابت حرف *Plural_Map_پایه::getCategoryName(Category c) {
    int32_t شاخص = c;
    أرجع (شاخص < 0 || شاخص >= UPRV_LENGTHOF(gPluralForms)) ?
            اشاره_تهی : gPluralForms[شاخص];
}


U_NAMESPACE_انتها

