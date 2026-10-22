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

#ضمن "unicode/utypes.h"
#ضمن "unicode/bytestrie.h"
#ضمن "unicode/bytestriebuilder.h"
#ضمن "unicode/stringpiece.h"
#ضمن "charstr.h"
#ضمن "cmemory.h"
#ضمن "uhash.h"
#ضمن "uarrsort.h"
#ضمن "uassert.h"
#ضمن "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (bytes, value) pairs with full copies
 * of the byte sequences, until the BytesTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

صنف بائٹاں_Trie_Element : عام UMemory {
عام:
    // Use compiler's default constructor, initializes nothing.

    فراغ سیٹ_منزل(StringPiece s, int32_t val, CharString &لڑیاں, UError_کوڈ &error_کوڈ);

    StringPiece getString(ثابت CharString& لڑیاں U_LIFETIME_BOUND) ثابت {
        int32_t وتھ=string_وتھ;
        int32_t length;
        إذا(وتھ>=0) {
            length = static_cast<uint8_t>(لڑیاں[وتھ++]);
        } وإلا {
            وتھ=~وتھ;
            length = (static_cast<int32_t>(static_cast<uint8_t>(لڑیاں[وتھ])) << 8) | static_cast<uint8_t>(لڑیاں[وتھ + 1]);
            وتھ+=2;
        }
        أرجع StringPiece(لڑیاں.انکڑے()+وتھ, length);
    }
    int32_t get_String_لمائی(ثابت CharString &لڑیاں) ثابت {
        int32_t وتھ=string_وتھ;
        إذا(وتھ>=0) {
            أرجع static_cast<uint8_t>(لڑیاں[وتھ]);
        } وإلا {
            وتھ=~وتھ;
            أرجع (static_cast<int32_t>(static_cast<uint8_t>(لڑیاں[وتھ])) << 8) | static_cast<uint8_t>(لڑیاں[وتھ + 1]);
        }
    }

    حرف charAt(int32_t اشاریہ, ثابت CharString &لڑیاں) ثابت { أرجع انکڑے(لڑیاں)[اشاریہ]; }

    int32_t get_قدر() ثابت { أرجع value; }

    int32_t compare_String_منزل(ثابت بائٹاں_Trie_Element &o, ثابت CharString &لڑیاں) ثابت;

خاص:
    ثابت حرف* انکڑے(ثابت CharString& لڑیاں U_LIFETIME_BOUND) ثابت {
        int32_t وتھ=string_وتھ;
        إذا(وتھ>=0) {
            ++وتھ;
        } وإلا {
            وتھ=~وتھ+2;
        }
        أرجع لڑیاں.انکڑے()+وتھ;
    }

    // If the stringOffset is non-negative, then the first strings byte contains
    // the string length.
    // If the stringOffset is negative, then the first two strings bytes contain
    // the string length (big-endian), and the offset needs to be bit-inverted.
    // (Compared with a stringLength field here, this saves 3 bytes per string for most strings.)
    int32_t string_وتھ;
    int32_t value;
};

فراغ
بائٹاں_Trie_Element::سیٹ_منزل(StringPiece s, int32_t val,
                        CharString &لڑیاں, UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    int32_t length=s.length();
    إذا(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع;
    }
    int32_t وتھ=لڑیاں.length();
    إذا(length>0xff) {
        وتھ=~وتھ;
        لڑیاں.append(static_cast<حرف>(length >> 8), error_کوڈ);
    }
    لڑیاں.append(static_cast<حرف>(length), error_کوڈ);
    string_وتھ=وتھ;
    value=val;
    لڑیاں.append(s, error_کوڈ);
}

int32_t
بائٹاں_Trie_Element::compare_String_منزل(ثابت بائٹاں_Trie_Element &other, ثابت CharString &لڑیاں) ثابت {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(لڑیاں);
    StringPiece otherString=other.getString(لڑیاں);
    int32_t لمائی_Diff=thisString.length()-otherString.length();
    int32_t common_لمائی;
    إذا(لمائی_Diff<=0) {
        common_لمائی=thisString.length();
    } وإلا {
        common_لمائی=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.انکڑے(), otherString.انکڑے(), common_لمائی);
    أرجع diff!=0 ? diff : لمائی_Diff;
}

