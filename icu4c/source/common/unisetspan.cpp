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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"
#ಅನ್ತರ್ಭಾವ "unisetspan.h"

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
ವರ್ಗ ಸ್ಥಾನಾಂತರ_List {  // Only ever stack-allocated, does not need to inherit UMemory.
ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಾನಾಂತರ_List() : list(staticList), capacity(0), length(0), ಆರಂಭ(0) {}

    ~ಸ್ಥಾನಾಂತರ_List() {
        ಆದರೆ(list!=staticList) {
            uprv_free(list);
        }
    }

    // Call exactly once if the list is to be used.
    ಶೂನ್ಯ ಗಣ_Max_ಉದ್ದಳತೆ(int32_t max_ಉದ್ದಳತೆ_2) {
        ಆದರೆ (max_ಉದ್ದಳತೆ_2 <= static_cast<int32_t>(ಗಾತ್ರ(staticList))) {
            capacity = static_cast<int32_t>(ಗಾತ್ರ(staticList));
        } ಇಲ್ಲದಿದ್ದರೆ {
            UBool* l = static_cast<UBool*>(uprv_malloc(max_ಉದ್ದಳತೆ_2));
            ಆದರೆ(l!=ಶೂನ್ಯಸೂಚಕ) {
                list=l;
                capacity=max_ಉದ್ದಳತೆ_2;
            }
        }
        uprv_memset(list, 0, capacity);
    }

    ಶೂನ್ಯ clear() {
        uprv_memset(list, 0, capacity);
        ಆರಂಭ=length=0;
    }

    UBool is_ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ length == 0;
    }

    // Reduce all stored offsets by delta, used when the current position
    // moves by delta.
    // There must not be any offsets lower than delta.
    // If there is an offset equal to delta, it is removed.
    // delta=[1..maxLength]
    ಶೂನ್ಯ ಸರಿಸು(int32_t delta) {
        int32_t i=ಆರಂಭ+delta;
        ಆದರೆ(i>=capacity) {
            i-=capacity;
        }
        ಆದರೆ(list[i]) {
            list[i]=ಸುಳ್ಳು;
            --length;
        }
        ಆರಂಭ=i;
    }

    // Add an offset. The list must not contain it yet.
    // offset=[1..maxLength]
    ಶೂನ್ಯ ಕೂಡಿಸು_ಸ್ಥಾನಾಂತರ(int32_t ಸ್ಥಾನಾಂತರ) {
        int32_t i=ಆರಂಭ+ಸ್ಥಾನಾಂತರ;
        ಆದರೆ(i>=capacity) {
            i-=capacity;
        }
        list[i]=ನಿಜ;
        ++length;
    }

    // offset=[1..maxLength]
    UBool ಒಳಗೊಂಡಿದೆ_ಸ್ಥಾನಾಂತರ(int32_t ಸ್ಥಾನಾಂತರ) ಸ್ಥಿರಾಂಕ {
        int32_t i=ಆರಂಭ+ಸ್ಥಾನಾಂತರ;
        ಆದರೆ(i>=capacity) {
            i-=capacity;
        }
        ಹಿಂತಿರುಗಿ list[i];
    }

    // Find the lowest stored offset from a non-empty list, remove it,
    // and reduce all other offsets by this minimum.
    // Returns [1..maxLength].
    int32_t ತೆಗೆ_Minimum() {
        // Look for the next offset in list[start+1..capacity-1].
        int32_t i=ಆರಂಭ, ಫಲಿತಾಂಶ;
        ಆಗ(++i<capacity) {
            ಆದರೆ(list[i]) {
                list[i]=ಸುಳ್ಳು;
                --length;
                ಫಲಿತಾಂಶ=i-ಆರಂಭ;
                ಆರಂಭ=i;
                ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
            }
        }
        // i==capacity

        // Wrap around and look for the next offset in list[0..start].
        // Since the list is not empty, there will be one.
        ಫಲಿತಾಂಶ=capacity-ಆರಂಭ;
        i=0;
        ಆಗ(!list[i]) {
            ++i;
        }
        list[i]=ಸುಳ್ಳು;
        --length;
        ಆರಂಭ=i;
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ+=i;
    }

ಖಾಸಗಿ:
    UBool *list;
    int32_t capacity;
    int32_t length;
    int32_t ಆರಂಭ;

    UBool staticList[16];
};

// Get the number of UTF-8 bytes for a UTF-16 (sub)string.
ಸ್ಥಿರ int32_t
get_UTF8_ಉದ್ದಳತೆ(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length) {
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    int32_t length8=0;
    u_str_ಗಮ್ಯ_UTF8(ಶೂನ್ಯಸೂಚಕ, 0, &length8, s, length, &error_ಸಂಕೇತ);
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ) || error_ಸಂಕೇತ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
        ಹಿಂತಿರುಗಿ length8;
    } ಇಲ್ಲದಿದ್ದರೆ {
        // The string contains an unpaired surrogate.
        // Ignore this string.
        ಹಿಂತಿರುಗಿ 0;
    }
}

// Append the UTF-8 version of the string to t and return the appended UTF-8 length.
ಸ್ಥಿರ int32_t
appendUTF8(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length, uint8_t *t, int32_t capacity) {
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    int32_t length8=0;
    u_str_ಗಮ್ಯ_UTF8(reinterpret_cast<ಅಕ್ಷರ*>(t), capacity, &length8, s, length, &error_ಸಂಕೇತ);
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ length8;
    } ಇಲ್ಲದಿದ್ದರೆ {
        // The string contains an unpaired surrogate.
        // Ignore this string.
        ಹಿಂತಿರುಗಿ 0;
    }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint8_t
make_ಹರವು_ಉದ್ದಳತೆ_Byte(int32_t ಹರವು_ಉದ್ದಳತೆ) {
    // 0xfe==UnicodeSetStringSpan::LONG_SPAN
    ಹಿಂತಿರುಗಿ ಹರವು_ಉದ್ದಳತೆ < 0xfe ? static_cast<uint8_t>(ಹರವು_ಉದ್ದಳತೆ) : static_cast<uint8_t>(0xfe);
}

