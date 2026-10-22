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

صنف بایټونه_Trie_Element : عام UMemory {
عام:
    // Use compiler's default constructor, initializes nothing.

    فراغ سېټ_منزل(StringPiece s, int32_t val, CharString &مزي_ګانې, UError_کوډ &error_کوډ);

    StringPiece getString(ثابت CharString& مزي_ګانې U_LIFETIME_BOUND) ثابت {
        int32_t دننه_واټن=string_دننه_واټن;
        int32_t length;
        إذا(دننه_واټن>=0) {
            length = static_cast<uint8_t>(مزي_ګانې[دننه_واټن++]);
        } وإلا {
            دننه_واټن=~دننه_واټن;
            length = (static_cast<int32_t>(static_cast<uint8_t>(مزي_ګانې[دننه_واټن])) << 8) | static_cast<uint8_t>(مزي_ګانې[دننه_واټن + 1]);
            دننه_واټن+=2;
        }
        أرجع StringPiece(مزي_ګانې.معلومات()+دننه_واټن, length);
    }
    int32_t get_String_اوږدتیا(ثابت CharString &مزي_ګانې) ثابت {
        int32_t دننه_واټن=string_دننه_واټن;
        إذا(دننه_واټن>=0) {
            أرجع static_cast<uint8_t>(مزي_ګانې[دننه_واټن]);
        } وإلا {
            دننه_واټن=~دننه_واټن;
            أرجع (static_cast<int32_t>(static_cast<uint8_t>(مزي_ګانې[دننه_واټن])) << 8) | static_cast<uint8_t>(مزي_ګانې[دننه_واټن + 1]);
        }
    }

    حرف charAt(int32_t شاخص, ثابت CharString &مزي_ګانې) ثابت { أرجع معلومات(مزي_ګانې)[شاخص]; }

    int32_t get_ارزښت() ثابت { أرجع value; }

    int32_t compare_String_منزل(ثابت بایټونه_Trie_Element &o, ثابت CharString &مزي_ګانې) ثابت;

خاص:
    ثابت حرف* معلومات(ثابت CharString& مزي_ګانې U_LIFETIME_BOUND) ثابت {
        int32_t دننه_واټن=string_دننه_واټن;
        إذا(دننه_واټن>=0) {
            ++دننه_واټن;
        } وإلا {
            دننه_واټن=~دننه_واټن+2;
        }
        أرجع مزي_ګانې.معلومات()+دننه_واټن;
    }

    // If the stringOffset is non-negative, then the first strings byte contains
    // the string length.
    // If the stringOffset is negative, then the first two strings bytes contain
    // the string length (big-endian), and the offset needs to be bit-inverted.
    // (Compared with a stringLength field here, this saves 3 bytes per string for most strings.)
    int32_t string_دننه_واټن;
    int32_t value;
};

فراغ
بایټونه_Trie_Element::سېټ_منزل(StringPiece s, int32_t val,
                        CharString &مزي_ګانې, UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع;
    }
    int32_t length=s.length();
    إذا(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع;
    }
    int32_t دننه_واټن=مزي_ګانې.length();
    إذا(length>0xff) {
        دننه_واټن=~دننه_واټن;
        مزي_ګانې.append(static_cast<حرف>(length >> 8), error_کوډ);
    }
    مزي_ګانې.append(static_cast<حرف>(length), error_کوډ);
    string_دننه_واټن=دننه_واټن;
    value=val;
    مزي_ګانې.append(s, error_کوډ);
}

int32_t
بایټونه_Trie_Element::compare_String_منزل(ثابت بایټونه_Trie_Element &other, ثابت CharString &مزي_ګانې) ثابت {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(مزي_ګانې);
    StringPiece otherString=other.getString(مزي_ګانې);
    int32_t اوږدتیا_Diff=thisString.length()-otherString.length();
    int32_t common_اوږدتیا;
    إذا(اوږدتیا_Diff<=0) {
        common_اوږدتیا=thisString.length();
    } وإلا {
        common_اوږدتیا=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.معلومات(), otherString.معلومات(), common_اوږدتیا);
    أرجع diff!=0 ? diff : اوږدتیا_Diff;
}