بائٹاں_Trie_Builder::بائٹاں_Trie_Builder(UError_کوڈ &error_کوڈ)
        : لڑیاں(خالی_سوچک), elements(خالی_سوچک), elements_سمرتھا(0), elements_لمائی(0),
          بائٹاں(خالی_سوچک), بائٹاں_سمرتھا(0), بائٹاں_لمائی(0) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    لڑیاں=جديد CharString();
    إذا(لڑیاں==خالی_سوچک) {
        error_کوڈ=U_حافظہ_ونڈ_ERROR;
    }
}

بائٹاں_Trie_Builder::~بائٹاں_Trie_Builder() {
    مٹاؤ لڑیاں;
    مٹاؤ[] elements;
    uprv_free(بائٹاں);
}

بائٹاں_Trie_Builder&
بائٹاں_Trie_Builder::جوڑو(StringPiece s, int32_t value, UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع *هذا;
    }
    إذا(بائٹاں_لمائی>0) {
        // Cannot add elements after building.
        error_کوڈ=U_NO_WRITE_PERMISSION;
        أرجع *هذا;
    }
    إذا(elements_لمائی==elements_سمرتھا) {
        int32_t new_سمرتھا;
        إذا(elements_سمرتھا==0) {
            new_سمرتھا=1024;
        } وإلا {
            new_سمرتھا=4*elements_سمرتھا;
        }
        بائٹاں_Trie_Element *newElements=جديد بائٹاں_Trie_Element[new_سمرتھا];
        إذا(newElements==خالی_سوچک) {
            error_کوڈ=U_حافظہ_ونڈ_ERROR;
            أرجع *هذا; // error instead of dereferencing null
        }
        إذا(elements_لمائی>0) {
            uprv_memcpy(newElements, elements, (size_t)elements_لمائی*حجم(بائٹاں_Trie_Element));
        }
        مٹاؤ[] elements;
        elements=newElements;
        elements_سمرتھا=new_سمرتھا;
    }
    elements[elements_لمائی++].سیٹ_منزل(s, value, *لڑیاں, error_کوڈ);
    أرجع *هذا;
}

U_CDECL_BEGIN

ساكن int32_t U_CALLCONV
compare_Element_لڑیاں(ثابت فراغ *context, ثابت فراغ *کھبے, ثابت فراغ *سجے) {
    ثابت CharString *لڑیاں=static_cast<ثابت CharString *>(context);
    ثابت بائٹاں_Trie_Element *بایاں_Element=static_cast<ثابت بائٹاں_Trie_Element *>(کھبے);
    ثابت بائٹاں_Trie_Element *دایاں_Element=static_cast<ثابت بائٹاں_Trie_Element *>(سجے);
    أرجع بایاں_Element->compare_String_منزل(*دایاں_Element, *لڑیاں);
}

U_CDECL_سرا

بائٹاں_Trie *
بائٹاں_Trie_Builder::build(UStringTrieBuildOption buildOption, UError_کوڈ &error_کوڈ) {
    build_بائٹاں(buildOption, error_کوڈ);
    بائٹاں_Trie *newTrie=خالی_سوچک;
    إذا(U_SUCCESS(error_کوڈ)) {
        newTrie=جديد بائٹاں_Trie(بائٹاں, بائٹاں+(بائٹاں_سمرتھا-بائٹاں_لمائی));
        إذا(newTrie==خالی_سوچک) {
            error_کوڈ=U_حافظہ_ونڈ_ERROR;
        } وإلا {
            بائٹاں=خالی_سوچک;  // The new trie now owns the array.
            بائٹاں_سمرتھا=0;
        }
    }
    أرجع newTrie;
}

StringPiece
بائٹاں_Trie_Builder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND {
    build_بائٹاں(buildOption, error_کوڈ);
    StringPiece نتیجہ;
    إذا(U_SUCCESS(error_کوڈ)) {
        نتیجہ.سیٹ(بائٹاں+(بائٹاں_سمرتھا-بائٹاں_لمائی), بائٹاں_لمائی);
    }
    أرجع نتیجہ;
}

