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

صنف بایتها_Trie_Element : عام UMemory {
عام:
    // Use compiler's default constructor, initializes nothing.

    فراغ مجموعه_مقصد(StringPiece s, int32_t val, CharString &رشتهها, UError_کد &error_کد);

    StringPiece getString(ثابت CharString& رشتهها U_LIFETIME_BOUND) ثابت {
        int32_t جابه_جایی=string_جابه_جایی;
        int32_t length;
        إذا(جابه_جایی>=0) {
            length = static_cast<uint8_t>(رشتهها[جابه_جایی++]);
        } وإلا {
            جابه_جایی=~جابه_جایی;
            length = (static_cast<int32_t>(static_cast<uint8_t>(رشتهها[جابه_جایی])) << 8) | static_cast<uint8_t>(رشتهها[جابه_جایی + 1]);
            جابه_جایی+=2;
        }
        أرجع StringPiece(رشتهها.داده()+جابه_جایی, length);
    }
    int32_t get_String_درازی(ثابت CharString &رشتهها) ثابت {
        int32_t جابه_جایی=string_جابه_جایی;
        إذا(جابه_جایی>=0) {
            أرجع static_cast<uint8_t>(رشتهها[جابه_جایی]);
        } وإلا {
            جابه_جایی=~جابه_جایی;
            أرجع (static_cast<int32_t>(static_cast<uint8_t>(رشتهها[جابه_جایی])) << 8) | static_cast<uint8_t>(رشتهها[جابه_جایی + 1]);
        }
    }

    حرف charAt(int32_t شاخص, ثابت CharString &رشتهها) ثابت { أرجع داده(رشتهها)[شاخص]; }

    int32_t get_مقدار() ثابت { أرجع value; }

    int32_t compare_String_مقصد(ثابت بایتها_Trie_Element &o, ثابت CharString &رشتهها) ثابت;

خاص:
    ثابت حرف* داده(ثابت CharString& رشتهها U_LIFETIME_BOUND) ثابت {
        int32_t جابه_جایی=string_جابه_جایی;
        إذا(جابه_جایی>=0) {
            ++جابه_جایی;
        } وإلا {
            جابه_جایی=~جابه_جایی+2;
        }
        أرجع رشتهها.داده()+جابه_جایی;
    }

    // If the stringOffset is non-negative, then the first strings byte contains
    // the string length.
    // If the stringOffset is negative, then the first two strings bytes contain
    // the string length (big-endian), and the offset needs to be bit-inverted.
    // (Compared with a stringLength field here, this saves 3 bytes per string for most strings.)
    int32_t string_جابه_جایی;
    int32_t value;
};

فراغ
بایتها_Trie_Element::مجموعه_مقصد(StringPiece s, int32_t val,
                        CharString &رشتهها, UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع;
    }
    int32_t length=s.length();
    إذا(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع;
    }
    int32_t جابه_جایی=رشتهها.length();
    إذا(length>0xff) {
        جابه_جایی=~جابه_جایی;
        رشتهها.append(static_cast<حرف>(length >> 8), error_کد);
    }
    رشتهها.append(static_cast<حرف>(length), error_کد);
    string_جابه_جایی=جابه_جایی;
    value=val;
    رشتهها.append(s, error_کد);
}

int32_t
بایتها_Trie_Element::compare_String_مقصد(ثابت بایتها_Trie_Element &other, ثابت CharString &رشتهها) ثابت {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(رشتهها);
    StringPiece otherString=other.getString(رشتهها);
    int32_t درازی_Diff=thisString.length()-otherString.length();
    int32_t common_درازی;
    إذا(درازی_Diff<=0) {
        common_درازی=thisString.length();
    } وإلا {
        common_درازی=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.داده(), otherString.داده(), common_درازی);
    أرجع diff!=0 ? diff : درازی_Diff;
}

