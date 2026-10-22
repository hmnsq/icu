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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf8.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unisetspan.h"

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
ਵਰਗ ਵਿੱਥ_List {  // Only ever stack-allocated, does not need to inherit UMemory.
ਜਨਤਕ:
    ਵਿੱਥ_List() : list(staticList), capacity(0), length(0), ਸ਼ੁਰੂ(0) {}

    ~ਵਿੱਥ_List() {
        ਜੇ(list!=staticList) {
            uprv_free(list);
        }
    }

    // Call exactly once if the list is to be used.
    ਖਾਲੀ ਸੈੱਟ_Max_ਲੰਮਾਈ(int32_t max_ਲੰਮਾਈ_2) {
        ਜੇ (max_ਲੰਮਾਈ_2 <= static_cast<int32_t>(ਆਕਾਰ(staticList))) {
            capacity = static_cast<int32_t>(ਆਕਾਰ(staticList));
        } ਨਹੀਂਤਾਂ {
            UBool* l = static_cast<UBool*>(uprv_malloc(max_ਲੰਮਾਈ_2));
            ਜੇ(l!=ਖਾਲੀ_ਸੂਚਕ) {
                list=l;
                capacity=max_ਲੰਮਾਈ_2;
            }
        }
        uprv_memset(list, 0, capacity);
    }

    ਖਾਲੀ clear() {
        uprv_memset(list, 0, capacity);
        ਸ਼ੁਰੂ=length=0;
    }

    UBool is_ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ {
        ਵਾਪਸ length == 0;
    }

    // Reduce all stored offsets by delta, used when the current position
    // moves by delta.
    // There must not be any offsets lower than delta.
    // If there is an offset equal to delta, it is removed.
    // delta=[1..maxLength]
    ਖਾਲੀ ਖਿਸਕਾਓ(int32_t delta) {
        int32_t i=ਸ਼ੁਰੂ+delta;
        ਜੇ(i>=capacity) {
            i-=capacity;
        }
        ਜੇ(list[i]) {
            list[i]=ਝੂਠ;
            --length;
        }
        ਸ਼ੁਰੂ=i;
    }

    // Add an offset. The list must not contain it yet.
    // offset=[1..maxLength]
    ਖਾਲੀ ਜੋੜੋ_ਵਿੱਥ(int32_t ਵਿੱਥ) {
        int32_t i=ਸ਼ੁਰੂ+ਵਿੱਥ;
        ਜੇ(i>=capacity) {
            i-=capacity;
        }
        list[i]=ਸੱਚ;
        ++length;
    }

    // offset=[1..maxLength]
    UBool ਸ਼ਾਮਲ_ਹੈ_ਵਿੱਥ(int32_t ਵਿੱਥ) ਅਚਲ {
        int32_t i=ਸ਼ੁਰੂ+ਵਿੱਥ;
        ਜੇ(i>=capacity) {
            i-=capacity;
        }
        ਵਾਪਸ list[i];
    }

    // Find the lowest stored offset from a non-empty list, remove it,
    // and reduce all other offsets by this minimum.
    // Returns [1..maxLength].
    int32_t ਕੱਢੋ_Minimum() {
        // Look for the next offset in list[start+1..capacity-1].
        int32_t i=ਸ਼ੁਰੂ, ਨਤੀਜਾ;
        ਜਦੋਂ(++i<capacity) {
            ਜੇ(list[i]) {
                list[i]=ਝੂਠ;
                --length;
                ਨਤੀਜਾ=i-ਸ਼ੁਰੂ;
                ਸ਼ੁਰੂ=i;
                ਵਾਪਸ ਨਤੀਜਾ;
            }
        }
        // i==capacity

        // Wrap around and look for the next offset in list[0..start].
        // Since the list is not empty, there will be one.
        ਨਤੀਜਾ=capacity-ਸ਼ੁਰੂ;
        i=0;
        ਜਦੋਂ(!list[i]) {
            ++i;
        }
        list[i]=ਝੂਠ;
        --length;
        ਸ਼ੁਰੂ=i;
        ਵਾਪਸ ਨਤੀਜਾ+=i;
    }

ਨਿਜੀ:
    UBool *list;
    int32_t capacity;
    int32_t length;
    int32_t ਸ਼ੁਰੂ;

    UBool staticList[16];
};

// Get the number of UTF-8 bytes for a UTF-16 (sub)string.
ਸਥਿਰ int32_t
get_UTF8_ਲੰਮਾਈ(ਅਚਲ char16_t *s, int32_t length) {
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    int32_t length8=0;
    u_str_ਮੰਜ਼ਿਲ_UTF8(ਖਾਲੀ_ਸੂਚਕ, 0, &length8, s, length, &error_ਕੋਡ);
    ਜੇ(U_SUCCESS(error_ਕੋਡ) || error_ਕੋਡ==U_ਬਫਰ_ਉਛਾਲ_ERROR) {
        ਵਾਪਸ length8;
    } ਨਹੀਂਤਾਂ {
        // The string contains an unpaired surrogate.
        // Ignore this string.
        ਵਾਪਸ 0;
    }
}

// Append the UTF-8 version of the string to t and return the appended UTF-8 length.
ਸਥਿਰ int32_t
appendUTF8(ਅਚਲ char16_t *s, int32_t length, uint8_t *t, int32_t capacity) {
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    int32_t length8=0;
    u_str_ਮੰਜ਼ਿਲ_UTF8(reinterpret_cast<ਅੱਖਰ*>(t), capacity, &length8, s, length, &error_ਕੋਡ);
    ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
        ਵਾਪਸ length8;
    } ਨਹੀਂਤਾਂ {
        // The string contains an unpaired surrogate.
        // Ignore this string.
        ਵਾਪਸ 0;
    }
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint8_t
make_ਵਿਸਤਾਰ_ਲੰਮਾਈ_Byte(int32_t ਵਿਸਤਾਰ_ਲੰਮਾਈ) {
    // 0xfe==UnicodeSetStringSpan::LONG_SPAN
    ਵਾਪਸ ਵਿਸਤਾਰ_ਲੰਮਾਈ < 0xfe ? static_cast<uint8_t>(ਵਿਸਤਾਰ_ਲੰਮਾਈ) : static_cast<uint8_t>(0xfe);
}