بایټونه_Trie_Builder::بایټونه_Trie_Builder(UError_کوډ &error_کوډ)
        : مزي_ګانې(تش_اشاره), elements(تش_اشاره), elements_ظرفیت(0), elements_اوږدتیا(0),
          بایټونه(تش_اشاره), بایټونه_ظرفیت(0), بایټونه_اوږدتیا(0) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع;
    }
    مزي_ګانې=جديد CharString();
    إذا(مزي_ګانې==تش_اشاره) {
        error_کوډ=U_حافظه_ویش_ERROR;
    }
}

بایټونه_Trie_Builder::~بایټونه_Trie_Builder() {
    ړنګول مزي_ګانې;
    ړنګول[] elements;
    uprv_free(بایټونه);
}

بایټونه_Trie_Builder&
بایټونه_Trie_Builder::جمع_کړه(StringPiece s, int32_t value, UError_کوډ& error_کوډ) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع *هذا;
    }
    إذا(بایټونه_اوږدتیا>0) {
        // Cannot add elements after building.
        error_کوډ=U_NO_WRITE_PERMISSION;
        أرجع *هذا;
    }
    إذا(elements_اوږدتیا==elements_ظرفیت) {
        int32_t new_ظرفیت;
        إذا(elements_ظرفیت==0) {
            new_ظرفیت=1024;
        } وإلا {
            new_ظرفیت=4*elements_ظرفیت;
        }
        بایټونه_Trie_Element *newElements=جديد بایټونه_Trie_Element[new_ظرفیت];
        إذا(newElements==تش_اشاره) {
            error_کوډ=U_حافظه_ویش_ERROR;
            أرجع *هذا; // error instead of dereferencing null
        }
        إذا(elements_اوږدتیا>0) {
            uprv_memcpy(newElements, elements, (size_t)elements_اوږدتیا*حجم(بایټونه_Trie_Element));
        }
        ړنګول[] elements;
        elements=newElements;
        elements_ظرفیت=new_ظرفیت;
    }
    elements[elements_اوږدتیا++].سېټ_منزل(s, value, *مزي_ګانې, error_کوډ);
    أرجع *هذا;
}

U_CDECL_BEGIN

ساكن int32_t U_CALLCONV
compare_Element_مزي_ګانې(ثابت فراغ *context, ثابت فراغ *کیڼ, ثابت فراغ *ښی) {
    ثابت CharString *مزي_ګانې=static_cast<ثابت CharString *>(context);
    ثابت بایټونه_Trie_Element *کیڼ_Element=static_cast<ثابت بایټونه_Trie_Element *>(کیڼ);
    ثابت بایټونه_Trie_Element *ښی_Element=static_cast<ثابت بایټونه_Trie_Element *>(ښی);
    أرجع کیڼ_Element->compare_String_منزل(*ښی_Element, *مزي_ګانې);
}

U_CDECL_پای_ټکی

بایټونه_Trie *
بایټونه_Trie_Builder::build(UStringTrieBuildOption buildOption, UError_کوډ &error_کوډ) {
    build_بایټونه(buildOption, error_کوډ);
    بایټونه_Trie *newTrie=تش_اشاره;
    إذا(U_SUCCESS(error_کوډ)) {
        newTrie=جديد بایټونه_Trie(بایټونه, بایټونه+(بایټونه_ظرفیت-بایټونه_اوږدتیا));
        إذا(newTrie==تش_اشاره) {
            error_کوډ=U_حافظه_ویش_ERROR;
        } وإلا {
            بایټونه=تش_اشاره;  // The new trie now owns the array.
            بایټونه_ظرفیت=0;
        }
    }
    أرجع newTrie;
}

StringPiece
بایټونه_Trie_Builder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UError_کوډ& error_کوډ) U_LIFETIME_BOUND {
    build_بایټونه(buildOption, error_کوډ);
    StringPiece پایله;
    إذا(U_SUCCESS(error_کوډ)) {
        پایله.سېټ(بایټونه+(بایټونه_ظرفیت-بایټونه_اوږدتیا), بایټونه_اوږدتیا);
    }
    أرجع پایله;
}

