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

#شامل "unicode/utypes.h"
#شامل "unicode/bytestrie.h"
#شامل "unicode/bytestriebuilder.h"
#شامل "unicode/stringpiece.h"
#شامل "charstr.h"
#شامل "cmemory.h"
#شامل "uhash.h"
#شامل "uarrsort.h"
#شامل "uassert.h"
#شامل "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (bytes, value) pairs with full copies
 * of the byte sequences, until the BytesTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

جماعت بائٹیں_Trie_Element : عوامی UMemory {
عوامی:
    // Use compiler's default constructor, initializes nothing.

    خالی سیٹ_مقصد(StringPiece s, int32_t val, CharString &لڑیاں, UError_رمز &error_رمز);

    StringPiece getString(ثابت CharString& لڑیاں U_LIFETIME_BOUND) ثابت {
        int32_t انحراف=string_انحراف;
        int32_t length;
        اگر(انحراف>=0) {
            length = static_cast<uint8_t>(لڑیاں[انحراف++]);
        } ورنہ {
            انحراف=~انحراف;
            length = (static_cast<int32_t>(static_cast<uint8_t>(لڑیاں[انحراف])) << 8) | static_cast<uint8_t>(لڑیاں[انحراف + 1]);
            انحراف+=2;
        }
        واپس StringPiece(لڑیاں.ڈیٹا()+انحراف, length);
    }
    int32_t get_String_لمبائی(ثابت CharString &لڑیاں) ثابت {
        int32_t انحراف=string_انحراف;
        اگر(انحراف>=0) {
            واپس static_cast<uint8_t>(لڑیاں[انحراف]);
        } ورنہ {
            انحراف=~انحراف;
            واپس (static_cast<int32_t>(static_cast<uint8_t>(لڑیاں[انحراف])) << 8) | static_cast<uint8_t>(لڑیاں[انحراف + 1]);
        }
    }

    حرف charAt(int32_t اشاریہ, ثابت CharString &لڑیاں) ثابت { واپس ڈیٹا(لڑیاں)[اشاریہ]; }

    int32_t get_قدر() ثابت { واپس value; }

    int32_t compare_String_مقصد(ثابت بائٹیں_Trie_Element &o, ثابت CharString &لڑیاں) ثابت;

نجی:
    ثابت حرف* ڈیٹا(ثابت CharString& لڑیاں U_LIFETIME_BOUND) ثابت {
        int32_t انحراف=string_انحراف;
        اگر(انحراف>=0) {
            ++انحراف;
        } ورنہ {
            انحراف=~انحراف+2;
        }
        واپس لڑیاں.ڈیٹا()+انحراف;
    }

    // If the stringOffset is non-negative, then the first strings byte contains
    // the string length.
    // If the stringOffset is negative, then the first two strings bytes contain
    // the string length (big-endian), and the offset needs to be bit-inverted.
    // (Compared with a stringLength field here, this saves 3 bytes per string for most strings.)
    int32_t string_انحراف;
    int32_t value;
};

خالی
بائٹیں_Trie_Element::سیٹ_مقصد(StringPiece s, int32_t val,
                        CharString &لڑیاں, UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس;
    }
    int32_t length=s.length();
    اگر(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس;
    }
    int32_t انحراف=لڑیاں.length();
    اگر(length>0xff) {
        انحراف=~انحراف;
        لڑیاں.append(static_cast<حرف>(length >> 8), error_رمز);
    }
    لڑیاں.append(static_cast<حرف>(length), error_رمز);
    string_انحراف=انحراف;
    value=val;
    لڑیاں.append(s, error_رمز);
}

int32_t
بائٹیں_Trie_Element::compare_String_مقصد(ثابت بائٹیں_Trie_Element &other, ثابت CharString &لڑیاں) ثابت {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(لڑیاں);
    StringPiece otherString=other.getString(لڑیاں);
    int32_t لمبائی_Diff=thisString.length()-otherString.length();
    int32_t common_لمبائی;
    اگر(لمبائی_Diff<=0) {
        common_لمبائی=thisString.length();
    } ورنہ {
        common_لمبائی=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.ڈیٹا(), otherString.ڈیٹا(), common_لمبائی);
    واپس diff!=0 ? diff : لمبائی_Diff;
}

