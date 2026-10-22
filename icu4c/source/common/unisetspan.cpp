// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2007-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  unisetspan.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2007mar01
*   created by: Markus W. Scherer
*/

#ضمن "unicode/utypes.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/utf8.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "uvector.h"
#ضمن "unisetspan.h"

U_NAMESPACE_BEGIN

/*
 * List of offsets from the current position from where to try matching
 * a code point or a string.
 * Store offsets rather than indexes to simplify the code and use the same list
 * for both increments (in span()) and decrements (in spanBack()).
 *
 * Assumption: The maximum offset is limited, and the offsets that are stored
 * at any one time are relatively dense, that is, there are normally no gaps of
 * hundreds or thousands of offset values.
 *
 * The implementation uses a circular buffer of byte flags,
 * each indicating whether the corresponding offset is in the list.
 * This avoids inserting into a sorted list of offsets (or absolute indexes) and
 * physically moving part of the list.
 *
 * Note: In principle, the caller should setMaxLength() to the maximum of the
 * max string length and U16_LENGTH/U8_LENGTH to account for
 * "long" single code points.
 * However, this implementation uses at least a staticList with more than
 * U8_LENGTH entries anyway.
 *
 * Note: If maxLength were guaranteed to be no more than 32 or 64,
 * the list could be stored as bit flags in a single integer.
 * Rather than handling a circular buffer with a start list index,
 * the integer would simply be shifted when lower offsets are removed.
 * UnicodeSet does not have a limit on the lengths of strings.
 */
صنف وٿي_List {  // Only ever stack-allocated, does not need to inherit UMemory.
عام:
    وٿي_List() : list(staticList), capacity(0), length(0), شروعات(0) {}

    ~وٿي_List() {
        إذا(list!=staticList) {
            uprv_free(list);
        }
    }

    // Call exactly once if the list is to be used.
    فراغ سيٽ_Max_ڊيگهائي(int32_t max_ڊيگهائي_2) {
        إذا (max_ڊيگهائي_2 <= static_cast<int32_t>(حجم(staticList))) {
            capacity = static_cast<int32_t>(حجم(staticList));
        } وإلا {
            UBool* l = static_cast<UBool*>(uprv_malloc(max_ڊيگهائي_2));
            إذا(l!=خالي_اشارو) {
                list=l;
                capacity=max_ڊيگهائي_2;
            }
        }
        uprv_memset(list, 0, capacity);
    }

    فراغ clear() {
        uprv_memset(list, 0, capacity);
        شروعات=length=0;
    }

    UBool is_خالي_جايون() ثابت {
        أرجع length == 0;
    }

    // Reduce all stored offsets by delta, used when the current position
    // moves by delta.
    // There must not be any offsets lower than delta.
    // If there is an offset equal to delta, it is removed.
    // delta=[1..maxLength]
    فراغ سرڪايو(int32_t delta) {
        int32_t i=شروعات+delta;
        إذا(i>=capacity) {
            i-=capacity;
        }
        إذا(list[i]) {
            list[i]=خطأ;
            --length;
        }
        شروعات=i;
    }

    // Add an offset. The list must not contain it yet.
    // offset=[1..maxLength]
    فراغ جوڙيو_وٿي(int32_t وٿي) {
        int32_t i=شروعات+وٿي;
        إذا(i>=capacity) {
            i-=capacity;
        }
        list[i]=سچ;
        ++length;
    }

    // offset=[1..maxLength]
    UBool شامل_آهي_وٿي(int32_t وٿي) ثابت {
        int32_t i=شروعات+وٿي;
        إذا(i>=capacity) {
            i-=capacity;
        }
        أرجع list[i];
    }

    // Find the lowest stored offset from a non-empty list, remove it,
    // and reduce all other offsets by this minimum.
    // Returns [1..maxLength].
    int32_t ڪڍو_Minimum() {
        // Look for the next offset in list[start+1..capacity-1].
        int32_t i=شروعات, نتيجو;
        طالما(++i<capacity) {
            إذا(list[i]) {
                list[i]=خطأ;
                --length;
                نتيجو=i-شروعات;
                شروعات=i;
                أرجع نتيجو;
            }
        }
        // i==capacity

        // Wrap around and look for the next offset in list[0..start].
        // Since the list is not empty, there will be one.
        نتيجو=capacity-شروعات;
        i=0;
        طالما(!list[i]) {
            ++i;
        }
        list[i]=خطأ;
        --length;
        شروعات=i;
        أرجع نتيجو+=i;
    }

خاص:
    UBool *list;
    int32_t capacity;
    int32_t length;
    int32_t شروعات;

    UBool staticList[16];
};

// Get the number of UTF-8 bytes for a UTF-16 (sub)string.
ساكن int32_t
get_UTF8_ڊيگهائي(ثابت char16_t *s, int32_t length) {
    UError_ڪوڊ error_ڪوڊ=U_ZERO_ERROR;
    int32_t length8=0;
    u_str_منزل_UTF8(خالي_اشارو, 0, &length8, s, length, &error_ڪوڊ);
    إذا(U_SUCCESS(error_ڪوڊ) || error_ڪوڊ==U_بفر_حد_کان_مٿي_ERROR) {
        أرجع length8;
    } وإلا {
        // The string contains an unpaired surrogate.
        // Ignore this string.
        أرجع 0;
    }
}

// Append the UTF-8 version of the string to t and return the appended UTF-8 length.
ساكن int32_t
appendUTF8(ثابت char16_t *s, int32_t length, uint8_t *t, int32_t capacity) {
    UError_ڪوڊ error_ڪوڊ=U_ZERO_ERROR;
    int32_t length8=0;
    u_str_منزل_UTF8(reinterpret_cast<حرف*>(t), capacity, &length8, s, length, &error_ڪوڊ);
    إذا(U_SUCCESS(error_ڪوڊ)) {
        أرجع length8;
    } وإلا {
        // The string contains an unpaired surrogate.
        // Ignore this string.
        أرجع 0;
    }
}

ساكن مضمن uint8_t
make_وسعت_ڊيگهائي_Byte(int32_t وسعت_ڊيگهائي) {
    // 0xfe==UnicodeSetStringSpan::LONG_SPAN
    أرجع وسعت_ڊيگهائي < 0xfe ? static_cast<uint8_t>(وسعت_ڊيگهائي) : static_cast<uint8_t>(0xfe);
}

