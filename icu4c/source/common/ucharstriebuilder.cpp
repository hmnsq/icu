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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucharstrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucharstriebuilder.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uarrsort.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (string, value) pairs with full copies
 * of the 16-bit-unit sequences, until the UCharsTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

ਵਰਗ UCharsTrieElement : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    // Use compiler's default constructor, initializes nothing.

    ਖਾਲੀ ਸੈੱਟ_ਮੰਜ਼ਿਲ(ਅਚਲ UnicodeString &s, int32_t val, UnicodeString &ਲੜੀਆਂ, UError_ਕੋਡ &error_ਕੋਡ);

    UnicodeString getString(ਅਚਲ UnicodeString& ਲੜੀਆਂ U_LIFETIME_BOUND) ਅਚਲ {
        int32_t length=ਲੜੀਆਂ[string_ਵਿੱਥ];
        ਵਾਪਸ ਲੜੀਆਂ.ਆਰਜ਼ੀ_Sub_String(string_ਵਿੱਥ+1, length);
    }
    int32_t get_String_ਲੰਮਾਈ(ਅਚਲ UnicodeString &ਲੜੀਆਂ) ਅਚਲ {
        ਵਾਪਸ ਲੜੀਆਂ[string_ਵਿੱਥ];
    }

    char16_t charAt(int32_t ਸੂਚਕਾਂਕ, ਅਚਲ UnicodeString &ਲੜੀਆਂ) ਅਚਲ {
        ਵਾਪਸ ਲੜੀਆਂ[string_ਵਿੱਥ+1+ਸੂਚਕਾਂਕ];
    }

    int32_t get_ਮੁੱਲ() ਅਚਲ { ਵਾਪਸ value; }

    int32_t compare_String_ਮੰਜ਼ਿਲ(ਅਚਲ UCharsTrieElement &o, ਅਚਲ UnicodeString &ਲੜੀਆਂ) ਅਚਲ;

ਨਿਜੀ:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t string_ਵਿੱਥ;
    int32_t value;
};

ਖਾਲੀ
UCharsTrieElement::ਸੈੱਟ_ਮੰਜ਼ਿਲ(ਅਚਲ UnicodeString &s, int32_t val,
                         UnicodeString &ਲੜੀਆਂ, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    int32_t length=s.length();
    ਜੇ(length>0xffff) {
        // Too long: We store the length in 1 unit.
        error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ;
    }
    string_ਵਿੱਥ=ਲੜੀਆਂ.length();
    ਲੜੀਆਂ.append(static_cast<char16_t>(length));
    value=val;
    ਲੜੀਆਂ.append(s);
}

int32_t
UCharsTrieElement::compare_String_ਮੰਜ਼ਿਲ(ਅਚਲ UCharsTrieElement &other, ਅਚਲ UnicodeString &ਲੜੀਆਂ) ਅਚਲ {
    ਵਾਪਸ getString(ਲੜੀਆਂ).compare(other.getString(ਲੜੀਆਂ));
}