فراغ
بائٹاں_Trie_Builder::build_بائٹاں(UStringTrieBuildOption buildOption, UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    إذا(بائٹاں!=خالی_سوچک && بائٹاں_لمائی>0) {
        // Already built.
        أرجع;
    }
    إذا(بائٹاں_لمائی==0) {
        إذا(elements_لمائی==0) {
            error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
            أرجع;
        }
        uprv_ترتیب_دیو_Array(elements, elements_لمائی, static_cast<int32_t>(حجم(بائٹاں_Trie_Element)),
                      compare_Element_لڑیاں, لڑیاں,
                      خطأ,  // need not be a stable sort
                      &error_کوڈ);
        إذا(U_FAILURE(error_کوڈ)) {
            أرجع;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*لڑیاں);
        لكل(int32_t i=1; i<elements_لمائی; ++i) {
            StringPiece current=elements[i].getString(*لڑیاں);
            إذا(prev==current) {
                error_کوڈ=U_ILLEGAL_دلیل_ERROR;
                أرجع;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    بائٹاں_لمائی=0;
    int32_t capacity=لڑیاں->length();
    إذا(capacity<1024) {
        capacity=1024;
    }
    إذا(بائٹاں_سمرتھا<capacity) {
        uprv_free(بائٹاں);
        بائٹاں=static_cast<حرف *>(uprv_malloc(capacity));
        إذا(بائٹاں==خالی_سوچک) {
            error_کوڈ=U_حافظہ_ونڈ_ERROR;
            بائٹاں_سمرتھا=0;
            أرجع;
        }
        بائٹاں_سمرتھا=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_لمائی, error_کوڈ);
    إذا(بائٹاں==خالی_سوچک) {
        error_کوڈ=U_حافظہ_ونڈ_ERROR;
    }
}

بائٹاں_Trie_Builder&
بائٹاں_Trie_Builder::clear() U_LIFETIME_BOUND {
    لڑیاں->clear();
    elements_لمائی=0;
    بائٹاں_لمائی=0;
    أرجع *هذا;
}

int32_t
بائٹاں_Trie_Builder::get_Element_String_لمائی(int32_t i) ثابت {
    أرجع elements[i].get_String_لمائی(*لڑیاں);
}

char16_t
بائٹاں_Trie_Builder::getElementUnit(int32_t i, int32_t byte_اشاریہ) ثابت {
    أرجع static_cast<uint8_t>(elements[i].charAt(byte_اشاریہ, *لڑیاں));
}

int32_t
بائٹاں_Trie_Builder::get_Element_قدر(int32_t i) ثابت {
    أرجع elements[i].get_قدر();
}

int32_t
بائٹاں_Trie_Builder::get_حد_بندی_دا_خطی_میل(int32_t first, int32_t آخری, int32_t byte_اشاریہ) ثابت {
    ثابت بائٹاں_Trie_Element &پہلا_Element=elements[first];
    ثابت بائٹاں_Trie_Element &آخری_Element=elements[آخری];
    int32_t min_String_لمائی=پہلا_Element.get_String_لمائی(*لڑیاں);
    طالما(++byte_اشاریہ<min_String_لمائی &&
            پہلا_Element.charAt(byte_اشاریہ, *لڑیاں)==
            آخری_Element.charAt(byte_اشاریہ, *لڑیاں)) {}
    أرجع byte_اشاریہ;
}

int32_t
بائٹاں_Trie_Builder::گنتی_Element_Units(int32_t شروع, int32_t حد_بندی, int32_t byte_اشاریہ) ثابت {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=شروع;
    افعل {
        حرف byte=elements[i++].charAt(byte_اشاریہ, *لڑیاں);
        طالما(i<حد_بندی && byte==elements[i].charAt(byte_اشاریہ, *لڑیاں)) {
            ++i;
        }
        ++length;
    } طالما(i<حد_بندی);
    أرجع length;
}

int32_t
بائٹاں_Trie_Builder::skipElementsBySomeUnits(int32_t i, int32_t byte_اشاریہ, int32_t count) ثابت {
    افعل {
        حرف byte=elements[i++].charAt(byte_اشاریہ, *لڑیاں);
        طالما(byte==elements[i].charAt(byte_اشاریہ, *لڑیاں)) {
            ++i;
        }
    } طالما(--count>0);
    أرجع i;
}

int32_t
بائٹاں_Trie_Builder::اشاریہ_دا_Element_With_اگلا_Unit(int32_t i, int32_t byte_اشاریہ, char16_t byte) ثابت {
    حرف b = static_cast<حرف>(byte);
    طالما(b==elements[i].charAt(byte_اشاریہ, *لڑیاں)) {
        ++i;
    }
    أرجع i;
}

بائٹاں_Trie_Builder::BTLinear_میل_گرہ::BTLinear_میل_گرہ(ثابت حرف *بائٹاں, int32_t len, Node *اگلا_گرہ)
        : خطی_میل_گرہ(len, اگلا_گرہ), s(بائٹاں) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(بائٹاں, len)));
}

