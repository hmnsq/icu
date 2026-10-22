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

PluralMapBase::Category
PluralMapBase::toCategory(ثابت حرف *pluralForm) {
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        إذا (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            إرجاع static_cast<Category>(i);
        }
    }
    إرجاع NONE;
}

PluralMapBase::Category
PluralMapBase::toCategory(ثابت UnicodeString &pluralForm) {
    CharString cCategory;
    UErrorCode status = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, status);    
    إرجاع U_FAILURE(status) ? NONE : toCategory(cCategory.data());
}

ثابت حرف *PluralMapBase::getCategoryName(Category c) {
    int32_t index = c;
    إرجاع (index < 0 || index >= UPRV_LENGTHOF(gPluralForms)) ?
            nullptr : gPluralForms[index];
}


U_NAMESPACE_END

