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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestriebuilder.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringpiece.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uarrsort.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (bytes, value) pairs with full copies
 * of the byte sequences, until the BytesTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

ਵਰਗ ਬਾਈਟਾਂ_Trie_Element : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    // Use compiler's default constructor, initializes nothing.

    ਖਾਲੀ ਸੈੱਟ_ਮੰਜ਼ਿਲ(StringPiece s, int32_t val, CharString &ਲੜੀਆਂ, UError_ਕੋਡ &error_ਕੋਡ);

    StringPiece getString(ਅਚਲ CharString& ਲੜੀਆਂ U_LIFETIME_BOUND) ਅਚਲ {
        int32_t ਵਿੱਥ=string_ਵਿੱਥ;
        int32_t length;
        ਜੇ(ਵਿੱਥ>=0) {
            length = static_cast<uint8_t>(ਲੜੀਆਂ[ਵਿੱਥ++]);
        } ਨਹੀਂਤਾਂ {
            ਵਿੱਥ=~ਵਿੱਥ;
            length = (static_cast<int32_t>(static_cast<uint8_t>(ਲੜੀਆਂ[ਵਿੱਥ])) << 8) | static_cast<uint8_t>(ਲੜੀਆਂ[ਵਿੱਥ + 1]);
            ਵਿੱਥ+=2;
        }
        ਵਾਪਸ StringPiece(ਲੜੀਆਂ.ਅੰਕੜੇ()+ਵਿੱਥ, length);
    }
    int32_t get_String_ਲੰਮਾਈ(ਅਚਲ CharString &ਲੜੀਆਂ) ਅਚਲ {
        int32_t ਵਿੱਥ=string_ਵਿੱਥ;
        ਜੇ(ਵਿੱਥ>=0) {
            ਵਾਪਸ static_cast<uint8_t>(ਲੜੀਆਂ[ਵਿੱਥ]);
        } ਨਹੀਂਤਾਂ {
            ਵਿੱਥ=~ਵਿੱਥ;
            ਵਾਪਸ (static_cast<int32_t>(static_cast<uint8_t>(ਲੜੀਆਂ[ਵਿੱਥ])) << 8) | static_cast<uint8_t>(ਲੜੀਆਂ[ਵਿੱਥ + 1]);
        }
    }

    ਅੱਖਰ charAt(int32_t ਸੂਚਕਾਂਕ, ਅਚਲ CharString &ਲੜੀਆਂ) ਅਚਲ { ਵਾਪਸ ਅੰਕੜੇ(ਲੜੀਆਂ)[ਸੂਚਕਾਂਕ]; }

    int32_t get_ਮੁੱਲ() ਅਚਲ { ਵਾਪਸ value; }

    int32_t compare_String_ਮੰਜ਼ਿਲ(ਅਚਲ ਬਾਈਟਾਂ_Trie_Element &o, ਅਚਲ CharString &ਲੜੀਆਂ) ਅਚਲ;

ਨਿਜੀ:
    ਅਚਲ ਅੱਖਰ* ਅੰਕੜੇ(ਅਚਲ CharString& ਲੜੀਆਂ U_LIFETIME_BOUND) ਅਚਲ {
        int32_t ਵਿੱਥ=string_ਵਿੱਥ;
        ਜੇ(ਵਿੱਥ>=0) {
            ++ਵਿੱਥ;
        } ਨਹੀਂਤਾਂ {
            ਵਿੱਥ=~ਵਿੱਥ+2;
        }
        ਵਾਪਸ ਲੜੀਆਂ.ਅੰਕੜੇ()+ਵਿੱਥ;
    }

    // If the stringOffset is non-negative, then the first strings byte contains
    // the string length.
    // If the stringOffset is negative, then the first two strings bytes contain
    // the string length (big-endian), and the offset needs to be bit-inverted.
    // (Compared with a stringLength field here, this saves 3 bytes per string for most strings.)
    int32_t string_ਵਿੱਥ;
    int32_t value;
};

