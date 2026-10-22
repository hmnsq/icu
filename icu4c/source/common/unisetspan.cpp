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
صنف وتھ_List {  // Only ever stack-allocated, does not need to inherit UMemory.
عام:
    وتھ_List() : list(staticList), capacity(0), length(0), شروع(0) {}

    ~وتھ_List() {
        إذا(list!=staticList) {
            uprv_free(list);
        }
    }

    // Call exactly once if the list is to be used.
    فراغ سیٹ_Max_لمائی(int32_t max_لمائی_2) {
        إذا (max_لمائی_2 <= static_cast<int32_t>(حجم(staticList))) {
            capacity = static_cast<int32_t>(حجم(staticList));
        } وإلا {
            UBool* l = static_cast<UBool*>(uprv_malloc(max_لمائی_2));
            إذا(l!=خالی_سوچک) {
                list=l;
                capacity=max_لمائی_2;
            }
        }
        uprv_memset(list, 0, capacity);
    }

    فراغ clear() {
        uprv_memset(list, 0, capacity);
        شروع=length=0;
    }

    UBool is_خالی_تھاواں() ثابت {
        أرجع length == 0;
    }

    // Reduce all stored offsets by delta, used when the current position
    // moves by delta.
    // There must not be any offsets lower than delta.
    // If there is an offset equal to delta, it is removed.
    // delta=[1..maxLength]
    فراغ کھسکاؤ(int32_t delta) {
        int32_t i=شروع+delta;
        إذا(i>=capacity) {
            i-=capacity;
        }
        إذا(list[i]) {
            list[i]=خطأ;
            --length;
        }
        شروع=i;
    }

    // Add an offset. The list must not contain it yet.
    // offset=[1..maxLength]
    فراغ جوڑو_وتھ(int32_t وتھ) {
        int32_t i=شروع+وتھ;
        إذا(i>=capacity) {
            i-=capacity;
        }
        list[i]=سچ;
        ++length;
    }

    // offset=[1..maxLength]
    UBool شامل_اے_وتھ(int32_t وتھ) ثابت {
        int32_t i=شروع+وتھ;
        إذا(i>=capacity) {
            i-=capacity;
        }
        أرجع list[i];
    }

    // Find the lowest stored offset from a non-empty list, remove it,
    // and reduce all other offsets by this minimum.
    // Returns [1..maxLength].
    int32_t نکالو_Minimum() {
        // Look for the next offset in list[start+1..capacity-1].
        int32_t i=شروع, نتیجہ;
        طالما(++i<capacity) {
            إذا(list[i]) {
                list[i]=خطأ;
                --length;
                نتیجہ=i-شروع;
                شروع=i;
                أرجع نتیجہ;
            }
        }
        // i==capacity

        // Wrap around and look for the next offset in list[0..start].
        // Since the list is not empty, there will be one.
        نتیجہ=capacity-شروع;
        i=0;
        طالما(!list[i]) {
            ++i;
        }
        list[i]=خطأ;
        --length;
        شروع=i;
        أرجع نتیجہ+=i;
    }

خاص:
    UBool *list;
    int32_t capacity;
    int32_t length;
    int32_t شروع;

    UBool staticList[16];
};

