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

#شامل "unicode/utypes.h"
#شامل "unicode/uniset.h"
#شامل "unicode/ustring.h"
#شامل "unicode/utf8.h"
#شامل "unicode/utf16.h"
#شامل "cmemory.h"
#شامل "uvector.h"
#شامل "unisetspan.h"

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
جماعت انحراف_List {  // Only ever stack-allocated, does not need to inherit UMemory.
عوامی:
    انحراف_List() : list(staticList), capacity(0), length(0), آغاز(0) {}

    ~انحراف_List() {
        اگر(list!=staticList) {
            uprv_free(list);
        }
    }

    // Call exactly once if the list is to be used.
    خالی سیٹ_Max_لمبائی(int32_t max_لمبائی_2) {
        اگر (max_لمبائی_2 <= static_cast<int32_t>(حجم(staticList))) {
            capacity = static_cast<int32_t>(حجم(staticList));
        } ورنہ {
            UBool* l = static_cast<UBool*>(uprv_malloc(max_لمبائی_2));
            اگر(l!=خالی_اشاریہ) {
                list=l;
                capacity=max_لمبائی_2;
            }
        }
        uprv_memset(list, 0, capacity);
    }

    خالی clear() {
        uprv_memset(list, 0, capacity);
        آغاز=length=0;
    }

    UBool is_خالی_خانے() ثابت {
        واپس length == 0;
    }

    // Reduce all stored offsets by delta, used when the current position
    // moves by delta.
    // There must not be any offsets lower than delta.
    // If there is an offset equal to delta, it is removed.
    // delta=[1..maxLength]
    خالی سرکاؤ(int32_t delta) {
        int32_t i=آغاز+delta;
        اگر(i>=capacity) {
            i-=capacity;
        }
        اگر(list[i]) {
            list[i]=جھوٹ;
            --length;
        }
        آغاز=i;
    }

    // Add an offset. The list must not contain it yet.
    // offset=[1..maxLength]
    خالی جوڑو_انحراف(int32_t انحراف) {
        int32_t i=آغاز+انحراف;
        اگر(i>=capacity) {
            i-=capacity;
        }
        list[i]=سچ;
        ++length;
    }

    // offset=[1..maxLength]
    UBool شامل_ہے_انحراف(int32_t انحراف) ثابت {
        int32_t i=آغاز+انحراف;
        اگر(i>=capacity) {
            i-=capacity;
        }
        واپس list[i];
    }

    // Find the lowest stored offset from a non-empty list, remove it,
    // and reduce all other offsets by this minimum.
    // Returns [1..maxLength].
    int32_t نکالو_Minimum() {
        // Look for the next offset in list[start+1..capacity-1].
        int32_t i=آغاز, نتیجہ;
        جب(++i<capacity) {
            اگر(list[i]) {
                list[i]=جھوٹ;
                --length;
                نتیجہ=i-آغاز;
                آغاز=i;
                واپس نتیجہ;
            }
        }
        // i==capacity

        // Wrap around and look for the next offset in list[0..start].
        // Since the list is not empty, there will be one.
        نتیجہ=capacity-آغاز;
        i=0;
        جب(!list[i]) {
            ++i;
        }
        list[i]=جھوٹ;
        --length;
        آغاز=i;
        واپس نتیجہ+=i;
    }

نجی:
    UBool *list;
    int32_t capacity;
    int32_t length;
    int32_t آغاز;

    UBool staticList[16];
};

// Get the number of UTF-8 bytes for a UTF-16 (sub)string.
ساکن int32_t
get_UTF8_لمبائی(ثابت char16_t *s, int32_t length) {
    UError_رمز error_رمز=U_ZERO_ERROR;
    int32_t length8=0;
    u_str_مقصد_UTF8(خالی_اشاریہ, 0, &length8, s, length, &error_رمز);
    اگر(U_SUCCESS(error_رمز) || error_رمز==U_بفر_حد_سے_تجاوز_ERROR) {
        واپس length8;
    } ورنہ {
        // The string contains an unpaired surrogate.
        // Ignore this string.
        واپس 0;
    }
}

// Append the UTF-8 version of the string to t and return the appended UTF-8 length.
ساکن int32_t
appendUTF8(ثابت char16_t *s, int32_t length, uint8_t *t, int32_t capacity) {
    UError_رمز error_رمز=U_ZERO_ERROR;
    int32_t length8=0;
    u_str_مقصد_UTF8(reinterpret_cast<حرف*>(t), capacity, &length8, s, length, &error_رمز);
    اگر(U_SUCCESS(error_رمز)) {
        واپس length8;
    } ورنہ {
        // The string contains an unpaired surrogate.
        // Ignore this string.
        واپس 0;
    }
}

ساکن اندرونی uint8_t
make_وسعت_لمبائی_Byte(int32_t وسعت_لمبائی) {
    // 0xfe==UnicodeSetStringSpan::LONG_SPAN
    واپس وسعت_لمبائی < 0xfe ? static_cast<uint8_t>(وسعت_لمبائی) : static_cast<uint8_t>(0xfe);
}