ਖਾਲੀ
ਬਾਈਟਾਂ_Trie_Element::ਸੈੱਟ_ਮੰਜ਼ਿਲ(StringPiece s, int32_t val,
                        CharString &ਲੜੀਆਂ, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    int32_t length=s.length();
    ਜੇ(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ;
    }
    int32_t ਵਿੱਥ=ਲੜੀਆਂ.length();
    ਜੇ(length>0xff) {
        ਵਿੱਥ=~ਵਿੱਥ;
        ਲੜੀਆਂ.append(static_cast<ਅੱਖਰ>(length >> 8), error_ਕੋਡ);
    }
    ਲੜੀਆਂ.append(static_cast<ਅੱਖਰ>(length), error_ਕੋਡ);
    string_ਵਿੱਥ=ਵਿੱਥ;
    value=val;
    ਲੜੀਆਂ.append(s, error_ਕੋਡ);
}

int32_t
ਬਾਈਟਾਂ_Trie_Element::compare_String_ਮੰਜ਼ਿਲ(ਅਚਲ ਬਾਈਟਾਂ_Trie_Element &other, ਅਚਲ CharString &ਲੜੀਆਂ) ਅਚਲ {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(ਲੜੀਆਂ);
    StringPiece otherString=other.getString(ਲੜੀਆਂ);
    int32_t ਲੰਮਾਈ_Diff=thisString.length()-otherString.length();
    int32_t common_ਲੰਮਾਈ;
    ਜੇ(ਲੰਮਾਈ_Diff<=0) {
        common_ਲੰਮਾਈ=thisString.length();
    } ਨਹੀਂਤਾਂ {
        common_ਲੰਮਾਈ=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.ਅੰਕੜੇ(), otherString.ਅੰਕੜੇ(), common_ਲੰਮਾਈ);
    ਵਾਪਸ diff!=0 ? diff : ਲੰਮਾਈ_Diff;
}

ਬਾਈਟਾਂ_Trie_Builder::ਬਾਈਟਾਂ_Trie_Builder(UError_ਕੋਡ &error_ਕੋਡ)
        : ਲੜੀਆਂ(ਖਾਲੀ_ਸੂਚਕ), elements(ਖਾਲੀ_ਸੂਚਕ), elements_ਸਮਰੱਥਾ(0), elements_ਲੰਮਾਈ(0),
          ਬਾਈਟਾਂ(ਖਾਲੀ_ਸੂਚਕ), ਬਾਈਟਾਂ_ਸਮਰੱਥਾ(0), ਬਾਈਟਾਂ_ਲੰਮਾਈ(0) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਲੜੀਆਂ=ਨਵਾਂ CharString();
    ਜੇ(ਲੜੀਆਂ==ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
}

ਬਾਈਟਾਂ_Trie_Builder::~ਬਾਈਟਾਂ_Trie_Builder() {
    ਮਿਟਾਓ ਲੜੀਆਂ;
    ਮਿਟਾਓ[] elements;
    uprv_free(ਬਾਈਟਾਂ);
}

ਬਾਈਟਾਂ_Trie_Builder&
ਬਾਈਟਾਂ_Trie_Builder::ਜੋੜੋ(StringPiece s, int32_t value, UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ *ਇਹ;
    }
    ਜੇ(ਬਾਈਟਾਂ_ਲੰਮਾਈ>0) {
        // Cannot add elements after building.
        error_ਕੋਡ=U_NO_WRITE_PERMISSION;
        ਵਾਪਸ *ਇਹ;
    }
    ਜੇ(elements_ਲੰਮਾਈ==elements_ਸਮਰੱਥਾ) {
        int32_t new_ਸਮਰੱਥਾ;
        ਜੇ(elements_ਸਮਰੱਥਾ==0) {
            new_ਸਮਰੱਥਾ=1024;
        } ਨਹੀਂਤਾਂ {
            new_ਸਮਰੱਥਾ=4*elements_ਸਮਰੱਥਾ;
        }
        ਬਾਈਟਾਂ_Trie_Element *newElements=ਨਵਾਂ ਬਾਈਟਾਂ_Trie_Element[new_ਸਮਰੱਥਾ];
        ਜੇ(newElements==ਖਾਲੀ_ਸੂਚਕ) {
            error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ *ਇਹ; // error instead of dereferencing null
        }
        ਜੇ(elements_ਲੰਮਾਈ>0) {
            uprv_memcpy(newElements, elements, (size_t)elements_ਲੰਮਾਈ*ਆਕਾਰ(ਬਾਈਟਾਂ_Trie_Element));
        }
        ਮਿਟਾਓ[] elements;
        elements=newElements;
        elements_ਸਮਰੱਥਾ=new_ਸਮਰੱਥਾ;
    }
    elements[elements_ਲੰਮਾਈ++].ਸੈੱਟ_ਮੰਜ਼ਿਲ(s, value, *ਲੜੀਆਂ, error_ਕੋਡ);
    ਵਾਪਸ *ਇਹ;
}