// Construct for all variants of span(), or only for any one variant.
// Initialize as little as possible, for single use.
Unicode_سيٽ_String_وسعت::Unicode_سيٽ_String_وسعت(ثابت Unicode_سيٽ &سيٽ,
                                           ثابت UVector &سيٽ_لڙيون,
                                           uint32_t which)
        : وسعت_سيٽ(0, 0x10ffff), p_وسعت_Not_سيٽ(خالي_اشارو), لڙيون(سيٽ_لڙيون),
          utf8Lengths(خالي_اشارو), وسعت_Lengths(خالي_اشارو), utf8(خالي_اشارو),
          utf8_ڊيگهائي(0),
          maxLength16(0), maxLength8(0),
          all(static_cast<UBool>(which == ALL)) {
    وسعت_سيٽ.retainAll(سيٽ);
    إذا(which&NOT_CONTAINED) {
        // Default to the same sets.
        // addToSpanNotSet() will create a separate set if necessary.
        p_وسعت_Not_سيٽ=&وسعت_سيٽ;
    }

    // Determine if the strings even need to be taken into account at all for span() etc.
    // If any string is relevant, then all strings need to be used for
    // span(longest match) but only the relevant ones for span(while contained).
    // TODO: Possible optimization: Distinguish CONTAINED vs. LONGEST_MATCH
    //   and do not store UTF-8 strings if !thisRelevant and CONTAINED.
    //   (Only store irrelevant UTF-8 strings for LONGEST_MATCH where they are relevant after all.)
    // Also count the lengths of the UTF-8 versions of the strings for memory allocation.
    int32_t لڙيون_ڊيگهائي=لڙيون.size();

    int32_t i, وسعت_ڊيگهائي;
    UBool someRelevant=خطأ;
    لكل(i=0; i<لڙيون_ڊيگهائي; ++i) {
        ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڙيون.elementAt(i));
        ثابت char16_t *s16=string.get_بفر();
        int32_t length16=string.length();
        إذا (length16==0) {
            استمر;  // skip the empty string
        }
        UBool thisRelevant;
        وسعت_ڊيگهائي=وسعت_سيٽ.وسعت(s16, length16, USET_وسعت_CONTAINED);
        إذا(وسعت_ڊيگهائي<length16) {  // Relevant string.
            someRelevant=thisRelevant=سچ;
        } وإلا {
            thisRelevant=خطأ;
        }
        إذا((which&UTF16) && length16>maxLength16) {
            maxLength16=length16;
        }
        إذا((which&UTF8) && (thisRelevant || (which&CONTAINED))) {
            int32_t length8=get_UTF8_ڊيگهائي(s16, length16);
            utf8_ڊيگهائي+=length8;
            إذا(length8>maxLength8) {
                maxLength8=length8;
            }
        }
    }
    إذا(!someRelevant) {
        maxLength16=maxLength8=0;
        أرجع;
    }

    // Freeze after checking for the need to use strings at all because freezing
    // a set takes some time and memory which are wasted if there are no relevant strings.
    إذا(all) {
        وسعت_سيٽ.freeze();
    }

    uint8_t *وسعت_پٺ_Lengths;
    uint8_t *وسعت_UTF8_Lengths;
    uint8_t *وسعت_پٺ_UTF8_Lengths;

    // Allocate a block of meta data.
    int32_t alloc_سائيز_2;
    إذا(all) {
        // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
        alloc_سائيز_2=لڙيون_ڊيگهائي*(4+1+1+1+1)+utf8_ڊيگهائي;
    } وإلا {
        alloc_سائيز_2=لڙيون_ڊيگهائي;  // One set of span lengths.
        إذا(which&UTF8) {
            // UTF-8 lengths and UTF-8 strings.
            alloc_سائيز_2+=لڙيون_ڊيگهائي*4+utf8_ڊيگهائي;
        }
    }
    إذا (alloc_سائيز_2 <= static_cast<int32_t>(حجم(staticLengths))) {
        utf8Lengths=staticLengths;
    } وإلا {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(alloc_سائيز_2));
        إذا(utf8Lengths==خالي_اشارو) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            أرجع;  // Out of memory.
        }
    }

    إذا(all) {
        // Store span lengths for all span() variants.
        وسعت_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths + لڙيون_ڊيگهائي);
        وسعت_پٺ_Lengths=وسعت_Lengths+لڙيون_ڊيگهائي;
        وسعت_UTF8_Lengths=وسعت_پٺ_Lengths+لڙيون_ڊيگهائي;
        وسعت_پٺ_UTF8_Lengths=وسعت_UTF8_Lengths+لڙيون_ڊيگهائي;
        utf8=وسعت_پٺ_UTF8_Lengths+لڙيون_ڊيگهائي;
    } وإلا {
        // Store span lengths for only one span() variant.
        إذا(which&UTF8) {
            وسعت_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths + لڙيون_ڊيگهائي);
            utf8=وسعت_Lengths+لڙيون_ڊيگهائي;
        } وإلا {
            وسعت_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths);
        }
        وسعت_پٺ_Lengths=وسعت_UTF8_Lengths=وسعت_پٺ_UTF8_Lengths=وسعت_Lengths;
    }

    // Set the meta data and pSpanNotSet and write the UTF-8 strings.
    int32_t utf8_ڳڻپ=0;  // Count UTF-8 bytes written so far.

    لكل(i=0; i<لڙيون_ڊيگهائي; ++i) {
        ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڙيون.elementAt(i));
        ثابت char16_t *s16=string.get_بفر();
        int32_t length16=string.length();
        وسعت_ڊيگهائي=وسعت_سيٽ.وسعت(s16, length16, USET_وسعت_CONTAINED);
        إذا(وسعت_ڊيگهائي<length16 && length16>0) {  // Relevant string.
            إذا(which&UTF16) {
                إذا(which&CONTAINED) {
                    إذا(which&FWD) {
                        وسعت_Lengths[i]=make_وسعت_ڊيگهائي_Byte(وسعت_ڊيگهائي);
                    }
                    إذا(which&پٺ_2) {
                        وسعت_ڊيگهائي=length16-وسعت_سيٽ.وسعت_پٺ(s16, length16, USET_وسعت_CONTAINED);
                        وسعت_پٺ_Lengths[i]=make_وسعت_ڊيگهائي_Byte(وسعت_ڊيگهائي);
                    }
                } وإلا /* not CONTAINED, not all, but NOT_CONTAINED */ {
                    وسعت_Lengths[i]=وسعت_پٺ_Lengths[i]=0;  // Only store a relevant/irrelevant flag.
                }
            }
            إذا(which&UTF8) {
                uint8_t *s8=utf8+utf8_ڳڻپ;
                int32_t length8=appendUTF8(s16, length16, s8, utf8_ڊيگهائي-utf8_ڳڻپ);
                utf8_ڳڻپ+=utf8Lengths[i]=length8;
                إذا(length8==0) {  // Irrelevant for UTF-8 because not representable in UTF-8.
                    وسعت_UTF8_Lengths[i] = وسعت_پٺ_UTF8_Lengths[i] = static_cast<uint8_t>(ALL_CP_CONTAINED);
                } وإلا {  // Relevant for UTF-8.
                    إذا(which&CONTAINED) {
                        إذا(which&FWD) {
                            وسعت_ڊيگهائي = وسعت_سيٽ.وسعت_UTF8(reinterpret_cast<ثابت حرف*>(s8), length8, USET_وسعت_CONTAINED);
                            وسعت_UTF8_Lengths[i]=make_وسعت_ڊيگهائي_Byte(وسعت_ڊيگهائي);
                        }
                        إذا(which&پٺ_2) {
                            وسعت_ڊيگهائي = length8 - وسعت_سيٽ.وسعت_پٺ_UTF8(reinterpret_cast<ثابت حرف*>(s8), length8, USET_وسعت_CONTAINED);
                            وسعت_پٺ_UTF8_Lengths[i]=make_وسعت_ڊيگهائي_Byte(وسعت_ڊيگهائي);
                        }
                    } وإلا /* not CONTAINED, not all, but NOT_CONTAINED */ {
                        وسعت_UTF8_Lengths[i]=وسعت_پٺ_UTF8_Lengths[i]=0;  // Only store a relevant/irrelevant flag.
                    }
                }
            }
            إذا(which&NOT_CONTAINED) {
                // Add string start and end code points to the spanNotSet so that
                // a span(while not contained) stops before any string.
                UChar32 c;
                إذا(which&FWD) {
                    int32_t len=0;
                    U16_ايندڙ(s16, len, length16, c);
                    جوڙيو_منزل_وسعت_Not_سيٽ(c);
                }
                إذا(which&پٺ_2) {
                    int32_t len=length16;
                    U16_PREV(s16, 0, len, c);
                    جوڙيو_منزل_وسعت_Not_سيٽ(c);
                }
            }
        } وإلا {  // Irrelevant string. (Also the empty string.)
            إذا(which&UTF8) {
                إذا(which&CONTAINED) {  // Only necessary for LONGEST_MATCH.
                    uint8_t *s8=utf8+utf8_ڳڻپ;
                    int32_t length8=appendUTF8(s16, length16, s8, utf8_ڊيگهائي-utf8_ڳڻپ);
                    utf8_ڳڻپ+=utf8Lengths[i]=length8;
                } وإلا {
                    utf8Lengths[i]=0;
                }
            }
            إذا(all) {
                وسعت_Lengths[i]=وسعت_پٺ_Lengths[i]=
                    وسعت_UTF8_Lengths[i]=وسعت_پٺ_UTF8_Lengths[i]=
                        static_cast<uint8_t>(ALL_CP_CONTAINED);
            } وإلا {
                // All spanXYZLengths pointers contain the same address.
                وسعت_Lengths[i] = static_cast<uint8_t>(ALL_CP_CONTAINED);
            }
        }
    }

    // Finish.
    إذا(all) {
        p_وسعت_Not_سيٽ->freeze();
    }
}

