// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 * Copyright (C) 2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 */

#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "pluralmap.h"

U_NAMESPACE_BEGIN

நிலையான மாறிலி எழுத்து * மாறிலி gPluralForms[] = {
        "other", "zero", "one", "two", "few", "many"};

PluralMapBase::Category
PluralMapBase::toCategory(மாறிலி எழுத்து *pluralForm) {
    க்கு (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        எனில் (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            திரும்பு static_cast<Category>(i);
        }
    }
    திரும்பு NONE;
}

PluralMapBase::Category
PluralMapBase::toCategory(மாறிலி UnicodeString &pluralForm) {
    CharString cCategory;
    UErrorCode status = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, status);    
    திரும்பு U_FAILURE(status) ? NONE : toCategory(cCategory.data());
}

மாறிலி எழுத்து *PluralMapBase::getCategoryName(Category c) {
    int32_t index = c;
    திரும்பு (index < 0 || index >= UPRV_LENGTHOF(gPluralForms)) ?
            nullptr : gPluralForms[index];
}


U_NAMESPACE_END