فراغ
بایټونه_Trie_Builder::build_بایټونه(UStringTrieBuildOption buildOption, UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع;
    }
    إذا(بایټونه!=تش_اشاره && بایټونه_اوږدتیا>0) {
        // Already built.
        أرجع;
    }
    إذا(بایټونه_اوږدتیا==0) {
        إذا(elements_اوږدتیا==0) {
            error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع;
        }
        uprv_ترتیبول_Array(elements, elements_اوږدتیا, static_cast<int32_t>(حجم(بایټونه_Trie_Element)),
                      compare_Element_مزي_ګانې, مزي_ګانې,
                      خطأ,  // need not be a stable sort
                      &error_کوډ);
        إذا(U_FAILURE(error_کوډ)) {
            أرجع;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*مزي_ګانې);
        لكل(int32_t i=1; i<elements_اوږدتیا; ++i) {
            StringPiece current=elements[i].getString(*مزي_ګانې);
            إذا(prev==current) {
                error_کوډ=U_ILLEGAL_دلیل_ERROR;
                أرجع;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    بایټونه_اوږدتیا=0;
    int32_t capacity=مزي_ګانې->length();
    إذا(capacity<1024) {
        capacity=1024;
    }
    إذا(بایټونه_ظرفیت<capacity) {
        uprv_free(بایټونه);
        بایټونه=static_cast<حرف *>(uprv_malloc(capacity));
        إذا(بایټونه==تش_اشاره) {
            error_کوډ=U_حافظه_ویش_ERROR;
            بایټونه_ظرفیت=0;
            أرجع;
        }
        بایټونه_ظرفیت=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_اوږدتیا, error_کوډ);
    إذا(بایټونه==تش_اشاره) {
        error_کوډ=U_حافظه_ویش_ERROR;
    }
}

بایټونه_Trie_Builder&
بایټونه_Trie_Builder::clear() U_LIFETIME_BOUND {
    مزي_ګانې->clear();
    elements_اوږدتیا=0;
    بایټونه_اوږدتیا=0;
    أرجع *هذا;
}

int32_t
بایټونه_Trie_Builder::get_Element_String_اوږدتیا(int32_t i) ثابت {
    أرجع elements[i].get_String_اوږدتیا(*مزي_ګانې);
}

char16_t
بایټونه_Trie_Builder::getElementUnit(int32_t i, int32_t byte_شاخص) ثابت {
    أرجع static_cast<uint8_t>(elements[i].charAt(byte_شاخص, *مزي_ګانې));
}

int32_t
بایټونه_Trie_Builder::get_Element_ارزښت(int32_t i) ثابت {
    أرجع elements[i].get_ارزښت();
}

int32_t
بایټونه_Trie_Builder::get_حد_د_خطي_سمون(int32_t first, int32_t وروستی, int32_t byte_شاخص) ثابت {
    ثابت بایټونه_Trie_Element &لومړی_Element=elements[first];
    ثابت بایټونه_Trie_Element &وروستی_Element=elements[وروستی];
    int32_t min_String_اوږدتیا=لومړی_Element.get_String_اوږدتیا(*مزي_ګانې);
    طالما(++byte_شاخص<min_String_اوږدتیا &&
            لومړی_Element.charAt(byte_شاخص, *مزي_ګانې)==
            وروستی_Element.charAt(byte_شاخص, *مزي_ګانې)) {}
    أرجع byte_شاخص;
}

int32_t
بایټونه_Trie_Builder::شمېر_Element_Units(int32_t پیل, int32_t حد, int32_t byte_شاخص) ثابت {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=پیل;
    افعل {
        حرف byte=elements[i++].charAt(byte_شاخص, *مزي_ګانې);
        طالما(i<حد && byte==elements[i].charAt(byte_شاخص, *مزي_ګانې)) {
            ++i;
        }
        ++length;
    } طالما(i<حد);
    أرجع length;
}

int32_t
بایټونه_Trie_Builder::skipElementsBySomeUnits(int32_t i, int32_t byte_شاخص, int32_t count) ثابت {
    افعل {
        حرف byte=elements[i++].charAt(byte_شاخص, *مزي_ګانې);
        طالما(byte==elements[i].charAt(byte_شاخص, *مزي_ګانې)) {
            ++i;
        }
    } طالما(--count>0);
    أرجع i;
}

int32_t
بایټونه_Trie_Builder::شاخص_د_Element_With_بل_Unit(int32_t i, int32_t byte_شاخص, char16_t byte) ثابت {
    حرف b = static_cast<حرف>(byte);
    طالما(b==elements[i].charAt(byte_شاخص, *مزي_ګانې)) {
        ++i;
    }
    أرجع i;
}

بایټونه_Trie_Builder::BTLinear_سمون_غوټه::BTLinear_سمون_غوټه(ثابت حرف *بایټونه, int32_t len, Node *بل_غوټه)
        : خطي_سمون_غوټه(len, بل_غوټه), s(بایټونه) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(بایټونه, len)));
}

