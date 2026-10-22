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

صنف بائيٽون_Trie_Element : عام UMemory {
عام:
    // Use compiler's default constructor, initializes nothing.

    فراغ سيٽ_منزل(StringPiece s, int32_t val, CharString &لڙيون, UError_ڪوڊ &error_ڪوڊ);

    StringPiece getString(ثابت CharString& لڙيون U_LIFETIME_BOUND) ثابت {
        int32_t وٿي=string_وٿي;
        int32_t length;
        إذا(وٿي>=0) {
            length = static_cast<uint8_t>(لڙيون[وٿي++]);
        } وإلا {
            وٿي=~وٿي;
            length = (static_cast<int32_t>(static_cast<uint8_t>(لڙيون[وٿي])) << 8) | static_cast<uint8_t>(لڙيون[وٿي + 1]);
            وٿي+=2;
        }
        أرجع StringPiece(لڙيون.معلومات()+وٿي, length);
    }
    int32_t get_String_ڊيگهائي(ثابت CharString &لڙيون) ثابت {
        int32_t وٿي=string_وٿي;
        إذا(وٿي>=0) {
            أرجع static_cast<uint8_t>(لڙيون[وٿي]);
        } وإلا {
            وٿي=~وٿي;
            أرجع (static_cast<int32_t>(static_cast<uint8_t>(لڙيون[وٿي])) << 8) | static_cast<uint8_t>(لڙيون[وٿي + 1]);
        }
    }

    حرف charAt(int32_t اشاريو, ثابت CharString &لڙيون) ثابت { أرجع معلومات(لڙيون)[اشاريو]; }

    int32_t get_قدر() ثابت { أرجع value; }

    int32_t compare_String_منزل(ثابت بائيٽون_Trie_Element &o, ثابت CharString &لڙيون) ثابت;

خاص:
    ثابت حرف* معلومات(ثابت CharString& لڙيون U_LIFETIME_BOUND) ثابت {
        int32_t وٿي=string_وٿي;
        إذا(وٿي>=0) {
            ++وٿي;
        } وإلا {
            وٿي=~وٿي+2;
        }
        أرجع لڙيون.معلومات()+وٿي;
    }

    // If the stringOffset is non-negative, then the first strings byte contains
    // the string length.
    // If the stringOffset is negative, then the first two strings bytes contain
    // the string length (big-endian), and the offset needs to be bit-inverted.
    // (Compared with a stringLength field here, this saves 3 bytes per string for most strings.)
    int32_t string_وٿي;
    int32_t value;
};

فراغ
بائيٽون_Trie_Element::سيٽ_منزل(StringPiece s, int32_t val,
                        CharString &لڙيون, UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    int32_t length=s.length();
    إذا(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع;
    }
    int32_t وٿي=لڙيون.length();
    إذا(length>0xff) {
        وٿي=~وٿي;
        لڙيون.append(static_cast<حرف>(length >> 8), error_ڪوڊ);
    }
    لڙيون.append(static_cast<حرف>(length), error_ڪوڊ);
    string_وٿي=وٿي;
    value=val;
    لڙيون.append(s, error_ڪوڊ);
}

int32_t
بائيٽون_Trie_Element::compare_String_منزل(ثابت بائيٽون_Trie_Element &other, ثابت CharString &لڙيون) ثابت {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(لڙيون);
    StringPiece otherString=other.getString(لڙيون);
    int32_t ڊيگهائي_Diff=thisString.length()-otherString.length();
    int32_t common_ڊيگهائي;
    إذا(ڊيگهائي_Diff<=0) {
        common_ڊيگهائي=thisString.length();
    } وإلا {
        common_ڊيگهائي=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.معلومات(), otherString.معلومات(), common_ڊيگهائي);
    أرجع diff!=0 ? diff : ڊيگهائي_Diff;
}

بائيٽون_Trie_Builder::بائيٽون_Trie_Builder(UError_ڪوڊ &error_ڪوڊ)
        : لڙيون(خالي_اشارو), elements(خالي_اشارو), elements_صلاحيت(0), elements_ڊيگهائي(0),
          بائيٽون(خالي_اشارو), بائيٽون_صلاحيت(0), بائيٽون_ڊيگهائي(0) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    لڙيون=جديد CharString();
    إذا(لڙيون==خالي_اشارو) {
        error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
    }
}