بائٹیں_Trie_Builder::بائٹیں_Trie_Builder(UError_رمز &error_رمز)
        : لڑیاں(خالی_اشاریہ), elements(خالی_اشاریہ), elements_استعداد(0), elements_لمبائی(0),
          بائٹیں(خالی_اشاریہ), بائٹیں_استعداد(0), بائٹیں_لمبائی(0) {
    اگر(U_FAILURE(error_رمز)) {
        واپس;
    }
    لڑیاں=نیا CharString();
    اگر(لڑیاں==خالی_اشاریہ) {
        error_رمز=U_حافظہ_تخصیص_ERROR;
    }
}

بائٹیں_Trie_Builder::~بائٹیں_Trie_Builder() {
    حذف لڑیاں;
    حذف[] elements;
    uprv_free(بائٹیں);
}

بائٹیں_Trie_Builder&
بائٹیں_Trie_Builder::جوڑو(StringPiece s, int32_t value, UError_رمز& error_رمز) U_LIFETIME_BOUND {
    اگر(U_FAILURE(error_رمز)) {
        واپس *یہ;
    }
    اگر(بائٹیں_لمبائی>0) {
        // Cannot add elements after building.
        error_رمز=U_NO_WRITE_PERMISSION;
        واپس *یہ;
    }
    اگر(elements_لمبائی==elements_استعداد) {
        int32_t new_استعداد;
        اگر(elements_استعداد==0) {
            new_استعداد=1024;
        } ورنہ {
            new_استعداد=4*elements_استعداد;
        }
        بائٹیں_Trie_Element *newElements=نیا بائٹیں_Trie_Element[new_استعداد];
        اگر(newElements==خالی_اشاریہ) {
            error_رمز=U_حافظہ_تخصیص_ERROR;
            واپس *یہ; // error instead of dereferencing null
        }
        اگر(elements_لمبائی>0) {
            uprv_memcpy(newElements, elements, (size_t)elements_لمبائی*حجم(بائٹیں_Trie_Element));
        }
        حذف[] elements;
        elements=newElements;
        elements_استعداد=new_استعداد;
    }
    elements[elements_لمبائی++].سیٹ_مقصد(s, value, *لڑیاں, error_رمز);
    واپس *یہ;
}

U_CDECL_BEGIN

ساکن int32_t U_CALLCONV
compare_Element_لڑیاں(ثابت خالی *context, ثابت خالی *بائیں, ثابت خالی *دائیں) {
    ثابت CharString *لڑیاں=static_cast<ثابت CharString *>(context);
    ثابت بائٹیں_Trie_Element *بایاں_Element=static_cast<ثابت بائٹیں_Trie_Element *>(بائیں);
    ثابت بائٹیں_Trie_Element *دایاں_Element=static_cast<ثابت بائٹیں_Trie_Element *>(دائیں);
    واپس بایاں_Element->compare_String_مقصد(*دایاں_Element, *لڑیاں);
}

U_CDECL_انتہا

بائٹیں_Trie *
بائٹیں_Trie_Builder::build(UStringTrieBuildOption buildOption, UError_رمز &error_رمز) {
    build_بائٹیں(buildOption, error_رمز);
    بائٹیں_Trie *newTrie=خالی_اشاریہ;
    اگر(U_SUCCESS(error_رمز)) {
        newTrie=نیا بائٹیں_Trie(بائٹیں, بائٹیں+(بائٹیں_استعداد-بائٹیں_لمبائی));
        اگر(newTrie==خالی_اشاریہ) {
            error_رمز=U_حافظہ_تخصیص_ERROR;
        } ورنہ {
            بائٹیں=خالی_اشاریہ;  // The new trie now owns the array.
            بائٹیں_استعداد=0;
        }
    }
    واپس newTrie;
}

StringPiece
بائٹیں_Trie_Builder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UError_رمز& error_رمز) U_LIFETIME_BOUND {
    build_بائٹیں(buildOption, error_رمز);
    StringPiece نتیجہ;
    اگر(U_SUCCESS(error_رمز)) {
        نتیجہ.سیٹ(بائٹیں+(بائٹیں_استعداد-بائٹیں_لمبائی), بائٹیں_لمبائی);
    }
    واپس نتیجہ;
}

