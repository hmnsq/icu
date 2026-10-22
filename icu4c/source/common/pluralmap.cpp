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

Plural_Map_आधारः::Category
Plural_Map_आधारः::गन्तव्यम्_Category(ध्रुव वर्ण *pluralForm) {
    प्रति (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        यदि (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            प्रत्यागम static_cast<Category>(i);
        }
    }
    प्रत्यागम NONE;
}

Plural_Map_आधारः::Category
Plural_Map_आधारः::गन्तव्यम्_Category(ध्रुव UnicodeString &pluralForm) {
    CharString cCategory;
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, स्थितिः);    
    प्रत्यागम U_FAILURE(स्थितिः) ? NONE : गन्तव्यम्_Category(cCategory.दत्त());
}

ध्रुव वर्ण *Plural_Map_आधारः::getCategoryName(Category c) {
    int32_t सूचकांक = c;
    प्रत्यागम (सूचकांक < 0 || सूचकांक >= UPRV_LENGTHOF(gPluralForms)) ?
            शून्यसूचकः : gPluralForms[सूचकांक];
}


U_NAMESPACE_अन्तः