بائيٽون_Trie_Builder::~بائيٽون_Trie_Builder() {
    ميٽيو لڙيون;
    ميٽيو[] elements;
    uprv_free(بائيٽون);
}

بائيٽون_Trie_Builder&
بائيٽون_Trie_Builder::جوڙيو(StringPiece s, int32_t value, UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع *هذا;
    }
    إذا(بائيٽون_ڊيگهائي>0) {
        // Cannot add elements after building.
        error_ڪوڊ=U_NO_WRITE_PERMISSION;
        أرجع *هذا;
    }
    إذا(elements_ڊيگهائي==elements_صلاحيت) {
        int32_t new_صلاحيت;
        إذا(elements_صلاحيت==0) {
            new_صلاحيت=1024;
        } وإلا {
            new_صلاحيت=4*elements_صلاحيت;
        }
        بائيٽون_Trie_Element *newElements=جديد بائيٽون_Trie_Element[new_صلاحيت];
        إذا(newElements==خالي_اشارو) {
            error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
            أرجع *هذا; // error instead of dereferencing null
        }
        إذا(elements_ڊيگهائي>0) {
            uprv_memcpy(newElements, elements, (size_t)elements_ڊيگهائي*حجم(بائيٽون_Trie_Element));
        }
        ميٽيو[] elements;
        elements=newElements;
        elements_صلاحيت=new_صلاحيت;
    }
    elements[elements_ڊيگهائي++].سيٽ_منزل(s, value, *لڙيون, error_ڪوڊ);
    أرجع *هذا;
}

U_CDECL_BEGIN

ساكن int32_t U_CALLCONV
compare_Element_لڙيون(ثابت فراغ *context, ثابت فراغ *کاٻي, ثابت فراغ *ساڄي) {
    ثابت CharString *لڙيون=static_cast<ثابت CharString *>(context);
    ثابت بائيٽون_Trie_Element *کاٻو_Element=static_cast<ثابت بائيٽون_Trie_Element *>(کاٻي);
    ثابت بائيٽون_Trie_Element *ساڄو_Element=static_cast<ثابت بائيٽون_Trie_Element *>(ساڄي);
    أرجع کاٻو_Element->compare_String_منزل(*ساڄو_Element, *لڙيون);
}

U_CDECL_پڇاڙي

بائيٽون_Trie *
بائيٽون_Trie_Builder::build(UStringTrieBuildOption buildOption, UError_ڪوڊ &error_ڪوڊ) {
    build_بائيٽون(buildOption, error_ڪوڊ);
    بائيٽون_Trie *newTrie=خالي_اشارو;
    إذا(U_SUCCESS(error_ڪوڊ)) {
        newTrie=جديد بائيٽون_Trie(بائيٽون, بائيٽون+(بائيٽون_صلاحيت-بائيٽون_ڊيگهائي));
        إذا(newTrie==خالي_اشارو) {
            error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        } وإلا {
            بائيٽون=خالي_اشارو;  // The new trie now owns the array.
            بائيٽون_صلاحيت=0;
        }
    }
    أرجع newTrie;
}

StringPiece
بائيٽون_Trie_Builder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND {
    build_بائيٽون(buildOption, error_ڪوڊ);
    StringPiece نتيجو;
    إذا(U_SUCCESS(error_ڪوڊ)) {
        نتيجو.سيٽ(بائيٽون+(بائيٽون_صلاحيت-بائيٽون_ڊيگهائي), بائيٽون_ڊيگهائي);
    }
    أرجع نتيجو;
}