خالی
بائٹیں_Trie_Builder::build_بائٹیں(UStringTrieBuildOption buildOption, UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس;
    }
    اگر(بائٹیں!=خالی_اشاریہ && بائٹیں_لمبائی>0) {
        // Already built.
        واپس;
    }
    اگر(بائٹیں_لمبائی==0) {
        اگر(elements_لمبائی==0) {
            error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس;
        }
        uprv_ترتیب_دو_Array(elements, elements_لمبائی, static_cast<int32_t>(حجم(بائٹیں_Trie_Element)),
                      compare_Element_لڑیاں, لڑیاں,
                      جھوٹ,  // need not be a stable sort
                      &error_رمز);
        اگر(U_FAILURE(error_رمز)) {
            واپس;
        }
        // Duplicate strings are not allowed.
        StringPiece پچھلا=elements[0].getString(*لڑیاں);
        برائے(int32_t i=1; i<elements_لمبائی; ++i) {
            StringPiece current=elements[i].getString(*لڑیاں);
            اگر(پچھلا==current) {
                error_رمز=U_ILLEGAL_دلیل_ERROR;
                واپس;
            }
            پچھلا=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    بائٹیں_لمبائی=0;
    int32_t capacity=لڑیاں->length();
    اگر(capacity<1024) {
        capacity=1024;
    }
    اگر(بائٹیں_استعداد<capacity) {
        uprv_free(بائٹیں);
        بائٹیں=static_cast<حرف *>(uprv_malloc(capacity));
        اگر(بائٹیں==خالی_اشاریہ) {
            error_رمز=U_حافظہ_تخصیص_ERROR;
            بائٹیں_استعداد=0;
            واپس;
        }
        بائٹیں_استعداد=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_لمبائی, error_رمز);
    اگر(بائٹیں==خالی_اشاریہ) {
        error_رمز=U_حافظہ_تخصیص_ERROR;
    }
}

بائٹیں_Trie_Builder&
بائٹیں_Trie_Builder::clear() U_LIFETIME_BOUND {
    لڑیاں->clear();
    elements_لمبائی=0;
    بائٹیں_لمبائی=0;
    واپس *یہ;
}

int32_t
بائٹیں_Trie_Builder::get_Element_String_لمبائی(int32_t i) ثابت {
    واپس elements[i].get_String_لمبائی(*لڑیاں);
}

char16_t
بائٹیں_Trie_Builder::getElementUnit(int32_t i, int32_t byte_اشاریہ) ثابت {
    واپس static_cast<uint8_t>(elements[i].charAt(byte_اشاریہ, *لڑیاں));
}

int32_t
بائٹیں_Trie_Builder::get_Element_قدر(int32_t i) ثابت {
    واپس elements[i].get_قدر();
}

int32_t
بائٹیں_Trie_Builder::get_حد_بندی_کا_خطی_مطابقت(int32_t first, int32_t آخری, int32_t byte_اشاریہ) ثابت {
    ثابت بائٹیں_Trie_Element &پہلا_Element=elements[first];
    ثابت بائٹیں_Trie_Element &آخری_Element=elements[آخری];
    int32_t min_String_لمبائی=پہلا_Element.get_String_لمبائی(*لڑیاں);
    جب(++byte_اشاریہ<min_String_لمبائی &&
            پہلا_Element.charAt(byte_اشاریہ, *لڑیاں)==
            آخری_Element.charAt(byte_اشاریہ, *لڑیاں)) {}
    واپس byte_اشاریہ;
}

int32_t
بائٹیں_Trie_Builder::گنتی_Element_Units(int32_t آغاز, int32_t حد_بندی, int32_t byte_اشاریہ) ثابت {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=آغاز;
    کرو {
        حرف byte=elements[i++].charAt(byte_اشاریہ, *لڑیاں);
        جب(i<حد_بندی && byte==elements[i].charAt(byte_اشاریہ, *لڑیاں)) {
            ++i;
        }
        ++length;
    } جب(i<حد_بندی);
    واپس length;
}

int32_t
بائٹیں_Trie_Builder::skipElementsBySomeUnits(int32_t i, int32_t byte_اشاریہ, int32_t count) ثابت {
    کرو {
        حرف byte=elements[i++].charAt(byte_اشاریہ, *لڑیاں);
        جب(byte==elements[i].charAt(byte_اشاریہ, *لڑیاں)) {
            ++i;
        }
    } جب(--count>0);
    واپس i;
}

int32_t
بائٹیں_Trie_Builder::اشاریہ_کا_Element_With_اگلا_Unit(int32_t i, int32_t byte_اشاریہ, char16_t byte) ثابت {
    حرف b = static_cast<حرف>(byte);
    جب(b==elements[i].charAt(byte_اشاریہ, *لڑیاں)) {
        ++i;
    }
    واپس i;
}

بائٹیں_Trie_Builder::BTLinear_مطابقت_گرہ::BTLinear_مطابقت_گرہ(ثابت حرف *بائٹیں, int32_t len, Node *اگلا_گرہ)
        : خطی_مطابقت_گرہ(len, اگلا_گرہ), s(بائٹیں) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(بائٹیں, len)));
}