// Construct for all variants of span(), or only for any one variant.
// Initialize as little as possible, for single use.
Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ(ਅਚਲ Unicode_ਸੈੱਟ &ਸੈੱਟ,
                                           ਅਚਲ UVector &ਸੈੱਟ_ਲੜੀਆਂ,
                                           uint32_t which)
        : ਵਿਸਤਾਰ_ਸੈੱਟ(0, 0x10ffff), p_ਵਿਸਤਾਰ_Not_ਸੈੱਟ(ਖਾਲੀ_ਸੂਚਕ), ਲੜੀਆਂ(ਸੈੱਟ_ਲੜੀਆਂ),
          utf8Lengths(ਖਾਲੀ_ਸੂਚਕ), ਵਿਸਤਾਰ_Lengths(ਖਾਲੀ_ਸੂਚਕ), utf8(ਖਾਲੀ_ਸੂਚਕ),
          utf8_ਲੰਮਾਈ(0),
          maxLength16(0), maxLength8(0),
          all(static_cast<UBool>(which == ALL)) {
    ਵਿਸਤਾਰ_ਸੈੱਟ.retainAll(ਸੈੱਟ);
    ਜੇ(which&NOT_CONTAINED) {
        // Default to the same sets.
        // addToSpanNotSet() will create a separate set if necessary.
        p_ਵਿਸਤਾਰ_Not_ਸੈੱਟ=&ਵਿਸਤਾਰ_ਸੈੱਟ;
    }

    // Determine if the strings even need to be taken into account at all for span() etc.
    // If any string is relevant, then all strings need to be used for
    // span(longest match) but only the relevant ones for span(while contained).
    // TODO: Possible optimization: Distinguish CONTAINED vs. LONGEST_MATCH
    //   and do not store UTF-8 strings if !thisRelevant and CONTAINED.
    //   (Only store irrelevant UTF-8 strings for LONGEST_MATCH where they are relevant after all.)
    // Also count the lengths of the UTF-8 versions of the strings for memory allocation.
    int32_t ਲੜੀਆਂ_ਲੰਮਾਈ=ਲੜੀਆਂ.size();

    int32_t i, ਵਿਸਤਾਰ_ਲੰਮਾਈ;
    UBool someRelevant=ਝੂਠ;
    ਲਈ(i=0; i<ਲੜੀਆਂ_ਲੰਮਾਈ; ++i) {
        ਅਚਲ UnicodeString& string = *static_cast<ਅਚਲ UnicodeString*>(ਲੜੀਆਂ.elementAt(i));
        ਅਚਲ char16_t *s16=string.get_ਬਫਰ();
        int32_t length16=string.length();
        ਜੇ (length16==0) {
            ਜਾਰੀ;  // skip the empty string
        }
        UBool thisRelevant;
        ਵਿਸਤਾਰ_ਲੰਮਾਈ=ਵਿਸਤਾਰ_ਸੈੱਟ.ਵਿਸਤਾਰ(s16, length16, USET_ਵਿਸਤਾਰ_CONTAINED);
        ਜੇ(ਵਿਸਤਾਰ_ਲੰਮਾਈ<length16) {  // Relevant string.
            someRelevant=thisRelevant=ਸੱਚ;
        } ਨਹੀਂਤਾਂ {
            thisRelevant=ਝੂਠ;
        }
        ਜੇ((which&UTF16) && length16>maxLength16) {
            maxLength16=length16;
        }
        ਜੇ((which&UTF8) && (thisRelevant || (which&CONTAINED))) {
            int32_t length8=get_UTF8_ਲੰਮਾਈ(s16, length16);
            utf8_ਲੰਮਾਈ+=length8;
            ਜੇ(length8>maxLength8) {
                maxLength8=length8;
            }
        }
    }
    ਜੇ(!someRelevant) {
        maxLength16=maxLength8=0;
        ਵਾਪਸ;
    }

    // Freeze after checking for the need to use strings at all because freezing
    // a set takes some time and memory which are wasted if there are no relevant strings.
    ਜੇ(all) {
        ਵਿਸਤਾਰ_ਸੈੱਟ.freeze();
    }

    uint8_t *ਵਿਸਤਾਰ_ਪਿੱਠ_Lengths;
    uint8_t *ਵਿਸਤਾਰ_UTF8_Lengths;
    uint8_t *ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8_Lengths;

    // Allocate a block of meta data.
    int32_t alloc_ਮਾਪ_2;
    ਜੇ(all) {
        // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
        alloc_ਮਾਪ_2=ਲੜੀਆਂ_ਲੰਮਾਈ*(4+1+1+1+1)+utf8_ਲੰਮਾਈ;
    } ਨਹੀਂਤਾਂ {
        alloc_ਮਾਪ_2=ਲੜੀਆਂ_ਲੰਮਾਈ;  // One set of span lengths.
        ਜੇ(which&UTF8) {
            // UTF-8 lengths and UTF-8 strings.
            alloc_ਮਾਪ_2+=ਲੜੀਆਂ_ਲੰਮਾਈ*4+utf8_ਲੰਮਾਈ;
        }
    }
    ਜੇ (alloc_ਮਾਪ_2 <= static_cast<int32_t>(ਆਕਾਰ(staticLengths))) {
        utf8Lengths=staticLengths;
    } ਨਹੀਂਤਾਂ {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(alloc_ਮਾਪ_2));
        ਜੇ(utf8Lengths==ਖਾਲੀ_ਸੂਚਕ) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            ਵਾਪਸ;  // Out of memory.
        }
    }

    ਜੇ(all) {
        // Store span lengths for all span() variants.
        ਵਿਸਤਾਰ_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths + ਲੜੀਆਂ_ਲੰਮਾਈ);
        ਵਿਸਤਾਰ_ਪਿੱਠ_Lengths=ਵਿਸਤਾਰ_Lengths+ਲੜੀਆਂ_ਲੰਮਾਈ;
        ਵਿਸਤਾਰ_UTF8_Lengths=ਵਿਸਤਾਰ_ਪਿੱਠ_Lengths+ਲੜੀਆਂ_ਲੰਮਾਈ;
        ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8_Lengths=ਵਿਸਤਾਰ_UTF8_Lengths+ਲੜੀਆਂ_ਲੰਮਾਈ;
        utf8=ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8_Lengths+ਲੜੀਆਂ_ਲੰਮਾਈ;
    } ਨਹੀਂਤਾਂ {
        // Store span lengths for only one span() variant.
        ਜੇ(which&UTF8) {
            ਵਿਸਤਾਰ_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths + ਲੜੀਆਂ_ਲੰਮਾਈ);
            utf8=ਵਿਸਤਾਰ_Lengths+ਲੜੀਆਂ_ਲੰਮਾਈ;
        } ਨਹੀਂਤਾਂ {
            ਵਿਸਤਾਰ_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths);
        }
        ਵਿਸਤਾਰ_ਪਿੱਠ_Lengths=ਵਿਸਤਾਰ_UTF8_Lengths=ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8_Lengths=ਵਿਸਤਾਰ_Lengths;
    }

    // Set the meta data and pSpanNotSet and write the UTF-8 strings.
    int32_t utf8_ਗਿਣਤੀ=0;  // Count UTF-8 bytes written so far.

    ਲਈ(i=0; i<ਲੜੀਆਂ_ਲੰਮਾਈ; ++i) {
        ਅਚਲ UnicodeString& string = *static_cast<ਅਚਲ UnicodeString*>(ਲੜੀਆਂ.elementAt(i));
        ਅਚਲ char16_t *s16=string.get_ਬਫਰ();
        int32_t length16=string.length();
        ਵਿਸਤਾਰ_ਲੰਮਾਈ=ਵਿਸਤਾਰ_ਸੈੱਟ.ਵਿਸਤਾਰ(s16, length16, USET_ਵਿਸਤਾਰ_CONTAINED);
        ਜੇ(ਵਿਸਤਾਰ_ਲੰਮਾਈ<length16 && length16>0) {  // Relevant string.
            ਜੇ(which&UTF16) {
                ਜੇ(which&CONTAINED) {
                    ਜੇ(which&FWD) {
                        ਵਿਸਤਾਰ_Lengths[i]=make_ਵਿਸਤਾਰ_ਲੰਮਾਈ_Byte(ਵਿਸਤਾਰ_ਲੰਮਾਈ);
                    }
                    ਜੇ(which&ਪਿੱਠ_2) {
                        ਵਿਸਤਾਰ_ਲੰਮਾਈ=length16-ਵਿਸਤਾਰ_ਸੈੱਟ.ਵਿਸਤਾਰ_ਪਿੱਠ(s16, length16, USET_ਵਿਸਤਾਰ_CONTAINED);
                        ਵਿਸਤਾਰ_ਪਿੱਠ_Lengths[i]=make_ਵਿਸਤਾਰ_ਲੰਮਾਈ_Byte(ਵਿਸਤਾਰ_ਲੰਮਾਈ);
                    }
                } ਨਹੀਂਤਾਂ /* not CONTAINED, not all, but NOT_CONTAINED */ {
                    ਵਿਸਤਾਰ_Lengths[i]=ਵਿਸਤਾਰ_ਪਿੱਠ_Lengths[i]=0;  // Only store a relevant/irrelevant flag.
                }
            }
            ਜੇ(which&UTF8) {
                uint8_t *s8=utf8+utf8_ਗਿਣਤੀ;
                int32_t length8=appendUTF8(s16, length16, s8, utf8_ਲੰਮਾਈ-utf8_ਗਿਣਤੀ);
                utf8_ਗਿਣਤੀ+=utf8Lengths[i]=length8;
                ਜੇ(length8==0) {  // Irrelevant for UTF-8 because not representable in UTF-8.
                    ਵਿਸਤਾਰ_UTF8_Lengths[i] = ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8_Lengths[i] = static_cast<uint8_t>(ALL_CP_CONTAINED);
                } ਨਹੀਂਤਾਂ {  // Relevant for UTF-8.
                    ਜੇ(which&CONTAINED) {
                        ਜੇ(which&FWD) {
                            ਵਿਸਤਾਰ_ਲੰਮਾਈ = ਵਿਸਤਾਰ_ਸੈੱਟ.ਵਿਸਤਾਰ_UTF8(reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(s8), length8, USET_ਵਿਸਤਾਰ_CONTAINED);
                            ਵਿਸਤਾਰ_UTF8_Lengths[i]=make_ਵਿਸਤਾਰ_ਲੰਮਾਈ_Byte(ਵਿਸਤਾਰ_ਲੰਮਾਈ);
                        }
                        ਜੇ(which&ਪਿੱਠ_2) {
                            ਵਿਸਤਾਰ_ਲੰਮਾਈ = length8 - ਵਿਸਤਾਰ_ਸੈੱਟ.ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8(reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(s8), length8, USET_ਵਿਸਤਾਰ_CONTAINED);
                            ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8_Lengths[i]=make_ਵਿਸਤਾਰ_ਲੰਮਾਈ_Byte(ਵਿਸਤਾਰ_ਲੰਮਾਈ);
                        }
                    } ਨਹੀਂਤਾਂ /* not CONTAINED, not all, but NOT_CONTAINED */ {
                        ਵਿਸਤਾਰ_UTF8_Lengths[i]=ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8_Lengths[i]=0;  // Only store a relevant/irrelevant flag.
                    }
                }
            }
            ਜੇ(which&NOT_CONTAINED) {
                // Add string start and end code points to the spanNotSet so that
                // a span(while not contained) stops before any string.
                UChar32 c;
                ਜੇ(which&FWD) {
                    int32_t len=0;
                    U16_ਅਗਲਾ(s16, len, length16, c);
                    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਵਿਸਤਾਰ_Not_ਸੈੱਟ(c);
                }
                ਜੇ(which&ਪਿੱਠ_2) {
                    int32_t len=length16;
                    U16_PREV(s16, 0, len, c);
                    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਵਿਸਤਾਰ_Not_ਸੈੱਟ(c);
                }
            }
        } ਨਹੀਂਤਾਂ {  // Irrelevant string. (Also the empty string.)
            ਜੇ(which&UTF8) {
                ਜੇ(which&CONTAINED) {  // Only necessary for LONGEST_MATCH.
                    uint8_t *s8=utf8+utf8_ਗਿਣਤੀ;
                    int32_t length8=appendUTF8(s16, length16, s8, utf8_ਲੰਮਾਈ-utf8_ਗਿਣਤੀ);
                    utf8_ਗਿਣਤੀ+=utf8Lengths[i]=length8;
                } ਨਹੀਂਤਾਂ {
                    utf8Lengths[i]=0;
                }
            }
            ਜੇ(all) {
                ਵਿਸਤਾਰ_Lengths[i]=ਵਿਸਤਾਰ_ਪਿੱਠ_Lengths[i]=
                    ਵਿਸਤਾਰ_UTF8_Lengths[i]=ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8_Lengths[i]=
                        static_cast<uint8_t>(ALL_CP_CONTAINED);
            } ਨਹੀਂਤਾਂ {
                // All spanXYZLengths pointers contain the same address.
                ਵਿਸਤਾਰ_Lengths[i] = static_cast<uint8_t>(ALL_CP_CONTAINED);
            }
        }
    }

    // Finish.
    ਜੇ(all) {
        p_ਵਿਸਤਾਰ_Not_ਸੈੱਟ->freeze();
    }
}