فراغ
بائيٽون_Trie_Builder::build_بائيٽون(UStringTrieBuildOption buildOption, UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    إذا(بائيٽون!=خالي_اشارو && بائيٽون_ڊيگهائي>0) {
        // Already built.
        أرجع;
    }
    إذا(بائيٽون_ڊيگهائي==0) {
        إذا(elements_ڊيگهائي==0) {
            error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
            أرجع;
        }
        uprv_ترتيب_ڏيو_Array(elements, elements_ڊيگهائي, static_cast<int32_t>(حجم(بائيٽون_Trie_Element)),
                      compare_Element_لڙيون, لڙيون,
                      خطأ,  // need not be a stable sort
                      &error_ڪوڊ);
        إذا(U_FAILURE(error_ڪوڊ)) {
            أرجع;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*لڙيون);
        لكل(int32_t i=1; i<elements_ڊيگهائي; ++i) {
            StringPiece current=elements[i].getString(*لڙيون);
            إذا(prev==current) {
                error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
                أرجع;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    بائيٽون_ڊيگهائي=0;
    int32_t capacity=لڙيون->length();
    إذا(capacity<1024) {
        capacity=1024;
    }
    إذا(بائيٽون_صلاحيت<capacity) {
        uprv_free(بائيٽون);
        بائيٽون=static_cast<حرف *>(uprv_malloc(capacity));
        إذا(بائيٽون==خالي_اشارو) {
            error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
            بائيٽون_صلاحيت=0;
            أرجع;
        }
        بائيٽون_صلاحيت=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_ڊيگهائي, error_ڪوڊ);
    إذا(بائيٽون==خالي_اشارو) {
        error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
    }
}

بائيٽون_Trie_Builder&
بائيٽون_Trie_Builder::clear() U_LIFETIME_BOUND {
    لڙيون->clear();
    elements_ڊيگهائي=0;
    بائيٽون_ڊيگهائي=0;
    أرجع *هذا;
}

int32_t
بائيٽون_Trie_Builder::get_Element_String_ڊيگهائي(int32_t i) ثابت {
    أرجع elements[i].get_String_ڊيگهائي(*لڙيون);
}

char16_t
بائيٽون_Trie_Builder::getElementUnit(int32_t i, int32_t byte_اشاريو) ثابت {
    أرجع static_cast<uint8_t>(elements[i].charAt(byte_اشاريو, *لڙيون));
}

int32_t
بائيٽون_Trie_Builder::get_Element_قدر(int32_t i) ثابت {
    أرجع elements[i].get_قدر();
}

int32_t
بائيٽون_Trie_Builder::get_حد_بندي_جو_لڪيري_ميلاپ(int32_t first, int32_t آخري, int32_t byte_اشاريو) ثابت {
    ثابت بائيٽون_Trie_Element &پهريون_Element=elements[first];
    ثابت بائيٽون_Trie_Element &آخري_Element=elements[آخري];
    int32_t min_String_ڊيگهائي=پهريون_Element.get_String_ڊيگهائي(*لڙيون);
    طالما(++byte_اشاريو<min_String_ڊيگهائي &&
            پهريون_Element.charAt(byte_اشاريو, *لڙيون)==
            آخري_Element.charAt(byte_اشاريو, *لڙيون)) {}
    أرجع byte_اشاريو;
}

int32_t
بائيٽون_Trie_Builder::ڳڻپ_Element_Units(int32_t شروعات, int32_t حد_بندي, int32_t byte_اشاريو) ثابت {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=شروعات;
    افعل {
        حرف byte=elements[i++].charAt(byte_اشاريو, *لڙيون);
        طالما(i<حد_بندي && byte==elements[i].charAt(byte_اشاريو, *لڙيون)) {
            ++i;
        }
        ++length;
    } طالما(i<حد_بندي);
    أرجع length;
}

int32_t
بائيٽون_Trie_Builder::skipElementsBySomeUnits(int32_t i, int32_t byte_اشاريو, int32_t count) ثابت {
    افعل {
        حرف byte=elements[i++].charAt(byte_اشاريو, *لڙيون);
        طالما(byte==elements[i].charAt(byte_اشاريو, *لڙيون)) {
            ++i;
        }
    } طالما(--count>0);
    أرجع i;
}

int32_t
بائيٽون_Trie_Builder::اشاريو_جو_Element_With_ايندڙ_Unit(int32_t i, int32_t byte_اشاريو, char16_t byte) ثابت {
    حرف b = static_cast<حرف>(byte);
    طالما(b==elements[i].charAt(byte_اشاريو, *لڙيون)) {
        ++i;
    }
    أرجع i;
}

بائيٽون_Trie_Builder::BTLinear_ميلاپ_ڳنڍ::BTLinear_ميلاپ_ڳنڍ(ثابت حرف *بائيٽون, int32_t len, Node *ايندڙ_ڳنڍ)
        : لڪيري_ميلاپ_ڳنڍ(len, ايندڙ_ڳنڍ), s(بائيٽون) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(بائيٽون, len)));
}