// Copy constructor. Assumes which==ALL for a frozen set.
Unicode_سيٽ_String_وسعت::Unicode_سيٽ_String_وسعت(ثابت Unicode_سيٽ_String_وسعت &other_String_وسعت,
                                           ثابت UVector &new_والد_سيٽ_لڙيون)
        : وسعت_سيٽ(other_String_وسعت.وسعت_سيٽ), p_وسعت_Not_سيٽ(خالي_اشارو), لڙيون(new_والد_سيٽ_لڙيون),
          utf8Lengths(خالي_اشارو), وسعت_Lengths(خالي_اشارو), utf8(خالي_اشارو),
          utf8_ڊيگهائي(other_String_وسعت.utf8_ڊيگهائي),
          maxLength16(other_String_وسعت.maxLength16), maxLength8(other_String_وسعت.maxLength8),
          all(سچ) {
    إذا(other_String_وسعت.p_وسعت_Not_سيٽ==&other_String_وسعت.وسعت_سيٽ) {
        p_وسعت_Not_سيٽ=&وسعت_سيٽ;
    } وإلا {
        p_وسعت_Not_سيٽ=other_String_وسعت.p_وسعت_Not_سيٽ->clone();
    }

    // Allocate a block of meta data.
    // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
    int32_t لڙيون_ڊيگهائي=لڙيون.size();
    int32_t alloc_سائيز_2=لڙيون_ڊيگهائي*(4+1+1+1+1)+utf8_ڊيگهائي;
    إذا (alloc_سائيز_2 <= static_cast<int32_t>(حجم(staticLengths))) {
        utf8Lengths=staticLengths;
    } وإلا {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(alloc_سائيز_2));
        إذا(utf8Lengths==خالي_اشارو) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            أرجع;  // Out of memory.
        }
    }

    وسعت_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths + لڙيون_ڊيگهائي);
    utf8=وسعت_Lengths+لڙيون_ڊيگهائي*4;
    uprv_memcpy(utf8Lengths, other_String_وسعت.utf8Lengths, alloc_سائيز_2);
}

Unicode_سيٽ_String_وسعت::~Unicode_سيٽ_String_وسعت() {
    إذا(p_وسعت_Not_سيٽ!=خالي_اشارو && p_وسعت_Not_سيٽ!=&وسعت_سيٽ) {
        ميٽيو p_وسعت_Not_سيٽ;
    }
    إذا(utf8Lengths!=خالي_اشارو && utf8Lengths!=staticLengths) {
        uprv_free(utf8Lengths);
    }
}

فراغ Unicode_سيٽ_String_وسعت::جوڙيو_منزل_وسعت_Not_سيٽ(UChar32 c) {
    إذا(p_وسعت_Not_سيٽ==خالي_اشارو || p_وسعت_Not_سيٽ==&وسعت_سيٽ) {
        إذا(وسعت_سيٽ.شامل_آهي(c)) {
            أرجع;  // Nothing to do.
        }
        Unicode_سيٽ *new_سيٽ=وسعت_سيٽ.cloneAsThawed();
        إذا(new_سيٽ==خالي_اشارو) {
            أرجع;  // Out of memory.
        } وإلا {
            p_وسعت_Not_سيٽ=new_سيٽ;
        }
    }
    p_وسعت_Not_سيٽ->جوڙيو(c);
}

// Compare strings without any argument checks. Requires length>0.
ساكن مضمن UBool
matches16(ثابت char16_t *s, ثابت char16_t *t, int32_t length) {
    افعل {
        إذا(*s++!=*t++) {
            أرجع خطأ;
        }
    } طالما(--length>0);
    أرجع سچ;
}

ساكن مضمن UBool
matches8(ثابت uint8_t *s, ثابت uint8_t *t, int32_t length) {
    افعل {
        إذا(*s++!=*t++) {
            أرجع خطأ;
        }
    } طالما(--length>0);
    أرجع سچ;
}

// Compare 16-bit Unicode strings (which may be malformed UTF-16)
// at code point boundaries.
// That is, each edge of a match must not be in the middle of a surrogate pair.
ساكن مضمن UBool
matches16CPB(ثابت char16_t *s, int32_t شروعات, int32_t حد_بندي, ثابت char16_t *t, int32_t length) {
    s+=شروعات;
    حد_بندي-=شروعات;
    أرجع matches16(s, t, length) &&
           !(0<شروعات && U16_IS_LEAD(s[-1]) && U16_IS_TRAIL(s[0])) &&
           !(length<حد_بندي && U16_IS_LEAD(s[length-1]) && U16_IS_TRAIL(s[length]));
}

// Does the set contain the next code point?
// If so, return its length; otherwise return its negative length.
ساكن مضمن int32_t
وسعت_One(ثابت Unicode_سيٽ &سيٽ, ثابت char16_t *s, int32_t length) {
    char16_t c=*s, c2;
    إذا(c>=0xd800 && c<=0xdbff && length>=2 && U16_IS_TRAIL(c2=s[1])) {
        أرجع سيٽ.شامل_آهي(U16_GET_SUPPLEMENTARY(c, c2)) ? 2 : -2;
    }
    أرجع سيٽ.شامل_آهي(c) ? 1 : -1;
}