// Get the number of UTF-8 bytes for a UTF-16 (sub)string.
ساكن int32_t
get_UTF8_لمائی(ثابت char16_t *s, int32_t length) {
    UError_کوڈ error_کوڈ=U_ZERO_ERROR;
    int32_t length8=0;
    u_str_منزل_UTF8(خالی_سوچک, 0, &length8, s, length, &error_کوڈ);
    إذا(U_SUCCESS(error_کوڈ) || error_کوڈ==U_بفر_حد_توں_ودھ_ERROR) {
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
    UError_کوڈ error_کوڈ=U_ZERO_ERROR;
    int32_t length8=0;
    u_str_منزل_UTF8(reinterpret_cast<حرف*>(t), capacity, &length8, s, length, &error_کوڈ);
    إذا(U_SUCCESS(error_کوڈ)) {
        أرجع length8;
    } وإلا {
        // The string contains an unpaired surrogate.
        // Ignore this string.
        أرجع 0;
    }
}

ساكن مضمن uint8_t
make_وسعت_لمائی_Byte(int32_t وسعت_لمائی) {
    // 0xfe==UnicodeSetStringSpan::LONG_SPAN
    أرجع وسعت_لمائی < 0xfe ? static_cast<uint8_t>(وسعت_لمائی) : static_cast<uint8_t>(0xfe);
}

// Construct for all variants of span(), or only for any one variant.
// Initialize as little as possible, for single use.
Unicode_سیٹ_String_وسعت::Unicode_سیٹ_String_وسعت(ثابت Unicode_سیٹ &سیٹ,
                                           ثابت UVector &سیٹ_لڑیاں,
                                           uint32_t which)
        : وسعت_سیٹ(0, 0x10ffff), p_وسعت_Not_سیٹ(خالی_سوچک), لڑیاں(سیٹ_لڑیاں),
          utf8Lengths(خالی_سوچک), وسعت_Lengths(خالی_سوچک), utf8(خالی_سوچک),
          utf8_لمائی(0),
          maxLength16(0), maxLength8(0),
          all(static_cast<UBool>(which == ALL)) {
    وسعت_سیٹ.retainAll(سیٹ);
    إذا(which&NOT_CONTAINED) {
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
    int32_t لڑیاں_لمائی=لڑیاں.size();

    int32_t i, وسعت_لمائی;
    UBool someRelevant=خطأ;
    لكل(i=0; i<لڑیاں_لمائی; ++i) {
        ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڑیاں.elementAt(i));
        ثابت char16_t *s16=string.get_بفر();
        int32_t length16=string.length();
        إذا (length16==0) {
            استمر;  // skip the empty string
        }
        UBool thisRelevant;
        وسعت_لمائی=وسعت_سیٹ.وسعت(s16, length16, USET_وسعت_CONTAINED);
        إذا(وسعت_لمائی<length16) {  // Relevant string.
            someRelevant=thisRelevant=سچ;
        } وإلا {
            thisRelevant=خطأ;
        }
        إذا((which&UTF16) && length16>maxLength16) {
            maxLength16=length16;
        }
        إذا((which&UTF8) && (thisRelevant || (which&CONTAINED))) {
            int32_t length8=get_UTF8_لمائی(s16, length16);
            utf8_لمائی+=length8;
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
        وسعت_سیٹ.freeze();
    }

    uint8_t *وسعت_پٹھ_Lengths;
    uint8_t *وسعت_UTF8_Lengths;
    uint8_t *وسعت_پٹھ_UTF8_Lengths;

    // Allocate a block of meta data.
    int32_t alloc_ناپ_2;
    إذا(all) {
        // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
        alloc_ناپ_2=لڑیاں_لمائی*(4+1+1+1+1)+utf8_لمائی;
    } وإلا {
        alloc_ناپ_2=لڑیاں_لمائی;  // One set of span lengths.
        إذا(which&UTF8) {
            // UTF-8 lengths and UTF-8 strings.
            alloc_ناپ_2+=لڑیاں_لمائی*4+utf8_لمائی;
        }
    }
    إذا (alloc_ناپ_2 <= static_cast<int32_t>(حجم(staticLengths))) {
        utf8Lengths=staticLengths;
    } وإلا {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(alloc_ناپ_2));
        إذا(utf8Lengths==خالی_سوچک) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            أرجع;  // Out of memory.
        }
    }

    إذا(all) {
        // Store span lengths for all span() variants.
        وسعت_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths + لڑیاں_لمائی);
        وسعت_پٹھ_Lengths=وسعت_Lengths+لڑیاں_لمائی;
        وسعت_UTF8_Lengths=وسعت_پٹھ_Lengths+لڑیاں_لمائی;
        وسعت_پٹھ_UTF8_Lengths=وسعت_UTF8_Lengths+لڑیاں_لمائی;
        utf8=وسعت_پٹھ_UTF8_Lengths+لڑیاں_لمائی;
    } وإلا {
        // Store span lengths for only one span() variant.
        إذا(which&UTF8) {
            وسعت_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths + لڑیاں_لمائی);
            utf8=وسعت_Lengths+لڑیاں_لمائی;
        } وإلا {
            وسعت_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths);
        }
        وسعت_پٹھ_Lengths=وسعت_UTF8_Lengths=وسعت_پٹھ_UTF8_Lengths=وسعت_Lengths;
    }

    // Set the meta data and pSpanNotSet and write the UTF-8 strings.
    int32_t utf8_گنتی=0;  // Count UTF-8 bytes written so far.

    لكل(i=0; i<لڑیاں_لمائی; ++i) {
        ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڑیاں.elementAt(i));
        ثابت char16_t *s16=string.get_بفر();
        int32_t length16=string.length();
        وسعت_لمائی=وسعت_سیٹ.وسعت(s16, length16, USET_وسعت_CONTAINED);
        إذا(وسعت_لمائی<length16 && length16>0) {  // Relevant string.
            إذا(which&UTF16) {
                إذا(which&CONTAINED) {
                    إذا(which&FWD) {
                        وسعت_Lengths[i]=make_وسعت_لمائی_Byte(وسعت_لمائی);
                    }
                    إذا(which&پٹھ_2) {
                        وسعت_لمائی=length16-وسعت_سیٹ.وسعت_پٹھ(s16, length16, USET_وسعت_CONTAINED);
                        وسعت_پٹھ_Lengths[i]=make_وسعت_لمائی_Byte(وسعت_لمائی);
                    }
                } وإلا /* not CONTAINED, not all, but NOT_CONTAINED */ {
                    وسعت_Lengths[i]=وسعت_پٹھ_Lengths[i]=0;  // Only store a relevant/irrelevant flag.
                }
            }
            إذا(which&UTF8) {
                uint8_t *s8=utf8+utf8_گنتی;
                int32_t length8=appendUTF8(s16, length16, s8, utf8_لمائی-utf8_گنتی);
                utf8_گنتی+=utf8Lengths[i]=length8;
                إذا(length8==0) {  // Irrelevant for UTF-8 because not representable in UTF-8.
                    وسعت_UTF8_Lengths[i] = وسعت_پٹھ_UTF8_Lengths[i] = static_cast<uint8_t>(ALL_CP_CONTAINED);
                } وإلا {  // Relevant for UTF-8.
                    إذا(which&CONTAINED) {
                        إذا(which&FWD) {
                            وسعت_لمائی = وسعت_سیٹ.وسعت_UTF8(reinterpret_cast<ثابت حرف*>(s8), length8, USET_وسعت_CONTAINED);
                            وسعت_UTF8_Lengths[i]=make_وسعت_لمائی_Byte(وسعت_لمائی);
                        }
                        إذا(which&پٹھ_2) {
                            وسعت_لمائی = length8 - وسعت_سیٹ.وسعت_پٹھ_UTF8(reinterpret_cast<ثابت حرف*>(s8), length8, USET_وسعت_CONTAINED);
                            وسعت_پٹھ_UTF8_Lengths[i]=make_وسعت_لمائی_Byte(وسعت_لمائی);
                        }
                    } وإلا /* not CONTAINED, not all, but NOT_CONTAINED */ {
                        وسعت_UTF8_Lengths[i]=وسعت_پٹھ_UTF8_Lengths[i]=0;  // Only store a relevant/irrelevant flag.
                    }
                }
            }
            إذا(which&NOT_CONTAINED) {
                // Add string start and end code points to the spanNotSet so that
                // a span(while not contained) stops before any string.
                UChar32 c;
                إذا(which&FWD) {
                    int32_t len=0;
                    U16_اگلا(s16, len, length16, c);
                    جوڑو_منزل_وسعت_Not_سیٹ(c);
                }
                إذا(which&پٹھ_2) {
                    int32_t len=length16;
                    U16_PREV(s16, 0, len, c);
                    جوڑو_منزل_وسعت_Not_سیٹ(c);
                }
            }
        } وإلا {  // Irrelevant string. (Also the empty string.)
            إذا(which&UTF8) {
                إذا(which&CONTAINED) {  // Only necessary for LONGEST_MATCH.
                    uint8_t *s8=utf8+utf8_گنتی;
                    int32_t length8=appendUTF8(s16, length16, s8, utf8_لمائی-utf8_گنتی);
                    utf8_گنتی+=utf8Lengths[i]=length8;
                } وإلا {
                    utf8Lengths[i]=0;
                }
            }
            إذا(all) {
                وسعت_Lengths[i]=وسعت_پٹھ_Lengths[i]=
                    وسعت_UTF8_Lengths[i]=وسعت_پٹھ_UTF8_Lengths[i]=
                        static_cast<uint8_t>(ALL_CP_CONTAINED);
            } وإلا {
                // All spanXYZLengths pointers contain the same address.
                وسعت_Lengths[i] = static_cast<uint8_t>(ALL_CP_CONTAINED);
            }
        }
    }

    // Finish.
    إذا(all) {
        p_وسعت_Not_سیٹ->freeze();
    }
}

