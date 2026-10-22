// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 * Copyright (C) 2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 */

#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "pluralmap.h"

U_NAMESPACE_BEGIN

स्थिर ध्रुव वर्ण * ध्रुव gPluralForms[] = {
        "other", "zero", "one", "two", "few", "many"};

PluralMapBase::Category
PluralMapBase::toCategory(ध्रुव वर्ण *pluralForm) {
    प्रति (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        यदि (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            प्रत्यागम static_cast<Category>(i);
        }
    }
    प्रत्यागम NONE;
}

PluralMapBase::Category
PluralMapBase::toCategory(ध्रुव UnicodeString &pluralForm) {
    CharString cCategory;
    UErrorCode status = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, status);    
    प्रत्यागम U_FAILURE(status) ? NONE : toCategory(cCategory.data());
}

ध्रुव वर्ण *PluralMapBase::getCategoryName(Category c) {
    int32_t index = c;
    प्रत्यागम (index < 0 || index >= UPRV_LENGTHOF(gPluralForms)) ?
            nullptr : gPluralForms[index];
}


U_NAMESPACE_END