منطقي
بائيٽون_Trie_Builder::BTLinear_ميلاپ_ڳنڍ::عامل==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    إذا(!لڪيري_ميلاپ_ڳنڍ::عامل==(other)) {
        أرجع خطأ;
    }
    ثابت BTLinear_ميلاپ_ڳنڍ &o=static_cast<ثابت BTLinear_ميلاپ_ڳنڍ &>(other);
    أرجع 0==uprv_memcmp(s, o.s, length);
}

فراغ
بائيٽون_Trie_Builder::BTLinear_ميلاپ_ڳنڍ::اكتب(StringTrieBuilder &builder) {
    بائيٽون_Trie_Builder &b=static_cast<بائيٽون_Trie_Builder &>(builder);
    next->اكتب(builder);
    b.اكتب(s, length);
    وٿي=b.اكتب(b.get_Min_لڪيري_ميلاپ()+length-1);
}

StringTrieBuilder::Node *
بائيٽون_Trie_Builder::create_لڪيري_ميلاپ_ڳنڍ(int32_t i, int32_t byte_اشاريو, int32_t length,
                                        Node *ايندڙ_ڳنڍ) ثابت {
    أرجع جديد BTLinear_ميلاپ_ڳنڍ(
            elements[i].getString(*لڙيون).معلومات()+byte_اشاريو,
            length,
            ايندڙ_ڳنڍ);
}

UBool
بائيٽون_Trie_Builder::ensure_صلاحيت(int32_t length) {
    إذا(بائيٽون==خالي_اشارو) {
        أرجع خطأ;  // previous memory allocation had failed
    }
    إذا(length>بائيٽون_صلاحيت) {
        int32_t new_صلاحيت=بائيٽون_صلاحيت;
        افعل {
            new_صلاحيت*=2;
        } طالما(new_صلاحيت<=length);
        حرف *new_بائيٽون=static_cast<حرف *>(uprv_malloc(new_صلاحيت));
        إذا(new_بائيٽون==خالي_اشارو) {
            // unable to allocate memory
            uprv_free(بائيٽون);
            بائيٽون=خالي_اشارو;
            بائيٽون_صلاحيت=0;
            أرجع خطأ;
        }
        uprv_memcpy(new_بائيٽون+(new_صلاحيت-بائيٽون_ڊيگهائي),
                    بائيٽون+(بائيٽون_صلاحيت-بائيٽون_ڊيگهائي), بائيٽون_ڊيگهائي);
        uprv_free(بائيٽون);
        بائيٽون=new_بائيٽون;
        بائيٽون_صلاحيت=new_صلاحيت;
    }
    أرجع سچ;
}

int32_t
بائيٽون_Trie_Builder::اكتب(int32_t byte) {
    int32_t new_ڊيگهائي=بائيٽون_ڊيگهائي+1;
    إذا(ensure_صلاحيت(new_ڊيگهائي)) {
        بائيٽون_ڊيگهائي=new_ڊيگهائي;
        بائيٽون[بائيٽون_صلاحيت - بائيٽون_ڊيگهائي] = static_cast<حرف>(byte);
    }
    أرجع بائيٽون_ڊيگهائي;
}

int32_t
بائيٽون_Trie_Builder::اكتب(ثابت حرف *b, int32_t length) {
    int32_t new_ڊيگهائي=بائيٽون_ڊيگهائي+length;
    إذا(ensure_صلاحيت(new_ڊيگهائي)) {
        بائيٽون_ڊيگهائي=new_ڊيگهائي;
        uprv_memcpy(بائيٽون+(بائيٽون_صلاحيت-بائيٽون_ڊيگهائي), b, length);
    }
    أرجع بائيٽون_ڊيگهائي;
}

int32_t
بائيٽون_Trie_Builder::writeElementUnits(int32_t i, int32_t byte_اشاريو, int32_t length) {
    أرجع اكتب(elements[i].getString(*لڙيون).معلومات()+byte_اشاريو, length);
}