منطقي
بائٹاں_Trie_Builder::BTLinear_میل_گرہ::عامل==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    إذا(!خطی_میل_گرہ::عامل==(other)) {
        أرجع خطأ;
    }
    ثابت BTLinear_میل_گرہ &o=static_cast<ثابت BTLinear_میل_گرہ &>(other);
    أرجع 0==uprv_memcmp(s, o.s, length);
}

فراغ
بائٹاں_Trie_Builder::BTLinear_میل_گرہ::اكتب(StringTrieBuilder &builder) {
    بائٹاں_Trie_Builder &b=static_cast<بائٹاں_Trie_Builder &>(builder);
    next->اكتب(builder);
    b.اكتب(s, length);
    وتھ=b.اكتب(b.get_Min_خطی_میل()+length-1);
}

StringTrieBuilder::Node *
بائٹاں_Trie_Builder::create_خطی_میل_گرہ(int32_t i, int32_t byte_اشاریہ, int32_t length,
                                        Node *اگلا_گرہ) ثابت {
    أرجع جديد BTLinear_میل_گرہ(
            elements[i].getString(*لڑیاں).انکڑے()+byte_اشاریہ,
            length,
            اگلا_گرہ);
}

UBool
بائٹاں_Trie_Builder::ensure_سمرتھا(int32_t length) {
    إذا(بائٹاں==خالی_سوچک) {
        أرجع خطأ;  // previous memory allocation had failed
    }
    إذا(length>بائٹاں_سمرتھا) {
        int32_t new_سمرتھا=بائٹاں_سمرتھا;
        افعل {
            new_سمرتھا*=2;
        } طالما(new_سمرتھا<=length);
        حرف *new_بائٹاں=static_cast<حرف *>(uprv_malloc(new_سمرتھا));
        إذا(new_بائٹاں==خالی_سوچک) {
            // unable to allocate memory
            uprv_free(بائٹاں);
            بائٹاں=خالی_سوچک;
            بائٹاں_سمرتھا=0;
            أرجع خطأ;
        }
        uprv_memcpy(new_بائٹاں+(new_سمرتھا-بائٹاں_لمائی),
                    بائٹاں+(بائٹاں_سمرتھا-بائٹاں_لمائی), بائٹاں_لمائی);
        uprv_free(بائٹاں);
        بائٹاں=new_بائٹاں;
        بائٹاں_سمرتھا=new_سمرتھا;
    }
    أرجع سچ;
}

int32_t
بائٹاں_Trie_Builder::اكتب(int32_t byte) {
    int32_t new_لمائی=بائٹاں_لمائی+1;
    إذا(ensure_سمرتھا(new_لمائی)) {
        بائٹاں_لمائی=new_لمائی;
        بائٹاں[بائٹاں_سمرتھا - بائٹاں_لمائی] = static_cast<حرف>(byte);
    }
    أرجع بائٹاں_لمائی;
}

int32_t
بائٹاں_Trie_Builder::اكتب(ثابت حرف *b, int32_t length) {
    int32_t new_لمائی=بائٹاں_لمائی+length;
    إذا(ensure_سمرتھا(new_لمائی)) {
        بائٹاں_لمائی=new_لمائی;
        uprv_memcpy(بائٹاں+(بائٹاں_سمرتھا-بائٹاں_لمائی), b, length);
    }
    أرجع بائٹاں_لمائی;
}

int32_t
بائٹاں_Trie_Builder::writeElementUnits(int32_t i, int32_t byte_اشاریہ, int32_t length) {
    أرجع اكتب(elements[i].getString(*لڑیاں).انکڑے()+byte_اشاریہ, length);
}