ساكن مضمن int32_t
وسعت_One_پٺ(ثابت Unicode_سيٽ &سيٽ, ثابت char16_t *s, int32_t length) {
    char16_t c=s[length-1], c2;
    إذا(c>=0xdc00 && c<=0xdfff && length>=2 && U16_IS_LEAD(c2=s[length-2])) {
        أرجع سيٽ.شامل_آهي(U16_GET_SUPPLEMENTARY(c2, c)) ? 2 : -2;
    }
    أرجع سيٽ.شامل_آهي(c) ? 1 : -1;
}

ساكن مضمن int32_t
وسعت_One_UTF8(ثابت Unicode_سيٽ &سيٽ, ثابت uint8_t *s, int32_t length) {
    UChar32 c=*s;
    إذا(U8_IS_SINGLE(c)) {
        أرجع سيٽ.شامل_آهي(c) ? 1 : -1;
    }
    // Take advantage of non-ASCII fastpaths in U8_NEXT_OR_FFFD().
    int32_t i=0;
    U8_ايندڙ_OR_FFFD(s, i, length, c);
    أرجع سيٽ.شامل_آهي(c) ? i : -i;
}

ساكن مضمن int32_t
وسعت_One_پٺ_UTF8(ثابت Unicode_سيٽ &سيٽ, ثابت uint8_t *s, int32_t length) {
    UChar32 c=s[length-1];
    إذا(U8_IS_SINGLE(c)) {
        أرجع سيٽ.شامل_آهي(c) ? 1 : -1;
    }
    int32_t i=length-1;
    c=utf8_prev_Char_محفوظ_Body(s, 0, &i, c, -3);
    length-=i;
    أرجع سيٽ.شامل_آهي(c) ? length : -length;
}

/*
 * Note: In span() when spanLength==0 (after a string match, or at the beginning
 * after an empty code point span) and in spanNot() and spanNotUTF8(),
 * string matching could use a binary search
 * because all string matches are done from the same start index.
 *
 * For UTF-8, this would require a comparison function that returns UTF-16 order.
 *
 * This optimization should not be necessary for normal UnicodeSets because
 * most sets have no strings, and most sets with strings have
 * very few very short strings.
 * For cases with many strings, it might be better to use a different API
 * and implementation with a DFA (state machine).
 */

/*
 * Algorithm for span(USET_SPAN_CONTAINED)
 *
 * Theoretical algorithm:
 * - Iterate through the string, and at each code point boundary:
 *   + If the code point there is in the set, then remember to continue after it.
 *   + If a set string matches at the current position, then remember to continue after it.
 *   + Either recursively span for each code point or string match,
 *     or recursively span for all but the shortest one and
 *     iteratively continue the span with the shortest local match.
 *   + Remember the longest recursive span (the farthest end point).
 *   + If there is no match at the current position, neither for the code point there
 *     nor for any set string, then stop and return the longest recursive span length.
 *
 * Optimized implementation:
 *
 * (We assume that most sets will have very few very short strings.
 * A span using a string-less set is extremely fast.)
 *
 * Create and cache a spanSet which contains all of the single code points
 * of the original set but none of its strings.
 *
 * - Start with spanLength=spanSet.span(USET_SPAN_CONTAINED).
 * - Loop:
 *   + Try to match each set string at the end of the spanLength.
 *     ~ Set strings that start with set-contained code points must be matched
 *       with a partial overlap because the recursive algorithm would have tried
 *       to match them at every position.
 *     ~ Set strings that entirely consist of set-contained code points
 *       are irrelevant for span(USET_SPAN_CONTAINED) because the
 *       recursive algorithm would continue after them anyway
 *       and find the longest recursive match from their end.
 *     ~ Rather than recursing, note each end point of a set string match.
 *   + If no set string matched after spanSet.span(), then return
 *     with where the spanSet.span() ended.
 *   + If at least one set string matched after spanSet.span(), then
 *     pop the shortest string match end point and continue
 *     the loop, trying to match all set strings from there.
 *   + If at least one more set string matched after a previous string match,
 *     then test if the code point after the previous string match is also
 *     contained in the set.
 *     Continue the loop with the shortest end point of either this code point
 *     or a matching set string.
 *   + If no more set string matched after a previous string match,
 *     then try another spanLength=spanSet.span(USET_SPAN_CONTAINED).
 *     Stop if spanLength==0, otherwise continue the loop.
 *
 * By noting each end point of a set string match,
 * the function visits each string position at most once and finishes
 * in linear time.
 *
 * The recursive algorithm may visit the same string position many times
 * if multiple paths lead to it and finishes in exponential time.
 */

/*
 * Algorithm for span(USET_SPAN_SIMPLE)
 *
 * Theoretical algorithm:
 * - Iterate through the string, and at each code point boundary:
 *   + If the code point there is in the set, then remember to continue after it.
 *   + If a set string matches at the current position, then remember to continue after it.
 *   + Continue from the farthest match position and ignore all others.
 *   + If there is no match at the current position,
 *     then stop and return the current position.
 *
 * Optimized implementation:
 *
 * (Same assumption and spanSet as above.)
 *
 * - Start with spanLength=spanSet.span(USET_SPAN_CONTAINED).
 * - Loop:
 *   + Try to match each set string at the end of the spanLength.
 *     ~ Set strings that start with set-contained code points must be matched
 *       with a partial overlap because the standard algorithm would have tried
 *       to match them earlier.
 *     ~ Set strings that entirely consist of set-contained code points
 *       must be matched with a full overlap because the longest-match algorithm
 *       would hide set string matches that end earlier.
 *       Such set strings need not be matched earlier inside the code point span
 *       because the standard algorithm would then have continued after
 *       the set string match anyway.
 *     ~ Remember the longest set string match (farthest end point) from the earliest
 *       starting point.
 *   + If no set string matched after spanSet.span(), then return
 *     with where the spanSet.span() ended.
 *   + If at least one set string matched, then continue the loop after the
 *     longest match from the earliest position.
 *   + If no more set string matched after a previous string match,
 *     then try another spanLength=spanSet.span(USET_SPAN_CONTAINED).
 *     Stop if spanLength==0, otherwise continue the loop.
 */

