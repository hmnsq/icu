// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 * Copyright (C) 2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 */

#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "pluralmap.h"

U_NAMESPACE_BEGIN

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ gPluralForms[] = {
        "other", "zero", "one", "two", "few", "many"};

PluralMapBase::Category
PluralMapBase::toCategory(ଅଚଳ ଅକ୍ଷର *pluralForm) {
    ପାଇଁ (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        ଯଦି (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            ଫେରନ୍ତୁ static_cast<Category>(i);
        }
    }
    ଫେରନ୍ତୁ NONE;
}

PluralMapBase::Category
PluralMapBase::toCategory(ଅଚଳ UnicodeString &pluralForm) {
    CharString cCategory;
    UErrorCode status = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, status);    
    ଫେରନ୍ତୁ U_FAILURE(status) ? NONE : toCategory(cCategory.data());
}

ଅଚଳ ଅକ୍ଷର *PluralMapBase::getCategoryName(Category c) {
    int32_t index = c;
    ଫେରନ୍ତୁ (index < 0 || index >= UPRV_LENGTHOF(gPluralForms)) ?
            nullptr : gPluralForms[index];
}


U_NAMESPACE_END