int32_t
بائٹاں_Trie_Builder::write_قدر_And_Final(int32_t i, UBool isFinal) {
    إذا(0<=i && i<=بائٹاں_Trie::k_Max_One_Byte_قدر) {
        أرجع اكتب(((بائٹاں_Trie::k_Min_One_Byte_قدر_Lead+i)<<1)|isFinal);
    }
    حرف int_بائٹاں[5];
    int32_t length=1;
    إذا(i<0 || i>0xffffff) {
        int_بائٹاں[0] = static_cast<حرف>(بائٹاں_Trie::k_Five_Byte_قدر_Lead);
        int_بائٹاں[1] = static_cast<حرف>(static_cast<uint32_t>(i) >> 24);
        int_بائٹاں[2] = static_cast<حرف>(static_cast<uint32_t>(i) >> 16);
        int_بائٹاں[3] = static_cast<حرف>(static_cast<uint32_t>(i) >> 8);
        int_بائٹاں[4] = static_cast<حرف>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } وإلا {
        إذا(i<=بائٹاں_Trie::k_Max_Two_Byte_قدر) {
            int_بائٹاں[0] = static_cast<حرف>(بائٹاں_Trie::k_Min_Two_Byte_قدر_Lead + (i >> 8));
        } وإلا {
            إذا(i<=بائٹاں_Trie::k_Max_Three_Byte_قدر) {
                int_بائٹاں[0] = static_cast<حرف>(بائٹاں_Trie::k_Min_Three_Byte_قدر_Lead + (i >> 16));
            } وإلا {
                int_بائٹاں[0] = static_cast<حرف>(بائٹاں_Trie::k_Four_Byte_قدر_Lead);
                int_بائٹاں[1] = static_cast<حرف>(i >> 16);
                length=2;
            }
            int_بائٹاں[length++] = static_cast<حرف>(i >> 8);
        }
        int_بائٹاں[length++] = static_cast<حرف>(i);
    }
    int_بائٹاں[0] = static_cast<حرف>((int_بائٹاں[0] << 1) | isFinal);
    أرجع اكتب(int_بائٹاں, length);
}

int32_t
بائٹاں_Trie_Builder::write_قدر_And_Type(UBool has_قدر, int32_t value, int32_t گرہ) {
    int32_t وتھ=اكتب(گرہ);
    إذا(has_قدر) {
        وتھ=write_قدر_And_Final(value, خطأ);
    }
    أرجع وتھ;
}

int32_t
بائٹاں_Trie_Builder::write_Delta_منزل(int32_t jump_نشانہ) {
    int32_t i=بائٹاں_لمائی-jump_نشانہ;
    U_ASSERT(i>=0);
    إذا(i<=بائٹاں_Trie::kMaxOneByteDelta) {
        أرجع اكتب(i);
    } وإلا {
        حرف int_بائٹاں[5];
        أرجع اكتب(int_بائٹاں, اندرونی_Encode_Delta(i, int_بائٹاں));
    }
}

int32_t
بائٹاں_Trie_Builder::اندرونی_Encode_Delta(int32_t i, حرف int_بائٹاں[]) {
    U_ASSERT(i>=0);
    إذا(i<=بائٹاں_Trie::kMaxOneByteDelta) {
        int_بائٹاں[0] = static_cast<حرف>(i);
        أرجع 1;
    }
    int32_t length=1;
    إذا(i<=بائٹاں_Trie::kMaxTwoByteDelta) {
        int_بائٹاں[0] = static_cast<حرف>(بائٹاں_Trie::kMinTwoByteDeltaLead + (i >> 8));
    } وإلا {
        إذا(i<=بائٹاں_Trie::kMaxThreeByteDelta) {
            int_بائٹاں[0] = static_cast<حرف>(بائٹاں_Trie::kMinThreeByteDeltaLead + (i >> 16));
        } وإلا {
            إذا(i<=0xffffff) {
                int_بائٹاں[0] = static_cast<حرف>(بائٹاں_Trie::kFourByteDeltaLead);
            } وإلا {
                int_بائٹاں[0] = static_cast<حرف>(بائٹاں_Trie::kFiveByteDeltaLead);
                int_بائٹاں[1] = static_cast<حرف>(i >> 24);
                length=2;
            }
            int_بائٹاں[length++] = static_cast<حرف>(i >> 16);
        }
        int_بائٹاں[length++] = static_cast<حرف>(i >> 8);
    }
    int_بائٹاں[length++] = static_cast<حرف>(i);
    أرجع length;
}

U_NAMESPACE_سرا