// Copy constructor. Assumes which==ALL for a frozen set.
Unicode_سیٹ_String_وسعت::Unicode_سیٹ_String_وسعت(ثابت Unicode_سیٹ_String_وسعت &other_String_وسعت,
                                           ثابت UVector &new_ماپا_سیٹ_لڑیاں)
        : وسعت_سیٹ(other_String_وسعت.وسعت_سیٹ), p_وسعت_Not_سیٹ(خالی_سوچک), لڑیاں(new_ماپا_سیٹ_لڑیاں),
          utf8Lengths(خالی_سوچک), وسعت_Lengths(خالی_سوچک), utf8(خالی_سوچک),
          utf8_لمائی(other_String_وسعت.utf8_لمائی),
          maxLength16(other_String_وسعت.maxLength16), maxLength8(other_String_وسعت.maxLength8),
          all(سچ) {
    إذا(other_String_وسعت.p_وسعت_Not_سیٹ==&other_String_وسعت.وسعت_سیٹ) {
        p_وسعت_Not_سیٹ=&وسعت_سیٹ;
    } وإلا {
        p_وسعت_Not_سیٹ=other_String_وسعت.p_وسعت_Not_سیٹ->clone();
    }

    // Allocate a block of meta data.
    // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
    int32_t لڑیاں_لمائی=لڑیاں.size();
    int32_t alloc_ناپ_2=لڑیاں_لمائی*(4+1+1+1+1)+utf8_لمائی;
    إذا (alloc_ناپ_2 <= static_cast<int32_t>(حجم(staticLengths))) {
        utf8Lengths=staticLengths;
    } وإلا {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(alloc_ناپ_2));
        إذا(utf8Lengths==خالی_سوچک) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            أرجع;  // Out of memory.
        }
    }

    وسعت_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths + لڑیاں_لمائی);
    utf8=وسعت_Lengths+لڑیاں_لمائی*4;
    uprv_memcpy(utf8Lengths, other_String_وسعت.utf8Lengths, alloc_ناپ_2);
}