منطقي
بایټونه_Trie_Builder::BTLinear_سمون_غوټه::چلوونکی==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع ریښتیا;
    }
    إذا(!خطي_سمون_غوټه::چلوونکی==(other)) {
        أرجع خطأ;
    }
    ثابت BTLinear_سمون_غوټه &o=static_cast<ثابت BTLinear_سمون_غوټه &>(other);
    أرجع 0==uprv_memcmp(s, o.s, length);
}

فراغ
بایټونه_Trie_Builder::BTLinear_سمون_غوټه::اكتب(StringTrieBuilder &builder) {
    بایټونه_Trie_Builder &b=static_cast<بایټونه_Trie_Builder &>(builder);
    next->اكتب(builder);
    b.اكتب(s, length);
    دننه_واټن=b.اكتب(b.get_Min_خطي_سمون()+length-1);
}

StringTrieBuilder::Node *
بایټونه_Trie_Builder::create_خطي_سمون_غوټه(int32_t i, int32_t byte_شاخص, int32_t length,
                                        Node *بل_غوټه) ثابت {
    أرجع جديد BTLinear_سمون_غوټه(
            elements[i].getString(*مزي_ګانې).معلومات()+byte_شاخص,
            length,
            بل_غوټه);
}

UBool
بایټونه_Trie_Builder::ensure_ظرفیت(int32_t length) {
    إذا(بایټونه==تش_اشاره) {
        أرجع خطأ;  // previous memory allocation had failed
    }
    إذا(length>بایټونه_ظرفیت) {
        int32_t new_ظرفیت=بایټونه_ظرفیت;
        افعل {
            new_ظرفیت*=2;
        } طالما(new_ظرفیت<=length);
        حرف *new_بایټونه=static_cast<حرف *>(uprv_malloc(new_ظرفیت));
        إذا(new_بایټونه==تش_اشاره) {
            // unable to allocate memory
            uprv_free(بایټونه);
            بایټونه=تش_اشاره;
            بایټونه_ظرفیت=0;
            أرجع خطأ;
        }
        uprv_memcpy(new_بایټونه+(new_ظرفیت-بایټونه_اوږدتیا),
                    بایټونه+(بایټونه_ظرفیت-بایټونه_اوږدتیا), بایټونه_اوږدتیا);
        uprv_free(بایټونه);
        بایټونه=new_بایټونه;
        بایټونه_ظرفیت=new_ظرفیت;
    }
    أرجع ریښتیا;
}

int32_t
بایټونه_Trie_Builder::اكتب(int32_t byte) {
    int32_t new_اوږدتیا=بایټونه_اوږدتیا+1;
    إذا(ensure_ظرفیت(new_اوږدتیا)) {
        بایټونه_اوږدتیا=new_اوږدتیا;
        بایټونه[بایټونه_ظرفیت - بایټونه_اوږدتیا] = static_cast<حرف>(byte);
    }
    أرجع بایټونه_اوږدتیا;
}

int32_t
بایټونه_Trie_Builder::اكتب(ثابت حرف *b, int32_t length) {
    int32_t new_اوږدتیا=بایټونه_اوږدتیا+length;
    إذا(ensure_ظرفیت(new_اوږدتیا)) {
        بایټونه_اوږدتیا=new_اوږدتیا;
        uprv_memcpy(بایټونه+(بایټونه_ظرفیت-بایټونه_اوږدتیا), b, length);
    }
    أرجع بایټونه_اوږدتیا;
}

int32_t
بایټونه_Trie_Builder::writeElementUnits(int32_t i, int32_t byte_شاخص, int32_t length) {
    أرجع اكتب(elements[i].getString(*مزي_ګانې).معلومات()+byte_شاخص, length);
}

