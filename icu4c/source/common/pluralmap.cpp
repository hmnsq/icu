// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 * Copyright (C) 2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 */

#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "pluralmap.h"

U_NAMESPACE_BEGIN

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ gPluralForms[] = {
        "other", "zero", "one", "two", "few", "many"};

PluralMapBase::Category
PluralMapBase::toCategory(ಧ್ರುವ ವರ್ಣ *pluralForm) {
    ಪ್ರತಿ (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        ಯದಿ (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            ಪ್ರತ್ಯಾಗಮ static_cast<Category>(i);
        }
    }
    ಪ್ರತ್ಯಾಗಮ NONE;
}

PluralMapBase::Category
PluralMapBase::toCategory(ಧ್ರುವ UnicodeString &pluralForm) {
    CharString cCategory;
    UErrorCode status = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, status);    
    ಪ್ರತ್ಯಾಗಮ U_FAILURE(status) ? NONE : toCategory(cCategory.data());
}

ಧ್ರುವ ವರ್ಣ *PluralMapBase::getCategoryName(Category c) {
    int32_t index = c;
    ಪ್ರತ್ಯಾಗಮ (index < 0 || index >= UPRV_LENGTHOF(gPluralForms)) ?
            nullptr : gPluralForms[index];
}


U_NAMESPACE_END