// Construct for all variants of span(), or only for any one variant.
// Initialize as little as possible, for single use.
Unicode_سیٹ_String_وسعت::Unicode_سیٹ_String_وسعت(ثابت Unicode_سیٹ &سیٹ,
                                           ثابت UVector &سیٹ_لڑیاں,
                                           uint32_t which)
        : وسعت_سیٹ(0, 0x10ffff), p_وسعت_Not_سیٹ(خالی_اشاریہ), لڑیاں(سیٹ_لڑیاں),
          utf8Lengths(خالی_اشاریہ), وسعت_Lengths(خالی_اشاریہ), utf8(خالی_اشاریہ),
          utf8_لمبائی(0),
          maxLength16(0), maxLength8(0),
          all(static_cast<UBool>(which == ALL)) {
    وسعت_سیٹ.retainAll(سیٹ);
    اگر(which&NOT_CONTAINED) {
        // Default to the same sets.
        // addToSpanNotSet() will create a separate set if necessary.
        p_وسعت_Not_سیٹ=&وسعت_سیٹ;
    }

    // Determine if the strings even need to be taken into account at all for span() etc.
    // If any string is relevant, then all strings need to be used for
    // span(longest match) but only the relevant ones for span(while contained).
    // TODO: Possible optimization: Distinguish CONTAINED vs. LONGEST_MATCH
    //   and do not store UTF-8 strings if !thisRelevant and CONTAINED.
    //   (Only store irrelevant UTF-8 strings for LONGEST_MATCH where they are relevant after all.)
    // Also count the lengths of the UTF-8 versions of the strings for memory allocation.
    int32_t لڑیاں_لمبائی=لڑیاں.size();

    int32_t i, وسعت_لمبائی;
    UBool someRelevant=جھوٹ;
    برائے(i=0; i<لڑیاں_لمبائی; ++i) {
        ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڑیاں.elementAt(i));
        ثابت char16_t *s16=string.get_بفر();
        int32_t length16=string.length();
        اگر (length16==0) {
            جاری;  // skip the empty string
        }
        UBool thisRelevant;
        وسعت_لمبائی=وسعت_سیٹ.وسعت(s16, length16, USET_وسعت_CONTAINED);
        اگر(وسعت_لمبائی<length16) {  // Relevant string.
            someRelevant=thisRelevant=سچ;
        } ورنہ {
            thisRelevant=جھوٹ;
        }
        اگر((which&UTF16) && length16>maxLength16) {
            maxLength16=length16;
        }
        اگر((which&UTF8) && (thisRelevant || (which&CONTAINED))) {
            int32_t length8=get_UTF8_لمبائی(s16, length16);
            utf8_لمبائی+=length8;
            اگر(length8>maxLength8) {
                maxLength8=length8;
            }
        }
    }
    اگر(!someRelevant) {
        maxLength16=maxLength8=0;
        واپس;
    }

    // Freeze after checking for the need to use strings at all because freezing
    // a set takes some time and memory which are wasted if there are no relevant strings.
    اگر(all) {
        وسعت_سیٹ.freeze();
    }

    uint8_t *وسعت_عقب_Lengths;
    uint8_t *وسعت_UTF8_Lengths;
    uint8_t *وسعت_عقب_UTF8_Lengths;

    // Allocate a block of meta data.
    int32_t alloc_سائز_2;
    اگر(all) {
        // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
        alloc_سائز_2=لڑیاں_لمبائی*(4+1+1+1+1)+utf8_لمبائی;
    } ورنہ {
        alloc_سائز_2=لڑیاں_لمبائی;  // One set of span lengths.
        اگر(which&UTF8) {
            // UTF-8 lengths and UTF-8 strings.
            alloc_سائز_2+=لڑیاں_لمبائی*4+utf8_لمبائی;
        }
    }
    اگر (alloc_سائز_2 <= static_cast<int32_t>(حجم(staticLengths))) {
        utf8Lengths=staticLengths;
    } ورنہ {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(alloc_سائز_2));
        اگر(utf8Lengths==خالی_اشاریہ) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            واپس;  // Out of memory.
        }
    }

    اگر(all) {
        // Store span lengths for all span() variants.
        وسعت_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths + لڑیاں_لمبائی);
        وسعت_عقب_Lengths=وسعت_Lengths+لڑیاں_لمبائی;
        وسعت_UTF8_Lengths=وسعت_عقب_Lengths+لڑیاں_لمبائی;
        وسعت_عقب_UTF8_Lengths=وسعت_UTF8_Lengths+لڑیاں_لمبائی;
        utf8=وسعت_عقب_UTF8_Lengths+لڑیاں_لمبائی;
    } ورنہ {
        // Store span lengths for only one span() variant.
        اگر(which&UTF8) {
            وسعت_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths + لڑیاں_لمبائی);
            utf8=وسعت_Lengths+لڑیاں_لمبائی;
        } ورنہ {
            وسعت_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths);
        }
        وسعت_عقب_Lengths=وسعت_UTF8_Lengths=وسعت_عقب_UTF8_Lengths=وسعت_Lengths;
    }

    // Set the meta data and pSpanNotSet and write the UTF-8 strings.
    int32_t utf8_گنتی=0;  // Count UTF-8 bytes written so far.

    برائے(i=0; i<لڑیاں_لمبائی; ++i) {
        ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڑیاں.elementAt(i));
        ثابت char16_t *s16=string.get_بفر();
        int32_t length16=string.length();
        وسعت_لمبائی=وسعت_سیٹ.وسعت(s16, length16, USET_وسعت_CONTAINED);
        اگر(وسعت_لمبائی<length16 && length16>0) {  // Relevant string.
            اگر(which&UTF16) {
                اگر(which&CONTAINED) {
                    اگر(which&FWD) {
                        وسعت_Lengths[i]=make_وسعت_لمبائی_Byte(وسعت_لمبائی);
                    }
                    اگر(which&عقب_2) {
                        وسعت_لمبائی=length16-وسعت_سیٹ.وسعت_عقب(s16, length16, USET_وسعت_CONTAINED);
                        وسعت_عقب_Lengths[i]=make_وسعت_لمبائی_Byte(وسعت_لمبائی);
                    }
                } ورنہ /* not CONTAINED, not all, but NOT_CONTAINED */ {
                    وسعت_Lengths[i]=وسعت_عقب_Lengths[i]=0;  // Only store a relevant/irrelevant flag.
                }
            }
            اگر(which&UTF8) {
                uint8_t *s8=utf8+utf8_گنتی;
                int32_t length8=appendUTF8(s16, length16, s8, utf8_لمبائی-utf8_گنتی);
                utf8_گنتی+=utf8Lengths[i]=length8;
                اگر(length8==0) {  // Irrelevant for UTF-8 because not representable in UTF-8.
                    وسعت_UTF8_Lengths[i] = وسعت_عقب_UTF8_Lengths[i] = static_cast<uint8_t>(ALL_CP_CONTAINED);
                } ورنہ {  // Relevant for UTF-8.
                    اگر(which&CONTAINED) {
                        اگر(which&FWD) {
                            وسعت_لمبائی = وسعت_سیٹ.وسعت_UTF8(reinterpret_cast<ثابت حرف*>(s8), length8, USET_وسعت_CONTAINED);
                            وسعت_UTF8_Lengths[i]=make_وسعت_لمبائی_Byte(وسعت_لمبائی);
                        }
                        اگر(which&عقب_2) {
                            وسعت_لمبائی = length8 - وسعت_سیٹ.وسعت_عقب_UTF8(reinterpret_cast<ثابت حرف*>(s8), length8, USET_وسعت_CONTAINED);
                            وسعت_عقب_UTF8_Lengths[i]=make_وسعت_لمبائی_Byte(وسعت_لمبائی);
                        }
                    } ورنہ /* not CONTAINED, not all, but NOT_CONTAINED */ {
                        وسعت_UTF8_Lengths[i]=وسعت_عقب_UTF8_Lengths[i]=0;  // Only store a relevant/irrelevant flag.
                    }
                }
            }
            اگر(which&NOT_CONTAINED) {
                // Add string start and end code points to the spanNotSet so that
                // a span(while not contained) stops before any string.
                UChar32 c;
                اگر(which&FWD) {
                    int32_t len=0;
                    U16_اگلا(s16, len, length16, c);
                    جوڑو_مقصد_وسعت_Not_سیٹ(c);
                }
                اگر(which&عقب_2) {
                    int32_t len=length16;
                    U16_پچھلا(s16, 0, len, c);
                    جوڑو_مقصد_وسعت_Not_سیٹ(c);
                }
            }
        } ورنہ {  // Irrelevant string. (Also the empty string.)
            اگر(which&UTF8) {
                اگر(which&CONTAINED) {  // Only necessary for LONGEST_MATCH.
                    uint8_t *s8=utf8+utf8_گنتی;
                    int32_t length8=appendUTF8(s16, length16, s8, utf8_لمبائی-utf8_گنتی);
                    utf8_گنتی+=utf8Lengths[i]=length8;
                } ورنہ {
                    utf8Lengths[i]=0;
                }
            }
            اگر(all) {
                وسعت_Lengths[i]=وسعت_عقب_Lengths[i]=
                    وسعت_UTF8_Lengths[i]=وسعت_عقب_UTF8_Lengths[i]=
                        static_cast<uint8_t>(ALL_CP_CONTAINED);
            } ورنہ {
                // All spanXYZLengths pointers contain the same address.
                وسعت_Lengths[i] = static_cast<uint8_t>(ALL_CP_CONTAINED);
            }
        }
    }

    // Finish.
    اگر(all) {
        p_وسعت_Not_سیٹ->freeze();
    }
}