int32_t
بائيٽون_Trie_Builder::write_قدر_And_Final(int32_t i, UBool isFinal) {
    إذا(0<=i && i<=بائيٽون_Trie::k_Max_One_Byte_قدر) {
        أرجع اكتب(((بائيٽون_Trie::k_Min_One_Byte_قدر_Lead+i)<<1)|isFinal);
    }
    حرف int_بائيٽون[5];
    int32_t length=1;
    إذا(i<0 || i>0xffffff) {
        int_بائيٽون[0] = static_cast<حرف>(بائيٽون_Trie::k_Five_Byte_قدر_Lead);
        int_بائيٽون[1] = static_cast<حرف>(static_cast<uint32_t>(i) >> 24);
        int_بائيٽون[2] = static_cast<حرف>(static_cast<uint32_t>(i) >> 16);
        int_بائيٽون[3] = static_cast<حرف>(static_cast<uint32_t>(i) >> 8);
        int_بائيٽون[4] = static_cast<حرف>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } وإلا {
        إذا(i<=بائيٽون_Trie::k_Max_Two_Byte_قدر) {
            int_بائيٽون[0] = static_cast<حرف>(بائيٽون_Trie::k_Min_Two_Byte_قدر_Lead + (i >> 8));
        } وإلا {
            إذا(i<=بائيٽون_Trie::k_Max_Three_Byte_قدر) {
                int_بائيٽون[0] = static_cast<حرف>(بائيٽون_Trie::k_Min_Three_Byte_قدر_Lead + (i >> 16));
            } وإلا {
                int_بائيٽون[0] = static_cast<حرف>(بائيٽون_Trie::k_Four_Byte_قدر_Lead);
                int_بائيٽون[1] = static_cast<حرف>(i >> 16);
                length=2;
            }
            int_بائيٽون[length++] = static_cast<حرف>(i >> 8);
        }
        int_بائيٽون[length++] = static_cast<حرف>(i);
    }
    int_بائيٽون[0] = static_cast<حرف>((int_بائيٽون[0] << 1) | isFinal);
    أرجع اكتب(int_بائيٽون, length);
}

int32_t
بائيٽون_Trie_Builder::write_قدر_And_Type(UBool has_قدر, int32_t value, int32_t ڳنڍ) {
    int32_t وٿي=اكتب(ڳنڍ);
    إذا(has_قدر) {
        وٿي=write_قدر_And_Final(value, خطأ);
    }
    أرجع وٿي;
}

int32_t
بائيٽون_Trie_Builder::write_Delta_منزل(int32_t jump_هدف) {
    int32_t i=بائيٽون_ڊيگهائي-jump_هدف;
    U_ASSERT(i>=0);
    إذا(i<=بائيٽون_Trie::kMaxOneByteDelta) {
        أرجع اكتب(i);
    } وإلا {
        حرف int_بائيٽون[5];
        أرجع اكتب(int_بائيٽون, اندروني_Encode_Delta(i, int_بائيٽون));
    }
}

int32_t
بائيٽون_Trie_Builder::اندروني_Encode_Delta(int32_t i, حرف int_بائيٽون[]) {
    U_ASSERT(i>=0);
    إذا(i<=بائيٽون_Trie::kMaxOneByteDelta) {
        int_بائيٽون[0] = static_cast<حرف>(i);
        أرجع 1;
    }
    int32_t length=1;
    إذا(i<=بائيٽون_Trie::kMaxTwoByteDelta) {
        int_بائيٽون[0] = static_cast<حرف>(بائيٽون_Trie::kMinTwoByteDeltaLead + (i >> 8));
    } وإلا {
        إذا(i<=بائيٽون_Trie::kMaxThreeByteDelta) {
            int_بائيٽون[0] = static_cast<حرف>(بائيٽون_Trie::kMinThreeByteDeltaLead + (i >> 16));
        } وإلا {
            إذا(i<=0xffffff) {
                int_بائيٽون[0] = static_cast<حرف>(بائيٽون_Trie::kFourByteDeltaLead);
            } وإلا {
                int_بائيٽون[0] = static_cast<حرف>(بائيٽون_Trie::kFiveByteDeltaLead);
                int_بائيٽون[1] = static_cast<حرف>(i >> 24);
                length=2;
            }
            int_بائيٽون[length++] = static_cast<حرف>(i >> 16);
        }
        int_بائيٽون[length++] = static_cast<حرف>(i >> 8);
    }
    int_بائيٽون[length++] = static_cast<حرف>(i);
    أرجع length;
}

U_NAMESPACE_پڇاڙي