منطقی
بائٹیں_Trie_Builder::BTLinear_مطابقت_گرہ::عامل==(ثابت Node &other) ثابت {
    اگر(یہ==&other) {
        واپس سچ;
    }
    اگر(!خطی_مطابقت_گرہ::عامل==(other)) {
        واپس جھوٹ;
    }
    ثابت BTLinear_مطابقت_گرہ &o=static_cast<ثابت BTLinear_مطابقت_گرہ &>(other);
    واپس 0==uprv_memcmp(s, o.s, length);
}

خالی
بائٹیں_Trie_Builder::BTLinear_مطابقت_گرہ::اكتب(StringTrieBuilder &builder) {
    بائٹیں_Trie_Builder &b=static_cast<بائٹیں_Trie_Builder &>(builder);
    next->اكتب(builder);
    b.اكتب(s, length);
    انحراف=b.اكتب(b.get_Min_خطی_مطابقت()+length-1);
}

StringTrieBuilder::Node *
بائٹیں_Trie_Builder::create_خطی_مطابقت_گرہ(int32_t i, int32_t byte_اشاریہ, int32_t length,
                                        Node *اگلا_گرہ) ثابت {
    واپس نیا BTLinear_مطابقت_گرہ(
            elements[i].getString(*لڑیاں).ڈیٹا()+byte_اشاریہ,
            length,
            اگلا_گرہ);
}

UBool
بائٹیں_Trie_Builder::ensure_استعداد(int32_t length) {
    اگر(بائٹیں==خالی_اشاریہ) {
        واپس جھوٹ;  // previous memory allocation had failed
    }
    اگر(length>بائٹیں_استعداد) {
        int32_t new_استعداد=بائٹیں_استعداد;
        کرو {
            new_استعداد*=2;
        } جب(new_استعداد<=length);
        حرف *new_بائٹیں=static_cast<حرف *>(uprv_malloc(new_استعداد));
        اگر(new_بائٹیں==خالی_اشاریہ) {
            // unable to allocate memory
            uprv_free(بائٹیں);
            بائٹیں=خالی_اشاریہ;
            بائٹیں_استعداد=0;
            واپس جھوٹ;
        }
        uprv_memcpy(new_بائٹیں+(new_استعداد-بائٹیں_لمبائی),
                    بائٹیں+(بائٹیں_استعداد-بائٹیں_لمبائی), بائٹیں_لمبائی);
        uprv_free(بائٹیں);
        بائٹیں=new_بائٹیں;
        بائٹیں_استعداد=new_استعداد;
    }
    واپس سچ;
}

int32_t
بائٹیں_Trie_Builder::اكتب(int32_t byte) {
    int32_t new_لمبائی=بائٹیں_لمبائی+1;
    اگر(ensure_استعداد(new_لمبائی)) {
        بائٹیں_لمبائی=new_لمبائی;
        بائٹیں[بائٹیں_استعداد - بائٹیں_لمبائی] = static_cast<حرف>(byte);
    }
    واپس بائٹیں_لمبائی;
}

int32_t
بائٹیں_Trie_Builder::اكتب(ثابت حرف *b, int32_t length) {
    int32_t new_لمبائی=بائٹیں_لمبائی+length;
    اگر(ensure_استعداد(new_لمبائی)) {
        بائٹیں_لمبائی=new_لمبائی;
        uprv_memcpy(بائٹیں+(بائٹیں_استعداد-بائٹیں_لمبائی), b, length);
    }
    واپس بائٹیں_لمبائی;
}

int32_t
بائٹیں_Trie_Builder::writeElementUnits(int32_t i, int32_t byte_اشاریہ, int32_t length) {
    واپس اكتب(elements[i].getString(*لڑیاں).ڈیٹا()+byte_اشاریہ, length);
}