// Copy constructor. Assumes which==ALL for a frozen set.
Unicode_سیٹ_String_وسعت::Unicode_سیٹ_String_وسعت(ثابت Unicode_سیٹ_String_وسعت &other_String_وسعت,
                                           ثابت UVector &new_والد_سیٹ_لڑیاں)
        : وسعت_سیٹ(other_String_وسعت.وسعت_سیٹ), p_وسعت_Not_سیٹ(خالی_اشاریہ), لڑیاں(new_والد_سیٹ_لڑیاں),
          utf8Lengths(خالی_اشاریہ), وسعت_Lengths(خالی_اشاریہ), utf8(خالی_اشاریہ),
          utf8_لمبائی(other_String_وسعت.utf8_لمبائی),
          maxLength16(other_String_وسعت.maxLength16), maxLength8(other_String_وسعت.maxLength8),
          all(سچ) {
    اگر(other_String_وسعت.p_وسعت_Not_سیٹ==&other_String_وسعت.وسعت_سیٹ) {
        p_وسعت_Not_سیٹ=&وسعت_سیٹ;
    } ورنہ {
        p_وسعت_Not_سیٹ=other_String_وسعت.p_وسعت_Not_سیٹ->clone();
    }

    // Allocate a block of meta data.
    // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
    int32_t لڑیاں_لمبائی=لڑیاں.size();
    int32_t alloc_سائز_2=لڑیاں_لمبائی*(4+1+1+1+1)+utf8_لمبائی;
    اگر (alloc_سائز_2 <= static_cast<int32_t>(حجم(staticLengths))) {
        utf8Lengths=staticLengths;
    } ورنہ {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(alloc_سائز_2));
        اگر(utf8Lengths==خالی_اشاریہ) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            واپس;  // Out of memory.
        }
    }

    وسعت_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths + لڑیاں_لمبائی);
    utf8=وسعت_Lengths+لڑیاں_لمبائی*4;
    uprv_memcpy(utf8Lengths, other_String_وسعت.utf8Lengths, alloc_سائز_2);
}

Unicode_سیٹ_String_وسعت::~Unicode_سیٹ_String_وسعت() {
    اگر(p_وسعت_Not_سیٹ!=خالی_اشاریہ && p_وسعت_Not_سیٹ!=&وسعت_سیٹ) {
        حذف p_وسعت_Not_سیٹ;
    }
    اگر(utf8Lengths!=خالی_اشاریہ && utf8Lengths!=staticLengths) {
        uprv_free(utf8Lengths);
    }
}

خالی Unicode_سیٹ_String_وسعت::جوڑو_مقصد_وسعت_Not_سیٹ(UChar32 c) {
    اگر(p_وسعت_Not_سیٹ==خالی_اشاریہ || p_وسعت_Not_سیٹ==&وسعت_سیٹ) {
        اگر(وسعت_سیٹ.شامل_ہے(c)) {
            واپس;  // Nothing to do.
        }
        Unicode_سیٹ *new_سیٹ=وسعت_سیٹ.cloneAsThawed();
        اگر(new_سیٹ==خالی_اشاریہ) {
            واپس;  // Out of memory.
        } ورنہ {
            p_وسعت_Not_سیٹ=new_سیٹ;
        }
    }
    p_وسعت_Not_سیٹ->جوڑو(c);
}

// Compare strings without any argument checks. Requires length>0.
ساکن اندرونی UBool
matches16(ثابت char16_t *s, ثابت char16_t *t, int32_t length) {
    کرو {
        اگر(*s++!=*t++) {
            واپس جھوٹ;
        }
    } جب(--length>0);
    واپس سچ;
}

