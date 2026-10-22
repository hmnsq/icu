// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  ucharstriebuilder.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010nov14
*   created by: Markus W. Scherer
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucharstrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucharstriebuilder.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uarrsort.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (string, value) pairs with full copies
 * of the 16-bit-unit sequences, until the UCharsTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

ವರ್ಗ UCharsTrieElement : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    // Use compiler's default constructor, initializes nothing.

    ಶೂನ್ಯ ಗಣ_ಗಮ್ಯ(ಸ್ಥಿರಾಂಕ UnicodeString &s, int32_t val, UnicodeString &ಅಕ್ಷರಸರಣಿಗಳು, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    UnicodeString getString(ಸ್ಥಿರಾಂಕ UnicodeString& ಅಕ್ಷರಸರಣಿಗಳು U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
        int32_t length=ಅಕ್ಷರಸರಣಿಗಳು[string_ಸ್ಥಾನಾಂತರ];
        ಹಿಂತಿರುಗಿ ಅಕ್ಷರಸರಣಿಗಳು.ತಾತ್ಕಾಲಿಕ_Sub_String(string_ಸ್ಥಾನಾಂತರ+1, length);
    }
    int32_t get_String_ಉದ್ದಳತೆ(ಸ್ಥಿರಾಂಕ UnicodeString &ಅಕ್ಷರಸರಣಿಗಳು) ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ ಅಕ್ಷರಸರಣಿಗಳು[string_ಸ್ಥಾನಾಂತರ];
    }

    char16_t charAt(int32_t ಸೂಚ್ಯಂಕ, ಸ್ಥಿರಾಂಕ UnicodeString &ಅಕ್ಷರಸರಣಿಗಳು) ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ ಅಕ್ಷರಸರಣಿಗಳು[string_ಸ್ಥಾನಾಂತರ+1+ಸೂಚ್ಯಂಕ];
    }

    int32_t get_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ value; }

    int32_t compare_String_ಗಮ್ಯ(ಸ್ಥಿರಾಂಕ UCharsTrieElement &o, ಸ್ಥಿರಾಂಕ UnicodeString &ಅಕ್ಷರಸರಣಿಗಳು) ಸ್ಥಿರಾಂಕ;

ಖಾಸಗಿ:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t string_ಸ್ಥಾನಾಂತರ;
    int32_t value;
};

ಶೂನ್ಯ
UCharsTrieElement::ಗಣ_ಗಮ್ಯ(ಸ್ಥಿರಾಂಕ UnicodeString &s, int32_t val,
                         UnicodeString &ಅಕ್ಷರಸರಣಿಗಳು, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    int32_t length=s.length();
    ಆದರೆ(length>0xffff) {
        // Too long: We store the length in 1 unit.
        error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    string_ಸ್ಥಾನಾಂತರ=ಅಕ್ಷರಸರಣಿಗಳು.length();
    ಅಕ್ಷರಸರಣಿಗಳು.append(static_cast<char16_t>(length));
    value=val;
    ಅಕ್ಷರಸರಣಿಗಳು.append(s);
}

int32_t
UCharsTrieElement::compare_String_ಗಮ್ಯ(ಸ್ಥಿರಾಂಕ UCharsTrieElement &other, ಸ್ಥಿರಾಂಕ UnicodeString &ಅಕ್ಷರಸರಣಿಗಳು) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ getString(ಅಕ್ಷರಸರಣಿಗಳು).compare(other.getString(ಅಕ್ಷರಸರಣಿಗಳು));
}