int32_t
بایټونه_Trie_Builder::write_ارزښت_And_Final(int32_t i, UBool isFinal) {
    إذا(0<=i && i<=بایټونه_Trie::k_Max_One_Byte_ارزښت) {
        أرجع اكتب(((بایټونه_Trie::k_Min_One_Byte_ارزښت_Lead+i)<<1)|isFinal);
    }
    حرف int_بایټونه[5];
    int32_t length=1;
    إذا(i<0 || i>0xffffff) {
        int_بایټونه[0] = static_cast<حرف>(بایټونه_Trie::k_Five_Byte_ارزښت_Lead);
        int_بایټونه[1] = static_cast<حرف>(static_cast<uint32_t>(i) >> 24);
        int_بایټونه[2] = static_cast<حرف>(static_cast<uint32_t>(i) >> 16);
        int_بایټونه[3] = static_cast<حرف>(static_cast<uint32_t>(i) >> 8);
        int_بایټونه[4] = static_cast<حرف>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } وإلا {
        إذا(i<=بایټونه_Trie::k_Max_Two_Byte_ارزښت) {
            int_بایټونه[0] = static_cast<حرف>(بایټونه_Trie::k_Min_Two_Byte_ارزښت_Lead + (i >> 8));
        } وإلا {
            إذا(i<=بایټونه_Trie::k_Max_Three_Byte_ارزښت) {
                int_بایټونه[0] = static_cast<حرف>(بایټونه_Trie::k_Min_Three_Byte_ارزښت_Lead + (i >> 16));
            } وإلا {
                int_بایټونه[0] = static_cast<حرف>(بایټونه_Trie::k_Four_Byte_ارزښت_Lead);
                int_بایټونه[1] = static_cast<حرف>(i >> 16);
                length=2;
            }
            int_بایټونه[length++] = static_cast<حرف>(i >> 8);
        }
        int_بایټونه[length++] = static_cast<حرف>(i);
    }
    int_بایټونه[0] = static_cast<حرف>((int_بایټونه[0] << 1) | isFinal);
    أرجع اكتب(int_بایټونه, length);
}

int32_t
بایټونه_Trie_Builder::write_ارزښت_And_Type(UBool has_ارزښت, int32_t value, int32_t غوټه) {
    int32_t دننه_واټن=اكتب(غوټه);
    إذا(has_ارزښت) {
        دننه_واټن=write_ارزښت_And_Final(value, خطأ);
    }
    أرجع دننه_واټن;
}

int32_t
بایټونه_Trie_Builder::write_Delta_منزل(int32_t jump_موخه) {
    int32_t i=بایټونه_اوږدتیا-jump_موخه;
    U_ASSERT(i>=0);
    إذا(i<=بایټونه_Trie::kMaxOneByteDelta) {
        أرجع اكتب(i);
    } وإلا {
        حرف int_بایټونه[5];
        أرجع اكتب(int_بایټونه, دننی_Encode_Delta(i, int_بایټونه));
    }
}

int32_t
بایټونه_Trie_Builder::دننی_Encode_Delta(int32_t i, حرف int_بایټونه[]) {
    U_ASSERT(i>=0);
    إذا(i<=بایټونه_Trie::kMaxOneByteDelta) {
        int_بایټونه[0] = static_cast<حرف>(i);
        أرجع 1;
    }
    int32_t length=1;
    إذا(i<=بایټونه_Trie::kMaxTwoByteDelta) {
        int_بایټونه[0] = static_cast<حرف>(بایټونه_Trie::kMinTwoByteDeltaLead + (i >> 8));
    } وإلا {
        إذا(i<=بایټونه_Trie::kMaxThreeByteDelta) {
            int_بایټونه[0] = static_cast<حرف>(بایټونه_Trie::kMinThreeByteDeltaLead + (i >> 16));
        } وإلا {
            إذا(i<=0xffffff) {
                int_بایټونه[0] = static_cast<حرف>(بایټونه_Trie::kFourByteDeltaLead);
            } وإلا {
                int_بایټونه[0] = static_cast<حرف>(بایټونه_Trie::kFiveByteDeltaLead);
                int_بایټونه[1] = static_cast<حرف>(i >> 24);
                length=2;
            }
            int_بایټونه[length++] = static_cast<حرف>(i >> 16);
        }
        int_بایټونه[length++] = static_cast<حرف>(i >> 8);
    }
    int_بایټونه[length++] = static_cast<حرف>(i);
    أرجع length;
}

U_NAMESPACE_پای_ټکی