ساکن اندرونی UBool
matches8(ثابت uint8_t *s, ثابت uint8_t *t, int32_t length) {
    کرو {
        اگر(*s++!=*t++) {
            واپس جھوٹ;
        }
    } جب(--length>0);
    واپس سچ;
}

// Compare 16-bit Unicode strings (which may be malformed UTF-16)
// at code point boundaries.
// That is, each edge of a match must not be in the middle of a surrogate pair.
ساکن اندرونی UBool
matches16CPB(ثابت char16_t *s, int32_t آغاز, int32_t حد_بندی, ثابت char16_t *t, int32_t length) {
    s+=آغاز;
    حد_بندی-=آغاز;
    واپس matches16(s, t, length) &&
           !(0<آغاز && U16_IS_LEAD(s[-1]) && U16_IS_TRAIL(s[0])) &&
           !(length<حد_بندی && U16_IS_LEAD(s[length-1]) && U16_IS_TRAIL(s[length]));
}

// Does the set contain the next code point?
// If so, return its length; otherwise return its negative length.
ساکن اندرونی int32_t
وسعت_One(ثابت Unicode_سیٹ &سیٹ, ثابت char16_t *s, int32_t length) {
    char16_t c=*s, c2;
    اگر(c>=0xd800 && c<=0xdbff && length>=2 && U16_IS_TRAIL(c2=s[1])) {
        واپس سیٹ.شامل_ہے(U16_GET_SUPPLEMENTARY(c, c2)) ? 2 : -2;
    }
    واپس سیٹ.شامل_ہے(c) ? 1 : -1;
}

ساکن اندرونی int32_t
وسعت_One_عقب(ثابت Unicode_سیٹ &سیٹ, ثابت char16_t *s, int32_t length) {
    char16_t c=s[length-1], c2;
    اگر(c>=0xdc00 && c<=0xdfff && length>=2 && U16_IS_LEAD(c2=s[length-2])) {
        واپس سیٹ.شامل_ہے(U16_GET_SUPPLEMENTARY(c2, c)) ? 2 : -2;
    }
    واپس سیٹ.شامل_ہے(c) ? 1 : -1;
}

ساکن اندرونی int32_t
وسعت_One_UTF8(ثابت Unicode_سیٹ &سیٹ, ثابت uint8_t *s, int32_t length) {
    UChar32 c=*s;
    اگر(U8_IS_SINGLE(c)) {
        واپس سیٹ.شامل_ہے(c) ? 1 : -1;
    }
    // Take advantage of non-ASCII fastpaths in U8_NEXT_OR_FFFD().
    int32_t i=0;
    U8_اگلا_OR_FFFD(s, i, length, c);
    واپس سیٹ.شامل_ہے(c) ? i : -i;
}