int32_t
بائٹیں_Trie_Builder::write_قدر_And_Final(int32_t i, UBool isFinal) {
    اگر(0<=i && i<=بائٹیں_Trie::k_Max_One_Byte_قدر) {
        واپس اكتب(((بائٹیں_Trie::k_Min_One_Byte_قدر_Lead+i)<<1)|isFinal);
    }
    حرف int_بائٹیں[5];
    int32_t length=1;
    اگر(i<0 || i>0xffffff) {
        int_بائٹیں[0] = static_cast<حرف>(بائٹیں_Trie::k_Five_Byte_قدر_Lead);
        int_بائٹیں[1] = static_cast<حرف>(static_cast<uint32_t>(i) >> 24);
        int_بائٹیں[2] = static_cast<حرف>(static_cast<uint32_t>(i) >> 16);
        int_بائٹیں[3] = static_cast<حرف>(static_cast<uint32_t>(i) >> 8);
        int_بائٹیں[4] = static_cast<حرف>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } ورنہ {
        اگر(i<=بائٹیں_Trie::k_Max_Two_Byte_قدر) {
            int_بائٹیں[0] = static_cast<حرف>(بائٹیں_Trie::k_Min_Two_Byte_قدر_Lead + (i >> 8));
        } ورنہ {
            اگر(i<=بائٹیں_Trie::k_Max_Three_Byte_قدر) {
                int_بائٹیں[0] = static_cast<حرف>(بائٹیں_Trie::k_Min_Three_Byte_قدر_Lead + (i >> 16));
            } ورنہ {
                int_بائٹیں[0] = static_cast<حرف>(بائٹیں_Trie::k_Four_Byte_قدر_Lead);
                int_بائٹیں[1] = static_cast<حرف>(i >> 16);
                length=2;
            }
            int_بائٹیں[length++] = static_cast<حرف>(i >> 8);
        }
        int_بائٹیں[length++] = static_cast<حرف>(i);
    }
    int_بائٹیں[0] = static_cast<حرف>((int_بائٹیں[0] << 1) | isFinal);
    واپس اكتب(int_بائٹیں, length);
}

int32_t
بائٹیں_Trie_Builder::write_قدر_And_Type(UBool has_قدر, int32_t value, int32_t گرہ) {
    int32_t انحراف=اكتب(گرہ);
    اگر(has_قدر) {
        انحراف=write_قدر_And_Final(value, جھوٹ);
    }
    واپس انحراف;
}

int32_t
بائٹیں_Trie_Builder::write_Delta_مقصد(int32_t jump_ہدف) {
    int32_t i=بائٹیں_لمبائی-jump_ہدف;
    U_ASSERT(i>=0);
    اگر(i<=بائٹیں_Trie::kMaxOneByteDelta) {
        واپس اكتب(i);
    } ورنہ {
        حرف int_بائٹیں[5];
        واپس اكتب(int_بائٹیں, اندرونی_Encode_Delta(i, int_بائٹیں));
    }
}

int32_t
بائٹیں_Trie_Builder::اندرونی_Encode_Delta(int32_t i, حرف int_بائٹیں[]) {
    U_ASSERT(i>=0);
    اگر(i<=بائٹیں_Trie::kMaxOneByteDelta) {
        int_بائٹیں[0] = static_cast<حرف>(i);
        واپس 1;
    }
    int32_t length=1;
    اگر(i<=بائٹیں_Trie::kMaxTwoByteDelta) {
        int_بائٹیں[0] = static_cast<حرف>(بائٹیں_Trie::kMinTwoByteDeltaLead + (i >> 8));
    } ورنہ {
        اگر(i<=بائٹیں_Trie::kMaxThreeByteDelta) {
            int_بائٹیں[0] = static_cast<حرف>(بائٹیں_Trie::kMinThreeByteDeltaLead + (i >> 16));
        } ورنہ {
            اگر(i<=0xffffff) {
                int_بائٹیں[0] = static_cast<حرف>(بائٹیں_Trie::kFourByteDeltaLead);
            } ورنہ {
                int_بائٹیں[0] = static_cast<حرف>(بائٹیں_Trie::kFiveByteDeltaLead);
                int_بائٹیں[1] = static_cast<حرف>(i >> 24);
                length=2;
            }
            int_بائٹیں[length++] = static_cast<حرف>(i >> 16);
        }
        int_بائٹیں[length++] = static_cast<حرف>(i >> 8);
    }
    int_بائٹیں[length++] = static_cast<حرف>(i);
    واپس length;
}

U_NAMESPACE_انتہا