UCharsTrieBuilder::UCharsTrieBuilder(UError_ਕੋਡ & /*errorCode*/)
        : elements(ਖਾਲੀ_ਸੂਚਕ), elements_ਸਮਰੱਥਾ(0), elements_ਲੰਮਾਈ(0),
          uchars(ਖਾਲੀ_ਸੂਚਕ), uchars_ਸਮਰੱਥਾ(0), uchars_ਲੰਮਾਈ(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    ਮਿਟਾਓ[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::ਜੋੜੋ(ਅਚਲ UnicodeString& s,
                       int32_t value,
                       UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ *ਇਹ;
    }
    ਜੇ(uchars_ਲੰਮਾਈ>0) {
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
        UCharsTrieElement *newElements=ਨਵਾਂ UCharsTrieElement[new_ਸਮਰੱਥਾ];
        ਜੇ(newElements==ਖਾਲੀ_ਸੂਚਕ) {
            error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ *ਇਹ;
        }
        ਜੇ(elements_ਲੰਮਾਈ>0) {
            uprv_memcpy(newElements, elements, (size_t)elements_ਲੰਮਾਈ*ਆਕਾਰ(UCharsTrieElement));
        }
        ਮਿਟਾਓ[] elements;
        elements=newElements;
        elements_ਸਮਰੱਥਾ=new_ਸਮਰੱਥਾ;
    }
    elements[elements_ਲੰਮਾਈ++].ਸੈੱਟ_ਮੰਜ਼ਿਲ(s, value, ਲੜੀਆਂ, error_ਕੋਡ);
    ਜੇ(U_SUCCESS(error_ਕੋਡ) && ਲੜੀਆਂ.isBogus()) {
        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
    ਵਾਪਸ *ਇਹ;
}

U_CDECL_BEGIN

ਸਥਿਰ int32_t U_CALLCONV
compare_Element_ਲੜੀਆਂ(ਅਚਲ ਖਾਲੀ *context, ਅਚਲ ਖਾਲੀ *ਖੱਬੇ, ਅਚਲ ਖਾਲੀ *ਸੱਜੇ) {
    ਅਚਲ UnicodeString *ਲੜੀਆਂ=static_cast<ਅਚਲ UnicodeString *>(context);
    ਅਚਲ UCharsTrieElement *ਖੱਬਾ_Element=static_cast<ਅਚਲ UCharsTrieElement *>(ਖੱਬੇ);
    ਅਚਲ UCharsTrieElement *ਸੱਜਾ_Element=static_cast<ਅਚਲ UCharsTrieElement *>(ਸੱਜੇ);
    ਵਾਪਸ ਖੱਬਾ_Element->compare_String_ਮੰਜ਼ਿਲ(*ਸੱਜਾ_Element, *ਲੜੀਆਂ);
}

U_CDECL_ਸਿਰਾ

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UError_ਕੋਡ &error_ਕੋਡ) {
    buildUChars(buildOption, error_ਕੋਡ);
    UCharsTrie *newTrie=ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
        newTrie=ਨਵਾਂ UCharsTrie(uchars, uchars+(uchars_ਸਮਰੱਥਾ-uchars_ਲੰਮਾਈ));
        ਜੇ(newTrie==ਖਾਲੀ_ਸੂਚਕ) {
            error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        } ਨਹੀਂਤਾਂ {
            uchars=ਖਾਲੀ_ਸੂਚਕ;  // The new trie now owns the array.
            uchars_ਸਮਰੱਥਾ=0;
        }
    }
    ਵਾਪਸ newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND,
                                      UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND {
    buildUChars(buildOption, error_ਕੋਡ);
    ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
        ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ(ਝੂਠ, uchars+(uchars_ਸਮਰੱਥਾ-uchars_ਲੰਮਾਈ), uchars_ਲੰਮਾਈ);
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਜੇ(uchars!=ਖਾਲੀ_ਸੂਚਕ && uchars_ਲੰਮਾਈ>0) {
        // Already built.
        ਵਾਪਸ;
    }
    ਜੇ(uchars_ਲੰਮਾਈ==0) {
        ਜੇ(elements_ਲੰਮਾਈ==0) {
            error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ;
        }
        ਜੇ(ਲੜੀਆਂ.isBogus()) {
            error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
        }
        uprv_ਛਾਂਟੋ_Array(elements, elements_ਲੰਮਾਈ, static_cast<int32_t>(ਆਕਾਰ(UCharsTrieElement)),
                      compare_Element_ਲੜੀਆਂ, &ਲੜੀਆਂ,
                      ਝੂਠ,  // need not be a stable sort
                      &error_ਕੋਡ);
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(ਲੜੀਆਂ);
        ਲਈ(int32_t i=1; i<elements_ਲੰਮਾਈ; ++i) {
            UnicodeString current=elements[i].getString(ਲੜੀਆਂ);
            ਜੇ(prev==current) {
                error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
                ਵਾਪਸ;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    uchars_ਲੰਮਾਈ=0;
    int32_t capacity=ਲੜੀਆਂ.length();
    ਜੇ(capacity<1024) {
        capacity=1024;
    }
    ਜੇ(uchars_ਸਮਰੱਥਾ<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        ਜੇ(uchars==ਖਾਲੀ_ਸੂਚਕ) {
            error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            uchars_ਸਮਰੱਥਾ=0;
            ਵਾਪਸ;
        }
        uchars_ਸਮਰੱਥਾ=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_ਲੰਮਾਈ, error_ਕੋਡ);
    ਜੇ(uchars==ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
}

int32_t
UCharsTrieBuilder::get_Element_String_ਲੰਮਾਈ(int32_t i) ਅਚਲ {
    ਵਾਪਸ elements[i].get_String_ਲੰਮਾਈ(ਲੜੀਆਂ);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unit_ਸੂਚਕਾਂਕ) ਅਚਲ {
    ਵਾਪਸ elements[i].charAt(unit_ਸੂਚਕਾਂਕ, ਲੜੀਆਂ);
}

int32_t
UCharsTrieBuilder::get_Element_ਮੁੱਲ(int32_t i) ਅਚਲ {
    ਵਾਪਸ elements[i].get_ਮੁੱਲ();
}

int32_t
UCharsTrieBuilder::get_ਹੱਦਬੰਦੀ_ਦਾ_ਰੇਖਿਕ_ਮੇਲ(int32_t first, int32_t ਆਖਰੀ, int32_t unit_ਸੂਚਕਾਂਕ) ਅਚਲ {
    ਅਚਲ UCharsTrieElement &ਪਹਿਲਾ_Element=elements[first];
    ਅਚਲ UCharsTrieElement &ਆਖਰੀ_Element=elements[ਆਖਰੀ];
    int32_t min_String_ਲੰਮਾਈ=ਪਹਿਲਾ_Element.get_String_ਲੰਮਾਈ(ਲੜੀਆਂ);
    ਜਦੋਂ(++unit_ਸੂਚਕਾਂਕ<min_String_ਲੰਮਾਈ &&
            ਪਹਿਲਾ_Element.charAt(unit_ਸੂਚਕਾਂਕ, ਲੜੀਆਂ)==
            ਆਖਰੀ_Element.charAt(unit_ਸੂਚਕਾਂਕ, ਲੜੀਆਂ)) {}
    ਵਾਪਸ unit_ਸੂਚਕਾਂਕ;
}

int32_t
UCharsTrieBuilder::ਗਿਣਤੀ_Element_Units(int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ, int32_t unit_ਸੂਚਕਾਂਕ) ਅਚਲ {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=ਸ਼ੁਰੂ;
    ਕਰੋ {
        char16_t unit=elements[i++].charAt(unit_ਸੂਚਕਾਂਕ, ਲੜੀਆਂ);
        ਜਦੋਂ(i<ਹੱਦਬੰਦੀ && unit==elements[i].charAt(unit_ਸੂਚਕਾਂਕ, ਲੜੀਆਂ)) {
            ++i;
        }
        ++length;
    } ਜਦੋਂ(i<ਹੱਦਬੰਦੀ);
    ਵਾਪਸ length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unit_ਸੂਚਕਾਂਕ, int32_t count) ਅਚਲ {
    ਕਰੋ {
        char16_t unit=elements[i++].charAt(unit_ਸੂਚਕਾਂਕ, ਲੜੀਆਂ);
        ਜਦੋਂ(unit==elements[i].charAt(unit_ਸੂਚਕਾਂਕ, ਲੜੀਆਂ)) {
            ++i;
        }
    } ਜਦੋਂ(--count>0);
    ਵਾਪਸ i;
}

int32_t
UCharsTrieBuilder::ਸੂਚਕਾਂਕ_ਦਾ_Element_With_ਅਗਲਾ_Unit(int32_t i, int32_t unit_ਸੂਚਕਾਂਕ, char16_t unit) ਅਚਲ {
    ਜਦੋਂ(unit==elements[i].charAt(unit_ਸੂਚਕਾਂਕ, ਲੜੀਆਂ)) {
        ++i;
    }
    ਵਾਪਸ i;
}

UCharsTrieBuilder::UCTLinear_ਮੇਲ_ਗੰਢ::UCTLinear_ਮੇਲ_ਗੰਢ(ਅਚਲ char16_t *units, int32_t len, Node *ਅਗਲਾ_ਗੰਢ)
        : ਰੇਖਿਕ_ਮੇਲ_ਗੰਢ(len, ਅਗਲਾ_ਗੰਢ), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

ਤਾਰਕਿਕ
UCharsTrieBuilder::UCTLinear_ਮੇਲ_ਗੰਢ::ਸੰਚਾਲਕ==(ਅਚਲ Node &other) ਅਚਲ {
    ਜੇ(ਇਹ==&other) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ(!ਰੇਖਿਕ_ਮੇਲ_ਗੰਢ::ਸੰਚਾਲਕ==(other)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਅਚਲ UCTLinear_ਮੇਲ_ਗੰਢ &o=static_cast<ਅਚਲ UCTLinear_ਮੇਲ_ਗੰਢ &>(other);
    ਵਾਪਸ 0==u_memcmp(s, o.s, length);
}

ਖਾਲੀ
UCharsTrieBuilder::UCTLinear_ਮੇਲ_ਗੰਢ::ਲਿਖ(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->ਲਿਖ(builder);
    b.ਲਿਖ(s, length);
    ਵਿੱਥ=b.write_ਮੁੱਲ_And_Type(has_ਮੁੱਲ, value, b.get_Min_ਰੇਖਿਕ_ਮੇਲ()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_ਰੇਖਿਕ_ਮੇਲ_ਗੰਢ(int32_t i, int32_t unit_ਸੂਚਕਾਂਕ, int32_t length,
                                         Node *ਅਗਲਾ_ਗੰਢ) ਅਚਲ {
    ਵਾਪਸ ਨਵਾਂ UCTLinear_ਮੇਲ_ਗੰਢ(
            elements[i].getString(ਲੜੀਆਂ).get_ਬਫਰ()+unit_ਸੂਚਕਾਂਕ,
            length,
            ਅਗਲਾ_ਗੰਢ);
}

UBool
UCharsTrieBuilder::ensure_ਸਮਰੱਥਾ(int32_t length) {
    ਜੇ(uchars==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਝੂਠ;  // previous memory allocation had failed
    }
    ਜੇ(length>uchars_ਸਮਰੱਥਾ) {
        int32_t new_ਸਮਰੱਥਾ=uchars_ਸਮਰੱਥਾ;
        ਕਰੋ {
            new_ਸਮਰੱਥਾ*=2;
        } ਜਦੋਂ(new_ਸਮਰੱਥਾ<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(new_ਸਮਰੱਥਾ*2));
        ਜੇ(newUChars==ਖਾਲੀ_ਸੂਚਕ) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=ਖਾਲੀ_ਸੂਚਕ;
            uchars_ਸਮਰੱਥਾ=0;
            ਵਾਪਸ ਝੂਠ;
        }
        u_memcpy(newUChars+(new_ਸਮਰੱਥਾ-uchars_ਲੰਮਾਈ),
                 uchars+(uchars_ਸਮਰੱਥਾ-uchars_ਲੰਮਾਈ), uchars_ਲੰਮਾਈ);
        uprv_free(uchars);
        uchars=newUChars;
        uchars_ਸਮਰੱਥਾ=new_ਸਮਰੱਥਾ;
    }
    ਵਾਪਸ ਸੱਚ;
}

int32_t
UCharsTrieBuilder::ਲਿਖ(int32_t unit) {
    int32_t new_ਲੰਮਾਈ=uchars_ਲੰਮਾਈ+1;
    ਜੇ(ensure_ਸਮਰੱਥਾ(new_ਲੰਮਾਈ)) {
        uchars_ਲੰਮਾਈ=new_ਲੰਮਾਈ;
        uchars[uchars_ਸਮਰੱਥਾ - uchars_ਲੰਮਾਈ] = static_cast<char16_t>(unit);
    }
    ਵਾਪਸ uchars_ਲੰਮਾਈ;
}

int32_t
UCharsTrieBuilder::ਲਿਖ(ਅਚਲ char16_t *s, int32_t length) {
    int32_t new_ਲੰਮਾਈ=uchars_ਲੰਮਾਈ+length;
    ਜੇ(ensure_ਸਮਰੱਥਾ(new_ਲੰਮਾਈ)) {
        uchars_ਲੰਮਾਈ=new_ਲੰਮਾਈ;
        u_memcpy(uchars+(uchars_ਸਮਰੱਥਾ-uchars_ਲੰਮਾਈ), s, length);
    }
    ਵਾਪਸ uchars_ਲੰਮਾਈ;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unit_ਸੂਚਕਾਂਕ, int32_t length) {
    ਵਾਪਸ ਲਿਖ(elements[i].getString(ਲੜੀਆਂ).get_ਬਫਰ()+unit_ਸੂਚਕਾਂਕ, length);
}

int32_t
UCharsTrieBuilder::write_ਮੁੱਲ_And_Final(int32_t i, UBool isFinal) {
    ਜੇ(0<=i && i<=UCharsTrie::k_Max_One_Unit_ਮੁੱਲ) {
        ਵਾਪਸ ਲਿਖ(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    ਜੇ(i<0 || i>UCharsTrie::k_Max_Two_Unit_ਮੁੱਲ) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_ਮੁੱਲ_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } ਨਹੀਂਤਾਂ {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_ਮੁੱਲ_Lead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    ਵਾਪਸ ਲਿਖ(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_ਮੁੱਲ_And_Type(UBool has_ਮੁੱਲ, int32_t value, int32_t ਗੰਢ) {
    ਜੇ(!has_ਮੁੱਲ) {
        ਵਾਪਸ ਲਿਖ(ਗੰਢ);
    }
    char16_t intUnits[3];
    int32_t length;
    ਜੇ(value<0 || value>UCharsTrie::k_Max_Two_Unit_ਗੰਢ_ਮੁੱਲ) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_ਗੰਢ_ਮੁੱਲ_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } ਨਹੀਂਤਾਂ ਜੇ(value<=UCharsTrie::k_Max_One_Unit_ਗੰਢ_ਮੁੱਲ) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } ਨਹੀਂਤਾਂ {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_ਗੰਢ_ਮੁੱਲ_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(ਗੰਢ);
    ਵਾਪਸ ਲਿਖ(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_Delta_ਮੰਜ਼ਿਲ(int32_t jump_ਨਿਸ਼ਾਨਾ) {
    int32_t i=uchars_ਲੰਮਾਈ-jump_ਨਿਸ਼ਾਨਾ;
    U_ASSERT(i>=0);
    ਜੇ(i<=UCharsTrie::kMaxOneUnitDelta) {
        ਵਾਪਸ ਲਿਖ(i);
    }
    char16_t intUnits[3];
    int32_t length;
    ਜੇ(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } ਨਹੀਂਤਾਂ {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    ਵਾਪਸ ਲਿਖ(intUnits, length);
}

U_NAMESPACE_ਸਿਰਾ
