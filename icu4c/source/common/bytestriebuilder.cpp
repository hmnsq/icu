// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  bytestriebuilder.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010sep25
*   created by: Markus W. Scherer
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestriebuilder.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "uarrsort.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (bytes, value) pairs with full copies
 * of the byte sequences, until the BytesTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

ವರ್ಗ ಬೈಟುಗಳು_Trie_Element : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    // Use compiler's default constructor, initializes nothing.

    ಶೂನ್ಯ ಗಣ_ಗಮ್ಯ(StringPiece s, int32_t val, CharString &ಅಕ್ಷರಸರಣಿಗಳು, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    StringPiece getString(ಸ್ಥಿರಾಂಕ CharString& ಅಕ್ಷರಸರಣಿಗಳು U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
        int32_t ಸ್ಥಾನಾಂತರ=string_ಸ್ಥಾನಾಂತರ;
        int32_t length;
        ಆದರೆ(ಸ್ಥಾನಾಂತರ>=0) {
            length = static_cast<uint8_t>(ಅಕ್ಷರಸರಣಿಗಳು[ಸ್ಥಾನಾಂತರ++]);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಸ್ಥಾನಾಂತರ=~ಸ್ಥಾನಾಂತರ;
            length = (static_cast<int32_t>(static_cast<uint8_t>(ಅಕ್ಷರಸರಣಿಗಳು[ಸ್ಥಾನಾಂತರ])) << 8) | static_cast<uint8_t>(ಅಕ್ಷರಸರಣಿಗಳು[ಸ್ಥಾನಾಂತರ + 1]);
            ಸ್ಥಾನಾಂತರ+=2;
        }
        ಹಿಂತಿರುಗಿ StringPiece(ಅಕ್ಷರಸರಣಿಗಳು.ದತ್ತಾಂಶ()+ಸ್ಥಾನಾಂತರ, length);
    }
    int32_t get_String_ಉದ್ದಳತೆ(ಸ್ಥಿರಾಂಕ CharString &ಅಕ್ಷರಸರಣಿಗಳು) ಸ್ಥಿರಾಂಕ {
        int32_t ಸ್ಥಾನಾಂತರ=string_ಸ್ಥಾನಾಂತರ;
        ಆದರೆ(ಸ್ಥಾನಾಂತರ>=0) {
            ಹಿಂತಿರುಗಿ static_cast<uint8_t>(ಅಕ್ಷರಸರಣಿಗಳು[ಸ್ಥಾನಾಂತರ]);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಸ್ಥಾನಾಂತರ=~ಸ್ಥಾನಾಂತರ;
            ಹಿಂತಿರುಗಿ (static_cast<int32_t>(static_cast<uint8_t>(ಅಕ್ಷರಸರಣಿಗಳು[ಸ್ಥಾನಾಂತರ])) << 8) | static_cast<uint8_t>(ಅಕ್ಷರಸರಣಿಗಳು[ಸ್ಥಾನಾಂತರ + 1]);
        }
    }

    ಅಕ್ಷರ charAt(int32_t ಸೂಚ್ಯಂಕ, ಸ್ಥಿರಾಂಕ CharString &ಅಕ್ಷರಸರಣಿಗಳು) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ(ಅಕ್ಷರಸರಣಿಗಳು)[ಸೂಚ್ಯಂಕ]; }

    int32_t get_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ value; }

    int32_t compare_String_ಗಮ್ಯ(ಸ್ಥಿರಾಂಕ ಬೈಟುಗಳು_Trie_Element &o, ಸ್ಥಿರಾಂಕ CharString &ಅಕ್ಷರಸರಣಿಗಳು) ಸ್ಥಿರಾಂಕ;