بایتها_Trie_Builder::بایتها_Trie_Builder(UError_کد &error_کد)
        : رشتهها(اشاره_تهی), elements(اشاره_تهی), elements_ظرفیت(0), elements_درازی(0),
          بایتها(اشاره_تهی), بایتها_ظرفیت(0), بایتها_درازی(0) {
    إذا(U_FAILURE(error_کد)) {
        أرجع;
    }
    رشتهها=جديد CharString();
    إذا(رشتهها==اشاره_تهی) {
        error_کد=U_حافظه_تخصیص_ERROR;
    }
}

بایتها_Trie_Builder::~بایتها_Trie_Builder() {
    حذف رشتهها;
    حذف[] elements;
    uprv_free(بایتها);
}

بایتها_Trie_Builder&
بایتها_Trie_Builder::جمع_کن(StringPiece s, int32_t value, UError_کد& error_کد) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_کد)) {
        أرجع *هذا;
    }
    إذا(بایتها_درازی>0) {
        // Cannot add elements after building.
        error_کد=U_NO_WRITE_PERMISSION;
        أرجع *هذا;
    }
    إذا(elements_درازی==elements_ظرفیت) {
        int32_t new_ظرفیت;
        إذا(elements_ظرفیت==0) {
            new_ظرفیت=1024;
        } وإلا {
            new_ظرفیت=4*elements_ظرفیت;
        }
        بایتها_Trie_Element *newElements=جديد بایتها_Trie_Element[new_ظرفیت];
        إذا(newElements==اشاره_تهی) {
            error_کد=U_حافظه_تخصیص_ERROR;
            أرجع *هذا; // error instead of dereferencing null
        }
        إذا(elements_درازی>0) {
            uprv_memcpy(newElements, elements, (size_t)elements_درازی*حجم(بایتها_Trie_Element));
        }
        حذف[] elements;
        elements=newElements;
        elements_ظرفیت=new_ظرفیت;
    }
    elements[elements_درازی++].مجموعه_مقصد(s, value, *رشتهها, error_کد);
    أرجع *هذا;
}

U_CDECL_BEGIN

ساكن int32_t U_CALLCONV
compare_Element_رشتهها(ثابت فراغ *context, ثابت فراغ *چپ, ثابت فراغ *راست) {
    ثابت CharString *رشتهها=static_cast<ثابت CharString *>(context);
    ثابت بایتها_Trie_Element *چپ_Element=static_cast<ثابت بایتها_Trie_Element *>(چپ);
    ثابت بایتها_Trie_Element *راست_Element=static_cast<ثابت بایتها_Trie_Element *>(راست);
    أرجع چپ_Element->compare_String_مقصد(*راست_Element, *رشتهها);
}

U_CDECL_انتها

بایتها_Trie *
بایتها_Trie_Builder::build(UStringTrieBuildOption buildOption, UError_کد &error_کد) {
    build_بایتها(buildOption, error_کد);
    بایتها_Trie *newTrie=اشاره_تهی;
    إذا(U_SUCCESS(error_کد)) {
        newTrie=جديد بایتها_Trie(بایتها, بایتها+(بایتها_ظرفیت-بایتها_درازی));
        إذا(newTrie==اشاره_تهی) {
            error_کد=U_حافظه_تخصیص_ERROR;
        } وإلا {
            بایتها=اشاره_تهی;  // The new trie now owns the array.
            بایتها_ظرفیت=0;
        }
    }
    أرجع newTrie;
}

StringPiece
بایتها_Trie_Builder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UError_کد& error_کد) U_LIFETIME_BOUND {
    build_بایتها(buildOption, error_کد);
    StringPiece نتیجه;
    إذا(U_SUCCESS(error_کد)) {
        نتیجه.مجموعه(بایتها+(بایتها_ظرفیت-بایتها_درازی), بایتها_درازی);
    }
    أرجع نتیجه;
}