// Copy constructor. Assumes which==ALL for a frozen set.
Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ(ਅਚਲ Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ &other_String_ਵਿਸਤਾਰ,
                                           ਅਚਲ UVector &new_ਮਾਪਾ_ਸੈੱਟ_ਲੜੀਆਂ)
        : ਵਿਸਤਾਰ_ਸੈੱਟ(other_String_ਵਿਸਤਾਰ.ਵਿਸਤਾਰ_ਸੈੱਟ), p_ਵਿਸਤਾਰ_Not_ਸੈੱਟ(ਖਾਲੀ_ਸੂਚਕ), ਲੜੀਆਂ(new_ਮਾਪਾ_ਸੈੱਟ_ਲੜੀਆਂ),
          utf8Lengths(ਖਾਲੀ_ਸੂਚਕ), ਵਿਸਤਾਰ_Lengths(ਖਾਲੀ_ਸੂਚਕ), utf8(ਖਾਲੀ_ਸੂਚਕ),
          utf8_ਲੰਮਾਈ(other_String_ਵਿਸਤਾਰ.utf8_ਲੰਮਾਈ),
          maxLength16(other_String_ਵਿਸਤਾਰ.maxLength16), maxLength8(other_String_ਵਿਸਤਾਰ.maxLength8),
          all(ਸੱਚ) {
    ਜੇ(other_String_ਵਿਸਤਾਰ.p_ਵਿਸਤਾਰ_Not_ਸੈੱਟ==&other_String_ਵਿਸਤਾਰ.ਵਿਸਤਾਰ_ਸੈੱਟ) {
        p_ਵਿਸਤਾਰ_Not_ਸੈੱਟ=&ਵਿਸਤਾਰ_ਸੈੱਟ;
    } ਨਹੀਂਤਾਂ {
        p_ਵਿਸਤਾਰ_Not_ਸੈੱਟ=other_String_ਵਿਸਤਾਰ.p_ਵਿਸਤਾਰ_Not_ਸੈੱਟ->clone();
    }

    // Allocate a block of meta data.
    // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
    int32_t ਲੜੀਆਂ_ਲੰਮਾਈ=ਲੜੀਆਂ.size();
    int32_t alloc_ਮਾਪ_2=ਲੜੀਆਂ_ਲੰਮਾਈ*(4+1+1+1+1)+utf8_ਲੰਮਾਈ;
    ਜੇ (alloc_ਮਾਪ_2 <= static_cast<int32_t>(ਆਕਾਰ(staticLengths))) {
        utf8Lengths=staticLengths;
    } ਨਹੀਂਤਾਂ {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(alloc_ਮਾਪ_2));
        ਜੇ(utf8Lengths==ਖਾਲੀ_ਸੂਚਕ) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            ਵਾਪਸ;  // Out of memory.
        }
    }

    ਵਿਸਤਾਰ_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths + ਲੜੀਆਂ_ਲੰਮਾਈ);
    utf8=ਵਿਸਤਾਰ_Lengths+ਲੜੀਆਂ_ਲੰਮਾਈ*4;
    uprv_memcpy(utf8Lengths, other_String_ਵਿਸਤਾਰ.utf8Lengths, alloc_ਮਾਪ_2);
}

Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::~Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ() {
    ਜੇ(p_ਵਿਸਤਾਰ_Not_ਸੈੱਟ!=ਖਾਲੀ_ਸੂਚਕ && p_ਵਿਸਤਾਰ_Not_ਸੈੱਟ!=&ਵਿਸਤਾਰ_ਸੈੱਟ) {
        ਮਿਟਾਓ p_ਵਿਸਤਾਰ_Not_ਸੈੱਟ;
    }
    ਜੇ(utf8Lengths!=ਖਾਲੀ_ਸੂਚਕ && utf8Lengths!=staticLengths) {
        uprv_free(utf8Lengths);
    }
}

ਖਾਲੀ Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਵਿਸਤਾਰ_Not_ਸੈੱਟ(UChar32 c) {
    ਜੇ(p_ਵਿਸਤਾਰ_Not_ਸੈੱਟ==ਖਾਲੀ_ਸੂਚਕ || p_ਵਿਸਤਾਰ_Not_ਸੈੱਟ==&ਵਿਸਤਾਰ_ਸੈੱਟ) {
        ਜੇ(ਵਿਸਤਾਰ_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(c)) {
            ਵਾਪਸ;  // Nothing to do.
        }
        Unicode_ਸੈੱਟ *new_ਸੈੱਟ=ਵਿਸਤਾਰ_ਸੈੱਟ.cloneAsThawed();
        ਜੇ(new_ਸੈੱਟ==ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ;  // Out of memory.
        } ਨਹੀਂਤਾਂ {
            p_ਵਿਸਤਾਰ_Not_ਸੈੱਟ=new_ਸੈੱਟ;
        }
    }
    p_ਵਿਸਤਾਰ_Not_ਸੈੱਟ->ਜੋੜੋ(c);
}