ಖಾಸಗಿ:
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ದತ್ತಾಂಶ(ಸ್ಥಿರಾಂಕ CharString& ಅಕ್ಷರಸರಣಿಗಳು U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
        int32_t ಸ್ಥಾನಾಂತರ=string_ಸ್ಥಾನಾಂತರ;
        ಆದರೆ(ಸ್ಥಾನಾಂತರ>=0) {
            ++ಸ್ಥಾನಾಂತರ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಸ್ಥಾನಾಂತರ=~ಸ್ಥಾನಾಂತರ+2;
        }
        ಹಿಂತಿರುಗಿ ಅಕ್ಷರಸರಣಿಗಳು.ದತ್ತಾಂಶ()+ಸ್ಥಾನಾಂತರ;
    }

    // If the stringOffset is non-negative, then the first strings byte contains
    // the string length.
    // If the stringOffset is negative, then the first two strings bytes contain
    // the string length (big-endian), and the offset needs to be bit-inverted.
    // (Compared with a stringLength field here, this saves 3 bytes per string for most strings.)
    int32_t string_ಸ್ಥಾನಾಂತರ;
    int32_t value;
};

ಶೂನ್ಯ
ಬೈಟುಗಳು_Trie_Element::ಗಣ_ಗಮ್ಯ(StringPiece s, int32_t val,
                        CharString &ಅಕ್ಷರಸರಣಿಗಳು, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    int32_t length=s.length();
    ಆದರೆ(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    int32_t ಸ್ಥಾನಾಂತರ=ಅಕ್ಷರಸರಣಿಗಳು.length();
    ಆದರೆ(length>0xff) {
        ಸ್ಥಾನಾಂತರ=~ಸ್ಥಾನಾಂತರ;
        ಅಕ್ಷರಸರಣಿಗಳು.append(static_cast<ಅಕ್ಷರ>(length >> 8), error_ಸಂಕೇತ);
    }
    ಅಕ್ಷರಸರಣಿಗಳು.append(static_cast<ಅಕ್ಷರ>(length), error_ಸಂಕೇತ);
    string_ಸ್ಥಾನಾಂತರ=ಸ್ಥಾನಾಂತರ;
    value=val;
    ಅಕ್ಷರಸರಣಿಗಳು.append(s, error_ಸಂಕೇತ);
}

int32_t
ಬೈಟುಗಳು_Trie_Element::compare_String_ಗಮ್ಯ(ಸ್ಥಿರಾಂಕ ಬೈಟುಗಳು_Trie_Element &other, ಸ್ಥಿರಾಂಕ CharString &ಅಕ್ಷರಸರಣಿಗಳು) ಸ್ಥಿರಾಂಕ {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(ಅಕ್ಷರಸರಣಿಗಳು);
    StringPiece otherString=other.getString(ಅಕ್ಷರಸರಣಿಗಳು);
    int32_t ಉದ್ದಳತೆ_Diff=thisString.length()-otherString.length();
    int32_t common_ಉದ್ದಳತೆ;
    ಆದರೆ(ಉದ್ದಳತೆ_Diff<=0) {
        common_ಉದ್ದಳತೆ=thisString.length();
    } ಇಲ್ಲದಿದ್ದರೆ {
        common_ಉದ್ದಳತೆ=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.ದತ್ತಾಂಶ(), otherString.ದತ್ತಾಂಶ(), common_ಉದ್ದಳತೆ);
    ಹಿಂತಿರುಗಿ diff!=0 ? diff : ಉದ್ದಳತೆ_Diff;
}

ಬೈಟುಗಳು_Trie_Builder::ಬೈಟುಗಳು_Trie_Builder(UError_ಸಂಕೇತ &error_ಸಂಕೇತ)
        : ಅಕ್ಷರಸರಣಿಗಳು(ಶೂನ್ಯಸೂಚಕ), elements(ಶೂನ್ಯಸೂಚಕ), elements_ಸಾಮರ್ಥ್ಯ(0), elements_ಉದ್ದಳತೆ(0),
          ಬೈಟುಗಳು(ಶೂನ್ಯಸೂಚಕ), ಬೈಟುಗಳು_ಸಾಮರ್ಥ್ಯ(0), ಬೈಟುಗಳು_ಉದ್ದಳತೆ(0) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಅಕ್ಷರಸರಣಿಗಳು=ಹೊಸ CharString();
    ಆದರೆ(ಅಕ್ಷರಸರಣಿಗಳು==ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
}

ಬೈಟುಗಳು_Trie_Builder::~ಬೈಟುಗಳು_Trie_Builder() {
    ಅಳಿಸು ಅಕ್ಷರಸರಣಿಗಳು;
    ಅಳಿಸು[] elements;
    uprv_free(ಬೈಟುಗಳು);
}

ಬೈಟುಗಳು_Trie_Builder&
ಬೈಟುಗಳು_Trie_Builder::ಕೂಡಿಸು(StringPiece s, int32_t value, UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಆದರೆ(ಬೈಟುಗಳು_ಉದ್ದಳತೆ>0) {
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
        ಬೈಟುಗಳು_Trie_Element *newElements=ಹೊಸ ಬೈಟುಗಳು_Trie_Element[new_ಸಾಮರ್ಥ್ಯ];
        ಆದರೆ(newElements==ಶೂನ್ಯಸೂಚಕ) {
            error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ *ಇದು; // error instead of dereferencing null
        }
        ಆದರೆ(elements_ಉದ್ದಳತೆ>0) {
            uprv_memcpy(newElements, elements, (size_t)elements_ಉದ್ದಳತೆ*ಗಾತ್ರ(ಬೈಟುಗಳು_Trie_Element));
        }
        ಅಳಿಸು[] elements;
        elements=newElements;
        elements_ಸಾಮರ್ಥ್ಯ=new_ಸಾಮರ್ಥ್ಯ;
    }
    elements[elements_ಉದ್ದಳತೆ++].ಗಣ_ಗಮ್ಯ(s, value, *ಅಕ್ಷರಸರಣಿಗಳು, error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ *ಇದು;
}

U_CDECL_BEGIN

ಸ್ಥಿರ int32_t U_CALLCONV
compare_Element_ಅಕ್ಷರಸರಣಿಗಳು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಎಡ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಲ) {
    ಸ್ಥಿರಾಂಕ CharString *ಅಕ್ಷರಸರಣಿಗಳು=static_cast<ಸ್ಥಿರಾಂಕ CharString *>(context);
    ಸ್ಥಿರಾಂಕ ಬೈಟುಗಳು_Trie_Element *ಎಡ_Element=static_cast<ಸ್ಥಿರಾಂಕ ಬೈಟುಗಳು_Trie_Element *>(ಎಡ);
    ಸ್ಥಿರಾಂಕ ಬೈಟುಗಳು_Trie_Element *ಬಲ_Element=static_cast<ಸ್ಥಿರಾಂಕ ಬೈಟುಗಳು_Trie_Element *>(ಬಲ);
    ಹಿಂತಿರುಗಿ ಎಡ_Element->compare_String_ಗಮ್ಯ(*ಬಲ_Element, *ಅಕ್ಷರಸರಣಿಗಳು);
}

U_CDECL_ಕೊನೆ

ಬೈಟುಗಳು_Trie *
ಬೈಟುಗಳು_Trie_Builder::build(UStringTrieBuildOption buildOption, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    build_ಬೈಟುಗಳು(buildOption, error_ಸಂಕೇತ);
    ಬೈಟುಗಳು_Trie *newTrie=ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
        newTrie=ಹೊಸ ಬೈಟುಗಳು_Trie(ಬೈಟುಗಳು, ಬೈಟುಗಳು+(ಬೈಟುಗಳು_ಸಾಮರ್ಥ್ಯ-ಬೈಟುಗಳು_ಉದ್ದಳತೆ));
        ಆದರೆ(newTrie==ಶೂನ್ಯಸೂಚಕ) {
            error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಬೈಟುಗಳು=ಶೂನ್ಯಸೂಚಕ;  // The new trie now owns the array.
            ಬೈಟುಗಳು_ಸಾಮರ್ಥ್ಯ=0;
        }
    }
    ಹಿಂತಿರುಗಿ newTrie;
}