فراغ
بایتها_Trie_Builder::build_بایتها(UStringTrieBuildOption buildOption, UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع;
    }
    إذا(بایتها!=اشاره_تهی && بایتها_درازی>0) {
        // Already built.
        أرجع;
    }
    إذا(بایتها_درازی==0) {
        إذا(elements_درازی==0) {
            error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع;
        }
        uprv_مرتب_سازی_Array(elements, elements_درازی, static_cast<int32_t>(حجم(بایتها_Trie_Element)),
                      compare_Element_رشتهها, رشتهها,
                      خطأ,  // need not be a stable sort
                      &error_کد);
        إذا(U_FAILURE(error_کد)) {
            أرجع;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*رشتهها);
        لكل(int32_t i=1; i<elements_درازی; ++i) {
            StringPiece current=elements[i].getString(*رشتهها);
            إذا(prev==current) {
                error_کد=U_ILLEGAL_آرگومان_ERROR;
                أرجع;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    بایتها_درازی=0;
    int32_t capacity=رشتهها->length();
    إذا(capacity<1024) {
        capacity=1024;
    }
    إذا(بایتها_ظرفیت<capacity) {
        uprv_free(بایتها);
        بایتها=static_cast<حرف *>(uprv_malloc(capacity));
        إذا(بایتها==اشاره_تهی) {
            error_کد=U_حافظه_تخصیص_ERROR;
            بایتها_ظرفیت=0;
            أرجع;
        }
        بایتها_ظرفیت=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_درازی, error_کد);
    إذا(بایتها==اشاره_تهی) {
        error_کد=U_حافظه_تخصیص_ERROR;
    }
}

بایتها_Trie_Builder&
بایتها_Trie_Builder::clear() U_LIFETIME_BOUND {
    رشتهها->clear();
    elements_درازی=0;
    بایتها_درازی=0;
    أرجع *هذا;
}

int32_t
بایتها_Trie_Builder::get_Element_String_درازی(int32_t i) ثابت {
    أرجع elements[i].get_String_درازی(*رشتهها);
}

char16_t
بایتها_Trie_Builder::getElementUnit(int32_t i, int32_t byte_شاخص) ثابت {
    أرجع static_cast<uint8_t>(elements[i].charAt(byte_شاخص, *رشتهها));
}

int32_t
بایتها_Trie_Builder::get_Element_مقدار(int32_t i) ثابت {
    أرجع elements[i].get_مقدار();
}

int32_t
بایتها_Trie_Builder::get_حد_از_خطی_تطابق(int32_t first, int32_t آخرین, int32_t byte_شاخص) ثابت {
    ثابت بایتها_Trie_Element &اولی_Element=elements[first];
    ثابت بایتها_Trie_Element &آخرین_Element=elements[آخرین];
    int32_t min_String_درازی=اولی_Element.get_String_درازی(*رشتهها);
    طالما(++byte_شاخص<min_String_درازی &&
            اولی_Element.charAt(byte_شاخص, *رشتهها)==
            آخرین_Element.charAt(byte_شاخص, *رشتهها)) {}
    أرجع byte_شاخص;
}

int32_t
بایتها_Trie_Builder::شمار_Element_Units(int32_t شروع, int32_t حد, int32_t byte_شاخص) ثابت {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=شروع;
    افعل {
        حرف byte=elements[i++].charAt(byte_شاخص, *رشتهها);
        طالما(i<حد && byte==elements[i].charAt(byte_شاخص, *رشتهها)) {
            ++i;
        }
        ++length;
    } طالما(i<حد);
    أرجع length;
}

int32_t
بایتها_Trie_Builder::skipElementsBySomeUnits(int32_t i, int32_t byte_شاخص, int32_t count) ثابت {
    افعل {
        حرف byte=elements[i++].charAt(byte_شاخص, *رشتهها);
        طالما(byte==elements[i].charAt(byte_شاخص, *رشتهها)) {
            ++i;
        }
    } طالما(--count>0);
    أرجع i;
}

int32_t
بایتها_Trie_Builder::شاخص_از_Element_With_بعدی_Unit(int32_t i, int32_t byte_شاخص, char16_t byte) ثابت {
    حرف b = static_cast<حرف>(byte);
    طالما(b==elements[i].charAt(byte_شاخص, *رشتهها)) {
        ++i;
    }
    أرجع i;
}

بایتها_Trie_Builder::BTLinear_تطابق_گره::BTLinear_تطابق_گره(ثابت حرف *بایتها, int32_t len, Node *بعدی_گره)
        : خطی_تطابق_گره(len, بعدی_گره), s(بایتها) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(بایتها, len)));
}