ساکن اندرونی int32_t
وسعت_One_عقب_UTF8(ثابت Unicode_سیٹ &سیٹ, ثابت uint8_t *s, int32_t length) {
    UChar32 c=s[length-1];
    اگر(U8_IS_SINGLE(c)) {
        واپس سیٹ.شامل_ہے(c) ? 1 : -1;
    }
    int32_t i=length-1;
    c=utf8_پچھلا_Char_محفوظ_Body(s, 0, &i, c, -3);
    length-=i;
    واپس سیٹ.شامل_ہے(c) ? length : -length;
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

int32_t Unicode_سیٹ_String_وسعت::وسعت(ثابت char16_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    اگر(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
        واپس وسعت_Not(s, length);
    }
    int32_t وسعت_لمبائی=وسعت_سیٹ.وسعت(s, length, USET_وسعت_CONTAINED);
    اگر(وسعت_لمبائی==length) {
        واپس length;
    }

    // Consider strings; they may overlap with the span.
    انحراف_List offsets;
    اگر(وسعت_Condition==USET_وسعت_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.سیٹ_Max_لمبائی(maxLength16);
    }
    int32_t pos=وسعت_لمبائی, rest=length-pos;
    int32_t i, لڑیاں_لمبائی=لڑیاں.size();
    برائے(;;) {
        اگر(وسعت_Condition==USET_وسعت_CONTAINED) {
            برائے(i=0; i<لڑیاں_لمبائی; ++i) {
                int32_t overlap=وسعت_Lengths[i];
                اگر(overlap==ALL_CP_CONTAINED) {
                    جاری;  // Irrelevant string. (Also the empty string.)
                }
                ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڑیاں.elementAt(i));
                ثابت char16_t *s16=string.get_بفر();
                int32_t length16=string.length();
                U_ASSERT(length>0);

                // Try to match this string at pos-overlap..pos.
                اگر(overlap>=LONG_وسعت) {
                    overlap=length16;
                    // While contained: No point matching fully inside the code point span.
                    U16_عقب_1(s16, 0, overlap);  // Length of the string minus the last code point.
                }
                اگر(overlap>وسعت_لمبائی) {
                    overlap=وسعت_لمبائی;
                }
                int32_t inc=length16-overlap;  // Keep overlap+inc==length16.
                برائے(;;) {
                    اگر(inc>rest) {
                        توڑو;
                    }
                    // Try to match if the increment is not listed already.
                    اگر(!offsets.شامل_ہے_انحراف(inc) && matches16CPB(s, pos-overlap, length, s16, length16)) {
                        اگر(inc==rest) {
                            واپس length;  // Reached the end of the string.
                        }
                        offsets.جوڑو_انحراف(inc);
                    }
                    اگر(overlap==0) {
                        توڑو;
                    }
                    --overlap;
                    ++inc;
                }
            }
        } ورنہ /* USET_SPAN_SIMPLE */ {
            int32_t maxInc=0, maxOverlap=0;
            برائے(i=0; i<لڑیاں_لمبائی; ++i) {
                int32_t overlap=وسعت_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the earliest start.

                ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڑیاں.elementAt(i));
                ثابت char16_t *s16=string.get_بفر();
                int32_t length16=string.length();
                اگر (length16==0) {
                    جاری;  // skip the empty string
                }

                // Try to match this string at pos-overlap..pos.
                اگر(overlap>=LONG_وسعت) {
                    overlap=length16;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the earliest start.
                }
                اگر(overlap>وسعت_لمبائی) {
                    overlap=وسعت_لمبائی;
                }
                int32_t inc=length16-overlap;  // Keep overlap+inc==length16.
                برائے(;;) {
                    اگر(inc>rest || overlap<maxOverlap) {
                        توڑو;
                    }
                    // Try to match if the string is longer or starts earlier.
                    اگر( (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ inc>maxInc) &&
                        matches16CPB(s, pos-overlap, length, s16, length16)
                    ) {
                        maxInc=inc;  // Longest match from earliest start.
                        maxOverlap=overlap;
                        توڑو;
                    }
                    --overlap;
                    ++inc;
                }
            }

            اگر(maxInc!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue after it.
                pos+=maxInc;
                rest-=maxInc;
                اگر(rest==0) {
                    واپس length;  // Reached the end of the string.
                }
                وسعت_لمبائی=0;  // Match strings from after a string match.
                جاری;
            }
        }
        // Finished trying to match all strings at pos.

        اگر(وسعت_لمبائی!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            اگر(offsets.is_خالی_خانے()) {
                واپس pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } ورنہ {
            // The position is after a string match (or a single code point).
            اگر(offsets.is_خالی_خانے()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                وسعت_لمبائی=وسعت_سیٹ.وسعت(s+pos, rest, USET_وسعت_CONTAINED);
                اگر( وسعت_لمبائی==rest || // Reached the end of the string, or
                    وسعت_لمبائی==0       // neither strings nor span progressed.
                ) {
                    واپس pos+وسعت_لمبائی;
                }
                pos+=وسعت_لمبائی;
                rest-=وسعت_لمبائی;
                جاری;  // spanLength>0: Match strings from after a span.
            } ورنہ {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                وسعت_لمبائی=وسعت_One(وسعت_سیٹ, s+pos, rest);
                اگر(وسعت_لمبائی>0) {
                    اگر(وسعت_لمبائی==rest) {
                        واپس length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=وسعت_لمبائی;
                    rest-=وسعت_لمبائی;
                    offsets.سرکاؤ(وسعت_لمبائی);
                    وسعت_لمبائی=0;
                    جاری;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t min_انحراف=offsets.نکالو_Minimum();
        pos+=min_انحراف;
        rest-=min_انحراف;
        وسعت_لمبائی=0;  // Match strings from after a string match.
    }
}

int32_t Unicode_سیٹ_String_وسعت::وسعت_عقب(ثابت char16_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    اگر(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
        واپس وسعت_Not_عقب(s, length);
    }
    int32_t pos=وسعت_سیٹ.وسعت_عقب(s, length, USET_وسعت_CONTAINED);
    اگر(pos==0) {
        واپس 0;
    }
    int32_t وسعت_لمبائی=length-pos;

    // Consider strings; they may overlap with the span.
    انحراف_List offsets;
    اگر(وسعت_Condition==USET_وسعت_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.سیٹ_Max_لمبائی(maxLength16);
    }
    int32_t i, لڑیاں_لمبائی=لڑیاں.size();
    uint8_t *وسعت_عقب_Lengths=وسعت_Lengths;
    اگر(all) {
        وسعت_عقب_Lengths+=لڑیاں_لمبائی;
    }
    برائے(;;) {
        اگر(وسعت_Condition==USET_وسعت_CONTAINED) {
            برائے(i=0; i<لڑیاں_لمبائی; ++i) {
                int32_t overlap=وسعت_عقب_Lengths[i];
                اگر(overlap==ALL_CP_CONTAINED) {
                    جاری;  // Irrelevant string. (Also the empty string.)
                }
                ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڑیاں.elementAt(i));
                ثابت char16_t *s16=string.get_بفر();
                int32_t length16=string.length();
                U_ASSERT(length>0);

                // Try to match this string at pos-(length16-overlap)..pos-length16.
                اگر(overlap>=LONG_وسعت) {
                    overlap=length16;
                    // While contained: No point matching fully inside the code point span.
                    int32_t len1=0;
                    U16_FWD_1(s16, len1, overlap);
                    overlap-=len1;  // Length of the string minus the first code point.
                }
                اگر(overlap>وسعت_لمبائی) {
                    overlap=وسعت_لمبائی;
                }
                int32_t dec=length16-overlap;  // Keep dec+overlap==length16.
                برائے(;;) {
                    اگر(dec>pos) {
                        توڑو;
                    }
                    // Try to match if the decrement is not listed already.
                    اگر(!offsets.شامل_ہے_انحراف(dec) && matches16CPB(s, pos-dec, length, s16, length16)) {
                        اگر(dec==pos) {
                            واپس 0;  // Reached the start of the string.
                        }
                        offsets.جوڑو_انحراف(dec);
                    }
                    اگر(overlap==0) {
                        توڑو;
                    }
                    --overlap;
                    ++dec;
                }
            }
        } ورنہ /* USET_SPAN_SIMPLE */ {
            int32_t maxDec=0, maxOverlap=0;
            برائے(i=0; i<لڑیاں_لمبائی; ++i) {
                int32_t overlap=وسعت_عقب_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڑیاں.elementAt(i));
                ثابت char16_t *s16=string.get_بفر();
                int32_t length16=string.length();
                اگر (length16==0) {
                    جاری;  // skip the empty string
                }

                // Try to match this string at pos-(length16-overlap)..pos-length16.
                اگر(overlap>=LONG_وسعت) {
                    overlap=length16;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the latest end.
                }
                اگر(overlap>وسعت_لمبائی) {
                    overlap=وسعت_لمبائی;
                }
                int32_t dec=length16-overlap;  // Keep dec+overlap==length16.
                برائے(;;) {
                    اگر(dec>pos || overlap<maxOverlap) {
                        توڑو;
                    }
                    // Try to match if the string is longer or ends later.
                    اگر( (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ dec>maxDec) &&
                        matches16CPB(s, pos-dec, length, s16, length16)
                    ) {
                        maxDec=dec;  // Longest match from latest end.
                        maxOverlap=overlap;
                        توڑو;
                    }
                    --overlap;
                    ++dec;
                }
            }

            اگر(maxDec!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue before it.
                pos-=maxDec;
                اگر(pos==0) {
                    واپس 0;  // Reached the start of the string.
                }
                وسعت_لمبائی=0;  // Match strings from before a string match.
                جاری;
            }
        }
        // Finished trying to match all strings at pos.

        اگر(وسعت_لمبائی!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            اگر(offsets.is_خالی_خانے()) {
                واپس pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } ورنہ {
            // The position is before a string match (or a single code point).
            اگر(offsets.is_خالی_خانے()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos=وسعت_سیٹ.وسعت_عقب(s, oldPos, USET_وسعت_CONTAINED);
                وسعت_لمبائی=oldPos-pos;
                اگر( pos==0 ||           // Reached the start of the string, or
                    وسعت_لمبائی==0       // neither strings nor span progressed.
                ) {
                    واپس pos;
                }
                جاری;  // spanLength>0: Match strings from before a span.
            } ورنہ {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                وسعت_لمبائی=وسعت_One_عقب(وسعت_سیٹ, s, pos);
                اگر(وسعت_لمبائی>0) {
                    اگر(وسعت_لمبائی==pos) {
                        واپس 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=وسعت_لمبائی;
                    offsets.سرکاؤ(وسعت_لمبائی);
                    وسعت_لمبائی=0;
                    جاری;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.نکالو_Minimum();
        وسعت_لمبائی=0;  // Match strings from before a string match.
    }
}

int32_t Unicode_سیٹ_String_وسعت::وسعت_UTF8(ثابت uint8_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    اگر(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
        واپس وسعت_Not_UTF8(s, length);
    }
    int32_t وسعت_لمبائی = وسعت_سیٹ.وسعت_UTF8(reinterpret_cast<ثابت حرف*>(s), length, USET_وسعت_CONTAINED);
    اگر(وسعت_لمبائی==length) {
        واپس length;
    }

    // Consider strings; they may overlap with the span.
    انحراف_List offsets;
    اگر(وسعت_Condition==USET_وسعت_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.سیٹ_Max_لمبائی(maxLength8);
    }
    int32_t pos=وسعت_لمبائی, rest=length-pos;
    int32_t i, لڑیاں_لمبائی=لڑیاں.size();
    uint8_t *وسعت_UTF8_Lengths=وسعت_Lengths;
    اگر(all) {
        وسعت_UTF8_Lengths+=2*لڑیاں_لمبائی;
    }
    برائے(;;) {
        ثابت uint8_t *s8=utf8;
        int32_t length8;
        اگر(وسعت_Condition==USET_وسعت_CONTAINED) {
            برائے(i=0; i<لڑیاں_لمبائی; ++i) {
                length8=utf8Lengths[i];
                اگر(length8==0) {
                    جاری;  // String not representable in UTF-8.
                }
                int32_t overlap=وسعت_UTF8_Lengths[i];
                اگر(overlap==ALL_CP_CONTAINED) {
                    s8+=length8;
                    جاری;  // Irrelevant string.
                }

                // Try to match this string at pos-overlap..pos.
                اگر(overlap>=LONG_وسعت) {
                    overlap=length8;
                    // While contained: No point matching fully inside the code point span.
                    U8_عقب_1(s8, 0, overlap);  // Length of the string minus the last code point.
                }
                اگر(overlap>وسعت_لمبائی) {
                    overlap=وسعت_لمبائی;
                }
                int32_t inc=length8-overlap;  // Keep overlap+inc==length8.
                برائے(;;) {
                    اگر(inc>rest) {
                        توڑو;
                    }
                    // Try to match if the increment is not listed already.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    اگر(!U8_IS_TRAIL(s[pos-overlap]) &&
                            !offsets.شامل_ہے_انحراف(inc) &&
                            matches8(s+pos-overlap, s8, length8)) {
                        اگر(inc==rest) {
                            واپس length;  // Reached the end of the string.
                        }
                        offsets.جوڑو_انحراف(inc);
                    }
                    اگر(overlap==0) {
                        توڑو;
                    }
                    --overlap;
                    ++inc;
                }
                s8+=length8;
            }
        } ورنہ /* USET_SPAN_SIMPLE */ {
            int32_t maxInc=0, maxOverlap=0;
            برائے(i=0; i<لڑیاں_لمبائی; ++i) {
                length8=utf8Lengths[i];
                اگر(length8==0) {
                    جاری;  // String not representable in UTF-8.
                }
                int32_t overlap=وسعت_UTF8_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the earliest start.

                // Try to match this string at pos-overlap..pos.
                اگر(overlap>=LONG_وسعت) {
                    overlap=length8;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the earliest start.
                }
                اگر(overlap>وسعت_لمبائی) {
                    overlap=وسعت_لمبائی;
                }
                int32_t inc=length8-overlap;  // Keep overlap+inc==length8.
                برائے(;;) {
                    اگر(inc>rest || overlap<maxOverlap) {
                        توڑو;
                    }
                    // Try to match if the string is longer or starts earlier.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    اگر(!U8_IS_TRAIL(s[pos-overlap]) &&
                            (overlap>maxOverlap ||
                                /* redundant overlap==maxOverlap && */ inc>maxInc) &&
                            matches8(s+pos-overlap, s8, length8)) {
                        maxInc=inc;  // Longest match from earliest start.
                        maxOverlap=overlap;
                        توڑو;
                    }
                    --overlap;
                    ++inc;
                }
                s8+=length8;
            }

            اگر(maxInc!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue after it.
                pos+=maxInc;
                rest-=maxInc;
                اگر(rest==0) {
                    واپس length;  // Reached the end of the string.
                }
                وسعت_لمبائی=0;  // Match strings from after a string match.
                جاری;
            }
        }
        // Finished trying to match all strings at pos.

        اگر(وسعت_لمبائی!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            اگر(offsets.is_خالی_خانے()) {
                واپس pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } ورنہ {
            // The position is after a string match (or a single code point).
            اگر(offsets.is_خالی_خانے()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                وسعت_لمبائی = وسعت_سیٹ.وسعت_UTF8(reinterpret_cast<ثابت حرف*>(s) + pos, rest, USET_وسعت_CONTAINED);
                اگر( وسعت_لمبائی==rest || // Reached the end of the string, or
                    وسعت_لمبائی==0       // neither strings nor span progressed.
                ) {
                    واپس pos+وسعت_لمبائی;
                }
                pos+=وسعت_لمبائی;
                rest-=وسعت_لمبائی;
                جاری;  // spanLength>0: Match strings from after a span.
            } ورنہ {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                وسعت_لمبائی=وسعت_One_UTF8(وسعت_سیٹ, s+pos, rest);
                اگر(وسعت_لمبائی>0) {
                    اگر(وسعت_لمبائی==rest) {
                        واپس length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=وسعت_لمبائی;
                    rest-=وسعت_لمبائی;
                    offsets.سرکاؤ(وسعت_لمبائی);
                    وسعت_لمبائی=0;
                    جاری;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t min_انحراف=offsets.نکالو_Minimum();
        pos+=min_انحراف;
        rest-=min_انحراف;
        وسعت_لمبائی=0;  // Match strings from after a string match.
    }
}

int32_t Unicode_سیٹ_String_وسعت::وسعت_عقب_UTF8(ثابت uint8_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    اگر(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
        واپس وسعت_Not_عقب_UTF8(s, length);
    }
    int32_t pos = وسعت_سیٹ.وسعت_عقب_UTF8(reinterpret_cast<ثابت حرف*>(s), length, USET_وسعت_CONTAINED);
    اگر(pos==0) {
        واپس 0;
    }
    int32_t وسعت_لمبائی=length-pos;

    // Consider strings; they may overlap with the span.
    انحراف_List offsets;
    اگر(وسعت_Condition==USET_وسعت_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.سیٹ_Max_لمبائی(maxLength8);
    }
    int32_t i, لڑیاں_لمبائی=لڑیاں.size();
    uint8_t *وسعت_عقب_UTF8_Lengths=وسعت_Lengths;
    اگر(all) {
        وسعت_عقب_UTF8_Lengths+=3*لڑیاں_لمبائی;
    }
    برائے(;;) {
        ثابت uint8_t *s8=utf8;
        int32_t length8;
        اگر(وسعت_Condition==USET_وسعت_CONTAINED) {
            برائے(i=0; i<لڑیاں_لمبائی; ++i) {
                length8=utf8Lengths[i];
                اگر(length8==0) {
                    جاری;  // String not representable in UTF-8.
                }
                int32_t overlap=وسعت_عقب_UTF8_Lengths[i];
                اگر(overlap==ALL_CP_CONTAINED) {
                    s8+=length8;
                    جاری;  // Irrelevant string.
                }

                // Try to match this string at pos-(length8-overlap)..pos-length8.
                اگر(overlap>=LONG_وسعت) {
                    overlap=length8;
                    // While contained: No point matching fully inside the code point span.
                    int32_t len1=0;
                    U8_FWD_1(s8, len1, overlap);
                    overlap-=len1;  // Length of the string minus the first code point.
                }
                اگر(overlap>وسعت_لمبائی) {
                    overlap=وسعت_لمبائی;
                }
                int32_t dec=length8-overlap;  // Keep dec+overlap==length8.
                برائے(;;) {
                    اگر(dec>pos) {
                        توڑو;
                    }
                    // Try to match if the decrement is not listed already.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    اگر( !U8_IS_TRAIL(s[pos-dec]) &&
                        !offsets.شامل_ہے_انحراف(dec) &&
                        matches8(s+pos-dec, s8, length8)
                    ) {
                        اگر(dec==pos) {
                            واپس 0;  // Reached the start of the string.
                        }
                        offsets.جوڑو_انحراف(dec);
                    }
                    اگر(overlap==0) {
                        توڑو;
                    }
                    --overlap;
                    ++dec;
                }
                s8+=length8;
            }
        } ورنہ /* USET_SPAN_SIMPLE */ {
            int32_t maxDec=0, maxOverlap=0;
            برائے(i=0; i<لڑیاں_لمبائی; ++i) {
                length8=utf8Lengths[i];
                اگر(length8==0) {
                    جاری;  // String not representable in UTF-8.
                }
                int32_t overlap=وسعت_عقب_UTF8_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                // Try to match this string at pos-(length8-overlap)..pos-length8.
                اگر(overlap>=LONG_وسعت) {
                    overlap=length8;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the latest end.
                }
                اگر(overlap>وسعت_لمبائی) {
                    overlap=وسعت_لمبائی;
                }
                int32_t dec=length8-overlap;  // Keep dec+overlap==length8.
                برائے(;;) {
                    اگر(dec>pos || overlap<maxOverlap) {
                        توڑو;
                    }
                    // Try to match if the string is longer or ends later.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    اگر( !U8_IS_TRAIL(s[pos-dec]) &&
                        (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ dec>maxDec) &&
                        matches8(s+pos-dec, s8, length8)
                    ) {
                        maxDec=dec;  // Longest match from latest end.
                        maxOverlap=overlap;
                        توڑو;
                    }
                    --overlap;
                    ++dec;
                }
                s8+=length8;
            }

            اگر(maxDec!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue before it.
                pos-=maxDec;
                اگر(pos==0) {
                    واپس 0;  // Reached the start of the string.
                }
                وسعت_لمبائی=0;  // Match strings from before a string match.
                جاری;
            }
        }
        // Finished trying to match all strings at pos.

        اگر(وسعت_لمبائی!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            اگر(offsets.is_خالی_خانے()) {
                واپس pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } ورنہ {
            // The position is before a string match (or a single code point).
            اگر(offsets.is_خالی_خانے()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos = وسعت_سیٹ.وسعت_عقب_UTF8(reinterpret_cast<ثابت حرف*>(s), oldPos, USET_وسعت_CONTAINED);
                وسعت_لمبائی=oldPos-pos;
                اگر( pos==0 ||           // Reached the start of the string, or
                    وسعت_لمبائی==0       // neither strings nor span progressed.
                ) {
                    واپس pos;
                }
                جاری;  // spanLength>0: Match strings from before a span.
            } ورنہ {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                وسعت_لمبائی=وسعت_One_عقب_UTF8(وسعت_سیٹ, s, pos);
                اگر(وسعت_لمبائی>0) {
                    اگر(وسعت_لمبائی==pos) {
                        واپس 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=وسعت_لمبائی;
                    offsets.سرکاؤ(وسعت_لمبائی);
                    وسعت_لمبائی=0;
                    جاری;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.نکالو_Minimum();
        وسعت_لمبائی=0;  // Match strings from before a string match.
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

int32_t Unicode_سیٹ_String_وسعت::وسعت_Not(ثابت char16_t *s, int32_t length) ثابت {
    int32_t pos=0, rest=length;
    int32_t i, لڑیاں_لمبائی=لڑیاں.size();
    کرو {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i=p_وسعت_Not_سیٹ->وسعت(s+pos, rest, USET_وسعت_NOT_CONTAINED);
        اگر(i==rest) {
            واپس length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_لمبائی=وسعت_One(وسعت_سیٹ, s+pos, rest);
        اگر(cp_لمبائی>0) {
            واپس pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        برائے(i=0; i<لڑیاں_لمبائی; ++i) {
            اگر(وسعت_Lengths[i]==ALL_CP_CONTAINED) {
                جاری;  // Irrelevant string. (Also the empty string.)
            }
            ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڑیاں.elementAt(i));
            ثابت char16_t *s16=string.get_بفر();
            int32_t length16=string.length();
            U_ASSERT(length>0);
            اگر(length16<=rest && matches16CPB(s, pos, length, s16, length16)) {
                واپس pos;  // There is a set element at pos.
            }
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos-=cp_لمبائی;
        rest+=cp_لمبائی;
    } جب(rest!=0);
    واپس length;  // Reached the end of the string.
}

int32_t Unicode_سیٹ_String_وسعت::وسعت_Not_عقب(ثابت char16_t *s, int32_t length) ثابت {
    int32_t pos=length;
    int32_t i, لڑیاں_لمبائی=لڑیاں.size();
    کرو {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos=p_وسعت_Not_سیٹ->وسعت_عقب(s, pos, USET_وسعت_NOT_CONTAINED);
        اگر(pos==0) {
            واپس 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_لمبائی=وسعت_One_عقب(وسعت_سیٹ, s, pos);
        اگر(cp_لمبائی>0) {
            واپس pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        برائے(i=0; i<لڑیاں_لمبائی; ++i) {
            // Use spanLengths rather than a spanBackLengths pointer because
            // it is easier and we only need to know whether the string is irrelevant
            // which is the same in either array.
            اگر(وسعت_Lengths[i]==ALL_CP_CONTAINED) {
                جاری;  // Irrelevant string. (Also the empty string.)
            }
            ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڑیاں.elementAt(i));
            ثابت char16_t *s16=string.get_بفر();
            int32_t length16=string.length();
            U_ASSERT(length>0);
            اگر(length16<=pos && matches16CPB(s, pos-length16, length, s16, length16)) {
                واپس pos;  // There is a set element at pos.
            }
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos+=cp_لمبائی;
    } جب(pos!=0);
    واپس 0;  // Reached the start of the string.
}

int32_t Unicode_سیٹ_String_وسعت::وسعت_Not_UTF8(ثابت uint8_t *s, int32_t length) ثابت {
    int32_t pos=0, rest=length;
    int32_t i, لڑیاں_لمبائی=لڑیاں.size();
    uint8_t *وسعت_UTF8_Lengths=وسعت_Lengths;
    اگر(all) {
        وسعت_UTF8_Lengths+=2*لڑیاں_لمبائی;
    }
    کرو {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i = p_وسعت_Not_سیٹ->وسعت_UTF8(reinterpret_cast<ثابت حرف*>(s) + pos, rest, USET_وسعت_NOT_CONTAINED);
        اگر(i==rest) {
            واپس length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_لمبائی=وسعت_One_UTF8(وسعت_سیٹ, s+pos, rest);
        اگر(cp_لمبائی>0) {
            واپس pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        ثابت uint8_t *s8=utf8;
        int32_t length8;
        برائے(i=0; i<لڑیاں_لمبائی; ++i) {
            length8=utf8Lengths[i];
            // ALL_CP_CONTAINED: Irrelevant string.
            اگر(length8!=0 && وسعت_UTF8_Lengths[i]!=ALL_CP_CONTAINED && length8<=rest && matches8(s+pos, s8, length8)) {
                واپس pos;  // There is a set element at pos.
            }
            s8+=length8;
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos-=cp_لمبائی;
        rest+=cp_لمبائی;
    } جب(rest!=0);
    واپس length;  // Reached the end of the string.
}

int32_t Unicode_سیٹ_String_وسعت::وسعت_Not_عقب_UTF8(ثابت uint8_t *s, int32_t length) ثابت {
    int32_t pos=length;
    int32_t i, لڑیاں_لمبائی=لڑیاں.size();
    uint8_t *وسعت_عقب_UTF8_Lengths=وسعت_Lengths;
    اگر(all) {
        وسعت_عقب_UTF8_Lengths+=3*لڑیاں_لمبائی;
    }
    کرو {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos = p_وسعت_Not_سیٹ->وسعت_عقب_UTF8(reinterpret_cast<ثابت حرف*>(s), pos, USET_وسعت_NOT_CONTAINED);
        اگر(pos==0) {
            واپس 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_لمبائی=وسعت_One_عقب_UTF8(وسعت_سیٹ, s, pos);
        اگر(cp_لمبائی>0) {
            واپس pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        ثابت uint8_t *s8=utf8;
        int32_t length8;
        برائے(i=0; i<لڑیاں_لمبائی; ++i) {
            length8=utf8Lengths[i];
            // ALL_CP_CONTAINED: Irrelevant string.
            اگر(length8!=0 && وسعت_عقب_UTF8_Lengths[i]!=ALL_CP_CONTAINED && length8<=pos && matches8(s+pos-length8, s8, length8)) {
                واپس pos;  // There is a set element at pos.
            }
            s8+=length8;
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos+=cp_لمبائی;
    } جب(pos!=0);
    واپس 0;  // Reached the start of the string.
}

U_NAMESPACE_انتہا