StringPiece
ಬೈಟುಗಳು_Trie_Builder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND {
    build_ಬೈಟುಗಳು(buildOption, error_ಸಂಕೇತ);
    StringPiece ಫಲಿತಾಂಶ;
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
        ಫಲಿತಾಂಶ.ಗಣ(ಬೈಟುಗಳು+(ಬೈಟುಗಳು_ಸಾಮರ್ಥ್ಯ-ಬೈಟುಗಳು_ಉದ್ದಳತೆ), ಬೈಟುಗಳು_ಉದ್ದಳತೆ);
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ
ಬೈಟುಗಳು_Trie_Builder::build_ಬೈಟುಗಳು(UStringTrieBuildOption buildOption, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ(ಬೈಟುಗಳು!=ಶೂನ್ಯಸೂಚಕ && ಬೈಟುಗಳು_ಉದ್ದಳತೆ>0) {
        // Already built.
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ(ಬೈಟುಗಳು_ಉದ್ದಳತೆ==0) {
        ಆದರೆ(elements_ಉದ್ದಳತೆ==0) {
            error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        uprv_ವಿಂಗಡಿಸು_Array(elements, elements_ಉದ್ದಳತೆ, static_cast<int32_t>(ಗಾತ್ರ(ಬೈಟುಗಳು_Trie_Element)),
                      compare_Element_ಅಕ್ಷರಸರಣಿಗಳು, ಅಕ್ಷರಸರಣಿಗಳು,
                      ಸುಳ್ಳು,  // need not be a stable sort
                      &error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*ಅಕ್ಷರಸರಣಿಗಳು);
        ಗಾಗಿ(int32_t i=1; i<elements_ಉದ್ದಳತೆ; ++i) {
            StringPiece current=elements[i].getString(*ಅಕ್ಷರಸರಣಿಗಳು);
            ಆದರೆ(prev==current) {
                error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    ಬೈಟುಗಳು_ಉದ್ದಳತೆ=0;
    int32_t capacity=ಅಕ್ಷರಸರಣಿಗಳು->length();
    ಆದರೆ(capacity<1024) {
        capacity=1024;
    }
    ಆದರೆ(ಬೈಟುಗಳು_ಸಾಮರ್ಥ್ಯ<capacity) {
        uprv_free(ಬೈಟುಗಳು);
        ಬೈಟುಗಳು=static_cast<ಅಕ್ಷರ *>(uprv_malloc(capacity));
        ಆದರೆ(ಬೈಟುಗಳು==ಶೂನ್ಯಸೂಚಕ) {
            error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಬೈಟುಗಳು_ಸಾಮರ್ಥ್ಯ=0;
            ಹಿಂತಿರುಗಿ;
        }
        ಬೈಟುಗಳು_ಸಾಮರ್ಥ್ಯ=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_ಉದ್ದಳತೆ, error_ಸಂಕೇತ);
    ಆದರೆ(ಬೈಟುಗಳು==ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
}

ಬೈಟುಗಳು_Trie_Builder&
ಬೈಟುಗಳು_Trie_Builder::clear() U_LIFETIME_BOUND {
    ಅಕ್ಷರಸರಣಿಗಳು->clear();
    elements_ಉದ್ದಳತೆ=0;
    ಬೈಟುಗಳು_ಉದ್ದಳತೆ=0;
    ಹಿಂತಿರುಗಿ *ಇದು;
}

int32_t
ಬೈಟುಗಳು_Trie_Builder::get_Element_String_ಉದ್ದಳತೆ(int32_t i) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ elements[i].get_String_ಉದ್ದಳತೆ(*ಅಕ್ಷರಸರಣಿಗಳು);
}

char16_t
ಬೈಟುಗಳು_Trie_Builder::getElementUnit(int32_t i, int32_t byte_ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ static_cast<uint8_t>(elements[i].charAt(byte_ಸೂಚ್ಯಂಕ, *ಅಕ್ಷರಸರಣಿಗಳು));
}

int32_t
ಬೈಟುಗಳು_Trie_Builder::get_Element_ಮೌಲ್ಯ(int32_t i) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ elements[i].get_ಮೌಲ್ಯ();
}

int32_t
ಬೈಟುಗಳು_Trie_Builder::get_ಎಲ್ಲೆ_ದ_ರೇಖೀಯ_ಹೊಂದಿಕೆ(int32_t first, int32_t ಕೊನೆಯ, int32_t byte_ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ {
    ಸ್ಥಿರಾಂಕ ಬೈಟುಗಳು_Trie_Element &ಮೊದಲ_Element=elements[first];
    ಸ್ಥಿರಾಂಕ ಬೈಟುಗಳು_Trie_Element &ಕೊನೆಯ_Element=elements[ಕೊನೆಯ];
    int32_t min_String_ಉದ್ದಳತೆ=ಮೊದಲ_Element.get_String_ಉದ್ದಳತೆ(*ಅಕ್ಷರಸರಣಿಗಳು);
    ಆಗ(++byte_ಸೂಚ್ಯಂಕ<min_String_ಉದ್ದಳತೆ &&
            ಮೊದಲ_Element.charAt(byte_ಸೂಚ್ಯಂಕ, *ಅಕ್ಷರಸರಣಿಗಳು)==
            ಕೊನೆಯ_Element.charAt(byte_ಸೂಚ್ಯಂಕ, *ಅಕ್ಷರಸರಣಿಗಳು)) {}
    ಹಿಂತಿರುಗಿ byte_ಸೂಚ್ಯಂಕ;
}

int32_t
ಬೈಟುಗಳು_Trie_Builder::ಎಣಿಕೆ_Element_Units(int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ, int32_t byte_ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=ಆರಂಭ;
    ಮಾಡು {
        ಅಕ್ಷರ byte=elements[i++].charAt(byte_ಸೂಚ್ಯಂಕ, *ಅಕ್ಷರಸರಣಿಗಳು);
        ಆಗ(i<ಎಲ್ಲೆ && byte==elements[i].charAt(byte_ಸೂಚ್ಯಂಕ, *ಅಕ್ಷರಸರಣಿಗಳು)) {
            ++i;
        }
        ++length;
    } ಆಗ(i<ಎಲ್ಲೆ);
    ಹಿಂತಿರುಗಿ length;
}

int32_t
ಬೈಟುಗಳು_Trie_Builder::skipElementsBySomeUnits(int32_t i, int32_t byte_ಸೂಚ್ಯಂಕ, int32_t count) ಸ್ಥಿರಾಂಕ {
    ಮಾಡು {
        ಅಕ್ಷರ byte=elements[i++].charAt(byte_ಸೂಚ್ಯಂಕ, *ಅಕ್ಷರಸರಣಿಗಳು);
        ಆಗ(byte==elements[i].charAt(byte_ಸೂಚ್ಯಂಕ, *ಅಕ್ಷರಸರಣಿಗಳು)) {
            ++i;
        }
    } ಆಗ(--count>0);
    ಹಿಂತಿರುಗಿ i;
}

int32_t
ಬೈಟುಗಳು_Trie_Builder::ಸೂಚ್ಯಂಕ_ದ_Element_With_ಮುಂದಿನ_Unit(int32_t i, int32_t byte_ಸೂಚ್ಯಂಕ, char16_t byte) ಸ್ಥಿರಾಂಕ {
    ಅಕ್ಷರ b = static_cast<ಅಕ್ಷರ>(byte);
    ಆಗ(b==elements[i].charAt(byte_ಸೂಚ್ಯಂಕ, *ಅಕ್ಷರಸರಣಿಗಳು)) {
        ++i;
    }
    ಹಿಂತಿರುಗಿ i;
}

ಬೈಟುಗಳು_Trie_Builder::BTLinear_ಹೊಂದಿಕೆ_ಗಂಟು::BTLinear_ಹೊಂದಿಕೆ_ಗಂಟು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು, int32_t len, Node *ಮುಂದಿನ_ಗಂಟು)
        : ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಗಂಟು(len, ಮುಂದಿನ_ಗಂಟು), s(ಬೈಟುಗಳು) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(ಬೈಟುಗಳು, len)));
}

ತಾರ್ಕಿಕ
ಬೈಟುಗಳು_Trie_Builder::BTLinear_ಹೊಂದಿಕೆ_ಗಂಟು::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Node &other) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(ಇದು==&other) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ(!ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಗಂಟು::ಕ್ರಿಯಾಕಾರಕ==(other)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಸ್ಥಿರಾಂಕ BTLinear_ಹೊಂದಿಕೆ_ಗಂಟು &o=static_cast<ಸ್ಥಿರಾಂಕ BTLinear_ಹೊಂದಿಕೆ_ಗಂಟು &>(other);
    ಹಿಂತಿರುಗಿ 0==uprv_memcmp(s, o.s, length);
}

ಶೂನ್ಯ
ಬೈಟುಗಳು_Trie_Builder::BTLinear_ಹೊಂದಿಕೆ_ಗಂಟು::ಲಿಖ(StringTrieBuilder &builder) {
    ಬೈಟುಗಳು_Trie_Builder &b=static_cast<ಬೈಟುಗಳು_Trie_Builder &>(builder);
    next->ಲಿಖ(builder);
    b.ಲಿಖ(s, length);
    ಸ್ಥಾನಾಂತರ=b.ಲಿಖ(b.get_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ()+length-1);
}

StringTrieBuilder::Node *
ಬೈಟುಗಳು_Trie_Builder::create_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಗಂಟು(int32_t i, int32_t byte_ಸೂಚ್ಯಂಕ, int32_t length,
                                        Node *ಮುಂದಿನ_ಗಂಟು) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಹೊಸ BTLinear_ಹೊಂದಿಕೆ_ಗಂಟು(
            elements[i].getString(*ಅಕ್ಷರಸರಣಿಗಳು).ದತ್ತಾಂಶ()+byte_ಸೂಚ್ಯಂಕ,
            length,
            ಮುಂದಿನ_ಗಂಟು);
}