منطقي
بایتها_Trie_Builder::BTLinear_تطابق_گره::عملگر==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع درست;
    }
    إذا(!خطی_تطابق_گره::عملگر==(other)) {
        أرجع خطأ;
    }
    ثابت BTLinear_تطابق_گره &o=static_cast<ثابت BTLinear_تطابق_گره &>(other);
    أرجع 0==uprv_memcmp(s, o.s, length);
}

فراغ
بایتها_Trie_Builder::BTLinear_تطابق_گره::اكتب(StringTrieBuilder &builder) {
    بایتها_Trie_Builder &b=static_cast<بایتها_Trie_Builder &>(builder);
    next->اكتب(builder);
    b.اكتب(s, length);
    جابه_جایی=b.اكتب(b.get_Min_خطی_تطابق()+length-1);
}

StringTrieBuilder::Node *
بایتها_Trie_Builder::create_خطی_تطابق_گره(int32_t i, int32_t byte_شاخص, int32_t length,
                                        Node *بعدی_گره) ثابت {
    أرجع جديد BTLinear_تطابق_گره(
            elements[i].getString(*رشتهها).داده()+byte_شاخص,
            length,
            بعدی_گره);
}

UBool
بایتها_Trie_Builder::ensure_ظرفیت(int32_t length) {
    إذا(بایتها==اشاره_تهی) {
        أرجع خطأ;  // previous memory allocation had failed
    }
    إذا(length>بایتها_ظرفیت) {
        int32_t new_ظرفیت=بایتها_ظرفیت;
        افعل {
            new_ظرفیت*=2;
        } طالما(new_ظرفیت<=length);
        حرف *new_بایتها=static_cast<حرف *>(uprv_malloc(new_ظرفیت));
        إذا(new_بایتها==اشاره_تهی) {
            // unable to allocate memory
            uprv_free(بایتها);
            بایتها=اشاره_تهی;
            بایتها_ظرفیت=0;
            أرجع خطأ;
        }
        uprv_memcpy(new_بایتها+(new_ظرفیت-بایتها_درازی),
                    بایتها+(بایتها_ظرفیت-بایتها_درازی), بایتها_درازی);
        uprv_free(بایتها);
        بایتها=new_بایتها;
        بایتها_ظرفیت=new_ظرفیت;
    }
    أرجع درست;
}

int32_t
بایتها_Trie_Builder::اكتب(int32_t byte) {
    int32_t new_درازی=بایتها_درازی+1;
    إذا(ensure_ظرفیت(new_درازی)) {
        بایتها_درازی=new_درازی;
        بایتها[بایتها_ظرفیت - بایتها_درازی] = static_cast<حرف>(byte);
    }
    أرجع بایتها_درازی;
}

int32_t
بایتها_Trie_Builder::اكتب(ثابت حرف *b, int32_t length) {
    int32_t new_درازی=بایتها_درازی+length;
    إذا(ensure_ظرفیت(new_درازی)) {
        بایتها_درازی=new_درازی;
        uprv_memcpy(بایتها+(بایتها_ظرفیت-بایتها_درازی), b, length);
    }
    أرجع بایتها_درازی;
}

int32_t
بایتها_Trie_Builder::writeElementUnits(int32_t i, int32_t byte_شاخص, int32_t length) {
    أرجع اكتب(elements[i].getString(*رشتهها).داده()+byte_شاخص, length);
}