Unicode_سیٹ_String_وسعت::~Unicode_سیٹ_String_وسعت() {
    إذا(p_وسعت_Not_سیٹ!=خالی_سوچک && p_وسعت_Not_سیٹ!=&وسعت_سیٹ) {
        مٹاؤ p_وسعت_Not_سیٹ;
    }
    إذا(utf8Lengths!=خالی_سوچک && utf8Lengths!=staticLengths) {
        uprv_free(utf8Lengths);
    }
}

فراغ Unicode_سیٹ_String_وسعت::جوڑو_منزل_وسعت_Not_سیٹ(UChar32 c) {
    إذا(p_وسعت_Not_سیٹ==خالی_سوچک || p_وسعت_Not_سیٹ==&وسعت_سیٹ) {
        إذا(وسعت_سیٹ.شامل_اے(c)) {
            أرجع;  // Nothing to do.
        }
        Unicode_سیٹ *new_سیٹ=وسعت_سیٹ.cloneAsThawed();
        إذا(new_سیٹ==خالی_سوچک) {
            أرجع;  // Out of memory.
        } وإلا {
            p_وسعت_Not_سیٹ=new_سیٹ;
        }
    }
    p_وسعت_Not_سیٹ->جوڑو(c);
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
matches16CPB(ثابت char16_t *s, int32_t شروع, int32_t حد_بندی, ثابت char16_t *t, int32_t length) {
    s+=شروع;
    حد_بندی-=شروع;
    أرجع matches16(s, t, length) &&
           !(0<شروع && U16_IS_LEAD(s[-1]) && U16_IS_TRAIL(s[0])) &&
           !(length<حد_بندی && U16_IS_LEAD(s[length-1]) && U16_IS_TRAIL(s[length]));
}

// Does the set contain the next code point?
// If so, return its length; otherwise return its negative length.
ساكن مضمن int32_t
وسعت_One(ثابت Unicode_سیٹ &سیٹ, ثابت char16_t *s, int32_t length) {
    char16_t c=*s, c2;
    إذا(c>=0xd800 && c<=0xdbff && length>=2 && U16_IS_TRAIL(c2=s[1])) {
        أرجع سیٹ.شامل_اے(U16_GET_SUPPLEMENTARY(c, c2)) ? 2 : -2;
    }
    أرجع سیٹ.شامل_اے(c) ? 1 : -1;
}

ساكن مضمن int32_t
وسعت_One_پٹھ(ثابت Unicode_سیٹ &سیٹ, ثابت char16_t *s, int32_t length) {
    char16_t c=s[length-1], c2;
    إذا(c>=0xdc00 && c<=0xdfff && length>=2 && U16_IS_LEAD(c2=s[length-2])) {
        أرجع سیٹ.شامل_اے(U16_GET_SUPPLEMENTARY(c2, c)) ? 2 : -2;
    }
    أرجع سیٹ.شامل_اے(c) ? 1 : -1;
}

ساكن مضمن int32_t
وسعت_One_UTF8(ثابت Unicode_سیٹ &سیٹ, ثابت uint8_t *s, int32_t length) {
    UChar32 c=*s;
    إذا(U8_IS_SINGLE(c)) {
        أرجع سیٹ.شامل_اے(c) ? 1 : -1;
    }
    // Take advantage of non-ASCII fastpaths in U8_NEXT_OR_FFFD().
    int32_t i=0;
    U8_اگلا_OR_FFFD(s, i, length, c);
    أرجع سیٹ.شامل_اے(c) ? i : -i;
}

ساكن مضمن int32_t
وسعت_One_پٹھ_UTF8(ثابت Unicode_سیٹ &سیٹ, ثابت uint8_t *s, int32_t length) {
    UChar32 c=s[length-1];
    إذا(U8_IS_SINGLE(c)) {
        أرجع سیٹ.شامل_اے(c) ? 1 : -1;
    }
    int32_t i=length-1;
    c=utf8_prev_Char_محفوظ_Body(s, 0, &i, c, -3);
    length-=i;
    أرجع سیٹ.شامل_اے(c) ? length : -length;
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
    إذا(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
        أرجع وسعت_Not(s, length);
    }
    int32_t وسعت_لمائی=وسعت_سیٹ.وسعت(s, length, USET_وسعت_CONTAINED);
    إذا(وسعت_لمائی==length) {
        أرجع length;
    }

    // Consider strings; they may overlap with the span.
    وتھ_List offsets;
    إذا(وسعت_Condition==USET_وسعت_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.سیٹ_Max_لمائی(maxLength16);
    }
    int32_t pos=وسعت_لمائی, rest=length-pos;
    int32_t i, لڑیاں_لمائی=لڑیاں.size();
    لكل(;;) {
        إذا(وسعت_Condition==USET_وسعت_CONTAINED) {
            لكل(i=0; i<لڑیاں_لمائی; ++i) {
                int32_t overlap=وسعت_Lengths[i];
                إذا(overlap==ALL_CP_CONTAINED) {
                    استمر;  // Irrelevant string. (Also the empty string.)
                }
                ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڑیاں.elementAt(i));
                ثابت char16_t *s16=string.get_بفر();
                int32_t length16=string.length();
                U_ASSERT(length>0);

                // Try to match this string at pos-overlap..pos.
                إذا(overlap>=LONG_وسعت) {
                    overlap=length16;
                    // While contained: No point matching fully inside the code point span.
                    U16_پٹھ_1(s16, 0, overlap);  // Length of the string minus the last code point.
                }
                إذا(overlap>وسعت_لمائی) {
                    overlap=وسعت_لمائی;
                }
                int32_t inc=length16-overlap;  // Keep overlap+inc==length16.
                لكل(;;) {
                    إذا(inc>rest) {
                        اكسر;
                    }
                    // Try to match if the increment is not listed already.
                    إذا(!offsets.شامل_اے_وتھ(inc) && matches16CPB(s, pos-overlap, length, s16, length16)) {
                        إذا(inc==rest) {
                            أرجع length;  // Reached the end of the string.
                        }
                        offsets.جوڑو_وتھ(inc);
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
            لكل(i=0; i<لڑیاں_لمائی; ++i) {
                int32_t overlap=وسعت_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the earliest start.

                ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڑیاں.elementAt(i));
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
                إذا(overlap>وسعت_لمائی) {
                    overlap=وسعت_لمائی;
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
                وسعت_لمائی=0;  // Match strings from after a string match.
                استمر;
            }
        }
        // Finished trying to match all strings at pos.

        إذا(وسعت_لمائی!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            إذا(offsets.is_خالی_تھاواں()) {
                أرجع pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } وإلا {
            // The position is after a string match (or a single code point).
            إذا(offsets.is_خالی_تھاواں()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                وسعت_لمائی=وسعت_سیٹ.وسعت(s+pos, rest, USET_وسعت_CONTAINED);
                إذا( وسعت_لمائی==rest || // Reached the end of the string, or
                    وسعت_لمائی==0       // neither strings nor span progressed.
                ) {
                    أرجع pos+وسعت_لمائی;
                }
                pos+=وسعت_لمائی;
                rest-=وسعت_لمائی;
                استمر;  // spanLength>0: Match strings from after a span.
            } وإلا {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                وسعت_لمائی=وسعت_One(وسعت_سیٹ, s+pos, rest);
                إذا(وسعت_لمائی>0) {
                    إذا(وسعت_لمائی==rest) {
                        أرجع length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=وسعت_لمائی;
                    rest-=وسعت_لمائی;
                    offsets.کھسکاؤ(وسعت_لمائی);
                    وسعت_لمائی=0;
                    استمر;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t min_وتھ=offsets.نکالو_Minimum();
        pos+=min_وتھ;
        rest-=min_وتھ;
        وسعت_لمائی=0;  // Match strings from after a string match.
    }
}

int32_t Unicode_سیٹ_String_وسعت::وسعت_پٹھ(ثابت char16_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    إذا(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
        أرجع وسعت_Not_پٹھ(s, length);
    }
    int32_t pos=وسعت_سیٹ.وسعت_پٹھ(s, length, USET_وسعت_CONTAINED);
    إذا(pos==0) {
        أرجع 0;
    }
    int32_t وسعت_لمائی=length-pos;

    // Consider strings; they may overlap with the span.
    وتھ_List offsets;
    إذا(وسعت_Condition==USET_وسعت_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.سیٹ_Max_لمائی(maxLength16);
    }
    int32_t i, لڑیاں_لمائی=لڑیاں.size();
    uint8_t *وسعت_پٹھ_Lengths=وسعت_Lengths;
    إذا(all) {
        وسعت_پٹھ_Lengths+=لڑیاں_لمائی;
    }
    لكل(;;) {
        إذا(وسعت_Condition==USET_وسعت_CONTAINED) {
            لكل(i=0; i<لڑیاں_لمائی; ++i) {
                int32_t overlap=وسعت_پٹھ_Lengths[i];
                إذا(overlap==ALL_CP_CONTAINED) {
                    استمر;  // Irrelevant string. (Also the empty string.)
                }
                ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڑیاں.elementAt(i));
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
                إذا(overlap>وسعت_لمائی) {
                    overlap=وسعت_لمائی;
                }
                int32_t dec=length16-overlap;  // Keep dec+overlap==length16.
                لكل(;;) {
                    إذا(dec>pos) {
                        اكسر;
                    }
                    // Try to match if the decrement is not listed already.
                    إذا(!offsets.شامل_اے_وتھ(dec) && matches16CPB(s, pos-dec, length, s16, length16)) {
                        إذا(dec==pos) {
                            أرجع 0;  // Reached the start of the string.
                        }
                        offsets.جوڑو_وتھ(dec);
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
            لكل(i=0; i<لڑیاں_لمائی; ++i) {
                int32_t overlap=وسعت_پٹھ_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڑیاں.elementAt(i));
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
                إذا(overlap>وسعت_لمائی) {
                    overlap=وسعت_لمائی;
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
                وسعت_لمائی=0;  // Match strings from before a string match.
                استمر;
            }
        }
        // Finished trying to match all strings at pos.

        إذا(وسعت_لمائی!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            إذا(offsets.is_خالی_تھاواں()) {
                أرجع pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } وإلا {
            // The position is before a string match (or a single code point).
            إذا(offsets.is_خالی_تھاواں()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos=وسعت_سیٹ.وسعت_پٹھ(s, oldPos, USET_وسعت_CONTAINED);
                وسعت_لمائی=oldPos-pos;
                إذا( pos==0 ||           // Reached the start of the string, or
                    وسعت_لمائی==0       // neither strings nor span progressed.
                ) {
                    أرجع pos;
                }
                استمر;  // spanLength>0: Match strings from before a span.
            } وإلا {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                وسعت_لمائی=وسعت_One_پٹھ(وسعت_سیٹ, s, pos);
                إذا(وسعت_لمائی>0) {
                    إذا(وسعت_لمائی==pos) {
                        أرجع 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=وسعت_لمائی;
                    offsets.کھسکاؤ(وسعت_لمائی);
                    وسعت_لمائی=0;
                    استمر;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.نکالو_Minimum();
        وسعت_لمائی=0;  // Match strings from before a string match.
    }
}

int32_t Unicode_سیٹ_String_وسعت::وسعت_UTF8(ثابت uint8_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    إذا(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
        أرجع وسعت_Not_UTF8(s, length);
    }
    int32_t وسعت_لمائی = وسعت_سیٹ.وسعت_UTF8(reinterpret_cast<ثابت حرف*>(s), length, USET_وسعت_CONTAINED);
    إذا(وسعت_لمائی==length) {
        أرجع length;
    }

    // Consider strings; they may overlap with the span.
    وتھ_List offsets;
    إذا(وسعت_Condition==USET_وسعت_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.سیٹ_Max_لمائی(maxLength8);
    }
    int32_t pos=وسعت_لمائی, rest=length-pos;
    int32_t i, لڑیاں_لمائی=لڑیاں.size();
    uint8_t *وسعت_UTF8_Lengths=وسعت_Lengths;
    إذا(all) {
        وسعت_UTF8_Lengths+=2*لڑیاں_لمائی;
    }
    لكل(;;) {
        ثابت uint8_t *s8=utf8;
        int32_t length8;
        إذا(وسعت_Condition==USET_وسعت_CONTAINED) {
            لكل(i=0; i<لڑیاں_لمائی; ++i) {
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
                    U8_پٹھ_1(s8, 0, overlap);  // Length of the string minus the last code point.
                }
                إذا(overlap>وسعت_لمائی) {
                    overlap=وسعت_لمائی;
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
                            !offsets.شامل_اے_وتھ(inc) &&
                            matches8(s+pos-overlap, s8, length8)) {
                        إذا(inc==rest) {
                            أرجع length;  // Reached the end of the string.
                        }
                        offsets.جوڑو_وتھ(inc);
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
            لكل(i=0; i<لڑیاں_لمائی; ++i) {
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
                إذا(overlap>وسعت_لمائی) {
                    overlap=وسعت_لمائی;
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
                وسعت_لمائی=0;  // Match strings from after a string match.
                استمر;
            }
        }
        // Finished trying to match all strings at pos.

        إذا(وسعت_لمائی!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            إذا(offsets.is_خالی_تھاواں()) {
                أرجع pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } وإلا {
            // The position is after a string match (or a single code point).
            إذا(offsets.is_خالی_تھاواں()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                وسعت_لمائی = وسعت_سیٹ.وسعت_UTF8(reinterpret_cast<ثابت حرف*>(s) + pos, rest, USET_وسعت_CONTAINED);
                إذا( وسعت_لمائی==rest || // Reached the end of the string, or
                    وسعت_لمائی==0       // neither strings nor span progressed.
                ) {
                    أرجع pos+وسعت_لمائی;
                }
                pos+=وسعت_لمائی;
                rest-=وسعت_لمائی;
                استمر;  // spanLength>0: Match strings from after a span.
            } وإلا {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                وسعت_لمائی=وسعت_One_UTF8(وسعت_سیٹ, s+pos, rest);
                إذا(وسعت_لمائی>0) {
                    إذا(وسعت_لمائی==rest) {
                        أرجع length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=وسعت_لمائی;
                    rest-=وسعت_لمائی;
                    offsets.کھسکاؤ(وسعت_لمائی);
                    وسعت_لمائی=0;
                    استمر;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t min_وتھ=offsets.نکالو_Minimum();
        pos+=min_وتھ;
        rest-=min_وتھ;
        وسعت_لمائی=0;  // Match strings from after a string match.
    }
}

int32_t Unicode_سیٹ_String_وسعت::وسعت_پٹھ_UTF8(ثابت uint8_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    إذا(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
        أرجع وسعت_Not_پٹھ_UTF8(s, length);
    }
    int32_t pos = وسعت_سیٹ.وسعت_پٹھ_UTF8(reinterpret_cast<ثابت حرف*>(s), length, USET_وسعت_CONTAINED);
    إذا(pos==0) {
        أرجع 0;
    }
    int32_t وسعت_لمائی=length-pos;

    // Consider strings; they may overlap with the span.
    وتھ_List offsets;
    إذا(وسعت_Condition==USET_وسعت_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.سیٹ_Max_لمائی(maxLength8);
    }
    int32_t i, لڑیاں_لمائی=لڑیاں.size();
    uint8_t *وسعت_پٹھ_UTF8_Lengths=وسعت_Lengths;
    إذا(all) {
        وسعت_پٹھ_UTF8_Lengths+=3*لڑیاں_لمائی;
    }
    لكل(;;) {
        ثابت uint8_t *s8=utf8;
        int32_t length8;
        إذا(وسعت_Condition==USET_وسعت_CONTAINED) {
            لكل(i=0; i<لڑیاں_لمائی; ++i) {
                length8=utf8Lengths[i];
                إذا(length8==0) {
                    استمر;  // String not representable in UTF-8.
                }
                int32_t overlap=وسعت_پٹھ_UTF8_Lengths[i];
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
                إذا(overlap>وسعت_لمائی) {
                    overlap=وسعت_لمائی;
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
                        !offsets.شامل_اے_وتھ(dec) &&
                        matches8(s+pos-dec, s8, length8)
                    ) {
                        إذا(dec==pos) {
                            أرجع 0;  // Reached the start of the string.
                        }
                        offsets.جوڑو_وتھ(dec);
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
            لكل(i=0; i<لڑیاں_لمائی; ++i) {
                length8=utf8Lengths[i];
                إذا(length8==0) {
                    استمر;  // String not representable in UTF-8.
                }
                int32_t overlap=وسعت_پٹھ_UTF8_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                // Try to match this string at pos-(length8-overlap)..pos-length8.
                إذا(overlap>=LONG_وسعت) {
                    overlap=length8;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the latest end.
                }
                إذا(overlap>وسعت_لمائی) {
                    overlap=وسعت_لمائی;
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
                وسعت_لمائی=0;  // Match strings from before a string match.
                استمر;
            }
        }
        // Finished trying to match all strings at pos.

        إذا(وسعت_لمائی!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            إذا(offsets.is_خالی_تھاواں()) {
                أرجع pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } وإلا {
            // The position is before a string match (or a single code point).
            إذا(offsets.is_خالی_تھاواں()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos = وسعت_سیٹ.وسعت_پٹھ_UTF8(reinterpret_cast<ثابت حرف*>(s), oldPos, USET_وسعت_CONTAINED);
                وسعت_لمائی=oldPos-pos;
                إذا( pos==0 ||           // Reached the start of the string, or
                    وسعت_لمائی==0       // neither strings nor span progressed.
                ) {
                    أرجع pos;
                }
                استمر;  // spanLength>0: Match strings from before a span.
            } وإلا {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                وسعت_لمائی=وسعت_One_پٹھ_UTF8(وسعت_سیٹ, s, pos);
                إذا(وسعت_لمائی>0) {
                    إذا(وسعت_لمائی==pos) {
                        أرجع 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=وسعت_لمائی;
                    offsets.کھسکاؤ(وسعت_لمائی);
                    وسعت_لمائی=0;
                    استمر;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.نکالو_Minimum();
        وسعت_لمائی=0;  // Match strings from before a string match.
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
    int32_t i, لڑیاں_لمائی=لڑیاں.size();
    افعل {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i=p_وسعت_Not_سیٹ->وسعت(s+pos, rest, USET_وسعت_NOT_CONTAINED);
        إذا(i==rest) {
            أرجع length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_لمائی=وسعت_One(وسعت_سیٹ, s+pos, rest);
        إذا(cp_لمائی>0) {
            أرجع pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        لكل(i=0; i<لڑیاں_لمائی; ++i) {
            إذا(وسعت_Lengths[i]==ALL_CP_CONTAINED) {
                استمر;  // Irrelevant string. (Also the empty string.)
            }
            ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڑیاں.elementAt(i));
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
        pos-=cp_لمائی;
        rest+=cp_لمائی;
    } طالما(rest!=0);
    أرجع length;  // Reached the end of the string.
}

int32_t Unicode_سیٹ_String_وسعت::وسعت_Not_پٹھ(ثابت char16_t *s, int32_t length) ثابت {
    int32_t pos=length;
    int32_t i, لڑیاں_لمائی=لڑیاں.size();
    افعل {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos=p_وسعت_Not_سیٹ->وسعت_پٹھ(s, pos, USET_وسعت_NOT_CONTAINED);
        إذا(pos==0) {
            أرجع 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_لمائی=وسعت_One_پٹھ(وسعت_سیٹ, s, pos);
        إذا(cp_لمائی>0) {
            أرجع pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        لكل(i=0; i<لڑیاں_لمائی; ++i) {
            // Use spanLengths rather than a spanBackLengths pointer because
            // it is easier and we only need to know whether the string is irrelevant
            // which is the same in either array.
            إذا(وسعت_Lengths[i]==ALL_CP_CONTAINED) {
                استمر;  // Irrelevant string. (Also the empty string.)
            }
            ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(لڑیاں.elementAt(i));
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
        pos+=cp_لمائی;
    } طالما(pos!=0);
    أرجع 0;  // Reached the start of the string.
}

int32_t Unicode_سیٹ_String_وسعت::وسعت_Not_UTF8(ثابت uint8_t *s, int32_t length) ثابت {
    int32_t pos=0, rest=length;
    int32_t i, لڑیاں_لمائی=لڑیاں.size();
    uint8_t *وسعت_UTF8_Lengths=وسعت_Lengths;
    إذا(all) {
        وسعت_UTF8_Lengths+=2*لڑیاں_لمائی;
    }
    افعل {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i = p_وسعت_Not_سیٹ->وسعت_UTF8(reinterpret_cast<ثابت حرف*>(s) + pos, rest, USET_وسعت_NOT_CONTAINED);
        إذا(i==rest) {
            أرجع length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_لمائی=وسعت_One_UTF8(وسعت_سیٹ, s+pos, rest);
        إذا(cp_لمائی>0) {
            أرجع pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        ثابت uint8_t *s8=utf8;
        int32_t length8;
        لكل(i=0; i<لڑیاں_لمائی; ++i) {
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
        pos-=cp_لمائی;
        rest+=cp_لمائی;
    } طالما(rest!=0);
    أرجع length;  // Reached the end of the string.
}

int32_t Unicode_سیٹ_String_وسعت::وسعت_Not_پٹھ_UTF8(ثابت uint8_t *s, int32_t length) ثابت {
    int32_t pos=length;
    int32_t i, لڑیاں_لمائی=لڑیاں.size();
    uint8_t *وسعت_پٹھ_UTF8_Lengths=وسعت_Lengths;
    إذا(all) {
        وسعت_پٹھ_UTF8_Lengths+=3*لڑیاں_لمائی;
    }
    افعل {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos = p_وسعت_Not_سیٹ->وسعت_پٹھ_UTF8(reinterpret_cast<ثابت حرف*>(s), pos, USET_وسعت_NOT_CONTAINED);
        إذا(pos==0) {
            أرجع 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_لمائی=وسعت_One_پٹھ_UTF8(وسعت_سیٹ, s, pos);
        إذا(cp_لمائی>0) {
            أرجع pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        ثابت uint8_t *s8=utf8;
        int32_t length8;
        لكل(i=0; i<لڑیاں_لمائی; ++i) {
            length8=utf8Lengths[i];
            // ALL_CP_CONTAINED: Irrelevant string.
            إذا(length8!=0 && وسعت_پٹھ_UTF8_Lengths[i]!=ALL_CP_CONTAINED && length8<=pos && matches8(s+pos-length8, s8, length8)) {
                أرجع pos;  // There is a set element at pos.
            }
            s8+=length8;
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos+=cp_لمائی;
    } طالما(pos!=0);
    أرجع 0;  // Reached the start of the string.
}

U_NAMESPACE_سرا