// Construct for all variants of span(), or only for any one variant.
// Initialize as little as possible, for single use.
Unicode_ಗಣ_String_ಹರವು::Unicode_ಗಣ_String_ಹರವು(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ &ಗಣ,
                                           ಸ್ಥಿರಾಂಕ UVector &ಗಣ_ಅಕ್ಷರಸರಣಿಗಳು,
                                           uint32_t which)
        : ಹರವು_ಗಣ(0, 0x10ffff), p_ಹರವು_Not_ಗಣ(ಶೂನ್ಯಸೂಚಕ), ಅಕ್ಷರಸರಣಿಗಳು(ಗಣ_ಅಕ್ಷರಸರಣಿಗಳು),
          utf8Lengths(ಶೂನ್ಯಸೂಚಕ), ಹರವು_Lengths(ಶೂನ್ಯಸೂಚಕ), utf8(ಶೂನ್ಯಸೂಚಕ),
          utf8_ಉದ್ದಳತೆ(0),
          maxLength16(0), maxLength8(0),
          all(static_cast<UBool>(which == ALL)) {
    ಹರವು_ಗಣ.retainAll(ಗಣ);
    ಆದರೆ(which&NOT_CONTAINED) {
        // Default to the same sets.
        // addToSpanNotSet() will create a separate set if necessary.
        p_ಹರವು_Not_ಗಣ=&ಹರವು_ಗಣ;
    }

    // Determine if the strings even need to be taken into account at all for span() etc.
    // If any string is relevant, then all strings need to be used for
    // span(longest match) but only the relevant ones for span(while contained).
    // TODO: Possible optimization: Distinguish CONTAINED vs. LONGEST_MATCH
    //   and do not store UTF-8 strings if !thisRelevant and CONTAINED.
    //   (Only store irrelevant UTF-8 strings for LONGEST_MATCH where they are relevant after all.)
    // Also count the lengths of the UTF-8 versions of the strings for memory allocation.
    int32_t ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ=ಅಕ್ಷರಸರಣಿಗಳು.size();

    int32_t i, ಹರವು_ಉದ್ದಳತೆ;
    UBool someRelevant=ಸುಳ್ಳು;
    ಗಾಗಿ(i=0; i<ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ; ++i) {
        ಸ್ಥಿರಾಂಕ UnicodeString& string = *static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(ಅಕ್ಷರಸರಣಿಗಳು.elementAt(i));
        ಸ್ಥಿರಾಂಕ char16_t *s16=string.get_ಬಫರ್();
        int32_t length16=string.length();
        ಆದರೆ (length16==0) {
            ಮುಂದುವರಿ;  // skip the empty string
        }
        UBool thisRelevant;
        ಹರವು_ಉದ್ದಳತೆ=ಹರವು_ಗಣ.ಹರವು(s16, length16, USET_ಹರವು_CONTAINED);
        ಆದರೆ(ಹರವು_ಉದ್ದಳತೆ<length16) {  // Relevant string.
            someRelevant=thisRelevant=ನಿಜ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            thisRelevant=ಸುಳ್ಳು;
        }
        ಆದರೆ((which&UTF16) && length16>maxLength16) {
            maxLength16=length16;
        }
        ಆದರೆ((which&UTF8) && (thisRelevant || (which&CONTAINED))) {
            int32_t length8=get_UTF8_ಉದ್ದಳತೆ(s16, length16);
            utf8_ಉದ್ದಳತೆ+=length8;
            ಆದರೆ(length8>maxLength8) {
                maxLength8=length8;
            }
        }
    }
    ಆದರೆ(!someRelevant) {
        maxLength16=maxLength8=0;
        ಹಿಂತಿರುಗಿ;
    }

    // Freeze after checking for the need to use strings at all because freezing
    // a set takes some time and memory which are wasted if there are no relevant strings.
    ಆದರೆ(all) {
        ಹರವು_ಗಣ.freeze();
    }

    uint8_t *ಹರವು_ಹಿಂಭಾಗ_Lengths;
    uint8_t *ಹರವು_UTF8_Lengths;
    uint8_t *ಹರವು_ಹಿಂಭಾಗ_UTF8_Lengths;

    // Allocate a block of meta data.
    int32_t alloc_ಗಾತ್ರ_2;
    ಆದರೆ(all) {
        // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
        alloc_ಗಾತ್ರ_2=ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ*(4+1+1+1+1)+utf8_ಉದ್ದಳತೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        alloc_ಗಾತ್ರ_2=ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ;  // One set of span lengths.
        ಆದರೆ(which&UTF8) {
            // UTF-8 lengths and UTF-8 strings.
            alloc_ಗಾತ್ರ_2+=ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ*4+utf8_ಉದ್ದಳತೆ;
        }
    }
    ಆದರೆ (alloc_ಗಾತ್ರ_2 <= static_cast<int32_t>(ಗಾತ್ರ(staticLengths))) {
        utf8Lengths=staticLengths;
    } ಇಲ್ಲದಿದ್ದರೆ {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(alloc_ಗಾತ್ರ_2));
        ಆದರೆ(utf8Lengths==ಶೂನ್ಯಸೂಚಕ) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            ಹಿಂತಿರುಗಿ;  // Out of memory.
        }
    }

    ಆದರೆ(all) {
        // Store span lengths for all span() variants.
        ಹರವು_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths + ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ);
        ಹರವು_ಹಿಂಭಾಗ_Lengths=ಹರವು_Lengths+ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ;
        ಹರವು_UTF8_Lengths=ಹರವು_ಹಿಂಭಾಗ_Lengths+ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ;
        ಹರವು_ಹಿಂಭಾಗ_UTF8_Lengths=ಹರವು_UTF8_Lengths+ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ;
        utf8=ಹರವು_ಹಿಂಭಾಗ_UTF8_Lengths+ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Store span lengths for only one span() variant.
        ಆದರೆ(which&UTF8) {
            ಹರವು_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths + ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ);
            utf8=ಹರವು_Lengths+ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹರವು_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths);
        }
        ಹರವು_ಹಿಂಭಾಗ_Lengths=ಹರವು_UTF8_Lengths=ಹರವು_ಹಿಂಭಾಗ_UTF8_Lengths=ಹರವು_Lengths;
    }

    // Set the meta data and pSpanNotSet and write the UTF-8 strings.
    int32_t utf8_ಎಣಿಕೆ=0;  // Count UTF-8 bytes written so far.

    ಗಾಗಿ(i=0; i<ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ; ++i) {
        ಸ್ಥಿರಾಂಕ UnicodeString& string = *static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(ಅಕ್ಷರಸರಣಿಗಳು.elementAt(i));
        ಸ್ಥಿರಾಂಕ char16_t *s16=string.get_ಬಫರ್();
        int32_t length16=string.length();
        ಹರವು_ಉದ್ದಳತೆ=ಹರವು_ಗಣ.ಹರವು(s16, length16, USET_ಹರವು_CONTAINED);
        ಆದರೆ(ಹರವು_ಉದ್ದಳತೆ<length16 && length16>0) {  // Relevant string.
            ಆದರೆ(which&UTF16) {
                ಆದರೆ(which&CONTAINED) {
                    ಆದರೆ(which&FWD) {
                        ಹರವು_Lengths[i]=make_ಹರವು_ಉದ್ದಳತೆ_Byte(ಹರವು_ಉದ್ದಳತೆ);
                    }
                    ಆದರೆ(which&ಹಿಂಭಾಗ_2) {
                        ಹರವು_ಉದ್ದಳತೆ=length16-ಹರವು_ಗಣ.ಹರವು_ಹಿಂಭಾಗ(s16, length16, USET_ಹರವು_CONTAINED);
                        ಹರವು_ಹಿಂಭಾಗ_Lengths[i]=make_ಹರವು_ಉದ್ದಳತೆ_Byte(ಹರವು_ಉದ್ದಳತೆ);
                    }
                } ಇಲ್ಲದಿದ್ದರೆ /* not CONTAINED, not all, but NOT_CONTAINED */ {
                    ಹರವು_Lengths[i]=ಹರವು_ಹಿಂಭಾಗ_Lengths[i]=0;  // Only store a relevant/irrelevant flag.
                }
            }
            ಆದರೆ(which&UTF8) {
                uint8_t *s8=utf8+utf8_ಎಣಿಕೆ;
                int32_t length8=appendUTF8(s16, length16, s8, utf8_ಉದ್ದಳತೆ-utf8_ಎಣಿಕೆ);
                utf8_ಎಣಿಕೆ+=utf8Lengths[i]=length8;
                ಆದರೆ(length8==0) {  // Irrelevant for UTF-8 because not representable in UTF-8.
                    ಹರವು_UTF8_Lengths[i] = ಹರವು_ಹಿಂಭಾಗ_UTF8_Lengths[i] = static_cast<uint8_t>(ALL_CP_CONTAINED);
                } ಇಲ್ಲದಿದ್ದರೆ {  // Relevant for UTF-8.
                    ಆದರೆ(which&CONTAINED) {
                        ಆದರೆ(which&FWD) {
                            ಹರವು_ಉದ್ದಳತೆ = ಹರವು_ಗಣ.ಹರವು_UTF8(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(s8), length8, USET_ಹರವು_CONTAINED);
                            ಹರವು_UTF8_Lengths[i]=make_ಹರವು_ಉದ್ದಳತೆ_Byte(ಹರವು_ಉದ್ದಳತೆ);
                        }
                        ಆದರೆ(which&ಹಿಂಭಾಗ_2) {
                            ಹರವು_ಉದ್ದಳತೆ = length8 - ಹರವು_ಗಣ.ಹರವು_ಹಿಂಭಾಗ_UTF8(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(s8), length8, USET_ಹರವು_CONTAINED);
                            ಹರವು_ಹಿಂಭಾಗ_UTF8_Lengths[i]=make_ಹರವು_ಉದ್ದಳತೆ_Byte(ಹರವು_ಉದ್ದಳತೆ);
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ /* not CONTAINED, not all, but NOT_CONTAINED */ {
                        ಹರವು_UTF8_Lengths[i]=ಹರವು_ಹಿಂಭಾಗ_UTF8_Lengths[i]=0;  // Only store a relevant/irrelevant flag.
                    }
                }
            }
            ಆದರೆ(which&NOT_CONTAINED) {
                // Add string start and end code points to the spanNotSet so that
                // a span(while not contained) stops before any string.
                UChar32 c;
                ಆದರೆ(which&FWD) {
                    int32_t len=0;
                    U16_ಮುಂದಿನ(s16, len, length16, c);
                    ಕೂಡಿಸು_ಗಮ್ಯ_ಹರವು_Not_ಗಣ(c);
                }
                ಆದರೆ(which&ಹಿಂಭಾಗ_2) {
                    int32_t len=length16;
                    U16_PREV(s16, 0, len, c);
                    ಕೂಡಿಸು_ಗಮ್ಯ_ಹರವು_Not_ಗಣ(c);
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ {  // Irrelevant string. (Also the empty string.)
            ಆದರೆ(which&UTF8) {
                ಆದರೆ(which&CONTAINED) {  // Only necessary for LONGEST_MATCH.
                    uint8_t *s8=utf8+utf8_ಎಣಿಕೆ;
                    int32_t length8=appendUTF8(s16, length16, s8, utf8_ಉದ್ದಳತೆ-utf8_ಎಣಿಕೆ);
                    utf8_ಎಣಿಕೆ+=utf8Lengths[i]=length8;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    utf8Lengths[i]=0;
                }
            }
            ಆದರೆ(all) {
                ಹರವು_Lengths[i]=ಹರವು_ಹಿಂಭಾಗ_Lengths[i]=
                    ಹರವು_UTF8_Lengths[i]=ಹರವು_ಹಿಂಭಾಗ_UTF8_Lengths[i]=
                        static_cast<uint8_t>(ALL_CP_CONTAINED);
            } ಇಲ್ಲದಿದ್ದರೆ {
                // All spanXYZLengths pointers contain the same address.
                ಹರವು_Lengths[i] = static_cast<uint8_t>(ALL_CP_CONTAINED);
            }
        }
    }

    // Finish.
    ಆದರೆ(all) {
        p_ಹರವು_Not_ಗಣ->freeze();
    }
}

// Copy constructor. Assumes which==ALL for a frozen set.
Unicode_ಗಣ_String_ಹರವು::Unicode_ಗಣ_String_ಹರವು(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ_String_ಹರವು &other_String_ಹರವು,
                                           ಸ್ಥಿರಾಂಕ UVector &new_ಪೋಷಕ_ಗಣ_ಅಕ್ಷರಸರಣಿಗಳು)
        : ಹರವು_ಗಣ(other_String_ಹರವು.ಹರವು_ಗಣ), p_ಹರವು_Not_ಗಣ(ಶೂನ್ಯಸೂಚಕ), ಅಕ್ಷರಸರಣಿಗಳು(new_ಪೋಷಕ_ಗಣ_ಅಕ್ಷರಸರಣಿಗಳು),
          utf8Lengths(ಶೂನ್ಯಸೂಚಕ), ಹರವು_Lengths(ಶೂನ್ಯಸೂಚಕ), utf8(ಶೂನ್ಯಸೂಚಕ),
          utf8_ಉದ್ದಳತೆ(other_String_ಹರವು.utf8_ಉದ್ದಳತೆ),
          maxLength16(other_String_ಹರವು.maxLength16), maxLength8(other_String_ಹರವು.maxLength8),
          all(ನಿಜ) {
    ಆದರೆ(other_String_ಹರವು.p_ಹರವು_Not_ಗಣ==&other_String_ಹರವು.ಹರವು_ಗಣ) {
        p_ಹರವು_Not_ಗಣ=&ಹರವು_ಗಣ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        p_ಹರವು_Not_ಗಣ=other_String_ಹರವು.p_ಹರವು_Not_ಗಣ->clone();
    }

    // Allocate a block of meta data.
    // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
    int32_t ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ=ಅಕ್ಷರಸರಣಿಗಳು.size();
    int32_t alloc_ಗಾತ್ರ_2=ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ*(4+1+1+1+1)+utf8_ಉದ್ದಳತೆ;
    ಆದರೆ (alloc_ಗಾತ್ರ_2 <= static_cast<int32_t>(ಗಾತ್ರ(staticLengths))) {
        utf8Lengths=staticLengths;
    } ಇಲ್ಲದಿದ್ದರೆ {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(alloc_ಗಾತ್ರ_2));
        ಆದರೆ(utf8Lengths==ಶೂನ್ಯಸೂಚಕ) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            ಹಿಂತಿರುಗಿ;  // Out of memory.
        }
    }

    ಹರವು_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths + ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ);
    utf8=ಹರವು_Lengths+ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ*4;
    uprv_memcpy(utf8Lengths, other_String_ಹರವು.utf8Lengths, alloc_ಗಾತ್ರ_2);
}

Unicode_ಗಣ_String_ಹರವು::~Unicode_ಗಣ_String_ಹರವು() {
    ಆದರೆ(p_ಹರವು_Not_ಗಣ!=ಶೂನ್ಯಸೂಚಕ && p_ಹರವು_Not_ಗಣ!=&ಹರವು_ಗಣ) {
        ಅಳಿಸು p_ಹರವು_Not_ಗಣ;
    }
    ಆದರೆ(utf8Lengths!=ಶೂನ್ಯಸೂಚಕ && utf8Lengths!=staticLengths) {
        uprv_free(utf8Lengths);
    }
}

ಶೂನ್ಯ Unicode_ಗಣ_String_ಹರವು::ಕೂಡಿಸು_ಗಮ್ಯ_ಹರವು_Not_ಗಣ(UChar32 c) {
    ಆದರೆ(p_ಹರವು_Not_ಗಣ==ಶೂನ್ಯಸೂಚಕ || p_ಹರವು_Not_ಗಣ==&ಹರವು_ಗಣ) {
        ಆದರೆ(ಹರವು_ಗಣ.ಒಳಗೊಂಡಿದೆ(c)) {
            ಹಿಂತಿರುಗಿ;  // Nothing to do.
        }
        Unicode_ಗಣ *new_ಗಣ=ಹರವು_ಗಣ.cloneAsThawed();
        ಆದರೆ(new_ಗಣ==ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ;  // Out of memory.
        } ಇಲ್ಲದಿದ್ದರೆ {
            p_ಹರವು_Not_ಗಣ=new_ಗಣ;
        }
    }
    p_ಹರವು_Not_ಗಣ->ಕೂಡಿಸು(c);
}

// Compare strings without any argument checks. Requires length>0.
ಸ್ಥಿರ ಸಾಲೊಳಗೆ UBool
matches16(ಸ್ಥಿರಾಂಕ char16_t *s, ಸ್ಥಿರಾಂಕ char16_t *t, int32_t length) {
    ಮಾಡು {
        ಆದರೆ(*s++!=*t++) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    } ಆಗ(--length>0);
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ UBool
matches8(ಸ್ಥಿರಾಂಕ uint8_t *s, ಸ್ಥಿರಾಂಕ uint8_t *t, int32_t length) {
    ಮಾಡು {
        ಆದರೆ(*s++!=*t++) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    } ಆಗ(--length>0);
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

// Compare 16-bit Unicode strings (which may be malformed UTF-16)
// at code point boundaries.
// That is, each edge of a match must not be in the middle of a surrogate pair.
ಸ್ಥಿರ ಸಾಲೊಳಗೆ UBool
matches16CPB(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ, ಸ್ಥಿರಾಂಕ char16_t *t, int32_t length) {
    s+=ಆರಂಭ;
    ಎಲ್ಲೆ-=ಆರಂಭ;
    ಹಿಂತಿರುಗಿ matches16(s, t, length) &&
           !(0<ಆರಂಭ && U16_IS_LEAD(s[-1]) && U16_IS_TRAIL(s[0])) &&
           !(length<ಎಲ್ಲೆ && U16_IS_LEAD(s[length-1]) && U16_IS_TRAIL(s[length]));
}

// Does the set contain the next code point?
// If so, return its length; otherwise return its negative length.
ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
ಹರವು_One(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ &ಗಣ, ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length) {
    char16_t c=*s, c2;
    ಆದರೆ(c>=0xd800 && c<=0xdbff && length>=2 && U16_IS_TRAIL(c2=s[1])) {
        ಹಿಂತಿರುಗಿ ಗಣ.ಒಳಗೊಂಡಿದೆ(U16_GET_SUPPLEMENTARY(c, c2)) ? 2 : -2;
    }
    ಹಿಂತಿರುಗಿ ಗಣ.ಒಳಗೊಂಡಿದೆ(c) ? 1 : -1;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
ಹರವು_One_ಹಿಂಭಾಗ(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ &ಗಣ, ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length) {
    char16_t c=s[length-1], c2;
    ಆದರೆ(c>=0xdc00 && c<=0xdfff && length>=2 && U16_IS_LEAD(c2=s[length-2])) {
        ಹಿಂತಿರುಗಿ ಗಣ.ಒಳಗೊಂಡಿದೆ(U16_GET_SUPPLEMENTARY(c2, c)) ? 2 : -2;
    }
    ಹಿಂತಿರುಗಿ ಗಣ.ಒಳಗೊಂಡಿದೆ(c) ? 1 : -1;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
ಹರವು_One_UTF8(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ &ಗಣ, ಸ್ಥಿರಾಂಕ uint8_t *s, int32_t length) {
    UChar32 c=*s;
    ಆದರೆ(U8_IS_SINGLE(c)) {
        ಹಿಂತಿರುಗಿ ಗಣ.ಒಳಗೊಂಡಿದೆ(c) ? 1 : -1;
    }
    // Take advantage of non-ASCII fastpaths in U8_NEXT_OR_FFFD().
    int32_t i=0;
    U8_ಮುಂದಿನ_OR_FFFD(s, i, length, c);
    ಹಿಂತಿರುಗಿ ಗಣ.ಒಳಗೊಂಡಿದೆ(c) ? i : -i;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
ಹರವು_One_ಹಿಂಭಾಗ_UTF8(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ &ಗಣ, ಸ್ಥಿರಾಂಕ uint8_t *s, int32_t length) {
    UChar32 c=s[length-1];
    ಆದರೆ(U8_IS_SINGLE(c)) {
        ಹಿಂತಿರುಗಿ ಗಣ.ಒಳಗೊಂಡಿದೆ(c) ? 1 : -1;
    }
    int32_t i=length-1;
    c=utf8_prev_Char_ಸುರಕ್ಷಿತ_Body(s, 0, &i, c, -3);
    length-=i;
    ಹಿಂತಿರುಗಿ ಗಣ.ಒಳಗೊಂಡಿದೆ(c) ? length : -length;
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

int32_t Unicode_ಗಣ_String_ಹರವು::ಹರವು(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length, USet_ಹರವು_Condition ಹರವು_Condition) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(ಹರವು_Condition==USET_ಹರವು_NOT_CONTAINED) {
        ಹಿಂತಿರುಗಿ ಹರವು_Not(s, length);
    }
    int32_t ಹರವು_ಉದ್ದಳತೆ=ಹರವು_ಗಣ.ಹರವು(s, length, USET_ಹರವು_CONTAINED);
    ಆದರೆ(ಹರವು_ಉದ್ದಳತೆ==length) {
        ಹಿಂತಿರುಗಿ length;
    }

    // Consider strings; they may overlap with the span.
    ಸ್ಥಾನಾಂತರ_List offsets;
    ಆದರೆ(ಹರವು_Condition==USET_ಹರವು_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.ಗಣ_Max_ಉದ್ದಳತೆ(maxLength16);
    }
    int32_t pos=ಹರವು_ಉದ್ದಳತೆ, rest=length-pos;
    int32_t i, ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ=ಅಕ್ಷರಸರಣಿಗಳು.size();
    ಗಾಗಿ(;;) {
        ಆದರೆ(ಹರವು_Condition==USET_ಹರವು_CONTAINED) {
            ಗಾಗಿ(i=0; i<ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ; ++i) {
                int32_t overlap=ಹರವು_Lengths[i];
                ಆದರೆ(overlap==ALL_CP_CONTAINED) {
                    ಮುಂದುವರಿ;  // Irrelevant string. (Also the empty string.)
                }
                ಸ್ಥಿರಾಂಕ UnicodeString& string = *static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(ಅಕ್ಷರಸರಣಿಗಳು.elementAt(i));
                ಸ್ಥಿರಾಂಕ char16_t *s16=string.get_ಬಫರ್();
                int32_t length16=string.length();
                U_ASSERT(length>0);

                // Try to match this string at pos-overlap..pos.
                ಆದರೆ(overlap>=LONG_ಹರವು) {
                    overlap=length16;
                    // While contained: No point matching fully inside the code point span.
                    U16_ಹಿಂಭಾಗ_1(s16, 0, overlap);  // Length of the string minus the last code point.
                }
                ಆದರೆ(overlap>ಹರವು_ಉದ್ದಳತೆ) {
                    overlap=ಹರವು_ಉದ್ದಳತೆ;
                }
                int32_t inc=length16-overlap;  // Keep overlap+inc==length16.
                ಗಾಗಿ(;;) {
                    ಆದರೆ(inc>rest) {
                        ನಿಲ್ಲಿಸು;
                    }
                    // Try to match if the increment is not listed already.
                    ಆದರೆ(!offsets.ಒಳಗೊಂಡಿದೆ_ಸ್ಥಾನಾಂತರ(inc) && matches16CPB(s, pos-overlap, length, s16, length16)) {
                        ಆದರೆ(inc==rest) {
                            ಹಿಂತಿರುಗಿ length;  // Reached the end of the string.
                        }
                        offsets.ಕೂಡಿಸು_ಸ್ಥಾನಾಂತರ(inc);
                    }
                    ಆದರೆ(overlap==0) {
                        ನಿಲ್ಲಿಸು;
                    }
                    --overlap;
                    ++inc;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ /* USET_SPAN_SIMPLE */ {
            int32_t maxInc=0, maxOverlap=0;
            ಗಾಗಿ(i=0; i<ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ; ++i) {
                int32_t overlap=ಹರವು_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the earliest start.

                ಸ್ಥಿರಾಂಕ UnicodeString& string = *static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(ಅಕ್ಷರಸರಣಿಗಳು.elementAt(i));
                ಸ್ಥಿರಾಂಕ char16_t *s16=string.get_ಬಫರ್();
                int32_t length16=string.length();
                ಆದರೆ (length16==0) {
                    ಮುಂದುವರಿ;  // skip the empty string
                }

                // Try to match this string at pos-overlap..pos.
                ಆದರೆ(overlap>=LONG_ಹರವು) {
                    overlap=length16;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the earliest start.
                }
                ಆದರೆ(overlap>ಹರವು_ಉದ್ದಳತೆ) {
                    overlap=ಹರವು_ಉದ್ದಳತೆ;
                }
                int32_t inc=length16-overlap;  // Keep overlap+inc==length16.
                ಗಾಗಿ(;;) {
                    ಆದರೆ(inc>rest || overlap<maxOverlap) {
                        ನಿಲ್ಲಿಸು;
                    }
                    // Try to match if the string is longer or starts earlier.
                    ಆದರೆ( (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ inc>maxInc) &&
                        matches16CPB(s, pos-overlap, length, s16, length16)
                    ) {
                        maxInc=inc;  // Longest match from earliest start.
                        maxOverlap=overlap;
                        ನಿಲ್ಲಿಸು;
                    }
                    --overlap;
                    ++inc;
                }
            }

            ಆದರೆ(maxInc!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue after it.
                pos+=maxInc;
                rest-=maxInc;
                ಆದರೆ(rest==0) {
                    ಹಿಂತಿರುಗಿ length;  // Reached the end of the string.
                }
                ಹರವು_ಉದ್ದಳತೆ=0;  // Match strings from after a string match.
                ಮುಂದುವರಿ;
            }
        }
        // Finished trying to match all strings at pos.

        ಆದರೆ(ಹರವು_ಉದ್ದಳತೆ!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            ಆದರೆ(offsets.is_ಖಾಲಿಗಳು()) {
                ಹಿಂತಿರುಗಿ pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } ಇಲ್ಲದಿದ್ದರೆ {
            // The position is after a string match (or a single code point).
            ಆದರೆ(offsets.is_ಖಾಲಿಗಳು()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                ಹರವು_ಉದ್ದಳತೆ=ಹರವು_ಗಣ.ಹರವು(s+pos, rest, USET_ಹರವು_CONTAINED);
                ಆದರೆ( ಹರವು_ಉದ್ದಳತೆ==rest || // Reached the end of the string, or
                    ಹರವು_ಉದ್ದಳತೆ==0       // neither strings nor span progressed.
                ) {
                    ಹಿಂತಿರುಗಿ pos+ಹರವು_ಉದ್ದಳತೆ;
                }
                pos+=ಹರವು_ಉದ್ದಳತೆ;
                rest-=ಹರವು_ಉದ್ದಳತೆ;
                ಮುಂದುವರಿ;  // spanLength>0: Match strings from after a span.
            } ಇಲ್ಲದಿದ್ದರೆ {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                ಹರವು_ಉದ್ದಳತೆ=ಹರವು_One(ಹರವು_ಗಣ, s+pos, rest);
                ಆದರೆ(ಹರವು_ಉದ್ದಳತೆ>0) {
                    ಆದರೆ(ಹರವು_ಉದ್ದಳತೆ==rest) {
                        ಹಿಂತಿರುಗಿ length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=ಹರವು_ಉದ್ದಳತೆ;
                    rest-=ಹರವು_ಉದ್ದಳತೆ;
                    offsets.ಸರಿಸು(ಹರವು_ಉದ್ದಳತೆ);
                    ಹರವು_ಉದ್ದಳತೆ=0;
                    ಮುಂದುವರಿ;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t min_ಸ್ಥಾನಾಂತರ=offsets.ತೆಗೆ_Minimum();
        pos+=min_ಸ್ಥಾನಾಂತರ;
        rest-=min_ಸ್ಥಾನಾಂತರ;
        ಹರವು_ಉದ್ದಳತೆ=0;  // Match strings from after a string match.
    }
}

int32_t Unicode_ಗಣ_String_ಹರವು::ಹರವು_ಹಿಂಭಾಗ(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length, USet_ಹರವು_Condition ಹರವು_Condition) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(ಹರವು_Condition==USET_ಹರವು_NOT_CONTAINED) {
        ಹಿಂತಿರುಗಿ ಹರವು_Not_ಹಿಂಭಾಗ(s, length);
    }
    int32_t pos=ಹರವು_ಗಣ.ಹರವು_ಹಿಂಭಾಗ(s, length, USET_ಹರವು_CONTAINED);
    ಆದರೆ(pos==0) {
        ಹಿಂತಿರುಗಿ 0;
    }
    int32_t ಹರವು_ಉದ್ದಳತೆ=length-pos;

    // Consider strings; they may overlap with the span.
    ಸ್ಥಾನಾಂತರ_List offsets;
    ಆದರೆ(ಹರವು_Condition==USET_ಹರವು_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.ಗಣ_Max_ಉದ್ದಳತೆ(maxLength16);
    }
    int32_t i, ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ=ಅಕ್ಷರಸರಣಿಗಳು.size();
    uint8_t *ಹರವು_ಹಿಂಭಾಗ_Lengths=ಹರವು_Lengths;
    ಆದರೆ(all) {
        ಹರವು_ಹಿಂಭಾಗ_Lengths+=ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ;
    }
    ಗಾಗಿ(;;) {
        ಆದರೆ(ಹರವು_Condition==USET_ಹರವು_CONTAINED) {
            ಗಾಗಿ(i=0; i<ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ; ++i) {
                int32_t overlap=ಹರವು_ಹಿಂಭಾಗ_Lengths[i];
                ಆದರೆ(overlap==ALL_CP_CONTAINED) {
                    ಮುಂದುವರಿ;  // Irrelevant string. (Also the empty string.)
                }
                ಸ್ಥಿರಾಂಕ UnicodeString& string = *static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(ಅಕ್ಷರಸರಣಿಗಳು.elementAt(i));
                ಸ್ಥಿರಾಂಕ char16_t *s16=string.get_ಬಫರ್();
                int32_t length16=string.length();
                U_ASSERT(length>0);

                // Try to match this string at pos-(length16-overlap)..pos-length16.
                ಆದರೆ(overlap>=LONG_ಹರವು) {
                    overlap=length16;
                    // While contained: No point matching fully inside the code point span.
                    int32_t len1=0;
                    U16_FWD_1(s16, len1, overlap);
                    overlap-=len1;  // Length of the string minus the first code point.
                }
                ಆದರೆ(overlap>ಹರವು_ಉದ್ದಳತೆ) {
                    overlap=ಹರವು_ಉದ್ದಳತೆ;
                }
                int32_t dec=length16-overlap;  // Keep dec+overlap==length16.
                ಗಾಗಿ(;;) {
                    ಆದರೆ(dec>pos) {
                        ನಿಲ್ಲಿಸು;
                    }
                    // Try to match if the decrement is not listed already.
                    ಆದರೆ(!offsets.ಒಳಗೊಂಡಿದೆ_ಸ್ಥಾನಾಂತರ(dec) && matches16CPB(s, pos-dec, length, s16, length16)) {
                        ಆದರೆ(dec==pos) {
                            ಹಿಂತಿರುಗಿ 0;  // Reached the start of the string.
                        }
                        offsets.ಕೂಡಿಸು_ಸ್ಥಾನಾಂತರ(dec);
                    }
                    ಆದರೆ(overlap==0) {
                        ನಿಲ್ಲಿಸು;
                    }
                    --overlap;
                    ++dec;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ /* USET_SPAN_SIMPLE */ {
            int32_t maxDec=0, maxOverlap=0;
            ಗಾಗಿ(i=0; i<ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ; ++i) {
                int32_t overlap=ಹರವು_ಹಿಂಭಾಗ_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                ಸ್ಥಿರಾಂಕ UnicodeString& string = *static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(ಅಕ್ಷರಸರಣಿಗಳು.elementAt(i));
                ಸ್ಥಿರಾಂಕ char16_t *s16=string.get_ಬಫರ್();
                int32_t length16=string.length();
                ಆದರೆ (length16==0) {
                    ಮುಂದುವರಿ;  // skip the empty string
                }

                // Try to match this string at pos-(length16-overlap)..pos-length16.
                ಆದರೆ(overlap>=LONG_ಹರವು) {
                    overlap=length16;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the latest end.
                }
                ಆದರೆ(overlap>ಹರವು_ಉದ್ದಳತೆ) {
                    overlap=ಹರವು_ಉದ್ದಳತೆ;
                }
                int32_t dec=length16-overlap;  // Keep dec+overlap==length16.
                ಗಾಗಿ(;;) {
                    ಆದರೆ(dec>pos || overlap<maxOverlap) {
                        ನಿಲ್ಲಿಸು;
                    }
                    // Try to match if the string is longer or ends later.
                    ಆದರೆ( (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ dec>maxDec) &&
                        matches16CPB(s, pos-dec, length, s16, length16)
                    ) {
                        maxDec=dec;  // Longest match from latest end.
                        maxOverlap=overlap;
                        ನಿಲ್ಲಿಸು;
                    }
                    --overlap;
                    ++dec;
                }
            }

            ಆದರೆ(maxDec!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue before it.
                pos-=maxDec;
                ಆದರೆ(pos==0) {
                    ಹಿಂತಿರುಗಿ 0;  // Reached the start of the string.
                }
                ಹರವು_ಉದ್ದಳತೆ=0;  // Match strings from before a string match.
                ಮುಂದುವರಿ;
            }
        }
        // Finished trying to match all strings at pos.

        ಆದರೆ(ಹರವು_ಉದ್ದಳತೆ!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            ಆದರೆ(offsets.is_ಖಾಲಿಗಳು()) {
                ಹಿಂತಿರುಗಿ pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } ಇಲ್ಲದಿದ್ದರೆ {
            // The position is before a string match (or a single code point).
            ಆದರೆ(offsets.is_ಖಾಲಿಗಳು()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos=ಹರವು_ಗಣ.ಹರವು_ಹಿಂಭಾಗ(s, oldPos, USET_ಹರವು_CONTAINED);
                ಹರವು_ಉದ್ದಳತೆ=oldPos-pos;
                ಆದರೆ( pos==0 ||           // Reached the start of the string, or
                    ಹರವು_ಉದ್ದಳತೆ==0       // neither strings nor span progressed.
                ) {
                    ಹಿಂತಿರುಗಿ pos;
                }
                ಮುಂದುವರಿ;  // spanLength>0: Match strings from before a span.
            } ಇಲ್ಲದಿದ್ದರೆ {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                ಹರವು_ಉದ್ದಳತೆ=ಹರವು_One_ಹಿಂಭಾಗ(ಹರವು_ಗಣ, s, pos);
                ಆದರೆ(ಹರವು_ಉದ್ದಳತೆ>0) {
                    ಆದರೆ(ಹರವು_ಉದ್ದಳತೆ==pos) {
                        ಹಿಂತಿರುಗಿ 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=ಹರವು_ಉದ್ದಳತೆ;
                    offsets.ಸರಿಸು(ಹರವು_ಉದ್ದಳತೆ);
                    ಹರವು_ಉದ್ದಳತೆ=0;
                    ಮುಂದುವರಿ;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.ತೆಗೆ_Minimum();
        ಹರವು_ಉದ್ದಳತೆ=0;  // Match strings from before a string match.
    }
}

int32_t Unicode_ಗಣ_String_ಹರವು::ಹರವು_UTF8(ಸ್ಥಿರಾಂಕ uint8_t *s, int32_t length, USet_ಹರವು_Condition ಹರವು_Condition) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(ಹರವು_Condition==USET_ಹರವು_NOT_CONTAINED) {
        ಹಿಂತಿರುಗಿ ಹರವು_Not_UTF8(s, length);
    }
    int32_t ಹರವು_ಉದ್ದಳತೆ = ಹರವು_ಗಣ.ಹರವು_UTF8(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(s), length, USET_ಹರವು_CONTAINED);
    ಆದರೆ(ಹರವು_ಉದ್ದಳತೆ==length) {
        ಹಿಂತಿರುಗಿ length;
    }

    // Consider strings; they may overlap with the span.
    ಸ್ಥಾನಾಂತರ_List offsets;
    ಆದರೆ(ಹರವು_Condition==USET_ಹರವು_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.ಗಣ_Max_ಉದ್ದಳತೆ(maxLength8);
    }
    int32_t pos=ಹರವು_ಉದ್ದಳತೆ, rest=length-pos;
    int32_t i, ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ=ಅಕ್ಷರಸರಣಿಗಳು.size();
    uint8_t *ಹರವು_UTF8_Lengths=ಹರವು_Lengths;
    ಆದರೆ(all) {
        ಹರವು_UTF8_Lengths+=2*ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ;
    }
    ಗಾಗಿ(;;) {
        ಸ್ಥಿರಾಂಕ uint8_t *s8=utf8;
        int32_t length8;
        ಆದರೆ(ಹರವು_Condition==USET_ಹರವು_CONTAINED) {
            ಗಾಗಿ(i=0; i<ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ; ++i) {
                length8=utf8Lengths[i];
                ಆದರೆ(length8==0) {
                    ಮುಂದುವರಿ;  // String not representable in UTF-8.
                }
                int32_t overlap=ಹರವು_UTF8_Lengths[i];
                ಆದರೆ(overlap==ALL_CP_CONTAINED) {
                    s8+=length8;
                    ಮುಂದುವರಿ;  // Irrelevant string.
                }

                // Try to match this string at pos-overlap..pos.
                ಆದರೆ(overlap>=LONG_ಹರವು) {
                    overlap=length8;
                    // While contained: No point matching fully inside the code point span.
                    U8_ಹಿಂಭಾಗ_1(s8, 0, overlap);  // Length of the string minus the last code point.
                }
                ಆದರೆ(overlap>ಹರವು_ಉದ್ದಳತೆ) {
                    overlap=ಹರವು_ಉದ್ದಳತೆ;
                }
                int32_t inc=length8-overlap;  // Keep overlap+inc==length8.
                ಗಾಗಿ(;;) {
                    ಆದರೆ(inc>rest) {
                        ನಿಲ್ಲಿಸು;
                    }
                    // Try to match if the increment is not listed already.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    ಆದರೆ(!U8_IS_TRAIL(s[pos-overlap]) &&
                            !offsets.ಒಳಗೊಂಡಿದೆ_ಸ್ಥಾನಾಂತರ(inc) &&
                            matches8(s+pos-overlap, s8, length8)) {
                        ಆದರೆ(inc==rest) {
                            ಹಿಂತಿರುಗಿ length;  // Reached the end of the string.
                        }
                        offsets.ಕೂಡಿಸು_ಸ್ಥಾನಾಂತರ(inc);
                    }
                    ಆದರೆ(overlap==0) {
                        ನಿಲ್ಲಿಸು;
                    }
                    --overlap;
                    ++inc;
                }
                s8+=length8;
            }
        } ಇಲ್ಲದಿದ್ದರೆ /* USET_SPAN_SIMPLE */ {
            int32_t maxInc=0, maxOverlap=0;
            ಗಾಗಿ(i=0; i<ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ; ++i) {
                length8=utf8Lengths[i];
                ಆದರೆ(length8==0) {
                    ಮುಂದುವರಿ;  // String not representable in UTF-8.
                }
                int32_t overlap=ಹರವು_UTF8_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the earliest start.

                // Try to match this string at pos-overlap..pos.
                ಆದರೆ(overlap>=LONG_ಹರವು) {
                    overlap=length8;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the earliest start.
                }
                ಆದರೆ(overlap>ಹರವು_ಉದ್ದಳತೆ) {
                    overlap=ಹರವು_ಉದ್ದಳತೆ;
                }
                int32_t inc=length8-overlap;  // Keep overlap+inc==length8.
                ಗಾಗಿ(;;) {
                    ಆದರೆ(inc>rest || overlap<maxOverlap) {
                        ನಿಲ್ಲಿಸು;
                    }
                    // Try to match if the string is longer or starts earlier.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    ಆದರೆ(!U8_IS_TRAIL(s[pos-overlap]) &&
                            (overlap>maxOverlap ||
                                /* redundant overlap==maxOverlap && */ inc>maxInc) &&
                            matches8(s+pos-overlap, s8, length8)) {
                        maxInc=inc;  // Longest match from earliest start.
                        maxOverlap=overlap;
                        ನಿಲ್ಲಿಸು;
                    }
                    --overlap;
                    ++inc;
                }
                s8+=length8;
            }

            ಆದರೆ(maxInc!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue after it.
                pos+=maxInc;
                rest-=maxInc;
                ಆದರೆ(rest==0) {
                    ಹಿಂತಿರುಗಿ length;  // Reached the end of the string.
                }
                ಹರವು_ಉದ್ದಳತೆ=0;  // Match strings from after a string match.
                ಮುಂದುವರಿ;
            }
        }
        // Finished trying to match all strings at pos.

        ಆದರೆ(ಹರವು_ಉದ್ದಳತೆ!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            ಆದರೆ(offsets.is_ಖಾಲಿಗಳು()) {
                ಹಿಂತಿರುಗಿ pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } ಇಲ್ಲದಿದ್ದರೆ {
            // The position is after a string match (or a single code point).
            ಆದರೆ(offsets.is_ಖಾಲಿಗಳು()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                ಹರವು_ಉದ್ದಳತೆ = ಹರವು_ಗಣ.ಹರವು_UTF8(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(s) + pos, rest, USET_ಹರವು_CONTAINED);
                ಆದರೆ( ಹರವು_ಉದ್ದಳತೆ==rest || // Reached the end of the string, or
                    ಹರವು_ಉದ್ದಳತೆ==0       // neither strings nor span progressed.
                ) {
                    ಹಿಂತಿರುಗಿ pos+ಹರವು_ಉದ್ದಳತೆ;
                }
                pos+=ಹರವು_ಉದ್ದಳತೆ;
                rest-=ಹರವು_ಉದ್ದಳತೆ;
                ಮುಂದುವರಿ;  // spanLength>0: Match strings from after a span.
            } ಇಲ್ಲದಿದ್ದರೆ {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                ಹರವು_ಉದ್ದಳತೆ=ಹರವು_One_UTF8(ಹರವು_ಗಣ, s+pos, rest);
                ಆದರೆ(ಹರವು_ಉದ್ದಳತೆ>0) {
                    ಆದರೆ(ಹರವು_ಉದ್ದಳತೆ==rest) {
                        ಹಿಂತಿರುಗಿ length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=ಹರವು_ಉದ್ದಳತೆ;
                    rest-=ಹರವು_ಉದ್ದಳತೆ;
                    offsets.ಸರಿಸು(ಹರವು_ಉದ್ದಳತೆ);
                    ಹರವು_ಉದ್ದಳತೆ=0;
                    ಮುಂದುವರಿ;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t min_ಸ್ಥಾನಾಂತರ=offsets.ತೆಗೆ_Minimum();
        pos+=min_ಸ್ಥಾನಾಂತರ;
        rest-=min_ಸ್ಥಾನಾಂತರ;
        ಹರವು_ಉದ್ದಳತೆ=0;  // Match strings from after a string match.
    }
}

int32_t Unicode_ಗಣ_String_ಹರವು::ಹರವು_ಹಿಂಭಾಗ_UTF8(ಸ್ಥಿರಾಂಕ uint8_t *s, int32_t length, USet_ಹರವು_Condition ಹರವು_Condition) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(ಹರವು_Condition==USET_ಹರವು_NOT_CONTAINED) {
        ಹಿಂತಿರುಗಿ ಹರವು_Not_ಹಿಂಭಾಗ_UTF8(s, length);
    }
    int32_t pos = ಹರವು_ಗಣ.ಹರವು_ಹಿಂಭಾಗ_UTF8(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(s), length, USET_ಹರವು_CONTAINED);
    ಆದರೆ(pos==0) {
        ಹಿಂತಿರುಗಿ 0;
    }
    int32_t ಹರವು_ಉದ್ದಳತೆ=length-pos;

    // Consider strings; they may overlap with the span.
    ಸ್ಥಾನಾಂತರ_List offsets;
    ಆದರೆ(ಹರವು_Condition==USET_ಹರವು_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.ಗಣ_Max_ಉದ್ದಳತೆ(maxLength8);
    }
    int32_t i, ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ=ಅಕ್ಷರಸರಣಿಗಳು.size();
    uint8_t *ಹರವು_ಹಿಂಭಾಗ_UTF8_Lengths=ಹರವು_Lengths;
    ಆದರೆ(all) {
        ಹರವು_ಹಿಂಭಾಗ_UTF8_Lengths+=3*ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ;
    }
    ಗಾಗಿ(;;) {
        ಸ್ಥಿರಾಂಕ uint8_t *s8=utf8;
        int32_t length8;
        ಆದರೆ(ಹರವು_Condition==USET_ಹರವು_CONTAINED) {
            ಗಾಗಿ(i=0; i<ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ; ++i) {
                length8=utf8Lengths[i];
                ಆದರೆ(length8==0) {
                    ಮುಂದುವರಿ;  // String not representable in UTF-8.
                }
                int32_t overlap=ಹರವು_ಹಿಂಭಾಗ_UTF8_Lengths[i];
                ಆದರೆ(overlap==ALL_CP_CONTAINED) {
                    s8+=length8;
                    ಮುಂದುವರಿ;  // Irrelevant string.
                }

                // Try to match this string at pos-(length8-overlap)..pos-length8.
                ಆದರೆ(overlap>=LONG_ಹರವು) {
                    overlap=length8;
                    // While contained: No point matching fully inside the code point span.
                    int32_t len1=0;
                    U8_FWD_1(s8, len1, overlap);
                    overlap-=len1;  // Length of the string minus the first code point.
                }
                ಆದರೆ(overlap>ಹರವು_ಉದ್ದಳತೆ) {
                    overlap=ಹರವು_ಉದ್ದಳತೆ;
                }
                int32_t dec=length8-overlap;  // Keep dec+overlap==length8.
                ಗಾಗಿ(;;) {
                    ಆದರೆ(dec>pos) {
                        ನಿಲ್ಲಿಸು;
                    }
                    // Try to match if the decrement is not listed already.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    ಆದರೆ( !U8_IS_TRAIL(s[pos-dec]) &&
                        !offsets.ಒಳಗೊಂಡಿದೆ_ಸ್ಥಾನಾಂತರ(dec) &&
                        matches8(s+pos-dec, s8, length8)
                    ) {
                        ಆದರೆ(dec==pos) {
                            ಹಿಂತಿರುಗಿ 0;  // Reached the start of the string.
                        }
                        offsets.ಕೂಡಿಸು_ಸ್ಥಾನಾಂತರ(dec);
                    }
                    ಆದರೆ(overlap==0) {
                        ನಿಲ್ಲಿಸು;
                    }
                    --overlap;
                    ++dec;
                }
                s8+=length8;
            }
        } ಇಲ್ಲದಿದ್ದರೆ /* USET_SPAN_SIMPLE */ {
            int32_t maxDec=0, maxOverlap=0;
            ಗಾಗಿ(i=0; i<ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ; ++i) {
                length8=utf8Lengths[i];
                ಆದರೆ(length8==0) {
                    ಮುಂದುವರಿ;  // String not representable in UTF-8.
                }
                int32_t overlap=ಹರವು_ಹಿಂಭಾಗ_UTF8_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                // Try to match this string at pos-(length8-overlap)..pos-length8.
                ಆದರೆ(overlap>=LONG_ಹರವು) {
                    overlap=length8;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the latest end.
                }
                ಆದರೆ(overlap>ಹರವು_ಉದ್ದಳತೆ) {
                    overlap=ಹರವು_ಉದ್ದಳತೆ;
                }
                int32_t dec=length8-overlap;  // Keep dec+overlap==length8.
                ಗಾಗಿ(;;) {
                    ಆದರೆ(dec>pos || overlap<maxOverlap) {
                        ನಿಲ್ಲಿಸು;
                    }
                    // Try to match if the string is longer or ends later.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    ಆದರೆ( !U8_IS_TRAIL(s[pos-dec]) &&
                        (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ dec>maxDec) &&
                        matches8(s+pos-dec, s8, length8)
                    ) {
                        maxDec=dec;  // Longest match from latest end.
                        maxOverlap=overlap;
                        ನಿಲ್ಲಿಸು;
                    }
                    --overlap;
                    ++dec;
                }
                s8+=length8;
            }

            ಆದರೆ(maxDec!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue before it.
                pos-=maxDec;
                ಆದರೆ(pos==0) {
                    ಹಿಂತಿರುಗಿ 0;  // Reached the start of the string.
                }
                ಹರವು_ಉದ್ದಳತೆ=0;  // Match strings from before a string match.
                ಮುಂದುವರಿ;
            }
        }
        // Finished trying to match all strings at pos.

        ಆದರೆ(ಹರವು_ಉದ್ದಳತೆ!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            ಆದರೆ(offsets.is_ಖಾಲಿಗಳು()) {
                ಹಿಂತಿರುಗಿ pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } ಇಲ್ಲದಿದ್ದರೆ {
            // The position is before a string match (or a single code point).
            ಆದರೆ(offsets.is_ಖಾಲಿಗಳು()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos = ಹರವು_ಗಣ.ಹರವು_ಹಿಂಭಾಗ_UTF8(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(s), oldPos, USET_ಹರವು_CONTAINED);
                ಹರವು_ಉದ್ದಳತೆ=oldPos-pos;
                ಆದರೆ( pos==0 ||           // Reached the start of the string, or
                    ಹರವು_ಉದ್ದಳತೆ==0       // neither strings nor span progressed.
                ) {
                    ಹಿಂತಿರುಗಿ pos;
                }
                ಮುಂದುವರಿ;  // spanLength>0: Match strings from before a span.
            } ಇಲ್ಲದಿದ್ದರೆ {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                ಹರವು_ಉದ್ದಳತೆ=ಹರವು_One_ಹಿಂಭಾಗ_UTF8(ಹರವು_ಗಣ, s, pos);
                ಆದರೆ(ಹರವು_ಉದ್ದಳತೆ>0) {
                    ಆದರೆ(ಹರವು_ಉದ್ದಳತೆ==pos) {
                        ಹಿಂತಿರುಗಿ 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=ಹರವು_ಉದ್ದಳತೆ;
                    offsets.ಸರಿಸು(ಹರವು_ಉದ್ದಳತೆ);
                    ಹರವು_ಉದ್ದಳತೆ=0;
                    ಮುಂದುವರಿ;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.ತೆಗೆ_Minimum();
        ಹರವು_ಉದ್ದಳತೆ=0;  // Match strings from before a string match.
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

int32_t Unicode_ಗಣ_String_ಹರವು::ಹರವು_Not(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length) ಸ್ಥಿರಾಂಕ {
    int32_t pos=0, rest=length;
    int32_t i, ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ=ಅಕ್ಷರಸರಣಿಗಳು.size();
    ಮಾಡು {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i=p_ಹರವು_Not_ಗಣ->ಹರವು(s+pos, rest, USET_ಹರವು_NOT_CONTAINED);
        ಆದರೆ(i==rest) {
            ಹಿಂತಿರುಗಿ length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_ಉದ್ದಳತೆ=ಹರವು_One(ಹರವು_ಗಣ, s+pos, rest);
        ಆದರೆ(cp_ಉದ್ದಳತೆ>0) {
            ಹಿಂತಿರುಗಿ pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        ಗಾಗಿ(i=0; i<ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ; ++i) {
            ಆದರೆ(ಹರವು_Lengths[i]==ALL_CP_CONTAINED) {
                ಮುಂದುವರಿ;  // Irrelevant string. (Also the empty string.)
            }
            ಸ್ಥಿರಾಂಕ UnicodeString& string = *static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(ಅಕ್ಷರಸರಣಿಗಳು.elementAt(i));
            ಸ್ಥಿರಾಂಕ char16_t *s16=string.get_ಬಫರ್();
            int32_t length16=string.length();
            U_ASSERT(length>0);
            ಆದರೆ(length16<=rest && matches16CPB(s, pos, length, s16, length16)) {
                ಹಿಂತಿರುಗಿ pos;  // There is a set element at pos.
            }
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos-=cp_ಉದ್ದಳತೆ;
        rest+=cp_ಉದ್ದಳತೆ;
    } ಆಗ(rest!=0);
    ಹಿಂತಿರುಗಿ length;  // Reached the end of the string.
}

int32_t Unicode_ಗಣ_String_ಹರವು::ಹರವು_Not_ಹಿಂಭಾಗ(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length) ಸ್ಥಿರಾಂಕ {
    int32_t pos=length;
    int32_t i, ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ=ಅಕ್ಷರಸರಣಿಗಳು.size();
    ಮಾಡು {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos=p_ಹರವು_Not_ಗಣ->ಹರವು_ಹಿಂಭಾಗ(s, pos, USET_ಹರವು_NOT_CONTAINED);
        ಆದರೆ(pos==0) {
            ಹಿಂತಿರುಗಿ 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_ಉದ್ದಳತೆ=ಹರವು_One_ಹಿಂಭಾಗ(ಹರವು_ಗಣ, s, pos);
        ಆದರೆ(cp_ಉದ್ದಳತೆ>0) {
            ಹಿಂತಿರುಗಿ pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        ಗಾಗಿ(i=0; i<ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ; ++i) {
            // Use spanLengths rather than a spanBackLengths pointer because
            // it is easier and we only need to know whether the string is irrelevant
            // which is the same in either array.
            ಆದರೆ(ಹರವು_Lengths[i]==ALL_CP_CONTAINED) {
                ಮುಂದುವರಿ;  // Irrelevant string. (Also the empty string.)
            }
            ಸ್ಥಿರಾಂಕ UnicodeString& string = *static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(ಅಕ್ಷರಸರಣಿಗಳು.elementAt(i));
            ಸ್ಥಿರಾಂಕ char16_t *s16=string.get_ಬಫರ್();
            int32_t length16=string.length();
            U_ASSERT(length>0);
            ಆದರೆ(length16<=pos && matches16CPB(s, pos-length16, length, s16, length16)) {
                ಹಿಂತಿರುಗಿ pos;  // There is a set element at pos.
            }
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos+=cp_ಉದ್ದಳತೆ;
    } ಆಗ(pos!=0);
    ಹಿಂತಿರುಗಿ 0;  // Reached the start of the string.
}

int32_t Unicode_ಗಣ_String_ಹರವು::ಹರವು_Not_UTF8(ಸ್ಥಿರಾಂಕ uint8_t *s, int32_t length) ಸ್ಥಿರಾಂಕ {
    int32_t pos=0, rest=length;
    int32_t i, ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ=ಅಕ್ಷರಸರಣಿಗಳು.size();
    uint8_t *ಹರವು_UTF8_Lengths=ಹರವು_Lengths;
    ಆದರೆ(all) {
        ಹರವು_UTF8_Lengths+=2*ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ;
    }
    ಮಾಡು {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i = p_ಹರವು_Not_ಗಣ->ಹರವು_UTF8(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(s) + pos, rest, USET_ಹರವು_NOT_CONTAINED);
        ಆದರೆ(i==rest) {
            ಹಿಂತಿರುಗಿ length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_ಉದ್ದಳತೆ=ಹರವು_One_UTF8(ಹರವು_ಗಣ, s+pos, rest);
        ಆದರೆ(cp_ಉದ್ದಳತೆ>0) {
            ಹಿಂತಿರುಗಿ pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        ಸ್ಥಿರಾಂಕ uint8_t *s8=utf8;
        int32_t length8;
        ಗಾಗಿ(i=0; i<ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ; ++i) {
            length8=utf8Lengths[i];
            // ALL_CP_CONTAINED: Irrelevant string.
            ಆದರೆ(length8!=0 && ಹರವು_UTF8_Lengths[i]!=ALL_CP_CONTAINED && length8<=rest && matches8(s+pos, s8, length8)) {
                ಹಿಂತಿರುಗಿ pos;  // There is a set element at pos.
            }
            s8+=length8;
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos-=cp_ಉದ್ದಳತೆ;
        rest+=cp_ಉದ್ದಳತೆ;
    } ಆಗ(rest!=0);
    ಹಿಂತಿರುಗಿ length;  // Reached the end of the string.
}

int32_t Unicode_ಗಣ_String_ಹರವು::ಹರವು_Not_ಹಿಂಭಾಗ_UTF8(ಸ್ಥಿರಾಂಕ uint8_t *s, int32_t length) ಸ್ಥಿರಾಂಕ {
    int32_t pos=length;
    int32_t i, ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ=ಅಕ್ಷರಸರಣಿಗಳು.size();
    uint8_t *ಹರವು_ಹಿಂಭಾಗ_UTF8_Lengths=ಹರವು_Lengths;
    ಆದರೆ(all) {
        ಹರವು_ಹಿಂಭಾಗ_UTF8_Lengths+=3*ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ;
    }
    ಮಾಡು {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos = p_ಹರವು_Not_ಗಣ->ಹರವು_ಹಿಂಭಾಗ_UTF8(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(s), pos, USET_ಹರವು_NOT_CONTAINED);
        ಆದರೆ(pos==0) {
            ಹಿಂತಿರುಗಿ 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_ಉದ್ದಳತೆ=ಹರವು_One_ಹಿಂಭಾಗ_UTF8(ಹರವು_ಗಣ, s, pos);
        ಆದರೆ(cp_ಉದ್ದಳತೆ>0) {
            ಹಿಂತಿರುಗಿ pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        ಸ್ಥಿರಾಂಕ uint8_t *s8=utf8;
        int32_t length8;
        ಗಾಗಿ(i=0; i<ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ; ++i) {
            length8=utf8Lengths[i];
            // ALL_CP_CONTAINED: Irrelevant string.
            ಆದರೆ(length8!=0 && ಹರವು_ಹಿಂಭಾಗ_UTF8_Lengths[i]!=ALL_CP_CONTAINED && length8<=pos && matches8(s+pos-length8, s8, length8)) {
                ಹಿಂತಿರುಗಿ pos;  // There is a set element at pos.
            }
            s8+=length8;
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos+=cp_ಉದ್ದಳತೆ;
    } ಆಗ(pos!=0);
    ಹಿಂತಿರುಗಿ 0;  // Reached the start of the string.
}

U_NAMESPACE_ಕೊನೆ