int32_t
بایتها_Trie_Builder::write_مقدار_And_Final(int32_t i, UBool isFinal) {
    إذا(0<=i && i<=بایتها_Trie::k_Max_One_Byte_مقدار) {
        أرجع اكتب(((بایتها_Trie::k_Min_One_Byte_مقدار_Lead+i)<<1)|isFinal);
    }
    حرف int_بایتها[5];
    int32_t length=1;
    إذا(i<0 || i>0xffffff) {
        int_بایتها[0] = static_cast<حرف>(بایتها_Trie::k_Five_Byte_مقدار_Lead);
        int_بایتها[1] = static_cast<حرف>(static_cast<uint32_t>(i) >> 24);
        int_بایتها[2] = static_cast<حرف>(static_cast<uint32_t>(i) >> 16);
        int_بایتها[3] = static_cast<حرف>(static_cast<uint32_t>(i) >> 8);
        int_بایتها[4] = static_cast<حرف>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } وإلا {
        إذا(i<=بایتها_Trie::k_Max_Two_Byte_مقدار) {
            int_بایتها[0] = static_cast<حرف>(بایتها_Trie::k_Min_Two_Byte_مقدار_Lead + (i >> 8));
        } وإلا {
            إذا(i<=بایتها_Trie::k_Max_Three_Byte_مقدار) {
                int_بایتها[0] = static_cast<حرف>(بایتها_Trie::k_Min_Three_Byte_مقدار_Lead + (i >> 16));
            } وإلا {
                int_بایتها[0] = static_cast<حرف>(بایتها_Trie::k_Four_Byte_مقدار_Lead);
                int_بایتها[1] = static_cast<حرف>(i >> 16);
                length=2;
            }
            int_بایتها[length++] = static_cast<حرف>(i >> 8);
        }
        int_بایتها[length++] = static_cast<حرف>(i);
    }
    int_بایتها[0] = static_cast<حرف>((int_بایتها[0] << 1) | isFinal);
    أرجع اكتب(int_بایتها, length);
}

int32_t
بایتها_Trie_Builder::write_مقدار_And_Type(UBool has_مقدار, int32_t value, int32_t گره) {
    int32_t جابه_جایی=اكتب(گره);
    إذا(has_مقدار) {
        جابه_جایی=write_مقدار_And_Final(value, خطأ);
    }
    أرجع جابه_جایی;
}

int32_t
بایتها_Trie_Builder::write_Delta_مقصد(int32_t jump_هدف) {
    int32_t i=بایتها_درازی-jump_هدف;
    U_ASSERT(i>=0);
    إذا(i<=بایتها_Trie::kMaxOneByteDelta) {
        أرجع اكتب(i);
    } وإلا {
        حرف int_بایتها[5];
        أرجع اكتب(int_بایتها, داخلی_Encode_Delta(i, int_بایتها));
    }
}

int32_t
بایتها_Trie_Builder::داخلی_Encode_Delta(int32_t i, حرف int_بایتها[]) {
    U_ASSERT(i>=0);
    إذا(i<=بایتها_Trie::kMaxOneByteDelta) {
        int_بایتها[0] = static_cast<حرف>(i);
        أرجع 1;
    }
    int32_t length=1;
    إذا(i<=بایتها_Trie::kMaxTwoByteDelta) {
        int_بایتها[0] = static_cast<حرف>(بایتها_Trie::kMinTwoByteDeltaLead + (i >> 8));
    } وإلا {
        إذا(i<=بایتها_Trie::kMaxThreeByteDelta) {
            int_بایتها[0] = static_cast<حرف>(بایتها_Trie::kMinThreeByteDeltaLead + (i >> 16));
        } وإلا {
            إذا(i<=0xffffff) {
                int_بایتها[0] = static_cast<حرف>(بایتها_Trie::kFourByteDeltaLead);
            } وإلا {
                int_بایتها[0] = static_cast<حرف>(بایتها_Trie::kFiveByteDeltaLead);
                int_بایتها[1] = static_cast<حرف>(i >> 24);
                length=2;
            }
            int_بایتها[length++] = static_cast<حرف>(i >> 16);
        }
        int_بایتها[length++] = static_cast<حرف>(i >> 8);
    }
    int_بایتها[length++] = static_cast<حرف>(i);
    أرجع length;
}

U_NAMESPACE_انتها