// Compare strings without any argument checks. Requires length>0.
ਸਥਿਰ ਸਤਰਅੰਦਰ UBool
matches16(ਅਚਲ char16_t *s, ਅਚਲ char16_t *t, int32_t length) {
    ਕਰੋ {
        ਜੇ(*s++!=*t++) {
            ਵਾਪਸ ਝੂਠ;
        }
    } ਜਦੋਂ(--length>0);
    ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ UBool
matches8(ਅਚਲ uint8_t *s, ਅਚਲ uint8_t *t, int32_t length) {
    ਕਰੋ {
        ਜੇ(*s++!=*t++) {
            ਵਾਪਸ ਝੂਠ;
        }
    } ਜਦੋਂ(--length>0);
    ਵਾਪਸ ਸੱਚ;
}

// Compare 16-bit Unicode strings (which may be malformed UTF-16)
// at code point boundaries.
// That is, each edge of a match must not be in the middle of a surrogate pair.
ਸਥਿਰ ਸਤਰਅੰਦਰ UBool
matches16CPB(ਅਚਲ char16_t *s, int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ, ਅਚਲ char16_t *t, int32_t length) {
    s+=ਸ਼ੁਰੂ;
    ਹੱਦਬੰਦੀ-=ਸ਼ੁਰੂ;
    ਵਾਪਸ matches16(s, t, length) &&
           !(0<ਸ਼ੁਰੂ && U16_IS_LEAD(s[-1]) && U16_IS_TRAIL(s[0])) &&
           !(length<ਹੱਦਬੰਦੀ && U16_IS_LEAD(s[length-1]) && U16_IS_TRAIL(s[length]));
}

// Does the set contain the next code point?
// If so, return its length; otherwise return its negative length.
ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
ਵਿਸਤਾਰ_One(ਅਚਲ Unicode_ਸੈੱਟ &ਸੈੱਟ, ਅਚਲ char16_t *s, int32_t length) {
    char16_t c=*s, c2;
    ਜੇ(c>=0xd800 && c<=0xdbff && length>=2 && U16_IS_TRAIL(c2=s[1])) {
        ਵਾਪਸ ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(U16_GET_SUPPLEMENTARY(c, c2)) ? 2 : -2;
    }
    ਵਾਪਸ ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(c) ? 1 : -1;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
ਵਿਸਤਾਰ_One_ਪਿੱਠ(ਅਚਲ Unicode_ਸੈੱਟ &ਸੈੱਟ, ਅਚਲ char16_t *s, int32_t length) {
    char16_t c=s[length-1], c2;
    ਜੇ(c>=0xdc00 && c<=0xdfff && length>=2 && U16_IS_LEAD(c2=s[length-2])) {
        ਵਾਪਸ ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(U16_GET_SUPPLEMENTARY(c2, c)) ? 2 : -2;
    }
    ਵਾਪਸ ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(c) ? 1 : -1;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
ਵਿਸਤਾਰ_One_UTF8(ਅਚਲ Unicode_ਸੈੱਟ &ਸੈੱਟ, ਅਚਲ uint8_t *s, int32_t length) {
    UChar32 c=*s;
    ਜੇ(U8_IS_SINGLE(c)) {
        ਵਾਪਸ ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(c) ? 1 : -1;
    }
    // Take advantage of non-ASCII fastpaths in U8_NEXT_OR_FFFD().
    int32_t i=0;
    U8_ਅਗਲਾ_OR_FFFD(s, i, length, c);
    ਵਾਪਸ ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(c) ? i : -i;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
ਵਿਸਤਾਰ_One_ਪਿੱਠ_UTF8(ਅਚਲ Unicode_ਸੈੱਟ &ਸੈੱਟ, ਅਚਲ uint8_t *s, int32_t length) {
    UChar32 c=s[length-1];
    ਜੇ(U8_IS_SINGLE(c)) {
        ਵਾਪਸ ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(c) ? 1 : -1;
    }
    int32_t i=length-1;
    c=utf8_prev_Char_ਸੁਰੱਖਿਅਤ_Body(s, 0, &i, c, -3);
    length-=i;
    ਵਾਪਸ ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(c) ? length : -length;
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

int32_t Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::ਵਿਸਤਾਰ(ਅਚਲ char16_t *s, int32_t length, USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition) ਅਚਲ {
    ਜੇ(ਵਿਸਤਾਰ_Condition==USET_ਵਿਸਤਾਰ_NOT_CONTAINED) {
        ਵਾਪਸ ਵਿਸਤਾਰ_Not(s, length);
    }
    int32_t ਵਿਸਤਾਰ_ਲੰਮਾਈ=ਵਿਸਤਾਰ_ਸੈੱਟ.ਵਿਸਤਾਰ(s, length, USET_ਵਿਸਤਾਰ_CONTAINED);
    ਜੇ(ਵਿਸਤਾਰ_ਲੰਮਾਈ==length) {
        ਵਾਪਸ length;
    }

    // Consider strings; they may overlap with the span.
    ਵਿੱਥ_List offsets;
    ਜੇ(ਵਿਸਤਾਰ_Condition==USET_ਵਿਸਤਾਰ_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.ਸੈੱਟ_Max_ਲੰਮਾਈ(maxLength16);
    }
    int32_t pos=ਵਿਸਤਾਰ_ਲੰਮਾਈ, rest=length-pos;
    int32_t i, ਲੜੀਆਂ_ਲੰਮਾਈ=ਲੜੀਆਂ.size();
    ਲਈ(;;) {
        ਜੇ(ਵਿਸਤਾਰ_Condition==USET_ਵਿਸਤਾਰ_CONTAINED) {
            ਲਈ(i=0; i<ਲੜੀਆਂ_ਲੰਮਾਈ; ++i) {
                int32_t overlap=ਵਿਸਤਾਰ_Lengths[i];
                ਜੇ(overlap==ALL_CP_CONTAINED) {
                    ਜਾਰੀ;  // Irrelevant string. (Also the empty string.)
                }
                ਅਚਲ UnicodeString& string = *static_cast<ਅਚਲ UnicodeString*>(ਲੜੀਆਂ.elementAt(i));
                ਅਚਲ char16_t *s16=string.get_ਬਫਰ();
                int32_t length16=string.length();
                U_ASSERT(length>0);

                // Try to match this string at pos-overlap..pos.
                ਜੇ(overlap>=LONG_ਵਿਸਤਾਰ) {
                    overlap=length16;
                    // While contained: No point matching fully inside the code point span.
                    U16_ਪਿੱਠ_1(s16, 0, overlap);  // Length of the string minus the last code point.
                }
                ਜੇ(overlap>ਵਿਸਤਾਰ_ਲੰਮਾਈ) {
                    overlap=ਵਿਸਤਾਰ_ਲੰਮਾਈ;
                }
                int32_t inc=length16-overlap;  // Keep overlap+inc==length16.
                ਲਈ(;;) {
                    ਜੇ(inc>rest) {
                        ਰੋਕੋ;
                    }
                    // Try to match if the increment is not listed already.
                    ਜੇ(!offsets.ਸ਼ਾਮਲ_ਹੈ_ਵਿੱਥ(inc) && matches16CPB(s, pos-overlap, length, s16, length16)) {
                        ਜੇ(inc==rest) {
                            ਵਾਪਸ length;  // Reached the end of the string.
                        }
                        offsets.ਜੋੜੋ_ਵਿੱਥ(inc);
                    }
                    ਜੇ(overlap==0) {
                        ਰੋਕੋ;
                    }
                    --overlap;
                    ++inc;
                }
            }
        } ਨਹੀਂਤਾਂ /* USET_SPAN_SIMPLE */ {
            int32_t maxInc=0, maxOverlap=0;
            ਲਈ(i=0; i<ਲੜੀਆਂ_ਲੰਮਾਈ; ++i) {
                int32_t overlap=ਵਿਸਤਾਰ_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the earliest start.

                ਅਚਲ UnicodeString& string = *static_cast<ਅਚਲ UnicodeString*>(ਲੜੀਆਂ.elementAt(i));
                ਅਚਲ char16_t *s16=string.get_ਬਫਰ();
                int32_t length16=string.length();
                ਜੇ (length16==0) {
                    ਜਾਰੀ;  // skip the empty string
                }

                // Try to match this string at pos-overlap..pos.
                ਜੇ(overlap>=LONG_ਵਿਸਤਾਰ) {
                    overlap=length16;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the earliest start.
                }
                ਜੇ(overlap>ਵਿਸਤਾਰ_ਲੰਮਾਈ) {
                    overlap=ਵਿਸਤਾਰ_ਲੰਮਾਈ;
                }
                int32_t inc=length16-overlap;  // Keep overlap+inc==length16.
                ਲਈ(;;) {
                    ਜੇ(inc>rest || overlap<maxOverlap) {
                        ਰੋਕੋ;
                    }
                    // Try to match if the string is longer or starts earlier.
                    ਜੇ( (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ inc>maxInc) &&
                        matches16CPB(s, pos-overlap, length, s16, length16)
                    ) {
                        maxInc=inc;  // Longest match from earliest start.
                        maxOverlap=overlap;
                        ਰੋਕੋ;
                    }
                    --overlap;
                    ++inc;
                }
            }

            ਜੇ(maxInc!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue after it.
                pos+=maxInc;
                rest-=maxInc;
                ਜੇ(rest==0) {
                    ਵਾਪਸ length;  // Reached the end of the string.
                }
                ਵਿਸਤਾਰ_ਲੰਮਾਈ=0;  // Match strings from after a string match.
                ਜਾਰੀ;
            }
        }
        // Finished trying to match all strings at pos.

        ਜੇ(ਵਿਸਤਾਰ_ਲੰਮਾਈ!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            ਜੇ(offsets.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                ਵਾਪਸ pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } ਨਹੀਂਤਾਂ {
            // The position is after a string match (or a single code point).
            ਜੇ(offsets.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                ਵਿਸਤਾਰ_ਲੰਮਾਈ=ਵਿਸਤਾਰ_ਸੈੱਟ.ਵਿਸਤਾਰ(s+pos, rest, USET_ਵਿਸਤਾਰ_CONTAINED);
                ਜੇ( ਵਿਸਤਾਰ_ਲੰਮਾਈ==rest || // Reached the end of the string, or
                    ਵਿਸਤਾਰ_ਲੰਮਾਈ==0       // neither strings nor span progressed.
                ) {
                    ਵਾਪਸ pos+ਵਿਸਤਾਰ_ਲੰਮਾਈ;
                }
                pos+=ਵਿਸਤਾਰ_ਲੰਮਾਈ;
                rest-=ਵਿਸਤਾਰ_ਲੰਮਾਈ;
                ਜਾਰੀ;  // spanLength>0: Match strings from after a span.
            } ਨਹੀਂਤਾਂ {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                ਵਿਸਤਾਰ_ਲੰਮਾਈ=ਵਿਸਤਾਰ_One(ਵਿਸਤਾਰ_ਸੈੱਟ, s+pos, rest);
                ਜੇ(ਵਿਸਤਾਰ_ਲੰਮਾਈ>0) {
                    ਜੇ(ਵਿਸਤਾਰ_ਲੰਮਾਈ==rest) {
                        ਵਾਪਸ length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=ਵਿਸਤਾਰ_ਲੰਮਾਈ;
                    rest-=ਵਿਸਤਾਰ_ਲੰਮਾਈ;
                    offsets.ਖਿਸਕਾਓ(ਵਿਸਤਾਰ_ਲੰਮਾਈ);
                    ਵਿਸਤਾਰ_ਲੰਮਾਈ=0;
                    ਜਾਰੀ;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t min_ਵਿੱਥ=offsets.ਕੱਢੋ_Minimum();
        pos+=min_ਵਿੱਥ;
        rest-=min_ਵਿੱਥ;
        ਵਿਸਤਾਰ_ਲੰਮਾਈ=0;  // Match strings from after a string match.
    }
}

int32_t Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::ਵਿਸਤਾਰ_ਪਿੱਠ(ਅਚਲ char16_t *s, int32_t length, USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition) ਅਚਲ {
    ਜੇ(ਵਿਸਤਾਰ_Condition==USET_ਵਿਸਤਾਰ_NOT_CONTAINED) {
        ਵਾਪਸ ਵਿਸਤਾਰ_Not_ਪਿੱਠ(s, length);
    }
    int32_t pos=ਵਿਸਤਾਰ_ਸੈੱਟ.ਵਿਸਤਾਰ_ਪਿੱਠ(s, length, USET_ਵਿਸਤਾਰ_CONTAINED);
    ਜੇ(pos==0) {
        ਵਾਪਸ 0;
    }
    int32_t ਵਿਸਤਾਰ_ਲੰਮਾਈ=length-pos;

    // Consider strings; they may overlap with the span.
    ਵਿੱਥ_List offsets;
    ਜੇ(ਵਿਸਤਾਰ_Condition==USET_ਵਿਸਤਾਰ_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.ਸੈੱਟ_Max_ਲੰਮਾਈ(maxLength16);
    }
    int32_t i, ਲੜੀਆਂ_ਲੰਮਾਈ=ਲੜੀਆਂ.size();
    uint8_t *ਵਿਸਤਾਰ_ਪਿੱਠ_Lengths=ਵਿਸਤਾਰ_Lengths;
    ਜੇ(all) {
        ਵਿਸਤਾਰ_ਪਿੱਠ_Lengths+=ਲੜੀਆਂ_ਲੰਮਾਈ;
    }
    ਲਈ(;;) {
        ਜੇ(ਵਿਸਤਾਰ_Condition==USET_ਵਿਸਤਾਰ_CONTAINED) {
            ਲਈ(i=0; i<ਲੜੀਆਂ_ਲੰਮਾਈ; ++i) {
                int32_t overlap=ਵਿਸਤਾਰ_ਪਿੱਠ_Lengths[i];
                ਜੇ(overlap==ALL_CP_CONTAINED) {
                    ਜਾਰੀ;  // Irrelevant string. (Also the empty string.)
                }
                ਅਚਲ UnicodeString& string = *static_cast<ਅਚਲ UnicodeString*>(ਲੜੀਆਂ.elementAt(i));
                ਅਚਲ char16_t *s16=string.get_ਬਫਰ();
                int32_t length16=string.length();
                U_ASSERT(length>0);

                // Try to match this string at pos-(length16-overlap)..pos-length16.
                ਜੇ(overlap>=LONG_ਵਿਸਤਾਰ) {
                    overlap=length16;
                    // While contained: No point matching fully inside the code point span.
                    int32_t len1=0;
                    U16_FWD_1(s16, len1, overlap);
                    overlap-=len1;  // Length of the string minus the first code point.
                }
                ਜੇ(overlap>ਵਿਸਤਾਰ_ਲੰਮਾਈ) {
                    overlap=ਵਿਸਤਾਰ_ਲੰਮਾਈ;
                }
                int32_t dec=length16-overlap;  // Keep dec+overlap==length16.
                ਲਈ(;;) {
                    ਜੇ(dec>pos) {
                        ਰੋਕੋ;
                    }
                    // Try to match if the decrement is not listed already.
                    ਜੇ(!offsets.ਸ਼ਾਮਲ_ਹੈ_ਵਿੱਥ(dec) && matches16CPB(s, pos-dec, length, s16, length16)) {
                        ਜੇ(dec==pos) {
                            ਵਾਪਸ 0;  // Reached the start of the string.
                        }
                        offsets.ਜੋੜੋ_ਵਿੱਥ(dec);
                    }
                    ਜੇ(overlap==0) {
                        ਰੋਕੋ;
                    }
                    --overlap;
                    ++dec;
                }
            }
        } ਨਹੀਂਤਾਂ /* USET_SPAN_SIMPLE */ {
            int32_t maxDec=0, maxOverlap=0;
            ਲਈ(i=0; i<ਲੜੀਆਂ_ਲੰਮਾਈ; ++i) {
                int32_t overlap=ਵਿਸਤਾਰ_ਪਿੱਠ_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                ਅਚਲ UnicodeString& string = *static_cast<ਅਚਲ UnicodeString*>(ਲੜੀਆਂ.elementAt(i));
                ਅਚਲ char16_t *s16=string.get_ਬਫਰ();
                int32_t length16=string.length();
                ਜੇ (length16==0) {
                    ਜਾਰੀ;  // skip the empty string
                }

                // Try to match this string at pos-(length16-overlap)..pos-length16.
                ਜੇ(overlap>=LONG_ਵਿਸਤਾਰ) {
                    overlap=length16;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the latest end.
                }
                ਜੇ(overlap>ਵਿਸਤਾਰ_ਲੰਮਾਈ) {
                    overlap=ਵਿਸਤਾਰ_ਲੰਮਾਈ;
                }
                int32_t dec=length16-overlap;  // Keep dec+overlap==length16.
                ਲਈ(;;) {
                    ਜੇ(dec>pos || overlap<maxOverlap) {
                        ਰੋਕੋ;
                    }
                    // Try to match if the string is longer or ends later.
                    ਜੇ( (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ dec>maxDec) &&
                        matches16CPB(s, pos-dec, length, s16, length16)
                    ) {
                        maxDec=dec;  // Longest match from latest end.
                        maxOverlap=overlap;
                        ਰੋਕੋ;
                    }
                    --overlap;
                    ++dec;
                }
            }

            ਜੇ(maxDec!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue before it.
                pos-=maxDec;
                ਜੇ(pos==0) {
                    ਵਾਪਸ 0;  // Reached the start of the string.
                }
                ਵਿਸਤਾਰ_ਲੰਮਾਈ=0;  // Match strings from before a string match.
                ਜਾਰੀ;
            }
        }
        // Finished trying to match all strings at pos.

        ਜੇ(ਵਿਸਤਾਰ_ਲੰਮਾਈ!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            ਜੇ(offsets.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                ਵਾਪਸ pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } ਨਹੀਂਤਾਂ {
            // The position is before a string match (or a single code point).
            ਜੇ(offsets.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos=ਵਿਸਤਾਰ_ਸੈੱਟ.ਵਿਸਤਾਰ_ਪਿੱਠ(s, oldPos, USET_ਵਿਸਤਾਰ_CONTAINED);
                ਵਿਸਤਾਰ_ਲੰਮਾਈ=oldPos-pos;
                ਜੇ( pos==0 ||           // Reached the start of the string, or
                    ਵਿਸਤਾਰ_ਲੰਮਾਈ==0       // neither strings nor span progressed.
                ) {
                    ਵਾਪਸ pos;
                }
                ਜਾਰੀ;  // spanLength>0: Match strings from before a span.
            } ਨਹੀਂਤਾਂ {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                ਵਿਸਤਾਰ_ਲੰਮਾਈ=ਵਿਸਤਾਰ_One_ਪਿੱਠ(ਵਿਸਤਾਰ_ਸੈੱਟ, s, pos);
                ਜੇ(ਵਿਸਤਾਰ_ਲੰਮਾਈ>0) {
                    ਜੇ(ਵਿਸਤਾਰ_ਲੰਮਾਈ==pos) {
                        ਵਾਪਸ 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=ਵਿਸਤਾਰ_ਲੰਮਾਈ;
                    offsets.ਖਿਸਕਾਓ(ਵਿਸਤਾਰ_ਲੰਮਾਈ);
                    ਵਿਸਤਾਰ_ਲੰਮਾਈ=0;
                    ਜਾਰੀ;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.ਕੱਢੋ_Minimum();
        ਵਿਸਤਾਰ_ਲੰਮਾਈ=0;  // Match strings from before a string match.
    }
}

int32_t Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::ਵਿਸਤਾਰ_UTF8(ਅਚਲ uint8_t *s, int32_t length, USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition) ਅਚਲ {
    ਜੇ(ਵਿਸਤਾਰ_Condition==USET_ਵਿਸਤਾਰ_NOT_CONTAINED) {
        ਵਾਪਸ ਵਿਸਤਾਰ_Not_UTF8(s, length);
    }
    int32_t ਵਿਸਤਾਰ_ਲੰਮਾਈ = ਵਿਸਤਾਰ_ਸੈੱਟ.ਵਿਸਤਾਰ_UTF8(reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(s), length, USET_ਵਿਸਤਾਰ_CONTAINED);
    ਜੇ(ਵਿਸਤਾਰ_ਲੰਮਾਈ==length) {
        ਵਾਪਸ length;
    }

    // Consider strings; they may overlap with the span.
    ਵਿੱਥ_List offsets;
    ਜੇ(ਵਿਸਤਾਰ_Condition==USET_ਵਿਸਤਾਰ_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.ਸੈੱਟ_Max_ਲੰਮਾਈ(maxLength8);
    }
    int32_t pos=ਵਿਸਤਾਰ_ਲੰਮਾਈ, rest=length-pos;
    int32_t i, ਲੜੀਆਂ_ਲੰਮਾਈ=ਲੜੀਆਂ.size();
    uint8_t *ਵਿਸਤਾਰ_UTF8_Lengths=ਵਿਸਤਾਰ_Lengths;
    ਜੇ(all) {
        ਵਿਸਤਾਰ_UTF8_Lengths+=2*ਲੜੀਆਂ_ਲੰਮਾਈ;
    }
    ਲਈ(;;) {
        ਅਚਲ uint8_t *s8=utf8;
        int32_t length8;
        ਜੇ(ਵਿਸਤਾਰ_Condition==USET_ਵਿਸਤਾਰ_CONTAINED) {
            ਲਈ(i=0; i<ਲੜੀਆਂ_ਲੰਮਾਈ; ++i) {
                length8=utf8Lengths[i];
                ਜੇ(length8==0) {
                    ਜਾਰੀ;  // String not representable in UTF-8.
                }
                int32_t overlap=ਵਿਸਤਾਰ_UTF8_Lengths[i];
                ਜੇ(overlap==ALL_CP_CONTAINED) {
                    s8+=length8;
                    ਜਾਰੀ;  // Irrelevant string.
                }

                // Try to match this string at pos-overlap..pos.
                ਜੇ(overlap>=LONG_ਵਿਸਤਾਰ) {
                    overlap=length8;
                    // While contained: No point matching fully inside the code point span.
                    U8_ਪਿੱਠ_1(s8, 0, overlap);  // Length of the string minus the last code point.
                }
                ਜੇ(overlap>ਵਿਸਤਾਰ_ਲੰਮਾਈ) {
                    overlap=ਵਿਸਤਾਰ_ਲੰਮਾਈ;
                }
                int32_t inc=length8-overlap;  // Keep overlap+inc==length8.
                ਲਈ(;;) {
                    ਜੇ(inc>rest) {
                        ਰੋਕੋ;
                    }
                    // Try to match if the increment is not listed already.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    ਜੇ(!U8_IS_TRAIL(s[pos-overlap]) &&
                            !offsets.ਸ਼ਾਮਲ_ਹੈ_ਵਿੱਥ(inc) &&
                            matches8(s+pos-overlap, s8, length8)) {
                        ਜੇ(inc==rest) {
                            ਵਾਪਸ length;  // Reached the end of the string.
                        }
                        offsets.ਜੋੜੋ_ਵਿੱਥ(inc);
                    }
                    ਜੇ(overlap==0) {
                        ਰੋਕੋ;
                    }
                    --overlap;
                    ++inc;
                }
                s8+=length8;
            }
        } ਨਹੀਂਤਾਂ /* USET_SPAN_SIMPLE */ {
            int32_t maxInc=0, maxOverlap=0;
            ਲਈ(i=0; i<ਲੜੀਆਂ_ਲੰਮਾਈ; ++i) {
                length8=utf8Lengths[i];
                ਜੇ(length8==0) {
                    ਜਾਰੀ;  // String not representable in UTF-8.
                }
                int32_t overlap=ਵਿਸਤਾਰ_UTF8_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the earliest start.

                // Try to match this string at pos-overlap..pos.
                ਜੇ(overlap>=LONG_ਵਿਸਤਾਰ) {
                    overlap=length8;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the earliest start.
                }
                ਜੇ(overlap>ਵਿਸਤਾਰ_ਲੰਮਾਈ) {
                    overlap=ਵਿਸਤਾਰ_ਲੰਮਾਈ;
                }
                int32_t inc=length8-overlap;  // Keep overlap+inc==length8.
                ਲਈ(;;) {
                    ਜੇ(inc>rest || overlap<maxOverlap) {
                        ਰੋਕੋ;
                    }
                    // Try to match if the string is longer or starts earlier.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    ਜੇ(!U8_IS_TRAIL(s[pos-overlap]) &&
                            (overlap>maxOverlap ||
                                /* redundant overlap==maxOverlap && */ inc>maxInc) &&
                            matches8(s+pos-overlap, s8, length8)) {
                        maxInc=inc;  // Longest match from earliest start.
                        maxOverlap=overlap;
                        ਰੋਕੋ;
                    }
                    --overlap;
                    ++inc;
                }
                s8+=length8;
            }

            ਜੇ(maxInc!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue after it.
                pos+=maxInc;
                rest-=maxInc;
                ਜੇ(rest==0) {
                    ਵਾਪਸ length;  // Reached the end of the string.
                }
                ਵਿਸਤਾਰ_ਲੰਮਾਈ=0;  // Match strings from after a string match.
                ਜਾਰੀ;
            }
        }
        // Finished trying to match all strings at pos.

        ਜੇ(ਵਿਸਤਾਰ_ਲੰਮਾਈ!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            ਜੇ(offsets.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                ਵਾਪਸ pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } ਨਹੀਂਤਾਂ {
            // The position is after a string match (or a single code point).
            ਜੇ(offsets.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                ਵਿਸਤਾਰ_ਲੰਮਾਈ = ਵਿਸਤਾਰ_ਸੈੱਟ.ਵਿਸਤਾਰ_UTF8(reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(s) + pos, rest, USET_ਵਿਸਤਾਰ_CONTAINED);
                ਜੇ( ਵਿਸਤਾਰ_ਲੰਮਾਈ==rest || // Reached the end of the string, or
                    ਵਿਸਤਾਰ_ਲੰਮਾਈ==0       // neither strings nor span progressed.
                ) {
                    ਵਾਪਸ pos+ਵਿਸਤਾਰ_ਲੰਮਾਈ;
                }
                pos+=ਵਿਸਤਾਰ_ਲੰਮਾਈ;
                rest-=ਵਿਸਤਾਰ_ਲੰਮਾਈ;
                ਜਾਰੀ;  // spanLength>0: Match strings from after a span.
            } ਨਹੀਂਤਾਂ {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                ਵਿਸਤਾਰ_ਲੰਮਾਈ=ਵਿਸਤਾਰ_One_UTF8(ਵਿਸਤਾਰ_ਸੈੱਟ, s+pos, rest);
                ਜੇ(ਵਿਸਤਾਰ_ਲੰਮਾਈ>0) {
                    ਜੇ(ਵਿਸਤਾਰ_ਲੰਮਾਈ==rest) {
                        ਵਾਪਸ length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=ਵਿਸਤਾਰ_ਲੰਮਾਈ;
                    rest-=ਵਿਸਤਾਰ_ਲੰਮਾਈ;
                    offsets.ਖਿਸਕਾਓ(ਵਿਸਤਾਰ_ਲੰਮਾਈ);
                    ਵਿਸਤਾਰ_ਲੰਮਾਈ=0;
                    ਜਾਰੀ;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t min_ਵਿੱਥ=offsets.ਕੱਢੋ_Minimum();
        pos+=min_ਵਿੱਥ;
        rest-=min_ਵਿੱਥ;
        ਵਿਸਤਾਰ_ਲੰਮਾਈ=0;  // Match strings from after a string match.
    }
}

int32_t Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8(ਅਚਲ uint8_t *s, int32_t length, USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition) ਅਚਲ {
    ਜੇ(ਵਿਸਤਾਰ_Condition==USET_ਵਿਸਤਾਰ_NOT_CONTAINED) {
        ਵਾਪਸ ਵਿਸਤਾਰ_Not_ਪਿੱਠ_UTF8(s, length);
    }
    int32_t pos = ਵਿਸਤਾਰ_ਸੈੱਟ.ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8(reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(s), length, USET_ਵਿਸਤਾਰ_CONTAINED);
    ਜੇ(pos==0) {
        ਵਾਪਸ 0;
    }
    int32_t ਵਿਸਤਾਰ_ਲੰਮਾਈ=length-pos;

    // Consider strings; they may overlap with the span.
    ਵਿੱਥ_List offsets;
    ਜੇ(ਵਿਸਤਾਰ_Condition==USET_ਵਿਸਤਾਰ_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.ਸੈੱਟ_Max_ਲੰਮਾਈ(maxLength8);
    }
    int32_t i, ਲੜੀਆਂ_ਲੰਮਾਈ=ਲੜੀਆਂ.size();
    uint8_t *ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8_Lengths=ਵਿਸਤਾਰ_Lengths;
    ਜੇ(all) {
        ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8_Lengths+=3*ਲੜੀਆਂ_ਲੰਮਾਈ;
    }
    ਲਈ(;;) {
        ਅਚਲ uint8_t *s8=utf8;
        int32_t length8;
        ਜੇ(ਵਿਸਤਾਰ_Condition==USET_ਵਿਸਤਾਰ_CONTAINED) {
            ਲਈ(i=0; i<ਲੜੀਆਂ_ਲੰਮਾਈ; ++i) {
                length8=utf8Lengths[i];
                ਜੇ(length8==0) {
                    ਜਾਰੀ;  // String not representable in UTF-8.
                }
                int32_t overlap=ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8_Lengths[i];
                ਜੇ(overlap==ALL_CP_CONTAINED) {
                    s8+=length8;
                    ਜਾਰੀ;  // Irrelevant string.
                }

                // Try to match this string at pos-(length8-overlap)..pos-length8.
                ਜੇ(overlap>=LONG_ਵਿਸਤਾਰ) {
                    overlap=length8;
                    // While contained: No point matching fully inside the code point span.
                    int32_t len1=0;
                    U8_FWD_1(s8, len1, overlap);
                    overlap-=len1;  // Length of the string minus the first code point.
                }
                ਜੇ(overlap>ਵਿਸਤਾਰ_ਲੰਮਾਈ) {
                    overlap=ਵਿਸਤਾਰ_ਲੰਮਾਈ;
                }
                int32_t dec=length8-overlap;  // Keep dec+overlap==length8.
                ਲਈ(;;) {
                    ਜੇ(dec>pos) {
                        ਰੋਕੋ;
                    }
                    // Try to match if the decrement is not listed already.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    ਜੇ( !U8_IS_TRAIL(s[pos-dec]) &&
                        !offsets.ਸ਼ਾਮਲ_ਹੈ_ਵਿੱਥ(dec) &&
                        matches8(s+pos-dec, s8, length8)
                    ) {
                        ਜੇ(dec==pos) {
                            ਵਾਪਸ 0;  // Reached the start of the string.
                        }
                        offsets.ਜੋੜੋ_ਵਿੱਥ(dec);
                    }
                    ਜੇ(overlap==0) {
                        ਰੋਕੋ;
                    }
                    --overlap;
                    ++dec;
                }
                s8+=length8;
            }
        } ਨਹੀਂਤਾਂ /* USET_SPAN_SIMPLE */ {
            int32_t maxDec=0, maxOverlap=0;
            ਲਈ(i=0; i<ਲੜੀਆਂ_ਲੰਮਾਈ; ++i) {
                length8=utf8Lengths[i];
                ਜੇ(length8==0) {
                    ਜਾਰੀ;  // String not representable in UTF-8.
                }
                int32_t overlap=ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                // Try to match this string at pos-(length8-overlap)..pos-length8.
                ਜੇ(overlap>=LONG_ਵਿਸਤਾਰ) {
                    overlap=length8;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the latest end.
                }
                ਜੇ(overlap>ਵਿਸਤਾਰ_ਲੰਮਾਈ) {
                    overlap=ਵਿਸਤਾਰ_ਲੰਮਾਈ;
                }
                int32_t dec=length8-overlap;  // Keep dec+overlap==length8.
                ਲਈ(;;) {
                    ਜੇ(dec>pos || overlap<maxOverlap) {
                        ਰੋਕੋ;
                    }
                    // Try to match if the string is longer or ends later.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    ਜੇ( !U8_IS_TRAIL(s[pos-dec]) &&
                        (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ dec>maxDec) &&
                        matches8(s+pos-dec, s8, length8)
                    ) {
                        maxDec=dec;  // Longest match from latest end.
                        maxOverlap=overlap;
                        ਰੋਕੋ;
                    }
                    --overlap;
                    ++dec;
                }
                s8+=length8;
            }

            ਜੇ(maxDec!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue before it.
                pos-=maxDec;
                ਜੇ(pos==0) {
                    ਵਾਪਸ 0;  // Reached the start of the string.
                }
                ਵਿਸਤਾਰ_ਲੰਮਾਈ=0;  // Match strings from before a string match.
                ਜਾਰੀ;
            }
        }
        // Finished trying to match all strings at pos.

        ਜੇ(ਵਿਸਤਾਰ_ਲੰਮਾਈ!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            ਜੇ(offsets.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                ਵਾਪਸ pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } ਨਹੀਂਤਾਂ {
            // The position is before a string match (or a single code point).
            ਜੇ(offsets.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos = ਵਿਸਤਾਰ_ਸੈੱਟ.ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8(reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(s), oldPos, USET_ਵਿਸਤਾਰ_CONTAINED);
                ਵਿਸਤਾਰ_ਲੰਮਾਈ=oldPos-pos;
                ਜੇ( pos==0 ||           // Reached the start of the string, or
                    ਵਿਸਤਾਰ_ਲੰਮਾਈ==0       // neither strings nor span progressed.
                ) {
                    ਵਾਪਸ pos;
                }
                ਜਾਰੀ;  // spanLength>0: Match strings from before a span.
            } ਨਹੀਂਤਾਂ {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                ਵਿਸਤਾਰ_ਲੰਮਾਈ=ਵਿਸਤਾਰ_One_ਪਿੱਠ_UTF8(ਵਿਸਤਾਰ_ਸੈੱਟ, s, pos);
                ਜੇ(ਵਿਸਤਾਰ_ਲੰਮਾਈ>0) {
                    ਜੇ(ਵਿਸਤਾਰ_ਲੰਮਾਈ==pos) {
                        ਵਾਪਸ 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=ਵਿਸਤਾਰ_ਲੰਮਾਈ;
                    offsets.ਖਿਸਕਾਓ(ਵਿਸਤਾਰ_ਲੰਮਾਈ);
                    ਵਿਸਤਾਰ_ਲੰਮਾਈ=0;
                    ਜਾਰੀ;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.ਕੱਢੋ_Minimum();
        ਵਿਸਤਾਰ_ਲੰਮਾਈ=0;  // Match strings from before a string match.
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

int32_t Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::ਵਿਸਤਾਰ_Not(ਅਚਲ char16_t *s, int32_t length) ਅਚਲ {
    int32_t pos=0, rest=length;
    int32_t i, ਲੜੀਆਂ_ਲੰਮਾਈ=ਲੜੀਆਂ.size();
    ਕਰੋ {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i=p_ਵਿਸਤਾਰ_Not_ਸੈੱਟ->ਵਿਸਤਾਰ(s+pos, rest, USET_ਵਿਸਤਾਰ_NOT_CONTAINED);
        ਜੇ(i==rest) {
            ਵਾਪਸ length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_ਲੰਮਾਈ=ਵਿਸਤਾਰ_One(ਵਿਸਤਾਰ_ਸੈੱਟ, s+pos, rest);
        ਜੇ(cp_ਲੰਮਾਈ>0) {
            ਵਾਪਸ pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        ਲਈ(i=0; i<ਲੜੀਆਂ_ਲੰਮਾਈ; ++i) {
            ਜੇ(ਵਿਸਤਾਰ_Lengths[i]==ALL_CP_CONTAINED) {
                ਜਾਰੀ;  // Irrelevant string. (Also the empty string.)
            }
            ਅਚਲ UnicodeString& string = *static_cast<ਅਚਲ UnicodeString*>(ਲੜੀਆਂ.elementAt(i));
            ਅਚਲ char16_t *s16=string.get_ਬਫਰ();
            int32_t length16=string.length();
            U_ASSERT(length>0);
            ਜੇ(length16<=rest && matches16CPB(s, pos, length, s16, length16)) {
                ਵਾਪਸ pos;  // There is a set element at pos.
            }
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos-=cp_ਲੰਮਾਈ;
        rest+=cp_ਲੰਮਾਈ;
    } ਜਦੋਂ(rest!=0);
    ਵਾਪਸ length;  // Reached the end of the string.
}

int32_t Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::ਵਿਸਤਾਰ_Not_ਪਿੱਠ(ਅਚਲ char16_t *s, int32_t length) ਅਚਲ {
    int32_t pos=length;
    int32_t i, ਲੜੀਆਂ_ਲੰਮਾਈ=ਲੜੀਆਂ.size();
    ਕਰੋ {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos=p_ਵਿਸਤਾਰ_Not_ਸੈੱਟ->ਵਿਸਤਾਰ_ਪਿੱਠ(s, pos, USET_ਵਿਸਤਾਰ_NOT_CONTAINED);
        ਜੇ(pos==0) {
            ਵਾਪਸ 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_ਲੰਮਾਈ=ਵਿਸਤਾਰ_One_ਪਿੱਠ(ਵਿਸਤਾਰ_ਸੈੱਟ, s, pos);
        ਜੇ(cp_ਲੰਮਾਈ>0) {
            ਵਾਪਸ pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        ਲਈ(i=0; i<ਲੜੀਆਂ_ਲੰਮਾਈ; ++i) {
            // Use spanLengths rather than a spanBackLengths pointer because
            // it is easier and we only need to know whether the string is irrelevant
            // which is the same in either array.
            ਜੇ(ਵਿਸਤਾਰ_Lengths[i]==ALL_CP_CONTAINED) {
                ਜਾਰੀ;  // Irrelevant string. (Also the empty string.)
            }
            ਅਚਲ UnicodeString& string = *static_cast<ਅਚਲ UnicodeString*>(ਲੜੀਆਂ.elementAt(i));
            ਅਚਲ char16_t *s16=string.get_ਬਫਰ();
            int32_t length16=string.length();
            U_ASSERT(length>0);
            ਜੇ(length16<=pos && matches16CPB(s, pos-length16, length, s16, length16)) {
                ਵਾਪਸ pos;  // There is a set element at pos.
            }
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos+=cp_ਲੰਮਾਈ;
    } ਜਦੋਂ(pos!=0);
    ਵਾਪਸ 0;  // Reached the start of the string.
}

int32_t Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::ਵਿਸਤਾਰ_Not_UTF8(ਅਚਲ uint8_t *s, int32_t length) ਅਚਲ {
    int32_t pos=0, rest=length;
    int32_t i, ਲੜੀਆਂ_ਲੰਮਾਈ=ਲੜੀਆਂ.size();
    uint8_t *ਵਿਸਤਾਰ_UTF8_Lengths=ਵਿਸਤਾਰ_Lengths;
    ਜੇ(all) {
        ਵਿਸਤਾਰ_UTF8_Lengths+=2*ਲੜੀਆਂ_ਲੰਮਾਈ;
    }
    ਕਰੋ {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i = p_ਵਿਸਤਾਰ_Not_ਸੈੱਟ->ਵਿਸਤਾਰ_UTF8(reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(s) + pos, rest, USET_ਵਿਸਤਾਰ_NOT_CONTAINED);
        ਜੇ(i==rest) {
            ਵਾਪਸ length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_ਲੰਮਾਈ=ਵਿਸਤਾਰ_One_UTF8(ਵਿਸਤਾਰ_ਸੈੱਟ, s+pos, rest);
        ਜੇ(cp_ਲੰਮਾਈ>0) {
            ਵਾਪਸ pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        ਅਚਲ uint8_t *s8=utf8;
        int32_t length8;
        ਲਈ(i=0; i<ਲੜੀਆਂ_ਲੰਮਾਈ; ++i) {
            length8=utf8Lengths[i];
            // ALL_CP_CONTAINED: Irrelevant string.
            ਜੇ(length8!=0 && ਵਿਸਤਾਰ_UTF8_Lengths[i]!=ALL_CP_CONTAINED && length8<=rest && matches8(s+pos, s8, length8)) {
                ਵਾਪਸ pos;  // There is a set element at pos.
            }
            s8+=length8;
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos-=cp_ਲੰਮਾਈ;
        rest+=cp_ਲੰਮਾਈ;
    } ਜਦੋਂ(rest!=0);
    ਵਾਪਸ length;  // Reached the end of the string.
}

int32_t Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::ਵਿਸਤਾਰ_Not_ਪਿੱਠ_UTF8(ਅਚਲ uint8_t *s, int32_t length) ਅਚਲ {
    int32_t pos=length;
    int32_t i, ਲੜੀਆਂ_ਲੰਮਾਈ=ਲੜੀਆਂ.size();
    uint8_t *ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8_Lengths=ਵਿਸਤਾਰ_Lengths;
    ਜੇ(all) {
        ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8_Lengths+=3*ਲੜੀਆਂ_ਲੰਮਾਈ;
    }
    ਕਰੋ {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos = p_ਵਿਸਤਾਰ_Not_ਸੈੱਟ->ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8(reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(s), pos, USET_ਵਿਸਤਾਰ_NOT_CONTAINED);
        ਜੇ(pos==0) {
            ਵਾਪਸ 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_ਲੰਮਾਈ=ਵਿਸਤਾਰ_One_ਪਿੱਠ_UTF8(ਵਿਸਤਾਰ_ਸੈੱਟ, s, pos);
        ਜੇ(cp_ਲੰਮਾਈ>0) {
            ਵਾਪਸ pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        ਅਚਲ uint8_t *s8=utf8;
        int32_t length8;
        ਲਈ(i=0; i<ਲੜੀਆਂ_ਲੰਮਾਈ; ++i) {
            length8=utf8Lengths[i];
            // ALL_CP_CONTAINED: Irrelevant string.
            ਜੇ(length8!=0 && ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8_Lengths[i]!=ALL_CP_CONTAINED && length8<=pos && matches8(s+pos-length8, s8, length8)) {
                ਵਾਪਸ pos;  // There is a set element at pos.
            }
            s8+=length8;
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos+=cp_ਲੰਮਾਈ;
    } ਜਦੋਂ(pos!=0);
    ਵਾਪਸ 0;  // Reached the start of the string.
}

U_NAMESPACE_ਸਿਰਾ