U_CDECL_BEGIN

ਸਥਿਰ int32_t U_CALLCONV
compare_Element_ਲੜੀਆਂ(ਅਚਲ ਖਾਲੀ *context, ਅਚਲ ਖਾਲੀ *ਖੱਬੇ, ਅਚਲ ਖਾਲੀ *ਸੱਜੇ) {
    ਅਚਲ CharString *ਲੜੀਆਂ=static_cast<ਅਚਲ CharString *>(context);
    ਅਚਲ ਬਾਈਟਾਂ_Trie_Element *ਖੱਬਾ_Element=static_cast<ਅਚਲ ਬਾਈਟਾਂ_Trie_Element *>(ਖੱਬੇ);
    ਅਚਲ ਬਾਈਟਾਂ_Trie_Element *ਸੱਜਾ_Element=static_cast<ਅਚਲ ਬਾਈਟਾਂ_Trie_Element *>(ਸੱਜੇ);
    ਵਾਪਸ ਖੱਬਾ_Element->compare_String_ਮੰਜ਼ਿਲ(*ਸੱਜਾ_Element, *ਲੜੀਆਂ);
}

U_CDECL_ਸਿਰਾ

ਬਾਈਟਾਂ_Trie *
ਬਾਈਟਾਂ_Trie_Builder::build(UStringTrieBuildOption buildOption, UError_ਕੋਡ &error_ਕੋਡ) {
    build_ਬਾਈਟਾਂ(buildOption, error_ਕੋਡ);
    ਬਾਈਟਾਂ_Trie *newTrie=ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
        newTrie=ਨਵਾਂ ਬਾਈਟਾਂ_Trie(ਬਾਈਟਾਂ, ਬਾਈਟਾਂ+(ਬਾਈਟਾਂ_ਸਮਰੱਥਾ-ਬਾਈਟਾਂ_ਲੰਮਾਈ));
        ਜੇ(newTrie==ਖਾਲੀ_ਸੂਚਕ) {
            error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        } ਨਹੀਂਤਾਂ {
            ਬਾਈਟਾਂ=ਖਾਲੀ_ਸੂਚਕ;  // The new trie now owns the array.
            ਬਾਈਟਾਂ_ਸਮਰੱਥਾ=0;
        }
    }
    ਵਾਪਸ newTrie;
}

