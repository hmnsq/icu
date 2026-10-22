// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 * Copyright (C) 2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 */

#অন্তর্ভাৱ "unicode/unistr.h"
#অন্তর্ভাৱ "charstr.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "pluralmap.h"

U_NAMESPACE_BEGIN

স্থির ধ্রুৱ ৱর্ণ * ধ্রুৱ gPluralForms[] = {
        "other", "zero", "one", "two", "few", "many"};

PluralMapBase::Category
PluralMapBase::toCategory(ধ্রুৱ ৱর্ণ *pluralForm) {
    বাবে (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        যদি (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            ঘূৰাই static_cast<Category>(i);
        }
    }
    ঘূৰাই NONE;
}

PluralMapBase::Category
PluralMapBase::toCategory(ধ্রুৱ UnicodeString &pluralForm) {
    CharString cCategory;
    UErrorCode status = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, status);    
    ঘূৰাই U_FAILURE(status) ? NONE : toCategory(cCategory.data());
}

ধ্রুৱ ৱর্ণ *PluralMapBase::getCategoryName(Category c) {
    int32_t index = c;
    ঘূৰাই (index < 0 || index >= UPRV_LENGTHOF(gPluralForms)) ?
            nullptr : gPluralForms[index];
}


U_NAMESPACE_END

