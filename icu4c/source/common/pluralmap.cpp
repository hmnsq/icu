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

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ gPluralForms[] = {
        "other", "zero", "one", "two", "few", "many"};

Plural_Map_ತಳ::Category
Plural_Map_ತಳ::ಗಮ್ಯ_Category(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pluralForm) {
    ಗಾಗಿ (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        ಆದರೆ (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            ಹಿಂತಿರುಗಿ static_cast<Category>(i);
        }
    }
    ಹಿಂತಿರುಗಿ NONE;
}

Plural_Map_ತಳ::Category
Plural_Map_ತಳ::ಗಮ್ಯ_Category(ಸ್ಥಿರಾಂಕ UnicodeString &pluralForm) {
    CharString cCategory;
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, ಸ್ಥಿತಿ_2);    
    ಹಿಂತಿರುಗಿ U_FAILURE(ಸ್ಥಿತಿ_2) ? NONE : ಗಮ್ಯ_Category(cCategory.ದತ್ತಾಂಶ());
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Plural_Map_ತಳ::getCategoryName(Category c) {
    int32_t ಸೂಚ್ಯಂಕ = c;
    ಹಿಂತಿರುಗಿ (ಸೂಚ್ಯಂಕ < 0 || ಸೂಚ್ಯಂಕ >= UPRV_LENGTHOF(gPluralForms)) ?
            ಶೂನ್ಯಸೂಚಕ : gPluralForms[ಸೂಚ್ಯಂಕ];
}


U_NAMESPACE_ಕೊನೆ