int32_t Unicode_سيٽ_String_وسعت::وسعت(ثابت char16_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    إذا(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
        أرجع وسعت_Not(s, length);
    }
    int32_t وسعت_ڊيگهائي=وسعت_سيٽ.وسعت(s, length, USET_وسعت_CONTAINED);
    إذا(وسعت_ڊيگهائي==length) {
        أرجع length;
    }

    // Consider strings; they may overlap with the span.
    وٿي_List offsets;
    إذا(وسعت_Condition==USET_وسعت_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.سيٽ_Max_ڊيگهائي(maxLength16);
    }
    int32_t pos=وسعت_ڊيگهائي, rest=length-pos;
    int32_t i, لڙيون_ڊيگهائي=لڙيون.size();
    لكل(;;) {
        إذا(وسعت_Condition==USET_وسعت_CONTAINED) {
            لكل(i=0; i<لڙيون_ڊيگهائي; ++i) {
                int32_t overlap=وسعت_Lengths[i];
                إذا(overlap==ALL_CP_CONTAINED) {
                    استمر;  // Irrelevant string. (Also the empty string.)
                }
                ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڙيون.elementAt(i));
                ثابت char16_t *s16=string.get_بفر();
                int32_t length16=string.length();
                U_ASSERT(length>0);

                // Try to match this string at pos-overlap..pos.
                إذا(overlap>=LONG_وسعت) {
                    overlap=length16;
                    // While contained: No point matching fully inside the code point span.
                    U16_پٺ_1(s16, 0, overlap);  // Length of the string minus the last code point.
                }
                إذا(overlap>وسعت_ڊيگهائي) {
                    overlap=وسعت_ڊيگهائي;
                }
                int32_t inc=length16-overlap;  // Keep overlap+inc==length16.
                لكل(;;) {
                    إذا(inc>rest) {
                        اكسر;
                    }
                    // Try to match if the increment is not listed already.
                    إذا(!offsets.شامل_آهي_وٿي(inc) && matches16CPB(s, pos-overlap, length, s16, length16)) {
                        إذا(inc==rest) {
                            أرجع length;  // Reached the end of the string.
                        }
                        offsets.جوڙيو_وٿي(inc);
                    }
                    إذا(overlap==0) {
                        اكسر;
                    }
                    --overlap;
                    ++inc;
                }
            }
        } وإلا /* USET_SPAN_SIMPLE */ {
            int32_t maxInc=0, maxOverlap=0;
            لكل(i=0; i<لڙيون_ڊيگهائي; ++i) {
                int32_t overlap=وسعت_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the earliest start.

                ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڙيون.elementAt(i));
                ثابت char16_t *s16=string.get_بفر();
                int32_t length16=string.length();
                إذا (length16==0) {
                    استمر;  // skip the empty string
                }

                // Try to match this string at pos-overlap..pos.
                إذا(overlap>=LONG_وسعت) {
                    overlap=length16;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the earliest start.
                }
                إذا(overlap>وسعت_ڊيگهائي) {
                    overlap=وسعت_ڊيگهائي;
                }
                int32_t inc=length16-overlap;  // Keep overlap+inc==length16.
                لكل(;;) {
                    إذا(inc>rest || overlap<maxOverlap) {
                        اكسر;
                    }
                    // Try to match if the string is longer or starts earlier.
                    إذا( (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ inc>maxInc) &&
                        matches16CPB(s, pos-overlap, length, s16, length16)
                    ) {
                        maxInc=inc;  // Longest match from earliest start.
                        maxOverlap=overlap;
                        اكسر;
                    }
                    --overlap;
                    ++inc;
                }
            }

            إذا(maxInc!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue after it.
                pos+=maxInc;
                rest-=maxInc;
                إذا(rest==0) {
                    أرجع length;  // Reached the end of the string.
                }
                وسعت_ڊيگهائي=0;  // Match strings from after a string match.
                استمر;
            }
        }
        // Finished trying to match all strings at pos.

        إذا(وسعت_ڊيگهائي!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            إذا(offsets.is_خالي_جايون()) {
                أرجع pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } وإلا {
            // The position is after a string match (or a single code point).
            إذا(offsets.is_خالي_جايون()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                وسعت_ڊيگهائي=وسعت_سيٽ.وسعت(s+pos, rest, USET_وسعت_CONTAINED);
                إذا( وسعت_ڊيگهائي==rest || // Reached the end of the string, or
                    وسعت_ڊيگهائي==0       // neither strings nor span progressed.
                ) {
                    أرجع pos+وسعت_ڊيگهائي;
                }
                pos+=وسعت_ڊيگهائي;
                rest-=وسعت_ڊيگهائي;
                استمر;  // spanLength>0: Match strings from after a span.
            } وإلا {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                وسعت_ڊيگهائي=وسعت_One(وسعت_سيٽ, s+pos, rest);
                إذا(وسعت_ڊيگهائي>0) {
                    إذا(وسعت_ڊيگهائي==rest) {
                        أرجع length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=وسعت_ڊيگهائي;
                    rest-=وسعت_ڊيگهائي;
                    offsets.سرڪايو(وسعت_ڊيگهائي);
                    وسعت_ڊيگهائي=0;
                    استمر;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t min_وٿي=offsets.ڪڍو_Minimum();
        pos+=min_وٿي;
        rest-=min_وٿي;
        وسعت_ڊيگهائي=0;  // Match strings from after a string match.
    }
}

int32_t Unicode_سيٽ_String_وسعت::وسعت_پٺ(ثابت char16_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    إذا(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
        أرجع وسعت_Not_پٺ(s, length);
    }
    int32_t pos=وسعت_سيٽ.وسعت_پٺ(s, length, USET_وسعت_CONTAINED);
    إذا(pos==0) {
        أرجع 0;
    }
    int32_t وسعت_ڊيگهائي=length-pos;

    // Consider strings; they may overlap with the span.
    وٿي_List offsets;
    إذا(وسعت_Condition==USET_وسعت_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.سيٽ_Max_ڊيگهائي(maxLength16);
    }
    int32_t i, لڙيون_ڊيگهائي=لڙيون.size();
    uint8_t *وسعت_پٺ_Lengths=وسعت_Lengths;
    إذا(all) {
        وسعت_پٺ_Lengths+=لڙيون_ڊيگهائي;
    }
    لكل(;;) {
        إذا(وسعت_Condition==USET_وسعت_CONTAINED) {
            لكل(i=0; i<لڙيون_ڊيگهائي; ++i) {
                int32_t overlap=وسعت_پٺ_Lengths[i];
                إذا(overlap==ALL_CP_CONTAINED) {
                    استمر;  // Irrelevant string. (Also the empty string.)
                }
                ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڙيون.elementAt(i));
                ثابت char16_t *s16=string.get_بفر();
                int32_t length16=string.length();
                U_ASSERT(length>0);

                // Try to match this string at pos-(length16-overlap)..pos-length16.
                إذا(overlap>=LONG_وسعت) {
                    overlap=length16;
                    // While contained: No point matching fully inside the code point span.
                    int32_t len1=0;
                    U16_FWD_1(s16, len1, overlap);
                    overlap-=len1;  // Length of the string minus the first code point.
                }
                إذا(overlap>وسعت_ڊيگهائي) {
                    overlap=وسعت_ڊيگهائي;
                }
                int32_t dec=length16-overlap;  // Keep dec+overlap==length16.
                لكل(;;) {
                    إذا(dec>pos) {
                        اكسر;
                    }
                    // Try to match if the decrement is not listed already.
                    إذا(!offsets.شامل_آهي_وٿي(dec) && matches16CPB(s, pos-dec, length, s16, length16)) {
                        إذا(dec==pos) {
                            أرجع 0;  // Reached the start of the string.
                        }
                        offsets.جوڙيو_وٿي(dec);
                    }
                    إذا(overlap==0) {
                        اكسر;
                    }
                    --overlap;
                    ++dec;
                }
            }
        } وإلا /* USET_SPAN_SIMPLE */ {
            int32_t maxDec=0, maxOverlap=0;
            لكل(i=0; i<لڙيون_ڊيگهائي; ++i) {
                int32_t overlap=وسعت_پٺ_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڙيون.elementAt(i));
                ثابت char16_t *s16=string.get_بفر();
                int32_t length16=string.length();
                إذا (length16==0) {
                    استمر;  // skip the empty string
                }

                // Try to match this string at pos-(length16-overlap)..pos-length16.
                إذا(overlap>=LONG_وسعت) {
                    overlap=length16;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the latest end.
                }
                إذا(overlap>وسعت_ڊيگهائي) {
                    overlap=وسعت_ڊيگهائي;
                }
                int32_t dec=length16-overlap;  // Keep dec+overlap==length16.
                لكل(;;) {
                    إذا(dec>pos || overlap<maxOverlap) {
                        اكسر;
                    }
                    // Try to match if the string is longer or ends later.
                    إذا( (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ dec>maxDec) &&
                        matches16CPB(s, pos-dec, length, s16, length16)
                    ) {
                        maxDec=dec;  // Longest match from latest end.
                        maxOverlap=overlap;
                        اكسر;
                    }
                    --overlap;
                    ++dec;
                }
            }

            إذا(maxDec!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue before it.
                pos-=maxDec;
                إذا(pos==0) {
                    أرجع 0;  // Reached the start of the string.
                }
                وسعت_ڊيگهائي=0;  // Match strings from before a string match.
                استمر;
            }
        }
        // Finished trying to match all strings at pos.

        إذا(وسعت_ڊيگهائي!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            إذا(offsets.is_خالي_جايون()) {
                أرجع pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } وإلا {
            // The position is before a string match (or a single code point).
            إذا(offsets.is_خالي_جايون()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos=وسعت_سيٽ.وسعت_پٺ(s, oldPos, USET_وسعت_CONTAINED);
                وسعت_ڊيگهائي=oldPos-pos;
                إذا( pos==0 ||           // Reached the start of the string, or
                    وسعت_ڊيگهائي==0       // neither strings nor span progressed.
                ) {
                    أرجع pos;
                }
                استمر;  // spanLength>0: Match strings from before a span.
            } وإلا {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                وسعت_ڊيگهائي=وسعت_One_پٺ(وسعت_سيٽ, s, pos);
                إذا(وسعت_ڊيگهائي>0) {
                    إذا(وسعت_ڊيگهائي==pos) {
                        أرجع 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=وسعت_ڊيگهائي;
                    offsets.سرڪايو(وسعت_ڊيگهائي);
                    وسعت_ڊيگهائي=0;
                    استمر;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.ڪڍو_Minimum();
        وسعت_ڊيگهائي=0;  // Match strings from before a string match.
    }
}

int32_t Unicode_سيٽ_String_وسعت::وسعت_UTF8(ثابت uint8_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    إذا(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
        أرجع وسعت_Not_UTF8(s, length);
    }
    int32_t وسعت_ڊيگهائي = وسعت_سيٽ.وسعت_UTF8(reinterpret_cast<ثابت حرف*>(s), length, USET_وسعت_CONTAINED);
    إذا(وسعت_ڊيگهائي==length) {
        أرجع length;
    }

    // Consider strings; they may overlap with the span.
    وٿي_List offsets;
    إذا(وسعت_Condition==USET_وسعت_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.سيٽ_Max_ڊيگهائي(maxLength8);
    }
    int32_t pos=وسعت_ڊيگهائي, rest=length-pos;
    int32_t i, لڙيون_ڊيگهائي=لڙيون.size();
    uint8_t *وسعت_UTF8_Lengths=وسعت_Lengths;
    إذا(all) {
        وسعت_UTF8_Lengths+=2*لڙيون_ڊيگهائي;
    }
    لكل(;;) {
        ثابت uint8_t *s8=utf8;
        int32_t length8;
        إذا(وسعت_Condition==USET_وسعت_CONTAINED) {
            لكل(i=0; i<لڙيون_ڊيگهائي; ++i) {
                length8=utf8Lengths[i];
                إذا(length8==0) {
                    استمر;  // String not representable in UTF-8.
                }
                int32_t overlap=وسعت_UTF8_Lengths[i];
                إذا(overlap==ALL_CP_CONTAINED) {
                    s8+=length8;
                    استمر;  // Irrelevant string.
                }

                // Try to match this string at pos-overlap..pos.
                إذا(overlap>=LONG_وسعت) {
                    overlap=length8;
                    // While contained: No point matching fully inside the code point span.
                    U8_پٺ_1(s8, 0, overlap);  // Length of the string minus the last code point.
                }
                إذا(overlap>وسعت_ڊيگهائي) {
                    overlap=وسعت_ڊيگهائي;
                }
                int32_t inc=length8-overlap;  // Keep overlap+inc==length8.
                لكل(;;) {
                    إذا(inc>rest) {
                        اكسر;
                    }
                    // Try to match if the increment is not listed already.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    إذا(!U8_IS_TRAIL(s[pos-overlap]) &&
                            !offsets.شامل_آهي_وٿي(inc) &&
                            matches8(s+pos-overlap, s8, length8)) {
                        إذا(inc==rest) {
                            أرجع length;  // Reached the end of the string.
                        }
                        offsets.جوڙيو_وٿي(inc);
                    }
                    إذا(overlap==0) {
                        اكسر;
                    }
                    --overlap;
                    ++inc;
                }
                s8+=length8;
            }
        } وإلا /* USET_SPAN_SIMPLE */ {
            int32_t maxInc=0, maxOverlap=0;
            لكل(i=0; i<لڙيون_ڊيگهائي; ++i) {
                length8=utf8Lengths[i];
                إذا(length8==0) {
                    استمر;  // String not representable in UTF-8.
                }
                int32_t overlap=وسعت_UTF8_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the earliest start.

                // Try to match this string at pos-overlap..pos.
                إذا(overlap>=LONG_وسعت) {
                    overlap=length8;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the earliest start.
                }
                إذا(overlap>وسعت_ڊيگهائي) {
                    overlap=وسعت_ڊيگهائي;
                }
                int32_t inc=length8-overlap;  // Keep overlap+inc==length8.
                لكل(;;) {
                    إذا(inc>rest || overlap<maxOverlap) {
                        اكسر;
                    }
                    // Try to match if the string is longer or starts earlier.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    إذا(!U8_IS_TRAIL(s[pos-overlap]) &&
                            (overlap>maxOverlap ||
                                /* redundant overlap==maxOverlap && */ inc>maxInc) &&
                            matches8(s+pos-overlap, s8, length8)) {
                        maxInc=inc;  // Longest match from earliest start.
                        maxOverlap=overlap;
                        اكسر;
                    }
                    --overlap;
                    ++inc;
                }
                s8+=length8;
            }

            إذا(maxInc!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue after it.
                pos+=maxInc;
                rest-=maxInc;
                إذا(rest==0) {
                    أرجع length;  // Reached the end of the string.
                }
                وسعت_ڊيگهائي=0;  // Match strings from after a string match.
                استمر;
            }
        }
        // Finished trying to match all strings at pos.

        إذا(وسعت_ڊيگهائي!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            إذا(offsets.is_خالي_جايون()) {
                أرجع pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } وإلا {
            // The position is after a string match (or a single code point).
            إذا(offsets.is_خالي_جايون()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                وسعت_ڊيگهائي = وسعت_سيٽ.وسعت_UTF8(reinterpret_cast<ثابت حرف*>(s) + pos, rest, USET_وسعت_CONTAINED);
                إذا( وسعت_ڊيگهائي==rest || // Reached the end of the string, or
                    وسعت_ڊيگهائي==0       // neither strings nor span progressed.
                ) {
                    أرجع pos+وسعت_ڊيگهائي;
                }
                pos+=وسعت_ڊيگهائي;
                rest-=وسعت_ڊيگهائي;
                استمر;  // spanLength>0: Match strings from after a span.
            } وإلا {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                وسعت_ڊيگهائي=وسعت_One_UTF8(وسعت_سيٽ, s+pos, rest);
                إذا(وسعت_ڊيگهائي>0) {
                    إذا(وسعت_ڊيگهائي==rest) {
                        أرجع length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=وسعت_ڊيگهائي;
                    rest-=وسعت_ڊيگهائي;
                    offsets.سرڪايو(وسعت_ڊيگهائي);
                    وسعت_ڊيگهائي=0;
                    استمر;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t min_وٿي=offsets.ڪڍو_Minimum();
        pos+=min_وٿي;
        rest-=min_وٿي;
        وسعت_ڊيگهائي=0;  // Match strings from after a string match.
    }
}

int32_t Unicode_سيٽ_String_وسعت::وسعت_پٺ_UTF8(ثابت uint8_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    إذا(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
        أرجع وسعت_Not_پٺ_UTF8(s, length);
    }
    int32_t pos = وسعت_سيٽ.وسعت_پٺ_UTF8(reinterpret_cast<ثابت حرف*>(s), length, USET_وسعت_CONTAINED);
    إذا(pos==0) {
        أرجع 0;
    }
    int32_t وسعت_ڊيگهائي=length-pos;

    // Consider strings; they may overlap with the span.
    وٿي_List offsets;
    إذا(وسعت_Condition==USET_وسعت_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.سيٽ_Max_ڊيگهائي(maxLength8);
    }
    int32_t i, لڙيون_ڊيگهائي=لڙيون.size();
    uint8_t *وسعت_پٺ_UTF8_Lengths=وسعت_Lengths;
    إذا(all) {
        وسعت_پٺ_UTF8_Lengths+=3*لڙيون_ڊيگهائي;
    }
    لكل(;;) {
        ثابت uint8_t *s8=utf8;
        int32_t length8;
        إذا(وسعت_Condition==USET_وسعت_CONTAINED) {
            لكل(i=0; i<لڙيون_ڊيگهائي; ++i) {
                length8=utf8Lengths[i];
                إذا(length8==0) {
                    استمر;  // String not representable in UTF-8.
                }
                int32_t overlap=وسعت_پٺ_UTF8_Lengths[i];
                إذا(overlap==ALL_CP_CONTAINED) {
                    s8+=length8;
                    استمر;  // Irrelevant string.
                }

                // Try to match this string at pos-(length8-overlap)..pos-length8.
                إذا(overlap>=LONG_وسعت) {
                    overlap=length8;
                    // While contained: No point matching fully inside the code point span.
                    int32_t len1=0;
                    U8_FWD_1(s8, len1, overlap);
                    overlap-=len1;  // Length of the string minus the first code point.
                }
                إذا(overlap>وسعت_ڊيگهائي) {
                    overlap=وسعت_ڊيگهائي;
                }
                int32_t dec=length8-overlap;  // Keep dec+overlap==length8.
                لكل(;;) {
                    إذا(dec>pos) {
                        اكسر;
                    }
                    // Try to match if the decrement is not listed already.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    إذا( !U8_IS_TRAIL(s[pos-dec]) &&
                        !offsets.شامل_آهي_وٿي(dec) &&
                        matches8(s+pos-dec, s8, length8)
                    ) {
                        إذا(dec==pos) {
                            أرجع 0;  // Reached the start of the string.
                        }
                        offsets.جوڙيو_وٿي(dec);
                    }
                    إذا(overlap==0) {
                        اكسر;
                    }
                    --overlap;
                    ++dec;
                }
                s8+=length8;
            }
        } وإلا /* USET_SPAN_SIMPLE */ {
            int32_t maxDec=0, maxOverlap=0;
            لكل(i=0; i<لڙيون_ڊيگهائي; ++i) {
                length8=utf8Lengths[i];
                إذا(length8==0) {
                    استمر;  // String not representable in UTF-8.
                }
                int32_t overlap=وسعت_پٺ_UTF8_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                // Try to match this string at pos-(length8-overlap)..pos-length8.
                إذا(overlap>=LONG_وسعت) {
                    overlap=length8;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the latest end.
                }
                إذا(overlap>وسعت_ڊيگهائي) {
                    overlap=وسعت_ڊيگهائي;
                }
                int32_t dec=length8-overlap;  // Keep dec+overlap==length8.
                لكل(;;) {
                    إذا(dec>pos || overlap<maxOverlap) {
                        اكسر;
                    }
                    // Try to match if the string is longer or ends later.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    إذا( !U8_IS_TRAIL(s[pos-dec]) &&
                        (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ dec>maxDec) &&
                        matches8(s+pos-dec, s8, length8)
                    ) {
                        maxDec=dec;  // Longest match from latest end.
                        maxOverlap=overlap;
                        اكسر;
                    }
                    --overlap;
                    ++dec;
                }
                s8+=length8;
            }

            إذا(maxDec!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue before it.
                pos-=maxDec;
                إذا(pos==0) {
                    أرجع 0;  // Reached the start of the string.
                }
                وسعت_ڊيگهائي=0;  // Match strings from before a string match.
                استمر;
            }
        }
        // Finished trying to match all strings at pos.

        إذا(وسعت_ڊيگهائي!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            إذا(offsets.is_خالي_جايون()) {
                أرجع pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } وإلا {
            // The position is before a string match (or a single code point).
            إذا(offsets.is_خالي_جايون()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos = وسعت_سيٽ.وسعت_پٺ_UTF8(reinterpret_cast<ثابت حرف*>(s), oldPos, USET_وسعت_CONTAINED);
                وسعت_ڊيگهائي=oldPos-pos;
                إذا( pos==0 ||           // Reached the start of the string, or
                    وسعت_ڊيگهائي==0       // neither strings nor span progressed.
                ) {
                    أرجع pos;
                }
                استمر;  // spanLength>0: Match strings from before a span.
            } وإلا {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                وسعت_ڊيگهائي=وسعت_One_پٺ_UTF8(وسعت_سيٽ, s, pos);
                إذا(وسعت_ڊيگهائي>0) {
                    إذا(وسعت_ڊيگهائي==pos) {
                        أرجع 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=وسعت_ڊيگهائي;
                    offsets.سرڪايو(وسعت_ڊيگهائي);
                    وسعت_ڊيگهائي=0;
                    استمر;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.ڪڍو_Minimum();
        وسعت_ڊيگهائي=0;  // Match strings from before a string match.
    }
}

/*
 * Algorithm for spanNot()==span(USET_SPAN_NOT_CONTAINED)
 *
 * Theoretical algorithm:
 * - Iterate through the string, and at each code point boundary:
 *   + If the code point there is in the set, then return with the current position.
 *   + If a set string matches at the current position, then return with the current position.
 *
 * Optimized implementation:
 *
 * (Same assumption as for span() above.)
 *
 * Create and cache a spanNotSet which contains all of the single code points
 * of the original set but none of its strings.
 * For each set string add its initial code point to the spanNotSet.
 * (Also add its final code point for spanNotBack().)
 *
 * - Loop:
 *   + Do spanLength=spanNotSet.span(USET_SPAN_NOT_CONTAINED).
 *   + If the current code point is in the original set, then
 *     return the current position.
 *   + If any set string matches at the current position, then
 *     return the current position.
 *   + If there is no match at the current position, neither for the code point there
 *     nor for any set string, then skip this code point and continue the loop.
 *     This happens for set-string-initial code points that were added to spanNotSet
 *     when there is not actually a match for such a set string.
 */

int32_t Unicode_سيٽ_String_وسعت::وسعت_Not(ثابت char16_t *s, int32_t length) ثابت {
    int32_t pos=0, rest=length;
    int32_t i, لڙيون_ڊيگهائي=لڙيون.size();
    افعل {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i=p_وسعت_Not_سيٽ->وسعت(s+pos, rest, USET_وسعت_NOT_CONTAINED);
        إذا(i==rest) {
            أرجع length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_ڊيگهائي=وسعت_One(وسعت_سيٽ, s+pos, rest);
        إذا(cp_ڊيگهائي>0) {
            أرجع pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        لكل(i=0; i<لڙيون_ڊيگهائي; ++i) {
            إذا(وسعت_Lengths[i]==ALL_CP_CONTAINED) {
                استمر;  // Irrelevant string. (Also the empty string.)
            }
            ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڙيون.elementAt(i));
            ثابت char16_t *s16=string.get_بفر();
            int32_t length16=string.length();
            U_ASSERT(length>0);
            إذا(length16<=rest && matches16CPB(s, pos, length, s16, length16)) {
                أرجع pos;  // There is a set element at pos.
            }
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos-=cp_ڊيگهائي;
        rest+=cp_ڊيگهائي;
    } طالما(rest!=0);
    أرجع length;  // Reached the end of the string.
}

int32_t Unicode_سيٽ_String_وسعت::وسعت_Not_پٺ(ثابت char16_t *s, int32_t length) ثابت {
    int32_t pos=length;
    int32_t i, لڙيون_ڊيگهائي=لڙيون.size();
    افعل {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos=p_وسعت_Not_سيٽ->وسعت_پٺ(s, pos, USET_وسعت_NOT_CONTAINED);
        إذا(pos==0) {
            أرجع 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_ڊيگهائي=وسعت_One_پٺ(وسعت_سيٽ, s, pos);
        إذا(cp_ڊيگهائي>0) {
            أرجع pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        لكل(i=0; i<لڙيون_ڊيگهائي; ++i) {
            // Use spanLengths rather than a spanBackLengths pointer because
            // it is easier and we only need to know whether the string is irrelevant
            // which is the same in either array.
            إذا(وسعت_Lengths[i]==ALL_CP_CONTAINED) {
                استمر;  // Irrelevant string. (Also the empty string.)
            }
            ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڙيون.elementAt(i));
            ثابت char16_t *s16=string.get_بفر();
            int32_t length16=string.length();
            U_ASSERT(length>0);
            إذا(length16<=pos && matches16CPB(s, pos-length16, length, s16, length16)) {
                أرجع pos;  // There is a set element at pos.
            }
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos+=cp_ڊيگهائي;
    } طالما(pos!=0);
    أرجع 0;  // Reached the start of the string.
}

int32_t Unicode_سيٽ_String_وسعت::وسعت_Not_UTF8(ثابت uint8_t *s, int32_t length) ثابت {
    int32_t pos=0, rest=length;
    int32_t i, لڙيون_ڊيگهائي=لڙيون.size();
    uint8_t *وسعت_UTF8_Lengths=وسعت_Lengths;
    إذا(all) {
        وسعت_UTF8_Lengths+=2*لڙيون_ڊيگهائي;
    }
    افعل {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i = p_وسعت_Not_سيٽ->وسعت_UTF8(reinterpret_cast<ثابت حرف*>(s) + pos, rest, USET_وسعت_NOT_CONTAINED);
        إذا(i==rest) {
            أرجع length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_ڊيگهائي=وسعت_One_UTF8(وسعت_سيٽ, s+pos, rest);
        إذا(cp_ڊيگهائي>0) {
            أرجع pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        ثابت uint8_t *s8=utf8;
        int32_t length8;
        لكل(i=0; i<لڙيون_ڊيگهائي; ++i) {
            length8=utf8Lengths[i];
            // ALL_CP_CONTAINED: Irrelevant string.
            إذا(length8!=0 && وسعت_UTF8_Lengths[i]!=ALL_CP_CONTAINED && length8<=rest && matches8(s+pos, s8, length8)) {
                أرجع pos;  // There is a set element at pos.
            }
            s8+=length8;
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos-=cp_ڊيگهائي;
        rest+=cp_ڊيگهائي;
    } طالما(rest!=0);
    أرجع length;  // Reached the end of the string.
}

int32_t Unicode_سيٽ_String_وسعت::وسعت_Not_پٺ_UTF8(ثابت uint8_t *s, int32_t length) ثابت {
    int32_t pos=length;
    int32_t i, لڙيون_ڊيگهائي=لڙيون.size();
    uint8_t *وسعت_پٺ_UTF8_Lengths=وسعت_Lengths;
    إذا(all) {
        وسعت_پٺ_UTF8_Lengths+=3*لڙيون_ڊيگهائي;
    }
    افعل {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos = p_وسعت_Not_سيٽ->وسعت_پٺ_UTF8(reinterpret_cast<ثابت حرف*>(s), pos, USET_وسعت_NOT_CONTAINED);
        إذا(pos==0) {
            أرجع 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_ڊيگهائي=وسعت_One_پٺ_UTF8(وسعت_سيٽ, s, pos);
        إذا(cp_ڊيگهائي>0) {
            أرجع pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        ثابت uint8_t *s8=utf8;
        int32_t length8;
        لكل(i=0; i<لڙيون_ڊيگهائي; ++i) {
            length8=utf8Lengths[i];
            // ALL_CP_CONTAINED: Irrelevant string.
            إذا(length8!=0 && وسعت_پٺ_UTF8_Lengths[i]!=ALL_CP_CONTAINED && length8<=pos && matches8(s+pos-length8, s8, length8)) {
                أرجع pos;  // There is a set element at pos.
            }
            s8+=length8;
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos+=cp_ڊيگهائي;
    } طالما(pos!=0);
    أرجع 0;  // Reached the start of the string.
}

U_NAMESPACE_پڇاڙي