UBool
ಬೈಟುಗಳು_Trie_Builder::ensure_ಸಾಮರ್ಥ್ಯ(int32_t length) {
    ಆದರೆ(ಬೈಟುಗಳು==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;  // previous memory allocation had failed
    }
    ಆದರೆ(length>ಬೈಟುಗಳು_ಸಾಮರ್ಥ್ಯ) {
        int32_t new_ಸಾಮರ್ಥ್ಯ=ಬೈಟುಗಳು_ಸಾಮರ್ಥ್ಯ;
        ಮಾಡು {
            new_ಸಾಮರ್ಥ್ಯ*=2;
        } ಆಗ(new_ಸಾಮರ್ಥ್ಯ<=length);
        ಅಕ್ಷರ *new_ಬೈಟುಗಳು=static_cast<ಅಕ್ಷರ *>(uprv_malloc(new_ಸಾಮರ್ಥ್ಯ));
        ಆದರೆ(new_ಬೈಟುಗಳು==ಶೂನ್ಯಸೂಚಕ) {
            // unable to allocate memory
            uprv_free(ಬೈಟುಗಳು);
            ಬೈಟುಗಳು=ಶೂನ್ಯಸೂಚಕ;
            ಬೈಟುಗಳು_ಸಾಮರ್ಥ್ಯ=0;
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        uprv_memcpy(new_ಬೈಟುಗಳು+(new_ಸಾಮರ್ಥ್ಯ-ಬೈಟುಗಳು_ಉದ್ದಳತೆ),
                    ಬೈಟುಗಳು+(ಬೈಟುಗಳು_ಸಾಮರ್ಥ್ಯ-ಬೈಟುಗಳು_ಉದ್ದಳತೆ), ಬೈಟುಗಳು_ಉದ್ದಳತೆ);
        uprv_free(ಬೈಟುಗಳು);
        ಬೈಟುಗಳು=new_ಬೈಟುಗಳು;
        ಬೈಟುಗಳು_ಸಾಮರ್ಥ್ಯ=new_ಸಾಮರ್ಥ್ಯ;
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

int32_t
ಬೈಟುಗಳು_Trie_Builder::ಲಿಖ(int32_t byte) {
    int32_t new_ಉದ್ದಳತೆ=ಬೈಟುಗಳು_ಉದ್ದಳತೆ+1;
    ಆದರೆ(ensure_ಸಾಮರ್ಥ್ಯ(new_ಉದ್ದಳತೆ)) {
        ಬೈಟುಗಳು_ಉದ್ದಳತೆ=new_ಉದ್ದಳತೆ;
        ಬೈಟುಗಳು[ಬೈಟುಗಳು_ಸಾಮರ್ಥ್ಯ - ಬೈಟುಗಳು_ಉದ್ದಳತೆ] = static_cast<ಅಕ್ಷರ>(byte);
    }
    ಹಿಂತಿರುಗಿ ಬೈಟುಗಳು_ಉದ್ದಳತೆ;
}

int32_t
ಬೈಟುಗಳು_Trie_Builder::ಲಿಖ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b, int32_t length) {
    int32_t new_ಉದ್ದಳತೆ=ಬೈಟುಗಳು_ಉದ್ದಳತೆ+length;
    ಆದರೆ(ensure_ಸಾಮರ್ಥ್ಯ(new_ಉದ್ದಳತೆ)) {
        ಬೈಟುಗಳು_ಉದ್ದಳತೆ=new_ಉದ್ದಳತೆ;
        uprv_memcpy(ಬೈಟುಗಳು+(ಬೈಟುಗಳು_ಸಾಮರ್ಥ್ಯ-ಬೈಟುಗಳು_ಉದ್ದಳತೆ), b, length);
    }
    ಹಿಂತಿರುಗಿ ಬೈಟುಗಳು_ಉದ್ದಳತೆ;
}

int32_t
ಬೈಟುಗಳು_Trie_Builder::writeElementUnits(int32_t i, int32_t byte_ಸೂಚ್ಯಂಕ, int32_t length) {
    ಹಿಂತಿರುಗಿ ಲಿಖ(elements[i].getString(*ಅಕ್ಷರಸರಣಿಗಳು).ದತ್ತಾಂಶ()+byte_ಸೂಚ್ಯಂಕ, length);
}

int32_t
ಬೈಟುಗಳು_Trie_Builder::write_ಮೌಲ್ಯ_And_Final(int32_t i, UBool isFinal) {
    ಆದರೆ(0<=i && i<=ಬೈಟುಗಳು_Trie::k_Max_One_Byte_ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ ಲಿಖ(((ಬೈಟುಗಳು_Trie::k_Min_One_Byte_ಮೌಲ್ಯ_Lead+i)<<1)|isFinal);
    }
    ಅಕ್ಷರ int_ಬೈಟುಗಳು[5];
    int32_t length=1;
    ಆದರೆ(i<0 || i>0xffffff) {
        int_ಬೈಟುಗಳು[0] = static_cast<ಅಕ್ಷರ>(ಬೈಟುಗಳು_Trie::k_Five_Byte_ಮೌಲ್ಯ_Lead);
        int_ಬೈಟುಗಳು[1] = static_cast<ಅಕ್ಷರ>(static_cast<uint32_t>(i) >> 24);
        int_ಬೈಟುಗಳು[2] = static_cast<ಅಕ್ಷರ>(static_cast<uint32_t>(i) >> 16);
        int_ಬೈಟುಗಳು[3] = static_cast<ಅಕ್ಷರ>(static_cast<uint32_t>(i) >> 8);
        int_ಬೈಟುಗಳು[4] = static_cast<ಅಕ್ಷರ>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(i<=ಬೈಟುಗಳು_Trie::k_Max_Two_Byte_ಮೌಲ್ಯ) {
            int_ಬೈಟುಗಳು[0] = static_cast<ಅಕ್ಷರ>(ಬೈಟುಗಳು_Trie::k_Min_Two_Byte_ಮೌಲ್ಯ_Lead + (i >> 8));
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ(i<=ಬೈಟುಗಳು_Trie::k_Max_Three_Byte_ಮೌಲ್ಯ) {
                int_ಬೈಟುಗಳು[0] = static_cast<ಅಕ್ಷರ>(ಬೈಟುಗಳು_Trie::k_Min_Three_Byte_ಮೌಲ್ಯ_Lead + (i >> 16));
            } ಇಲ್ಲದಿದ್ದರೆ {
                int_ಬೈಟುಗಳು[0] = static_cast<ಅಕ್ಷರ>(ಬೈಟುಗಳು_Trie::k_Four_Byte_ಮೌಲ್ಯ_Lead);
                int_ಬೈಟುಗಳು[1] = static_cast<ಅಕ್ಷರ>(i >> 16);
                length=2;
            }
            int_ಬೈಟುಗಳು[length++] = static_cast<ಅಕ್ಷರ>(i >> 8);
        }
        int_ಬೈಟುಗಳು[length++] = static_cast<ಅಕ್ಷರ>(i);
    }
    int_ಬೈಟುಗಳು[0] = static_cast<ಅಕ್ಷರ>((int_ಬೈಟುಗಳು[0] << 1) | isFinal);
    ಹಿಂತಿರುಗಿ ಲಿಖ(int_ಬೈಟುಗಳು, length);
}

int32_t
ಬೈಟುಗಳು_Trie_Builder::write_ಮೌಲ್ಯ_And_Type(UBool has_ಮೌಲ್ಯ, int32_t value, int32_t ಗಂಟು) {
    int32_t ಸ್ಥಾನಾಂತರ=ಲಿಖ(ಗಂಟು);
    ಆದರೆ(has_ಮೌಲ್ಯ) {
        ಸ್ಥಾನಾಂತರ=write_ಮೌಲ್ಯ_And_Final(value, ಸುಳ್ಳು);
    }
    ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;
}

int32_t
ಬೈಟುಗಳು_Trie_Builder::write_Delta_ಗಮ್ಯ(int32_t jump_ಗುರಿ) {
    int32_t i=ಬೈಟುಗಳು_ಉದ್ದಳತೆ-jump_ಗುರಿ;
    U_ASSERT(i>=0);
    ಆದರೆ(i<=ಬೈಟುಗಳು_Trie::kMaxOneByteDelta) {
        ಹಿಂತಿರುಗಿ ಲಿಖ(i);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಅಕ್ಷರ int_ಬೈಟುಗಳು[5];
        ಹಿಂತಿರುಗಿ ಲಿಖ(int_ಬೈಟುಗಳು, ಆಂತರಿಕ_Encode_Delta(i, int_ಬೈಟುಗಳು));
    }
}

int32_t
ಬೈಟುಗಳು_Trie_Builder::ಆಂತರಿಕ_Encode_Delta(int32_t i, ಅಕ್ಷರ int_ಬೈಟುಗಳು[]) {
    U_ASSERT(i>=0);
    ಆದರೆ(i<=ಬೈಟುಗಳು_Trie::kMaxOneByteDelta) {
        int_ಬೈಟುಗಳು[0] = static_cast<ಅಕ್ಷರ>(i);
        ಹಿಂತಿರುಗಿ 1;
    }
    int32_t length=1;
    ಆದರೆ(i<=ಬೈಟುಗಳು_Trie::kMaxTwoByteDelta) {
        int_ಬೈಟುಗಳು[0] = static_cast<ಅಕ್ಷರ>(ಬೈಟುಗಳು_Trie::kMinTwoByteDeltaLead + (i >> 8));
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(i<=ಬೈಟುಗಳು_Trie::kMaxThreeByteDelta) {
            int_ಬೈಟುಗಳು[0] = static_cast<ಅಕ್ಷರ>(ಬೈಟುಗಳು_Trie::kMinThreeByteDeltaLead + (i >> 16));
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ(i<=0xffffff) {
                int_ಬೈಟುಗಳು[0] = static_cast<ಅಕ್ಷರ>(ಬೈಟುಗಳು_Trie::kFourByteDeltaLead);
            } ಇಲ್ಲದಿದ್ದರೆ {
                int_ಬೈಟುಗಳು[0] = static_cast<ಅಕ್ಷರ>(ಬೈಟುಗಳು_Trie::kFiveByteDeltaLead);
                int_ಬೈಟುಗಳು[1] = static_cast<ಅಕ್ಷರ>(i >> 24);
                length=2;
            }
            int_ಬೈಟುಗಳು[length++] = static_cast<ಅಕ್ಷರ>(i >> 16);
        }
        int_ಬೈಟುಗಳು[length++] = static_cast<ಅಕ್ಷರ>(i >> 8);
    }
    int_ಬೈಟುಗಳು[length++] = static_cast<ಅಕ್ಷರ>(i);
    ಹಿಂತಿರುಗಿ length;
}

U_NAMESPACE_ಕೊನೆ