StringPiece
ਬਾਈਟਾਂ_Trie_Builder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND {
    build_ਬਾਈਟਾਂ(buildOption, error_ਕੋਡ);
    StringPiece ਨਤੀਜਾ;
    ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
        ਨਤੀਜਾ.ਸੈੱਟ(ਬਾਈਟਾਂ+(ਬਾਈਟਾਂ_ਸਮਰੱਥਾ-ਬਾਈਟਾਂ_ਲੰਮਾਈ), ਬਾਈਟਾਂ_ਲੰਮਾਈ);
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ
ਬਾਈਟਾਂ_Trie_Builder::build_ਬਾਈਟਾਂ(UStringTrieBuildOption buildOption, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਜੇ(ਬਾਈਟਾਂ!=ਖਾਲੀ_ਸੂਚਕ && ਬਾਈਟਾਂ_ਲੰਮਾਈ>0) {
        // Already built.
        ਵਾਪਸ;
    }
    ਜੇ(ਬਾਈਟਾਂ_ਲੰਮਾਈ==0) {
        ਜੇ(elements_ਲੰਮਾਈ==0) {
            error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ;
        }
        uprv_ਛਾਂਟੋ_Array(elements, elements_ਲੰਮਾਈ, static_cast<int32_t>(ਆਕਾਰ(ਬਾਈਟਾਂ_Trie_Element)),
                      compare_Element_ਲੜੀਆਂ, ਲੜੀਆਂ,
                      ਝੂਠ,  // need not be a stable sort
                      &error_ਕੋਡ);
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*ਲੜੀਆਂ);
        ਲਈ(int32_t i=1; i<elements_ਲੰਮਾਈ; ++i) {
            StringPiece current=elements[i].getString(*ਲੜੀਆਂ);
            ਜੇ(prev==current) {
                error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
                ਵਾਪਸ;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    ਬਾਈਟਾਂ_ਲੰਮਾਈ=0;
    int32_t capacity=ਲੜੀਆਂ->length();
    ਜੇ(capacity<1024) {
        capacity=1024;
    }
    ਜੇ(ਬਾਈਟਾਂ_ਸਮਰੱਥਾ<capacity) {
        uprv_free(ਬਾਈਟਾਂ);
        ਬਾਈਟਾਂ=static_cast<ਅੱਖਰ *>(uprv_malloc(capacity));
        ਜੇ(ਬਾਈਟਾਂ==ਖਾਲੀ_ਸੂਚਕ) {
            error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਬਾਈਟਾਂ_ਸਮਰੱਥਾ=0;
            ਵਾਪਸ;
        }
        ਬਾਈਟਾਂ_ਸਮਰੱਥਾ=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_ਲੰਮਾਈ, error_ਕੋਡ);
    ਜੇ(ਬਾਈਟਾਂ==ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
}

ਬਾਈਟਾਂ_Trie_Builder&
ਬਾਈਟਾਂ_Trie_Builder::clear() U_LIFETIME_BOUND {
    ਲੜੀਆਂ->clear();
    elements_ਲੰਮਾਈ=0;
    ਬਾਈਟਾਂ_ਲੰਮਾਈ=0;
    ਵਾਪਸ *ਇਹ;
}

int32_t
ਬਾਈਟਾਂ_Trie_Builder::get_Element_String_ਲੰਮਾਈ(int32_t i) ਅਚਲ {
    ਵਾਪਸ elements[i].get_String_ਲੰਮਾਈ(*ਲੜੀਆਂ);
}

char16_t
ਬਾਈਟਾਂ_Trie_Builder::getElementUnit(int32_t i, int32_t byte_ਸੂਚਕਾਂਕ) ਅਚਲ {
    ਵਾਪਸ static_cast<uint8_t>(elements[i].charAt(byte_ਸੂਚਕਾਂਕ, *ਲੜੀਆਂ));
}

int32_t
ਬਾਈਟਾਂ_Trie_Builder::get_Element_ਮੁੱਲ(int32_t i) ਅਚਲ {
    ਵਾਪਸ elements[i].get_ਮੁੱਲ();
}

int32_t
ਬਾਈਟਾਂ_Trie_Builder::get_ਹੱਦਬੰਦੀ_ਦਾ_ਰੇਖਿਕ_ਮੇਲ(int32_t first, int32_t ਆਖਰੀ, int32_t byte_ਸੂਚਕਾਂਕ) ਅਚਲ {
    ਅਚਲ ਬਾਈਟਾਂ_Trie_Element &ਪਹਿਲਾ_Element=elements[first];
    ਅਚਲ ਬਾਈਟਾਂ_Trie_Element &ਆਖਰੀ_Element=elements[ਆਖਰੀ];
    int32_t min_String_ਲੰਮਾਈ=ਪਹਿਲਾ_Element.get_String_ਲੰਮਾਈ(*ਲੜੀਆਂ);
    ਜਦੋਂ(++byte_ਸੂਚਕਾਂਕ<min_String_ਲੰਮਾਈ &&
            ਪਹਿਲਾ_Element.charAt(byte_ਸੂਚਕਾਂਕ, *ਲੜੀਆਂ)==
            ਆਖਰੀ_Element.charAt(byte_ਸੂਚਕਾਂਕ, *ਲੜੀਆਂ)) {}
    ਵਾਪਸ byte_ਸੂਚਕਾਂਕ;
}

int32_t
ਬਾਈਟਾਂ_Trie_Builder::ਗਿਣਤੀ_Element_Units(int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ, int32_t byte_ਸੂਚਕਾਂਕ) ਅਚਲ {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=ਸ਼ੁਰੂ;
    ਕਰੋ {
        ਅੱਖਰ byte=elements[i++].charAt(byte_ਸੂਚਕਾਂਕ, *ਲੜੀਆਂ);
        ਜਦੋਂ(i<ਹੱਦਬੰਦੀ && byte==elements[i].charAt(byte_ਸੂਚਕਾਂਕ, *ਲੜੀਆਂ)) {
            ++i;
        }
        ++length;
    } ਜਦੋਂ(i<ਹੱਦਬੰਦੀ);
    ਵਾਪਸ length;
}

int32_t
ਬਾਈਟਾਂ_Trie_Builder::skipElementsBySomeUnits(int32_t i, int32_t byte_ਸੂਚਕਾਂਕ, int32_t count) ਅਚਲ {
    ਕਰੋ {
        ਅੱਖਰ byte=elements[i++].charAt(byte_ਸੂਚਕਾਂਕ, *ਲੜੀਆਂ);
        ਜਦੋਂ(byte==elements[i].charAt(byte_ਸੂਚਕਾਂਕ, *ਲੜੀਆਂ)) {
            ++i;
        }
    } ਜਦੋਂ(--count>0);
    ਵਾਪਸ i;
}

int32_t
ਬਾਈਟਾਂ_Trie_Builder::ਸੂਚਕਾਂਕ_ਦਾ_Element_With_ਅਗਲਾ_Unit(int32_t i, int32_t byte_ਸੂਚਕਾਂਕ, char16_t byte) ਅਚਲ {
    ਅੱਖਰ b = static_cast<ਅੱਖਰ>(byte);
    ਜਦੋਂ(b==elements[i].charAt(byte_ਸੂਚਕਾਂਕ, *ਲੜੀਆਂ)) {
        ++i;
    }
    ਵਾਪਸ i;
}

ਬਾਈਟਾਂ_Trie_Builder::BTLinear_ਮੇਲ_ਗੰਢ::BTLinear_ਮੇਲ_ਗੰਢ(ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ, int32_t len, Node *ਅਗਲਾ_ਗੰਢ)
        : ਰੇਖਿਕ_ਮੇਲ_ਗੰਢ(len, ਅਗਲਾ_ਗੰਢ), s(ਬਾਈਟਾਂ) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(ਬਾਈਟਾਂ, len)));
}

ਤਾਰਕਿਕ
ਬਾਈਟਾਂ_Trie_Builder::BTLinear_ਮੇਲ_ਗੰਢ::ਸੰਚਾਲਕ==(ਅਚਲ Node &other) ਅਚਲ {
    ਜੇ(ਇਹ==&other) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ(!ਰੇਖਿਕ_ਮੇਲ_ਗੰਢ::ਸੰਚਾਲਕ==(other)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਅਚਲ BTLinear_ਮੇਲ_ਗੰਢ &o=static_cast<ਅਚਲ BTLinear_ਮੇਲ_ਗੰਢ &>(other);
    ਵਾਪਸ 0==uprv_memcmp(s, o.s, length);
}

ਖਾਲੀ
ਬਾਈਟਾਂ_Trie_Builder::BTLinear_ਮੇਲ_ਗੰਢ::ਲਿਖ(StringTrieBuilder &builder) {
    ਬਾਈਟਾਂ_Trie_Builder &b=static_cast<ਬਾਈਟਾਂ_Trie_Builder &>(builder);
    next->ਲਿਖ(builder);
    b.ਲਿਖ(s, length);
    ਵਿੱਥ=b.ਲਿਖ(b.get_Min_ਰੇਖਿਕ_ਮੇਲ()+length-1);
}

StringTrieBuilder::Node *
ਬਾਈਟਾਂ_Trie_Builder::create_ਰੇਖਿਕ_ਮੇਲ_ਗੰਢ(int32_t i, int32_t byte_ਸੂਚਕਾਂਕ, int32_t length,
                                        Node *ਅਗਲਾ_ਗੰਢ) ਅਚਲ {
    ਵਾਪਸ ਨਵਾਂ BTLinear_ਮੇਲ_ਗੰਢ(
            elements[i].getString(*ਲੜੀਆਂ).ਅੰਕੜੇ()+byte_ਸੂਚਕਾਂਕ,
            length,
            ਅਗਲਾ_ਗੰਢ);
}

UBool
ਬਾਈਟਾਂ_Trie_Builder::ensure_ਸਮਰੱਥਾ(int32_t length) {
    ਜੇ(ਬਾਈਟਾਂ==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਝੂਠ;  // previous memory allocation had failed
    }
    ਜੇ(length>ਬਾਈਟਾਂ_ਸਮਰੱਥਾ) {
        int32_t new_ਸਮਰੱਥਾ=ਬਾਈਟਾਂ_ਸਮਰੱਥਾ;
        ਕਰੋ {
            new_ਸਮਰੱਥਾ*=2;
        } ਜਦੋਂ(new_ਸਮਰੱਥਾ<=length);
        ਅੱਖਰ *new_ਬਾਈਟਾਂ=static_cast<ਅੱਖਰ *>(uprv_malloc(new_ਸਮਰੱਥਾ));
        ਜੇ(new_ਬਾਈਟਾਂ==ਖਾਲੀ_ਸੂਚਕ) {
            // unable to allocate memory
            uprv_free(ਬਾਈਟਾਂ);
            ਬਾਈਟਾਂ=ਖਾਲੀ_ਸੂਚਕ;
            ਬਾਈਟਾਂ_ਸਮਰੱਥਾ=0;
            ਵਾਪਸ ਝੂਠ;
        }
        uprv_memcpy(new_ਬਾਈਟਾਂ+(new_ਸਮਰੱਥਾ-ਬਾਈਟਾਂ_ਲੰਮਾਈ),
                    ਬਾਈਟਾਂ+(ਬਾਈਟਾਂ_ਸਮਰੱਥਾ-ਬਾਈਟਾਂ_ਲੰਮਾਈ), ਬਾਈਟਾਂ_ਲੰਮਾਈ);
        uprv_free(ਬਾਈਟਾਂ);
        ਬਾਈਟਾਂ=new_ਬਾਈਟਾਂ;
        ਬਾਈਟਾਂ_ਸਮਰੱਥਾ=new_ਸਮਰੱਥਾ;
    }
    ਵਾਪਸ ਸੱਚ;
}

int32_t
ਬਾਈਟਾਂ_Trie_Builder::ਲਿਖ(int32_t byte) {
    int32_t new_ਲੰਮਾਈ=ਬਾਈਟਾਂ_ਲੰਮਾਈ+1;
    ਜੇ(ensure_ਸਮਰੱਥਾ(new_ਲੰਮਾਈ)) {
        ਬਾਈਟਾਂ_ਲੰਮਾਈ=new_ਲੰਮਾਈ;
        ਬਾਈਟਾਂ[ਬਾਈਟਾਂ_ਸਮਰੱਥਾ - ਬਾਈਟਾਂ_ਲੰਮਾਈ] = static_cast<ਅੱਖਰ>(byte);
    }
    ਵਾਪਸ ਬਾਈਟਾਂ_ਲੰਮਾਈ;
}

int32_t
ਬਾਈਟਾਂ_Trie_Builder::ਲਿਖ(ਅਚਲ ਅੱਖਰ *b, int32_t length) {
    int32_t new_ਲੰਮਾਈ=ਬਾਈਟਾਂ_ਲੰਮਾਈ+length;
    ਜੇ(ensure_ਸਮਰੱਥਾ(new_ਲੰਮਾਈ)) {
        ਬਾਈਟਾਂ_ਲੰਮਾਈ=new_ਲੰਮਾਈ;
        uprv_memcpy(ਬਾਈਟਾਂ+(ਬਾਈਟਾਂ_ਸਮਰੱਥਾ-ਬਾਈਟਾਂ_ਲੰਮਾਈ), b, length);
    }
    ਵਾਪਸ ਬਾਈਟਾਂ_ਲੰਮਾਈ;
}

int32_t
ਬਾਈਟਾਂ_Trie_Builder::writeElementUnits(int32_t i, int32_t byte_ਸੂਚਕਾਂਕ, int32_t length) {
    ਵਾਪਸ ਲਿਖ(elements[i].getString(*ਲੜੀਆਂ).ਅੰਕੜੇ()+byte_ਸੂਚਕਾਂਕ, length);
}

int32_t
ਬਾਈਟਾਂ_Trie_Builder::write_ਮੁੱਲ_And_Final(int32_t i, UBool isFinal) {
    ਜੇ(0<=i && i<=ਬਾਈਟਾਂ_Trie::k_Max_One_Byte_ਮੁੱਲ) {
        ਵਾਪਸ ਲਿਖ(((ਬਾਈਟਾਂ_Trie::k_Min_One_Byte_ਮੁੱਲ_Lead+i)<<1)|isFinal);
    }
    ਅੱਖਰ int_ਬਾਈਟਾਂ[5];
    int32_t length=1;
    ਜੇ(i<0 || i>0xffffff) {
        int_ਬਾਈਟਾਂ[0] = static_cast<ਅੱਖਰ>(ਬਾਈਟਾਂ_Trie::k_Five_Byte_ਮੁੱਲ_Lead);
        int_ਬਾਈਟਾਂ[1] = static_cast<ਅੱਖਰ>(static_cast<uint32_t>(i) >> 24);
        int_ਬਾਈਟਾਂ[2] = static_cast<ਅੱਖਰ>(static_cast<uint32_t>(i) >> 16);
        int_ਬਾਈਟਾਂ[3] = static_cast<ਅੱਖਰ>(static_cast<uint32_t>(i) >> 8);
        int_ਬਾਈਟਾਂ[4] = static_cast<ਅੱਖਰ>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } ਨਹੀਂਤਾਂ {
        ਜੇ(i<=ਬਾਈਟਾਂ_Trie::k_Max_Two_Byte_ਮੁੱਲ) {
            int_ਬਾਈਟਾਂ[0] = static_cast<ਅੱਖਰ>(ਬਾਈਟਾਂ_Trie::k_Min_Two_Byte_ਮੁੱਲ_Lead + (i >> 8));
        } ਨਹੀਂਤਾਂ {
            ਜੇ(i<=ਬਾਈਟਾਂ_Trie::k_Max_Three_Byte_ਮੁੱਲ) {
                int_ਬਾਈਟਾਂ[0] = static_cast<ਅੱਖਰ>(ਬਾਈਟਾਂ_Trie::k_Min_Three_Byte_ਮੁੱਲ_Lead + (i >> 16));
            } ਨਹੀਂਤਾਂ {
                int_ਬਾਈਟਾਂ[0] = static_cast<ਅੱਖਰ>(ਬਾਈਟਾਂ_Trie::k_Four_Byte_ਮੁੱਲ_Lead);
                int_ਬਾਈਟਾਂ[1] = static_cast<ਅੱਖਰ>(i >> 16);
                length=2;
            }
            int_ਬਾਈਟਾਂ[length++] = static_cast<ਅੱਖਰ>(i >> 8);
        }
        int_ਬਾਈਟਾਂ[length++] = static_cast<ਅੱਖਰ>(i);
    }
    int_ਬਾਈਟਾਂ[0] = static_cast<ਅੱਖਰ>((int_ਬਾਈਟਾਂ[0] << 1) | isFinal);
    ਵਾਪਸ ਲਿਖ(int_ਬਾਈਟਾਂ, length);
}

int32_t
ਬਾਈਟਾਂ_Trie_Builder::write_ਮੁੱਲ_And_Type(UBool has_ਮੁੱਲ, int32_t value, int32_t ਗੰਢ) {
    int32_t ਵਿੱਥ=ਲਿਖ(ਗੰਢ);
    ਜੇ(has_ਮੁੱਲ) {
        ਵਿੱਥ=write_ਮੁੱਲ_And_Final(value, ਝੂਠ);
    }
    ਵਾਪਸ ਵਿੱਥ;
}

int32_t
ਬਾਈਟਾਂ_Trie_Builder::write_Delta_ਮੰਜ਼ਿਲ(int32_t jump_ਨਿਸ਼ਾਨਾ) {
    int32_t i=ਬਾਈਟਾਂ_ਲੰਮਾਈ-jump_ਨਿਸ਼ਾਨਾ;
    U_ASSERT(i>=0);
    ਜੇ(i<=ਬਾਈਟਾਂ_Trie::kMaxOneByteDelta) {
        ਵਾਪਸ ਲਿਖ(i);
    } ਨਹੀਂਤਾਂ {
        ਅੱਖਰ int_ਬਾਈਟਾਂ[5];
        ਵਾਪਸ ਲਿਖ(int_ਬਾਈਟਾਂ, ਅੰਦਰੂਨੀ_Encode_Delta(i, int_ਬਾਈਟਾਂ));
    }
}

int32_t
ਬਾਈਟਾਂ_Trie_Builder::ਅੰਦਰੂਨੀ_Encode_Delta(int32_t i, ਅੱਖਰ int_ਬਾਈਟਾਂ[]) {
    U_ASSERT(i>=0);
    ਜੇ(i<=ਬਾਈਟਾਂ_Trie::kMaxOneByteDelta) {
        int_ਬਾਈਟਾਂ[0] = static_cast<ਅੱਖਰ>(i);
        ਵਾਪਸ 1;
    }
    int32_t length=1;
    ਜੇ(i<=ਬਾਈਟਾਂ_Trie::kMaxTwoByteDelta) {
        int_ਬਾਈਟਾਂ[0] = static_cast<ਅੱਖਰ>(ਬਾਈਟਾਂ_Trie::kMinTwoByteDeltaLead + (i >> 8));
    } ਨਹੀਂਤਾਂ {
        ਜੇ(i<=ਬਾਈਟਾਂ_Trie::kMaxThreeByteDelta) {
            int_ਬਾਈਟਾਂ[0] = static_cast<ਅੱਖਰ>(ਬਾਈਟਾਂ_Trie::kMinThreeByteDeltaLead + (i >> 16));
        } ਨਹੀਂਤਾਂ {
            ਜੇ(i<=0xffffff) {
                int_ਬਾਈਟਾਂ[0] = static_cast<ਅੱਖਰ>(ਬਾਈਟਾਂ_Trie::kFourByteDeltaLead);
            } ਨਹੀਂਤਾਂ {
                int_ਬਾਈਟਾਂ[0] = static_cast<ਅੱਖਰ>(ਬਾਈਟਾਂ_Trie::kFiveByteDeltaLead);
                int_ਬਾਈਟਾਂ[1] = static_cast<ਅੱਖਰ>(i >> 24);
                length=2;
            }
            int_ਬਾਈਟਾਂ[length++] = static_cast<ਅੱਖਰ>(i >> 16);
        }
        int_ਬਾਈਟਾਂ[length++] = static_cast<ਅੱਖਰ>(i >> 8);
    }
    int_ਬਾਈਟਾਂ[length++] = static_cast<ਅੱਖਰ>(i);
    ਵਾਪਸ length;
}

U_NAMESPACE_ਸਿਰਾ