UCharsTrieBuilder::UCharsTrieBuilder(UError_ಸಂಕೇತ & /*errorCode*/)
        : elements(ಶೂನ್ಯಸೂಚಕ), elements_ಸಾಮರ್ಥ್ಯ(0), elements_ಉದ್ದಳತೆ(0),
          uchars(ಶೂನ್ಯಸೂಚಕ), uchars_ಸಾಮರ್ಥ್ಯ(0), uchars_ಉದ್ದಳತೆ(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    ಅಳಿಸು[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::ಕೂಡಿಸು(ಸ್ಥಿರಾಂಕ UnicodeString& s,
                       int32_t value,
                       UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಆದರೆ(uchars_ಉದ್ದಳತೆ>0) {
        // Cannot add elements after building.
        error_ಸಂಕೇತ=U_NO_WRITE_PERMISSION;
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಆದರೆ(elements_ಉದ್ದಳತೆ==elements_ಸಾಮರ್ಥ್ಯ) {
        int32_t new_ಸಾಮರ್ಥ್ಯ;
        ಆದರೆ(elements_ಸಾಮರ್ಥ್ಯ==0) {
            new_ಸಾಮರ್ಥ್ಯ=1024;
        } ಇಲ್ಲದಿದ್ದರೆ {
            new_ಸಾಮರ್ಥ್ಯ=4*elements_ಸಾಮರ್ಥ್ಯ;
        }
        UCharsTrieElement *newElements=ಹೊಸ UCharsTrieElement[new_ಸಾಮರ್ಥ್ಯ];
        ಆದರೆ(newElements==ಶೂನ್ಯಸೂಚಕ) {
            error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ *ಇದು;
        }
        ಆದರೆ(elements_ಉದ್ದಳತೆ>0) {
            uprv_memcpy(newElements, elements, (size_t)elements_ಉದ್ದಳತೆ*ಗಾತ್ರ(UCharsTrieElement));
        }
        ಅಳಿಸು[] elements;
        elements=newElements;
        elements_ಸಾಮರ್ಥ್ಯ=new_ಸಾಮರ್ಥ್ಯ;
    }
    elements[elements_ಉದ್ದಳತೆ++].ಗಣ_ಗಮ್ಯ(s, value, ಅಕ್ಷರಸರಣಿಗಳು, error_ಸಂಕೇತ);
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ) && ಅಕ್ಷರಸರಣಿಗಳು.isBogus()) {
        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

U_CDECL_BEGIN

ಸ್ಥಿರ int32_t U_CALLCONV
compare_Element_ಅಕ್ಷರಸರಣಿಗಳು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಎಡ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಲ) {
    ಸ್ಥಿರಾಂಕ UnicodeString *ಅಕ್ಷರಸರಣಿಗಳು=static_cast<ಸ್ಥಿರಾಂಕ UnicodeString *>(context);
    ಸ್ಥಿರಾಂಕ UCharsTrieElement *ಎಡ_Element=static_cast<ಸ್ಥಿರಾಂಕ UCharsTrieElement *>(ಎಡ);
    ಸ್ಥಿರಾಂಕ UCharsTrieElement *ಬಲ_Element=static_cast<ಸ್ಥಿರಾಂಕ UCharsTrieElement *>(ಬಲ);
    ಹಿಂತಿರುಗಿ ಎಡ_Element->compare_String_ಗಮ್ಯ(*ಬಲ_Element, *ಅಕ್ಷರಸರಣಿಗಳು);
}

U_CDECL_ಕೊನೆ

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    buildUChars(buildOption, error_ಸಂಕೇತ);
    UCharsTrie *newTrie=ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
        newTrie=ಹೊಸ UCharsTrie(uchars, uchars+(uchars_ಸಾಮರ್ಥ್ಯ-uchars_ಉದ್ದಳತೆ));
        ಆದರೆ(newTrie==ಶೂನ್ಯಸೂಚಕ) {
            error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ {
            uchars=ಶೂನ್ಯಸೂಚಕ;  // The new trie now owns the array.
            uchars_ಸಾಮರ್ಥ್ಯ=0;
        }
    }
    ಹಿಂತಿರುಗಿ newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND,
                                      UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND {
    buildUChars(buildOption, error_ಸಂಕೇತ);
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
        ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ(ಸುಳ್ಳು, uchars+(uchars_ಸಾಮರ್ಥ್ಯ-uchars_ಉದ್ದಳತೆ), uchars_ಉದ್ದಳತೆ);
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ(uchars!=ಶೂನ್ಯಸೂಚಕ && uchars_ಉದ್ದಳತೆ>0) {
        // Already built.
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ(uchars_ಉದ್ದಳತೆ==0) {
        ಆದರೆ(elements_ಉದ್ದಳತೆ==0) {
            error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ(ಅಕ್ಷರಸರಣಿಗಳು.isBogus()) {
            error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        uprv_ವಿಂಗಡಿಸು_Array(elements, elements_ಉದ್ದಳತೆ, static_cast<int32_t>(ಗಾತ್ರ(UCharsTrieElement)),
                      compare_Element_ಅಕ್ಷರಸರಣಿಗಳು, &ಅಕ್ಷರಸರಣಿಗಳು,
                      ಸುಳ್ಳು,  // need not be a stable sort
                      &error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(ಅಕ್ಷರಸರಣಿಗಳು);
        ಗಾಗಿ(int32_t i=1; i<elements_ಉದ್ದಳತೆ; ++i) {
            UnicodeString current=elements[i].getString(ಅಕ್ಷರಸರಣಿಗಳು);
            ಆದರೆ(prev==current) {
                error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    uchars_ಉದ್ದಳತೆ=0;
    int32_t capacity=ಅಕ್ಷರಸರಣಿಗಳು.length();
    ಆದರೆ(capacity<1024) {
        capacity=1024;
    }
    ಆದರೆ(uchars_ಸಾಮರ್ಥ್ಯ<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        ಆದರೆ(uchars==ಶೂನ್ಯಸೂಚಕ) {
            error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            uchars_ಸಾಮರ್ಥ್ಯ=0;
            ಹಿಂತಿರುಗಿ;
        }
        uchars_ಸಾಮರ್ಥ್ಯ=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_ಉದ್ದಳತೆ, error_ಸಂಕೇತ);
    ಆದರೆ(uchars==ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
}

int32_t
UCharsTrieBuilder::get_Element_String_ಉದ್ದಳತೆ(int32_t i) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ elements[i].get_String_ಉದ್ದಳತೆ(ಅಕ್ಷರಸರಣಿಗಳು);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unit_ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ elements[i].charAt(unit_ಸೂಚ್ಯಂಕ, ಅಕ್ಷರಸರಣಿಗಳು);
}

int32_t
UCharsTrieBuilder::get_Element_ಮೌಲ್ಯ(int32_t i) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ elements[i].get_ಮೌಲ್ಯ();
}

int32_t
UCharsTrieBuilder::get_ಎಲ್ಲೆ_ದ_ರೇಖೀಯ_ಹೊಂದಿಕೆ(int32_t first, int32_t ಕೊನೆಯ, int32_t unit_ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ {
    ಸ್ಥಿರಾಂಕ UCharsTrieElement &ಮೊದಲ_Element=elements[first];
    ಸ್ಥಿರಾಂಕ UCharsTrieElement &ಕೊನೆಯ_Element=elements[ಕೊನೆಯ];
    int32_t min_String_ಉದ್ದಳತೆ=ಮೊದಲ_Element.get_String_ಉದ್ದಳತೆ(ಅಕ್ಷರಸರಣಿಗಳು);
    ಆಗ(++unit_ಸೂಚ್ಯಂಕ<min_String_ಉದ್ದಳತೆ &&
            ಮೊದಲ_Element.charAt(unit_ಸೂಚ್ಯಂಕ, ಅಕ್ಷರಸರಣಿಗಳು)==
            ಕೊನೆಯ_Element.charAt(unit_ಸೂಚ್ಯಂಕ, ಅಕ್ಷರಸರಣಿಗಳು)) {}
    ಹಿಂತಿರುಗಿ unit_ಸೂಚ್ಯಂಕ;
}

int32_t
UCharsTrieBuilder::ಎಣಿಕೆ_Element_Units(int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ, int32_t unit_ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=ಆರಂಭ;
    ಮಾಡು {
        char16_t unit=elements[i++].charAt(unit_ಸೂಚ್ಯಂಕ, ಅಕ್ಷರಸರಣಿಗಳು);
        ಆಗ(i<ಎಲ್ಲೆ && unit==elements[i].charAt(unit_ಸೂಚ್ಯಂಕ, ಅಕ್ಷರಸರಣಿಗಳು)) {
            ++i;
        }
        ++length;
    } ಆಗ(i<ಎಲ್ಲೆ);
    ಹಿಂತಿರುಗಿ length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unit_ಸೂಚ್ಯಂಕ, int32_t count) ಸ್ಥಿರಾಂಕ {
    ಮಾಡು {
        char16_t unit=elements[i++].charAt(unit_ಸೂಚ್ಯಂಕ, ಅಕ್ಷರಸರಣಿಗಳು);
        ಆಗ(unit==elements[i].charAt(unit_ಸೂಚ್ಯಂಕ, ಅಕ್ಷರಸರಣಿಗಳು)) {
            ++i;
        }
    } ಆಗ(--count>0);
    ಹಿಂತಿರುಗಿ i;
}

int32_t
UCharsTrieBuilder::ಸೂಚ್ಯಂಕ_ದ_Element_With_ಮುಂದಿನ_Unit(int32_t i, int32_t unit_ಸೂಚ್ಯಂಕ, char16_t unit) ಸ್ಥಿರಾಂಕ {
    ಆಗ(unit==elements[i].charAt(unit_ಸೂಚ್ಯಂಕ, ಅಕ್ಷರಸರಣಿಗಳು)) {
        ++i;
    }
    ಹಿಂತಿರುಗಿ i;
}

UCharsTrieBuilder::UCTLinear_ಹೊಂದಿಕೆ_ಗಂಟು::UCTLinear_ಹೊಂದಿಕೆ_ಗಂಟು(ಸ್ಥಿರಾಂಕ char16_t *units, int32_t len, Node *ಮುಂದಿನ_ಗಂಟು)
        : ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಗಂಟು(len, ಮುಂದಿನ_ಗಂಟು), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

ತಾರ್ಕಿಕ
UCharsTrieBuilder::UCTLinear_ಹೊಂದಿಕೆ_ಗಂಟು::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Node &other) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(ಇದು==&other) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ(!ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಗಂಟು::ಕ್ರಿಯಾಕಾರಕ==(other)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಸ್ಥಿರಾಂಕ UCTLinear_ಹೊಂದಿಕೆ_ಗಂಟು &o=static_cast<ಸ್ಥಿರಾಂಕ UCTLinear_ಹೊಂದಿಕೆ_ಗಂಟು &>(other);
    ಹಿಂತಿರುಗಿ 0==u_memcmp(s, o.s, length);
}

ಶೂನ್ಯ
UCharsTrieBuilder::UCTLinear_ಹೊಂದಿಕೆ_ಗಂಟು::ಲಿಖ(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->ಲಿಖ(builder);
    b.ಲಿಖ(s, length);
    ಸ್ಥಾನಾಂತರ=b.write_ಮೌಲ್ಯ_And_Type(has_ಮೌಲ್ಯ, value, b.get_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಗಂಟು(int32_t i, int32_t unit_ಸೂಚ್ಯಂಕ, int32_t length,
                                         Node *ಮುಂದಿನ_ಗಂಟು) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಹೊಸ UCTLinear_ಹೊಂದಿಕೆ_ಗಂಟು(
            elements[i].getString(ಅಕ್ಷರಸರಣಿಗಳು).get_ಬಫರ್()+unit_ಸೂಚ್ಯಂಕ,
            length,
            ಮುಂದಿನ_ಗಂಟು);
}

UBool
UCharsTrieBuilder::ensure_ಸಾಮರ್ಥ್ಯ(int32_t length) {
    ಆದರೆ(uchars==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;  // previous memory allocation had failed
    }
    ಆದರೆ(length>uchars_ಸಾಮರ್ಥ್ಯ) {
        int32_t new_ಸಾಮರ್ಥ್ಯ=uchars_ಸಾಮರ್ಥ್ಯ;
        ಮಾಡು {
            new_ಸಾಮರ್ಥ್ಯ*=2;
        } ಆಗ(new_ಸಾಮರ್ಥ್ಯ<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(new_ಸಾಮರ್ಥ್ಯ*2));
        ಆದರೆ(newUChars==ಶೂನ್ಯಸೂಚಕ) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=ಶೂನ್ಯಸೂಚಕ;
            uchars_ಸಾಮರ್ಥ್ಯ=0;
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        u_memcpy(newUChars+(new_ಸಾಮರ್ಥ್ಯ-uchars_ಉದ್ದಳತೆ),
                 uchars+(uchars_ಸಾಮರ್ಥ್ಯ-uchars_ಉದ್ದಳತೆ), uchars_ಉದ್ದಳತೆ);
        uprv_free(uchars);
        uchars=newUChars;
        uchars_ಸಾಮರ್ಥ್ಯ=new_ಸಾಮರ್ಥ್ಯ;
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

int32_t
UCharsTrieBuilder::ಲಿಖ(int32_t unit) {
    int32_t new_ಉದ್ದಳತೆ=uchars_ಉದ್ದಳತೆ+1;
    ಆದರೆ(ensure_ಸಾಮರ್ಥ್ಯ(new_ಉದ್ದಳತೆ)) {
        uchars_ಉದ್ದಳತೆ=new_ಉದ್ದಳತೆ;
        uchars[uchars_ಸಾಮರ್ಥ್ಯ - uchars_ಉದ್ದಳತೆ] = static_cast<char16_t>(unit);
    }
    ಹಿಂತಿರುಗಿ uchars_ಉದ್ದಳತೆ;
}

int32_t
UCharsTrieBuilder::ಲಿಖ(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length) {
    int32_t new_ಉದ್ದಳತೆ=uchars_ಉದ್ದಳತೆ+length;
    ಆದರೆ(ensure_ಸಾಮರ್ಥ್ಯ(new_ಉದ್ದಳತೆ)) {
        uchars_ಉದ್ದಳತೆ=new_ಉದ್ದಳತೆ;
        u_memcpy(uchars+(uchars_ಸಾಮರ್ಥ್ಯ-uchars_ಉದ್ದಳತೆ), s, length);
    }
    ಹಿಂತಿರುಗಿ uchars_ಉದ್ದಳತೆ;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unit_ಸೂಚ್ಯಂಕ, int32_t length) {
    ಹಿಂತಿರುಗಿ ಲಿಖ(elements[i].getString(ಅಕ್ಷರಸರಣಿಗಳು).get_ಬಫರ್()+unit_ಸೂಚ್ಯಂಕ, length);
}

int32_t
UCharsTrieBuilder::write_ಮೌಲ್ಯ_And_Final(int32_t i, UBool isFinal) {
    ಆದರೆ(0<=i && i<=UCharsTrie::k_Max_One_Unit_ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ ಲಿಖ(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    ಆದರೆ(i<0 || i>UCharsTrie::k_Max_Two_Unit_ಮೌಲ್ಯ) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_ಮೌಲ್ಯ_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } ಇಲ್ಲದಿದ್ದರೆ {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_ಮೌಲ್ಯ_Lead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    ಹಿಂತಿರುಗಿ ಲಿಖ(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_ಮೌಲ್ಯ_And_Type(UBool has_ಮೌಲ್ಯ, int32_t value, int32_t ಗಂಟು) {
    ಆದರೆ(!has_ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ ಲಿಖ(ಗಂಟು);
    }
    char16_t intUnits[3];
    int32_t length;
    ಆದರೆ(value<0 || value>UCharsTrie::k_Max_Two_Unit_ಗಂಟು_ಮೌಲ್ಯ) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_ಗಂಟು_ಮೌಲ್ಯ_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(value<=UCharsTrie::k_Max_One_Unit_ಗಂಟು_ಮೌಲ್ಯ) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } ಇಲ್ಲದಿದ್ದರೆ {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_ಗಂಟು_ಮೌಲ್ಯ_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(ಗಂಟು);
    ಹಿಂತಿರುಗಿ ಲಿಖ(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_Delta_ಗಮ್ಯ(int32_t jump_ಗುರಿ) {
    int32_t i=uchars_ಉದ್ದಳತೆ-jump_ಗುರಿ;
    U_ASSERT(i>=0);
    ಆದರೆ(i<=UCharsTrie::kMaxOneUnitDelta) {
        ಹಿಂತಿರುಗಿ ಲಿಖ(i);
    }
    char16_t intUnits[3];
    int32_t length;
    ಆದರೆ(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } ಇಲ್ಲದಿದ್ದರೆ {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    ಹಿಂತಿರುಗಿ ಲಿಖ(intUnits, length);
}

U_NAMESPACE_ಕೊನೆ
