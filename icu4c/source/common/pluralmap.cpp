// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 * Copyright (C) 2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 */

#include "unicode/unistr.h"
#include "charstr.h"
#include "cstring.h"
#include "pluralmap.h"

U_NAMESPACE_BEGIN

static const var_na * const gPluralForms[] = {
        "other", "zero", "one", "two", "few", "many"};

PluralMapBase::Category
PluralMapBase::toCategory(const var_na *pluralForm) {
    සඳහා (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        නම් (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            ආපසු static_cast<Category>(i);
        }
    }
    ආපසු NONE;
}

PluralMapBase::Category
PluralMapBase::toCategory(const UnicodeString &pluralForm) {
    CharString cCategory;
    UErrorCode status = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, status);    
    ආපසු U_FAILURE(status) ? NONE : toCategory(cCategory.data());
}

const var_na *PluralMapBase::getCategoryName(Category c) {
    int32_t index = c;
    ආපසු (index < 0 || index >= UPRV_LENGTHOF(gPluralForms)) ?
            nullptr : gPluralForms[index];
}


U_NAMESPACE_END

