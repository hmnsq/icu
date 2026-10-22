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

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/utf8.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "uvector.h"
#అన్తర్భావ "unisetspan.h"

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
తరగతి స్థానభ్రంశం_List {  // Only ever stack-allocated, does not need to inherit UMemory.
బహిరంగ:
    స్థానభ్రంశం_List() : list(staticList), capacity(0), length(0), ప్రారంభం(0) {}

    ~స్థానభ్రంశం_List() {
        ఒకవేళ(list!=staticList) {
            uprv_free(list);
        }
    }

    // Call exactly once if the list is to be used.
    శూన్య సమితి_Max_నిడివి(int32_t max_నిడివి_2) {
        ఒకవేళ (max_నిడివి_2 <= static_cast<int32_t>(పరిమాణం(staticList))) {
            capacity = static_cast<int32_t>(పరిమాణం(staticList));
        } లేకపోతే {
            UBool* l = static_cast<UBool*>(uprv_malloc(max_నిడివి_2));
            ఒకవేళ(l!=శూన్యసూచిక) {
                list=l;
                capacity=max_నిడివి_2;
            }
        }
        uprv_memset(list, 0, capacity);
    }

    శూన్య clear() {
        uprv_memset(list, 0, capacity);
        ప్రారంభం=length=0;
    }

    UBool is_ఖాళీలు() స్థిరరాశి {
        తిరిగి length == 0;
    }

    // Reduce all stored offsets by delta, used when the current position
    // moves by delta.
    // There must not be any offsets lower than delta.
    // If there is an offset equal to delta, it is removed.
    // delta=[1..maxLength]
    శూన్య జరుపు(int32_t delta) {
        int32_t i=ప్రారంభం+delta;
        ఒకవేళ(i>=capacity) {
            i-=capacity;
        }
        ఒకవేళ(list[i]) {
            list[i]=అబద్ధం;
            --length;
        }
        ప్రారంభం=i;
    }

    // Add an offset. The list must not contain it yet.
    // offset=[1..maxLength]
    శూన్య కూడు_స్థానభ్రంశం(int32_t స్థానభ్రంశం) {
        int32_t i=ప్రారంభం+స్థానభ్రంశం;
        ఒకవేళ(i>=capacity) {
            i-=capacity;
        }
        list[i]=నిజం;
        ++length;
    }

    // offset=[1..maxLength]
    UBool కలిగియుంది_స్థానభ్రంశం(int32_t స్థానభ్రంశం) స్థిరరాశి {
        int32_t i=ప్రారంభం+స్థానభ్రంశం;
        ఒకవేళ(i>=capacity) {
            i-=capacity;
        }
        తిరిగి list[i];
    }

    // Find the lowest stored offset from a non-empty list, remove it,
    // and reduce all other offsets by this minimum.
    // Returns [1..maxLength].
    int32_t తీయి_Minimum() {
        // Look for the next offset in list[start+1..capacity-1].
        int32_t i=ప్రారంభం, ఫలితం;
        అయితే(++i<capacity) {
            ఒకవేళ(list[i]) {
                list[i]=అబద్ధం;
                --length;
                ఫలితం=i-ప్రారంభం;
                ప్రారంభం=i;
                తిరిగి ఫలితం;
            }
        }
        // i==capacity

        // Wrap around and look for the next offset in list[0..start].
        // Since the list is not empty, there will be one.
        ఫలితం=capacity-ప్రారంభం;
        i=0;
        అయితే(!list[i]) {
            ++i;
        }
        list[i]=అబద్ధం;
        --length;
        ప్రారంభం=i;
        తిరిగి ఫలితం+=i;
    }

అంతరంగ:
    UBool *list;
    int32_t capacity;
    int32_t length;
    int32_t ప్రారంభం;

    UBool staticList[16];
};

// Get the number of UTF-8 bytes for a UTF-16 (sub)string.
స్థిర int32_t
get_UTF8_నిడివి(స్థిరరాశి char16_t *s, int32_t length) {
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    int32_t length8=0;
    u_str_గమ్యం_UTF8(శూన్యసూచిక, 0, &length8, s, length, &error_సంకేతం);
    ఒకవేళ(U_SUCCESS(error_సంకేతం) || error_సంకేతం==U_బఫరు_అధికప్రవాహం_ERROR) {
        తిరిగి length8;
    } లేకపోతే {
        // The string contains an unpaired surrogate.
        // Ignore this string.
        తిరిగి 0;
    }
}

// Append the UTF-8 version of the string to t and return the appended UTF-8 length.
స్థిర int32_t
appendUTF8(స్థిరరాశి char16_t *s, int32_t length, uint8_t *t, int32_t capacity) {
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    int32_t length8=0;
    u_str_గమ్యం_UTF8(reinterpret_cast<అక్షరం*>(t), capacity, &length8, s, length, &error_సంకేతం);
    ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
        తిరిగి length8;
    } లేకపోతే {
        // The string contains an unpaired surrogate.
        // Ignore this string.
        తిరిగి 0;
    }
}

స్థిర అంతఃవరుస uint8_t
make_పరిధి_నిడివి_Byte(int32_t పరిధి_నిడివి) {
    // 0xfe==UnicodeSetStringSpan::LONG_SPAN
    తిరిగి పరిధి_నిడివి < 0xfe ? static_cast<uint8_t>(పరిధి_నిడివి) : static_cast<uint8_t>(0xfe);
}

// Construct for all variants of span(), or only for any one variant.
// Initialize as little as possible, for single use.
Unicode_సమితి_String_పరిధి::Unicode_సమితి_String_పరిధి(స్థిరరాశి Unicode_సమితి &సమితి,
                                           స్థిరరాశి UVector &సమితి_అక్షరశ్రేణులు,
                                           uint32_t which)
        : పరిధి_సమితి(0, 0x10ffff), p_పరిధి_Not_సమితి(శూన్యసూచిక), అక్షరశ్రేణులు(సమితి_అక్షరశ్రేణులు),
          utf8Lengths(శూన్యసూచిక), పరిధి_Lengths(శూన్యసూచిక), utf8(శూన్యసూచిక),
          utf8_నిడివి(0),
          maxLength16(0), maxLength8(0),
          all(static_cast<UBool>(which == ALL)) {
    పరిధి_సమితి.retainAll(సమితి);
    ఒకవేళ(which&NOT_CONTAINED) {
        // Default to the same sets.
        // addToSpanNotSet() will create a separate set if necessary.
        p_పరిధి_Not_సమితి=&పరిధి_సమితి;
    }

    // Determine if the strings even need to be taken into account at all for span() etc.
    // If any string is relevant, then all strings need to be used for
    // span(longest match) but only the relevant ones for span(while contained).
    // TODO: Possible optimization: Distinguish CONTAINED vs. LONGEST_MATCH
    //   and do not store UTF-8 strings if !thisRelevant and CONTAINED.
    //   (Only store irrelevant UTF-8 strings for LONGEST_MATCH where they are relevant after all.)
    // Also count the lengths of the UTF-8 versions of the strings for memory allocation.
    int32_t అక్షరశ్రేణులు_నిడివి=అక్షరశ్రేణులు.size();

    int32_t i, పరిధి_నిడివి;
    UBool someRelevant=అబద్ధం;
    కొరకు(i=0; i<అక్షరశ్రేణులు_నిడివి; ++i) {
        స్థిరరాశి UnicodeString& string = *static_cast<స్థిరరాశి UnicodeString*>(అక్షరశ్రేణులు.elementAt(i));
        స్థిరరాశి char16_t *s16=string.get_బఫరు();
        int32_t length16=string.length();
        ఒకవేళ (length16==0) {
            కొనసాగు;  // skip the empty string
        }
        UBool thisRelevant;
        పరిధి_నిడివి=పరిధి_సమితి.పరిధి(s16, length16, USET_పరిధి_CONTAINED);
        ఒకవేళ(పరిధి_నిడివి<length16) {  // Relevant string.
            someRelevant=thisRelevant=నిజం;
        } లేకపోతే {
            thisRelevant=అబద్ధం;
        }
        ఒకవేళ((which&UTF16) && length16>maxLength16) {
            maxLength16=length16;
        }
        ఒకవేళ((which&UTF8) && (thisRelevant || (which&CONTAINED))) {
            int32_t length8=get_UTF8_నిడివి(s16, length16);
            utf8_నిడివి+=length8;
            ఒకవేళ(length8>maxLength8) {
                maxLength8=length8;
            }
        }
    }
    ఒకవేళ(!someRelevant) {
        maxLength16=maxLength8=0;
        తిరిగి;
    }

    // Freeze after checking for the need to use strings at all because freezing
    // a set takes some time and memory which are wasted if there are no relevant strings.
    ఒకవేళ(all) {
        పరిధి_సమితి.freeze();
    }

    uint8_t *పరిధి_వెనుక_Lengths;
    uint8_t *పరిధి_UTF8_Lengths;
    uint8_t *పరిధి_వెనుక_UTF8_Lengths;

    // Allocate a block of meta data.
    int32_t alloc_కొలత_2;
    ఒకవేళ(all) {
        // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
        alloc_కొలత_2=అక్షరశ్రేణులు_నిడివి*(4+1+1+1+1)+utf8_నిడివి;
    } లేకపోతే {
        alloc_కొలత_2=అక్షరశ్రేణులు_నిడివి;  // One set of span lengths.
        ఒకవేళ(which&UTF8) {
            // UTF-8 lengths and UTF-8 strings.
            alloc_కొలత_2+=అక్షరశ్రేణులు_నిడివి*4+utf8_నిడివి;
        }
    }
    ఒకవేళ (alloc_కొలత_2 <= static_cast<int32_t>(పరిమాణం(staticLengths))) {
        utf8Lengths=staticLengths;
    } లేకపోతే {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(alloc_కొలత_2));
        ఒకవేళ(utf8Lengths==శూన్యసూచిక) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            తిరిగి;  // Out of memory.
        }
    }

    ఒకవేళ(all) {
        // Store span lengths for all span() variants.
        పరిధి_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths + అక్షరశ్రేణులు_నిడివి);
        పరిధి_వెనుక_Lengths=పరిధి_Lengths+అక్షరశ్రేణులు_నిడివి;
        పరిధి_UTF8_Lengths=పరిధి_వెనుక_Lengths+అక్షరశ్రేణులు_నిడివి;
        పరిధి_వెనుక_UTF8_Lengths=పరిధి_UTF8_Lengths+అక్షరశ్రేణులు_నిడివి;
        utf8=పరిధి_వెనుక_UTF8_Lengths+అక్షరశ్రేణులు_నిడివి;
    } లేకపోతే {
        // Store span lengths for only one span() variant.
        ఒకవేళ(which&UTF8) {
            పరిధి_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths + అక్షరశ్రేణులు_నిడివి);
            utf8=పరిధి_Lengths+అక్షరశ్రేణులు_నిడివి;
        } లేకపోతే {
            పరిధి_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths);
        }
        పరిధి_వెనుక_Lengths=పరిధి_UTF8_Lengths=పరిధి_వెనుక_UTF8_Lengths=పరిధి_Lengths;
    }

    // Set the meta data and pSpanNotSet and write the UTF-8 strings.
    int32_t utf8_లెక్క=0;  // Count UTF-8 bytes written so far.

    కొరకు(i=0; i<అక్షరశ్రేణులు_నిడివి; ++i) {
        స్థిరరాశి UnicodeString& string = *static_cast<స్థిరరాశి UnicodeString*>(అక్షరశ్రేణులు.elementAt(i));
        స్థిరరాశి char16_t *s16=string.get_బఫరు();
        int32_t length16=string.length();
        పరిధి_నిడివి=పరిధి_సమితి.పరిధి(s16, length16, USET_పరిధి_CONTAINED);
        ఒకవేళ(పరిధి_నిడివి<length16 && length16>0) {  // Relevant string.
            ఒకవేళ(which&UTF16) {
                ఒకవేళ(which&CONTAINED) {
                    ఒకవేళ(which&FWD) {
                        పరిధి_Lengths[i]=make_పరిధి_నిడివి_Byte(పరిధి_నిడివి);
                    }
                    ఒకవేళ(which&వెనుక_2) {
                        పరిధి_నిడివి=length16-పరిధి_సమితి.పరిధి_వెనుక(s16, length16, USET_పరిధి_CONTAINED);
                        పరిధి_వెనుక_Lengths[i]=make_పరిధి_నిడివి_Byte(పరిధి_నిడివి);
                    }
                } లేకపోతే /* not CONTAINED, not all, but NOT_CONTAINED */ {
                    పరిధి_Lengths[i]=పరిధి_వెనుక_Lengths[i]=0;  // Only store a relevant/irrelevant flag.
                }
            }
            ఒకవేళ(which&UTF8) {
                uint8_t *s8=utf8+utf8_లెక్క;
                int32_t length8=appendUTF8(s16, length16, s8, utf8_నిడివి-utf8_లెక్క);
                utf8_లెక్క+=utf8Lengths[i]=length8;
                ఒకవేళ(length8==0) {  // Irrelevant for UTF-8 because not representable in UTF-8.
                    పరిధి_UTF8_Lengths[i] = పరిధి_వెనుక_UTF8_Lengths[i] = static_cast<uint8_t>(ALL_CP_CONTAINED);
                } లేకపోతే {  // Relevant for UTF-8.
                    ఒకవేళ(which&CONTAINED) {
                        ఒకవేళ(which&FWD) {
                            పరిధి_నిడివి = పరిధి_సమితి.పరిధి_UTF8(reinterpret_cast<స్థిరరాశి అక్షరం*>(s8), length8, USET_పరిధి_CONTAINED);
                            పరిధి_UTF8_Lengths[i]=make_పరిధి_నిడివి_Byte(పరిధి_నిడివి);
                        }
                        ఒకవేళ(which&వెనుక_2) {
                            పరిధి_నిడివి = length8 - పరిధి_సమితి.పరిధి_వెనుక_UTF8(reinterpret_cast<స్థిరరాశి అక్షరం*>(s8), length8, USET_పరిధి_CONTAINED);
                            పరిధి_వెనుక_UTF8_Lengths[i]=make_పరిధి_నిడివి_Byte(పరిధి_నిడివి);
                        }
                    } లేకపోతే /* not CONTAINED, not all, but NOT_CONTAINED */ {
                        పరిధి_UTF8_Lengths[i]=పరిధి_వెనుక_UTF8_Lengths[i]=0;  // Only store a relevant/irrelevant flag.
                    }
                }
            }
            ఒకవేళ(which&NOT_CONTAINED) {
                // Add string start and end code points to the spanNotSet so that
                // a span(while not contained) stops before any string.
                UChar32 c;
                ఒకవేళ(which&FWD) {
                    int32_t len=0;
                    U16_తదుపరి(s16, len, length16, c);
                    కూడు_గమ్యం_పరిధి_Not_సమితి(c);
                }
                ఒకవేళ(which&వెనుక_2) {
                    int32_t len=length16;
                    U16_PREV(s16, 0, len, c);
                    కూడు_గమ్యం_పరిధి_Not_సమితి(c);
                }
            }
        } లేకపోతే {  // Irrelevant string. (Also the empty string.)
            ఒకవేళ(which&UTF8) {
                ఒకవేళ(which&CONTAINED) {  // Only necessary for LONGEST_MATCH.
                    uint8_t *s8=utf8+utf8_లెక్క;
                    int32_t length8=appendUTF8(s16, length16, s8, utf8_నిడివి-utf8_లెక్క);
                    utf8_లెక్క+=utf8Lengths[i]=length8;
                } లేకపోతే {
                    utf8Lengths[i]=0;
                }
            }
            ఒకవేళ(all) {
                పరిధి_Lengths[i]=పరిధి_వెనుక_Lengths[i]=
                    పరిధి_UTF8_Lengths[i]=పరిధి_వెనుక_UTF8_Lengths[i]=
                        static_cast<uint8_t>(ALL_CP_CONTAINED);
            } లేకపోతే {
                // All spanXYZLengths pointers contain the same address.
                పరిధి_Lengths[i] = static_cast<uint8_t>(ALL_CP_CONTAINED);
            }
        }
    }

    // Finish.
    ఒకవేళ(all) {
        p_పరిధి_Not_సమితి->freeze();
    }
}

// Copy constructor. Assumes which==ALL for a frozen set.
Unicode_సమితి_String_పరిధి::Unicode_సమితి_String_పరిధి(స్థిరరాశి Unicode_సమితి_String_పరిధి &other_String_పరిధి,
                                           స్థిరరాశి UVector &new_మాతృ_సమితి_అక్షరశ్రేణులు)
        : పరిధి_సమితి(other_String_పరిధి.పరిధి_సమితి), p_పరిధి_Not_సమితి(శూన్యసూచిక), అక్షరశ్రేణులు(new_మాతృ_సమితి_అక్షరశ్రేణులు),
          utf8Lengths(శూన్యసూచిక), పరిధి_Lengths(శూన్యసూచిక), utf8(శూన్యసూచిక),
          utf8_నిడివి(other_String_పరిధి.utf8_నిడివి),
          maxLength16(other_String_పరిధి.maxLength16), maxLength8(other_String_పరిధి.maxLength8),
          all(నిజం) {
    ఒకవేళ(other_String_పరిధి.p_పరిధి_Not_సమితి==&other_String_పరిధి.పరిధి_సమితి) {
        p_పరిధి_Not_సమితి=&పరిధి_సమితి;
    } లేకపోతే {
        p_పరిధి_Not_సమితి=other_String_పరిధి.p_పరిధి_Not_సమితి->clone();
    }

    // Allocate a block of meta data.
    // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
    int32_t అక్షరశ్రేణులు_నిడివి=అక్షరశ్రేణులు.size();
    int32_t alloc_కొలత_2=అక్షరశ్రేణులు_నిడివి*(4+1+1+1+1)+utf8_నిడివి;
    ఒకవేళ (alloc_కొలత_2 <= static_cast<int32_t>(పరిమాణం(staticLengths))) {
        utf8Lengths=staticLengths;
    } లేకపోతే {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(alloc_కొలత_2));
        ఒకవేళ(utf8Lengths==శూన్యసూచిక) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            తిరిగి;  // Out of memory.
        }
    }

    పరిధి_Lengths = reinterpret_cast<uint8_t*>(utf8Lengths + అక్షరశ్రేణులు_నిడివి);
    utf8=పరిధి_Lengths+అక్షరశ్రేణులు_నిడివి*4;
    uprv_memcpy(utf8Lengths, other_String_పరిధి.utf8Lengths, alloc_కొలత_2);
}

Unicode_సమితి_String_పరిధి::~Unicode_సమితి_String_పరిధి() {
    ఒకవేళ(p_పరిధి_Not_సమితి!=శూన్యసూచిక && p_పరిధి_Not_సమితి!=&పరిధి_సమితి) {
        తొలగించు p_పరిధి_Not_సమితి;
    }
    ఒకవేళ(utf8Lengths!=శూన్యసూచిక && utf8Lengths!=staticLengths) {
        uprv_free(utf8Lengths);
    }
}

శూన్య Unicode_సమితి_String_పరిధి::కూడు_గమ్యం_పరిధి_Not_సమితి(UChar32 c) {
    ఒకవేళ(p_పరిధి_Not_సమితి==శూన్యసూచిక || p_పరిధి_Not_సమితి==&పరిధి_సమితి) {
        ఒకవేళ(పరిధి_సమితి.కలిగియుంది(c)) {
            తిరిగి;  // Nothing to do.
        }
        Unicode_సమితి *new_సమితి=పరిధి_సమితి.cloneAsThawed();
        ఒకవేళ(new_సమితి==శూన్యసూచిక) {
            తిరిగి;  // Out of memory.
        } లేకపోతే {
            p_పరిధి_Not_సమితి=new_సమితి;
        }
    }
    p_పరిధి_Not_సమితి->కూడు(c);
}

// Compare strings without any argument checks. Requires length>0.
స్థిర అంతఃవరుస UBool
matches16(స్థిరరాశి char16_t *s, స్థిరరాశి char16_t *t, int32_t length) {
    చేయి {
        ఒకవేళ(*s++!=*t++) {
            తిరిగి అబద్ధం;
        }
    } అయితే(--length>0);
    తిరిగి నిజం;
}

స్థిర అంతఃవరుస UBool
matches8(స్థిరరాశి uint8_t *s, స్థిరరాశి uint8_t *t, int32_t length) {
    చేయి {
        ఒకవేళ(*s++!=*t++) {
            తిరిగి అబద్ధం;
        }
    } అయితే(--length>0);
    తిరిగి నిజం;
}

// Compare 16-bit Unicode strings (which may be malformed UTF-16)
// at code point boundaries.
// That is, each edge of a match must not be in the middle of a surrogate pair.
స్థిర అంతఃవరుస UBool
matches16CPB(స్థిరరాశి char16_t *s, int32_t ప్రారంభం, int32_t హద్దు, స్థిరరాశి char16_t *t, int32_t length) {
    s+=ప్రారంభం;
    హద్దు-=ప్రారంభం;
    తిరిగి matches16(s, t, length) &&
           !(0<ప్రారంభం && U16_IS_LEAD(s[-1]) && U16_IS_TRAIL(s[0])) &&
           !(length<హద్దు && U16_IS_LEAD(s[length-1]) && U16_IS_TRAIL(s[length]));
}

// Does the set contain the next code point?
// If so, return its length; otherwise return its negative length.
స్థిర అంతఃవరుస int32_t
పరిధి_One(స్థిరరాశి Unicode_సమితి &సమితి, స్థిరరాశి char16_t *s, int32_t length) {
    char16_t c=*s, c2;
    ఒకవేళ(c>=0xd800 && c<=0xdbff && length>=2 && U16_IS_TRAIL(c2=s[1])) {
        తిరిగి సమితి.కలిగియుంది(U16_GET_SUPPLEMENTARY(c, c2)) ? 2 : -2;
    }
    తిరిగి సమితి.కలిగియుంది(c) ? 1 : -1;
}

స్థిర అంతఃవరుస int32_t
పరిధి_One_వెనుక(స్థిరరాశి Unicode_సమితి &సమితి, స్థిరరాశి char16_t *s, int32_t length) {
    char16_t c=s[length-1], c2;
    ఒకవేళ(c>=0xdc00 && c<=0xdfff && length>=2 && U16_IS_LEAD(c2=s[length-2])) {
        తిరిగి సమితి.కలిగియుంది(U16_GET_SUPPLEMENTARY(c2, c)) ? 2 : -2;
    }
    తిరిగి సమితి.కలిగియుంది(c) ? 1 : -1;
}

స్థిర అంతఃవరుస int32_t
పరిధి_One_UTF8(స్థిరరాశి Unicode_సమితి &సమితి, స్థిరరాశి uint8_t *s, int32_t length) {
    UChar32 c=*s;
    ఒకవేళ(U8_IS_SINGLE(c)) {
        తిరిగి సమితి.కలిగియుంది(c) ? 1 : -1;
    }
    // Take advantage of non-ASCII fastpaths in U8_NEXT_OR_FFFD().
    int32_t i=0;
    U8_తదుపరి_OR_FFFD(s, i, length, c);
    తిరిగి సమితి.కలిగియుంది(c) ? i : -i;
}

స్థిర అంతఃవరుస int32_t
పరిధి_One_వెనుక_UTF8(స్థిరరాశి Unicode_సమితి &సమితి, స్థిరరాశి uint8_t *s, int32_t length) {
    UChar32 c=s[length-1];
    ఒకవేళ(U8_IS_SINGLE(c)) {
        తిరిగి సమితి.కలిగియుంది(c) ? 1 : -1;
    }
    int32_t i=length-1;
    c=utf8_prev_Char_సురక్షిత_Body(s, 0, &i, c, -3);
    length-=i;
    తిరిగి సమితి.కలిగియుంది(c) ? length : -length;
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

int32_t Unicode_సమితి_String_పరిధి::పరిధి(స్థిరరాశి char16_t *s, int32_t length, USet_పరిధి_Condition పరిధి_Condition) స్థిరరాశి {
    ఒకవేళ(పరిధి_Condition==USET_పరిధి_NOT_CONTAINED) {
        తిరిగి పరిధి_Not(s, length);
    }
    int32_t పరిధి_నిడివి=పరిధి_సమితి.పరిధి(s, length, USET_పరిధి_CONTAINED);
    ఒకవేళ(పరిధి_నిడివి==length) {
        తిరిగి length;
    }

    // Consider strings; they may overlap with the span.
    స్థానభ్రంశం_List offsets;
    ఒకవేళ(పరిధి_Condition==USET_పరిధి_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.సమితి_Max_నిడివి(maxLength16);
    }
    int32_t pos=పరిధి_నిడివి, rest=length-pos;
    int32_t i, అక్షరశ్రేణులు_నిడివి=అక్షరశ్రేణులు.size();
    కొరకు(;;) {
        ఒకవేళ(పరిధి_Condition==USET_పరిధి_CONTAINED) {
            కొరకు(i=0; i<అక్షరశ్రేణులు_నిడివి; ++i) {
                int32_t overlap=పరిధి_Lengths[i];
                ఒకవేళ(overlap==ALL_CP_CONTAINED) {
                    కొనసాగు;  // Irrelevant string. (Also the empty string.)
                }
                స్థిరరాశి UnicodeString& string = *static_cast<స్థిరరాశి UnicodeString*>(అక్షరశ్రేణులు.elementAt(i));
                స్థిరరాశి char16_t *s16=string.get_బఫరు();
                int32_t length16=string.length();
                U_ASSERT(length>0);

                // Try to match this string at pos-overlap..pos.
                ఒకవేళ(overlap>=LONG_పరిధి) {
                    overlap=length16;
                    // While contained: No point matching fully inside the code point span.
                    U16_వెనుక_1(s16, 0, overlap);  // Length of the string minus the last code point.
                }
                ఒకవేళ(overlap>పరిధి_నిడివి) {
                    overlap=పరిధి_నిడివి;
                }
                int32_t inc=length16-overlap;  // Keep overlap+inc==length16.
                కొరకు(;;) {
                    ఒకవేళ(inc>rest) {
                        ఆపు;
                    }
                    // Try to match if the increment is not listed already.
                    ఒకవేళ(!offsets.కలిగియుంది_స్థానభ్రంశం(inc) && matches16CPB(s, pos-overlap, length, s16, length16)) {
                        ఒకవేళ(inc==rest) {
                            తిరిగి length;  // Reached the end of the string.
                        }
                        offsets.కూడు_స్థానభ్రంశం(inc);
                    }
                    ఒకవేళ(overlap==0) {
                        ఆపు;
                    }
                    --overlap;
                    ++inc;
                }
            }
        } లేకపోతే /* USET_SPAN_SIMPLE */ {
            int32_t maxInc=0, maxOverlap=0;
            కొరకు(i=0; i<అక్షరశ్రేణులు_నిడివి; ++i) {
                int32_t overlap=పరిధి_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the earliest start.

                స్థిరరాశి UnicodeString& string = *static_cast<స్థిరరాశి UnicodeString*>(అక్షరశ్రేణులు.elementAt(i));
                స్థిరరాశి char16_t *s16=string.get_బఫరు();
                int32_t length16=string.length();
                ఒకవేళ (length16==0) {
                    కొనసాగు;  // skip the empty string
                }

                // Try to match this string at pos-overlap..pos.
                ఒకవేళ(overlap>=LONG_పరిధి) {
                    overlap=length16;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the earliest start.
                }
                ఒకవేళ(overlap>పరిధి_నిడివి) {
                    overlap=పరిధి_నిడివి;
                }
                int32_t inc=length16-overlap;  // Keep overlap+inc==length16.
                కొరకు(;;) {
                    ఒకవేళ(inc>rest || overlap<maxOverlap) {
                        ఆపు;
                    }
                    // Try to match if the string is longer or starts earlier.
                    ఒకవేళ( (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ inc>maxInc) &&
                        matches16CPB(s, pos-overlap, length, s16, length16)
                    ) {
                        maxInc=inc;  // Longest match from earliest start.
                        maxOverlap=overlap;
                        ఆపు;
                    }
                    --overlap;
                    ++inc;
                }
            }

            ఒకవేళ(maxInc!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue after it.
                pos+=maxInc;
                rest-=maxInc;
                ఒకవేళ(rest==0) {
                    తిరిగి length;  // Reached the end of the string.
                }
                పరిధి_నిడివి=0;  // Match strings from after a string match.
                కొనసాగు;
            }
        }
        // Finished trying to match all strings at pos.

        ఒకవేళ(పరిధి_నిడివి!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            ఒకవేళ(offsets.is_ఖాళీలు()) {
                తిరిగి pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } లేకపోతే {
            // The position is after a string match (or a single code point).
            ఒకవేళ(offsets.is_ఖాళీలు()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                పరిధి_నిడివి=పరిధి_సమితి.పరిధి(s+pos, rest, USET_పరిధి_CONTAINED);
                ఒకవేళ( పరిధి_నిడివి==rest || // Reached the end of the string, or
                    పరిధి_నిడివి==0       // neither strings nor span progressed.
                ) {
                    తిరిగి pos+పరిధి_నిడివి;
                }
                pos+=పరిధి_నిడివి;
                rest-=పరిధి_నిడివి;
                కొనసాగు;  // spanLength>0: Match strings from after a span.
            } లేకపోతే {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                పరిధి_నిడివి=పరిధి_One(పరిధి_సమితి, s+pos, rest);
                ఒకవేళ(పరిధి_నిడివి>0) {
                    ఒకవేళ(పరిధి_నిడివి==rest) {
                        తిరిగి length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=పరిధి_నిడివి;
                    rest-=పరిధి_నిడివి;
                    offsets.జరుపు(పరిధి_నిడివి);
                    పరిధి_నిడివి=0;
                    కొనసాగు;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t min_స్థానభ్రంశం=offsets.తీయి_Minimum();
        pos+=min_స్థానభ్రంశం;
        rest-=min_స్థానభ్రంశం;
        పరిధి_నిడివి=0;  // Match strings from after a string match.
    }
}

int32_t Unicode_సమితి_String_పరిధి::పరిధి_వెనుక(స్థిరరాశి char16_t *s, int32_t length, USet_పరిధి_Condition పరిధి_Condition) స్థిరరాశి {
    ఒకవేళ(పరిధి_Condition==USET_పరిధి_NOT_CONTAINED) {
        తిరిగి పరిధి_Not_వెనుక(s, length);
    }
    int32_t pos=పరిధి_సమితి.పరిధి_వెనుక(s, length, USET_పరిధి_CONTAINED);
    ఒకవేళ(pos==0) {
        తిరిగి 0;
    }
    int32_t పరిధి_నిడివి=length-pos;

    // Consider strings; they may overlap with the span.
    స్థానభ్రంశం_List offsets;
    ఒకవేళ(పరిధి_Condition==USET_పరిధి_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.సమితి_Max_నిడివి(maxLength16);
    }
    int32_t i, అక్షరశ్రేణులు_నిడివి=అక్షరశ్రేణులు.size();
    uint8_t *పరిధి_వెనుక_Lengths=పరిధి_Lengths;
    ఒకవేళ(all) {
        పరిధి_వెనుక_Lengths+=అక్షరశ్రేణులు_నిడివి;
    }
    కొరకు(;;) {
        ఒకవేళ(పరిధి_Condition==USET_పరిధి_CONTAINED) {
            కొరకు(i=0; i<అక్షరశ్రేణులు_నిడివి; ++i) {
                int32_t overlap=పరిధి_వెనుక_Lengths[i];
                ఒకవేళ(overlap==ALL_CP_CONTAINED) {
                    కొనసాగు;  // Irrelevant string. (Also the empty string.)
                }
                స్థిరరాశి UnicodeString& string = *static_cast<స్థిరరాశి UnicodeString*>(అక్షరశ్రేణులు.elementAt(i));
                స్థిరరాశి char16_t *s16=string.get_బఫరు();
                int32_t length16=string.length();
                U_ASSERT(length>0);

                // Try to match this string at pos-(length16-overlap)..pos-length16.
                ఒకవేళ(overlap>=LONG_పరిధి) {
                    overlap=length16;
                    // While contained: No point matching fully inside the code point span.
                    int32_t len1=0;
                    U16_FWD_1(s16, len1, overlap);
                    overlap-=len1;  // Length of the string minus the first code point.
                }
                ఒకవేళ(overlap>పరిధి_నిడివి) {
                    overlap=పరిధి_నిడివి;
                }
                int32_t dec=length16-overlap;  // Keep dec+overlap==length16.
                కొరకు(;;) {
                    ఒకవేళ(dec>pos) {
                        ఆపు;
                    }
                    // Try to match if the decrement is not listed already.
                    ఒకవేళ(!offsets.కలిగియుంది_స్థానభ్రంశం(dec) && matches16CPB(s, pos-dec, length, s16, length16)) {
                        ఒకవేళ(dec==pos) {
                            తిరిగి 0;  // Reached the start of the string.
                        }
                        offsets.కూడు_స్థానభ్రంశం(dec);
                    }
                    ఒకవేళ(overlap==0) {
                        ఆపు;
                    }
                    --overlap;
                    ++dec;
                }
            }
        } లేకపోతే /* USET_SPAN_SIMPLE */ {
            int32_t maxDec=0, maxOverlap=0;
            కొరకు(i=0; i<అక్షరశ్రేణులు_నిడివి; ++i) {
                int32_t overlap=పరిధి_వెనుక_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                స్థిరరాశి UnicodeString& string = *static_cast<స్థిరరాశి UnicodeString*>(అక్షరశ్రేణులు.elementAt(i));
                స్థిరరాశి char16_t *s16=string.get_బఫరు();
                int32_t length16=string.length();
                ఒకవేళ (length16==0) {
                    కొనసాగు;  // skip the empty string
                }

                // Try to match this string at pos-(length16-overlap)..pos-length16.
                ఒకవేళ(overlap>=LONG_పరిధి) {
                    overlap=length16;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the latest end.
                }
                ఒకవేళ(overlap>పరిధి_నిడివి) {
                    overlap=పరిధి_నిడివి;
                }
                int32_t dec=length16-overlap;  // Keep dec+overlap==length16.
                కొరకు(;;) {
                    ఒకవేళ(dec>pos || overlap<maxOverlap) {
                        ఆపు;
                    }
                    // Try to match if the string is longer or ends later.
                    ఒకవేళ( (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ dec>maxDec) &&
                        matches16CPB(s, pos-dec, length, s16, length16)
                    ) {
                        maxDec=dec;  // Longest match from latest end.
                        maxOverlap=overlap;
                        ఆపు;
                    }
                    --overlap;
                    ++dec;
                }
            }

            ఒకవేళ(maxDec!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue before it.
                pos-=maxDec;
                ఒకవేళ(pos==0) {
                    తిరిగి 0;  // Reached the start of the string.
                }
                పరిధి_నిడివి=0;  // Match strings from before a string match.
                కొనసాగు;
            }
        }
        // Finished trying to match all strings at pos.

        ఒకవేళ(పరిధి_నిడివి!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            ఒకవేళ(offsets.is_ఖాళీలు()) {
                తిరిగి pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } లేకపోతే {
            // The position is before a string match (or a single code point).
            ఒకవేళ(offsets.is_ఖాళీలు()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos=పరిధి_సమితి.పరిధి_వెనుక(s, oldPos, USET_పరిధి_CONTAINED);
                పరిధి_నిడివి=oldPos-pos;
                ఒకవేళ( pos==0 ||           // Reached the start of the string, or
                    పరిధి_నిడివి==0       // neither strings nor span progressed.
                ) {
                    తిరిగి pos;
                }
                కొనసాగు;  // spanLength>0: Match strings from before a span.
            } లేకపోతే {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                పరిధి_నిడివి=పరిధి_One_వెనుక(పరిధి_సమితి, s, pos);
                ఒకవేళ(పరిధి_నిడివి>0) {
                    ఒకవేళ(పరిధి_నిడివి==pos) {
                        తిరిగి 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=పరిధి_నిడివి;
                    offsets.జరుపు(పరిధి_నిడివి);
                    పరిధి_నిడివి=0;
                    కొనసాగు;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.తీయి_Minimum();
        పరిధి_నిడివి=0;  // Match strings from before a string match.
    }
}

int32_t Unicode_సమితి_String_పరిధి::పరిధి_UTF8(స్థిరరాశి uint8_t *s, int32_t length, USet_పరిధి_Condition పరిధి_Condition) స్థిరరాశి {
    ఒకవేళ(పరిధి_Condition==USET_పరిధి_NOT_CONTAINED) {
        తిరిగి పరిధి_Not_UTF8(s, length);
    }
    int32_t పరిధి_నిడివి = పరిధి_సమితి.పరిధి_UTF8(reinterpret_cast<స్థిరరాశి అక్షరం*>(s), length, USET_పరిధి_CONTAINED);
    ఒకవేళ(పరిధి_నిడివి==length) {
        తిరిగి length;
    }

    // Consider strings; they may overlap with the span.
    స్థానభ్రంశం_List offsets;
    ఒకవేళ(పరిధి_Condition==USET_పరిధి_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.సమితి_Max_నిడివి(maxLength8);
    }
    int32_t pos=పరిధి_నిడివి, rest=length-pos;
    int32_t i, అక్షరశ్రేణులు_నిడివి=అక్షరశ్రేణులు.size();
    uint8_t *పరిధి_UTF8_Lengths=పరిధి_Lengths;
    ఒకవేళ(all) {
        పరిధి_UTF8_Lengths+=2*అక్షరశ్రేణులు_నిడివి;
    }
    కొరకు(;;) {
        స్థిరరాశి uint8_t *s8=utf8;
        int32_t length8;
        ఒకవేళ(పరిధి_Condition==USET_పరిధి_CONTAINED) {
            కొరకు(i=0; i<అక్షరశ్రేణులు_నిడివి; ++i) {
                length8=utf8Lengths[i];
                ఒకవేళ(length8==0) {
                    కొనసాగు;  // String not representable in UTF-8.
                }
                int32_t overlap=పరిధి_UTF8_Lengths[i];
                ఒకవేళ(overlap==ALL_CP_CONTAINED) {
                    s8+=length8;
                    కొనసాగు;  // Irrelevant string.
                }

                // Try to match this string at pos-overlap..pos.
                ఒకవేళ(overlap>=LONG_పరిధి) {
                    overlap=length8;
                    // While contained: No point matching fully inside the code point span.
                    U8_వెనుక_1(s8, 0, overlap);  // Length of the string minus the last code point.
                }
                ఒకవేళ(overlap>పరిధి_నిడివి) {
                    overlap=పరిధి_నిడివి;
                }
                int32_t inc=length8-overlap;  // Keep overlap+inc==length8.
                కొరకు(;;) {
                    ఒకవేళ(inc>rest) {
                        ఆపు;
                    }
                    // Try to match if the increment is not listed already.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    ఒకవేళ(!U8_IS_TRAIL(s[pos-overlap]) &&
                            !offsets.కలిగియుంది_స్థానభ్రంశం(inc) &&
                            matches8(s+pos-overlap, s8, length8)) {
                        ఒకవేళ(inc==rest) {
                            తిరిగి length;  // Reached the end of the string.
                        }
                        offsets.కూడు_స్థానభ్రంశం(inc);
                    }
                    ఒకవేళ(overlap==0) {
                        ఆపు;
                    }
                    --overlap;
                    ++inc;
                }
                s8+=length8;
            }
        } లేకపోతే /* USET_SPAN_SIMPLE */ {
            int32_t maxInc=0, maxOverlap=0;
            కొరకు(i=0; i<అక్షరశ్రేణులు_నిడివి; ++i) {
                length8=utf8Lengths[i];
                ఒకవేళ(length8==0) {
                    కొనసాగు;  // String not representable in UTF-8.
                }
                int32_t overlap=పరిధి_UTF8_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the earliest start.

                // Try to match this string at pos-overlap..pos.
                ఒకవేళ(overlap>=LONG_పరిధి) {
                    overlap=length8;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the earliest start.
                }
                ఒకవేళ(overlap>పరిధి_నిడివి) {
                    overlap=పరిధి_నిడివి;
                }
                int32_t inc=length8-overlap;  // Keep overlap+inc==length8.
                కొరకు(;;) {
                    ఒకవేళ(inc>rest || overlap<maxOverlap) {
                        ఆపు;
                    }
                    // Try to match if the string is longer or starts earlier.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    ఒకవేళ(!U8_IS_TRAIL(s[pos-overlap]) &&
                            (overlap>maxOverlap ||
                                /* redundant overlap==maxOverlap && */ inc>maxInc) &&
                            matches8(s+pos-overlap, s8, length8)) {
                        maxInc=inc;  // Longest match from earliest start.
                        maxOverlap=overlap;
                        ఆపు;
                    }
                    --overlap;
                    ++inc;
                }
                s8+=length8;
            }

            ఒకవేళ(maxInc!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue after it.
                pos+=maxInc;
                rest-=maxInc;
                ఒకవేళ(rest==0) {
                    తిరిగి length;  // Reached the end of the string.
                }
                పరిధి_నిడివి=0;  // Match strings from after a string match.
                కొనసాగు;
            }
        }
        // Finished trying to match all strings at pos.

        ఒకవేళ(పరిధి_నిడివి!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            ఒకవేళ(offsets.is_ఖాళీలు()) {
                తిరిగి pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } లేకపోతే {
            // The position is after a string match (or a single code point).
            ఒకవేళ(offsets.is_ఖాళీలు()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                పరిధి_నిడివి = పరిధి_సమితి.పరిధి_UTF8(reinterpret_cast<స్థిరరాశి అక్షరం*>(s) + pos, rest, USET_పరిధి_CONTAINED);
                ఒకవేళ( పరిధి_నిడివి==rest || // Reached the end of the string, or
                    పరిధి_నిడివి==0       // neither strings nor span progressed.
                ) {
                    తిరిగి pos+పరిధి_నిడివి;
                }
                pos+=పరిధి_నిడివి;
                rest-=పరిధి_నిడివి;
                కొనసాగు;  // spanLength>0: Match strings from after a span.
            } లేకపోతే {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                పరిధి_నిడివి=పరిధి_One_UTF8(పరిధి_సమితి, s+pos, rest);
                ఒకవేళ(పరిధి_నిడివి>0) {
                    ఒకవేళ(పరిధి_నిడివి==rest) {
                        తిరిగి length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=పరిధి_నిడివి;
                    rest-=పరిధి_నిడివి;
                    offsets.జరుపు(పరిధి_నిడివి);
                    పరిధి_నిడివి=0;
                    కొనసాగు;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t min_స్థానభ్రంశం=offsets.తీయి_Minimum();
        pos+=min_స్థానభ్రంశం;
        rest-=min_స్థానభ్రంశం;
        పరిధి_నిడివి=0;  // Match strings from after a string match.
    }
}

int32_t Unicode_సమితి_String_పరిధి::పరిధి_వెనుక_UTF8(స్థిరరాశి uint8_t *s, int32_t length, USet_పరిధి_Condition పరిధి_Condition) స్థిరరాశి {
    ఒకవేళ(పరిధి_Condition==USET_పరిధి_NOT_CONTAINED) {
        తిరిగి పరిధి_Not_వెనుక_UTF8(s, length);
    }
    int32_t pos = పరిధి_సమితి.పరిధి_వెనుక_UTF8(reinterpret_cast<స్థిరరాశి అక్షరం*>(s), length, USET_పరిధి_CONTAINED);
    ఒకవేళ(pos==0) {
        తిరిగి 0;
    }
    int32_t పరిధి_నిడివి=length-pos;

    // Consider strings; they may overlap with the span.
    స్థానభ్రంశం_List offsets;
    ఒకవేళ(పరిధి_Condition==USET_పరిధి_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.సమితి_Max_నిడివి(maxLength8);
    }
    int32_t i, అక్షరశ్రేణులు_నిడివి=అక్షరశ్రేణులు.size();
    uint8_t *పరిధి_వెనుక_UTF8_Lengths=పరిధి_Lengths;
    ఒకవేళ(all) {
        పరిధి_వెనుక_UTF8_Lengths+=3*అక్షరశ్రేణులు_నిడివి;
    }
    కొరకు(;;) {
        స్థిరరాశి uint8_t *s8=utf8;
        int32_t length8;
        ఒకవేళ(పరిధి_Condition==USET_పరిధి_CONTAINED) {
            కొరకు(i=0; i<అక్షరశ్రేణులు_నిడివి; ++i) {
                length8=utf8Lengths[i];
                ఒకవేళ(length8==0) {
                    కొనసాగు;  // String not representable in UTF-8.
                }
                int32_t overlap=పరిధి_వెనుక_UTF8_Lengths[i];
                ఒకవేళ(overlap==ALL_CP_CONTAINED) {
                    s8+=length8;
                    కొనసాగు;  // Irrelevant string.
                }

                // Try to match this string at pos-(length8-overlap)..pos-length8.
                ఒకవేళ(overlap>=LONG_పరిధి) {
                    overlap=length8;
                    // While contained: No point matching fully inside the code point span.
                    int32_t len1=0;
                    U8_FWD_1(s8, len1, overlap);
                    overlap-=len1;  // Length of the string minus the first code point.
                }
                ఒకవేళ(overlap>పరిధి_నిడివి) {
                    overlap=పరిధి_నిడివి;
                }
                int32_t dec=length8-overlap;  // Keep dec+overlap==length8.
                కొరకు(;;) {
                    ఒకవేళ(dec>pos) {
                        ఆపు;
                    }
                    // Try to match if the decrement is not listed already.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    ఒకవేళ( !U8_IS_TRAIL(s[pos-dec]) &&
                        !offsets.కలిగియుంది_స్థానభ్రంశం(dec) &&
                        matches8(s+pos-dec, s8, length8)
                    ) {
                        ఒకవేళ(dec==pos) {
                            తిరిగి 0;  // Reached the start of the string.
                        }
                        offsets.కూడు_స్థానభ్రంశం(dec);
                    }
                    ఒకవేళ(overlap==0) {
                        ఆపు;
                    }
                    --overlap;
                    ++dec;
                }
                s8+=length8;
            }
        } లేకపోతే /* USET_SPAN_SIMPLE */ {
            int32_t maxDec=0, maxOverlap=0;
            కొరకు(i=0; i<అక్షరశ్రేణులు_నిడివి; ++i) {
                length8=utf8Lengths[i];
                ఒకవేళ(length8==0) {
                    కొనసాగు;  // String not representable in UTF-8.
                }
                int32_t overlap=పరిధి_వెనుక_UTF8_Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                // Try to match this string at pos-(length8-overlap)..pos-length8.
                ఒకవేళ(overlap>=LONG_పరిధి) {
                    overlap=length8;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the latest end.
                }
                ఒకవేళ(overlap>పరిధి_నిడివి) {
                    overlap=పరిధి_నిడివి;
                }
                int32_t dec=length8-overlap;  // Keep dec+overlap==length8.
                కొరకు(;;) {
                    ఒకవేళ(dec>pos || overlap<maxOverlap) {
                        ఆపు;
                    }
                    // Try to match if the string is longer or ends later.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    ఒకవేళ( !U8_IS_TRAIL(s[pos-dec]) &&
                        (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ dec>maxDec) &&
                        matches8(s+pos-dec, s8, length8)
                    ) {
                        maxDec=dec;  // Longest match from latest end.
                        maxOverlap=overlap;
                        ఆపు;
                    }
                    --overlap;
                    ++dec;
                }
                s8+=length8;
            }

            ఒకవేళ(maxDec!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue before it.
                pos-=maxDec;
                ఒకవేళ(pos==0) {
                    తిరిగి 0;  // Reached the start of the string.
                }
                పరిధి_నిడివి=0;  // Match strings from before a string match.
                కొనసాగు;
            }
        }
        // Finished trying to match all strings at pos.

        ఒకవేళ(పరిధి_నిడివి!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            ఒకవేళ(offsets.is_ఖాళీలు()) {
                తిరిగి pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } లేకపోతే {
            // The position is before a string match (or a single code point).
            ఒకవేళ(offsets.is_ఖాళీలు()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos = పరిధి_సమితి.పరిధి_వెనుక_UTF8(reinterpret_cast<స్థిరరాశి అక్షరం*>(s), oldPos, USET_పరిధి_CONTAINED);
                పరిధి_నిడివి=oldPos-pos;
                ఒకవేళ( pos==0 ||           // Reached the start of the string, or
                    పరిధి_నిడివి==0       // neither strings nor span progressed.
                ) {
                    తిరిగి pos;
                }
                కొనసాగు;  // spanLength>0: Match strings from before a span.
            } లేకపోతే {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                పరిధి_నిడివి=పరిధి_One_వెనుక_UTF8(పరిధి_సమితి, s, pos);
                ఒకవేళ(పరిధి_నిడివి>0) {
                    ఒకవేళ(పరిధి_నిడివి==pos) {
                        తిరిగి 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=పరిధి_నిడివి;
                    offsets.జరుపు(పరిధి_నిడివి);
                    పరిధి_నిడివి=0;
                    కొనసాగు;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.తీయి_Minimum();
        పరిధి_నిడివి=0;  // Match strings from before a string match.
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

int32_t Unicode_సమితి_String_పరిధి::పరిధి_Not(స్థిరరాశి char16_t *s, int32_t length) స్థిరరాశి {
    int32_t pos=0, rest=length;
    int32_t i, అక్షరశ్రేణులు_నిడివి=అక్షరశ్రేణులు.size();
    చేయి {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i=p_పరిధి_Not_సమితి->పరిధి(s+pos, rest, USET_పరిధి_NOT_CONTAINED);
        ఒకవేళ(i==rest) {
            తిరిగి length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_నిడివి=పరిధి_One(పరిధి_సమితి, s+pos, rest);
        ఒకవేళ(cp_నిడివి>0) {
            తిరిగి pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        కొరకు(i=0; i<అక్షరశ్రేణులు_నిడివి; ++i) {
            ఒకవేళ(పరిధి_Lengths[i]==ALL_CP_CONTAINED) {
                కొనసాగు;  // Irrelevant string. (Also the empty string.)
            }
            స్థిరరాశి UnicodeString& string = *static_cast<స్థిరరాశి UnicodeString*>(అక్షరశ్రేణులు.elementAt(i));
            స్థిరరాశి char16_t *s16=string.get_బఫరు();
            int32_t length16=string.length();
            U_ASSERT(length>0);
            ఒకవేళ(length16<=rest && matches16CPB(s, pos, length, s16, length16)) {
                తిరిగి pos;  // There is a set element at pos.
            }
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos-=cp_నిడివి;
        rest+=cp_నిడివి;
    } అయితే(rest!=0);
    తిరిగి length;  // Reached the end of the string.
}

int32_t Unicode_సమితి_String_పరిధి::పరిధి_Not_వెనుక(స్థిరరాశి char16_t *s, int32_t length) స్థిరరాశి {
    int32_t pos=length;
    int32_t i, అక్షరశ్రేణులు_నిడివి=అక్షరశ్రేణులు.size();
    చేయి {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos=p_పరిధి_Not_సమితి->పరిధి_వెనుక(s, pos, USET_పరిధి_NOT_CONTAINED);
        ఒకవేళ(pos==0) {
            తిరిగి 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_నిడివి=పరిధి_One_వెనుక(పరిధి_సమితి, s, pos);
        ఒకవేళ(cp_నిడివి>0) {
            తిరిగి pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        కొరకు(i=0; i<అక్షరశ్రేణులు_నిడివి; ++i) {
            // Use spanLengths rather than a spanBackLengths pointer because
            // it is easier and we only need to know whether the string is irrelevant
            // which is the same in either array.
            ఒకవేళ(పరిధి_Lengths[i]==ALL_CP_CONTAINED) {
                కొనసాగు;  // Irrelevant string. (Also the empty string.)
            }
            స్థిరరాశి UnicodeString& string = *static_cast<స్థిరరాశి UnicodeString*>(అక్షరశ్రేణులు.elementAt(i));
            స్థిరరాశి char16_t *s16=string.get_బఫరు();
            int32_t length16=string.length();
            U_ASSERT(length>0);
            ఒకవేళ(length16<=pos && matches16CPB(s, pos-length16, length, s16, length16)) {
                తిరిగి pos;  // There is a set element at pos.
            }
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos+=cp_నిడివి;
    } అయితే(pos!=0);
    తిరిగి 0;  // Reached the start of the string.
}

int32_t Unicode_సమితి_String_పరిధి::పరిధి_Not_UTF8(స్థిరరాశి uint8_t *s, int32_t length) స్థిరరాశి {
    int32_t pos=0, rest=length;
    int32_t i, అక్షరశ్రేణులు_నిడివి=అక్షరశ్రేణులు.size();
    uint8_t *పరిధి_UTF8_Lengths=పరిధి_Lengths;
    ఒకవేళ(all) {
        పరిధి_UTF8_Lengths+=2*అక్షరశ్రేణులు_నిడివి;
    }
    చేయి {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i = p_పరిధి_Not_సమితి->పరిధి_UTF8(reinterpret_cast<స్థిరరాశి అక్షరం*>(s) + pos, rest, USET_పరిధి_NOT_CONTAINED);
        ఒకవేళ(i==rest) {
            తిరిగి length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_నిడివి=పరిధి_One_UTF8(పరిధి_సమితి, s+pos, rest);
        ఒకవేళ(cp_నిడివి>0) {
            తిరిగి pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        స్థిరరాశి uint8_t *s8=utf8;
        int32_t length8;
        కొరకు(i=0; i<అక్షరశ్రేణులు_నిడివి; ++i) {
            length8=utf8Lengths[i];
            // ALL_CP_CONTAINED: Irrelevant string.
            ఒకవేళ(length8!=0 && పరిధి_UTF8_Lengths[i]!=ALL_CP_CONTAINED && length8<=rest && matches8(s+pos, s8, length8)) {
                తిరిగి pos;  // There is a set element at pos.
            }
            s8+=length8;
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos-=cp_నిడివి;
        rest+=cp_నిడివి;
    } అయితే(rest!=0);
    తిరిగి length;  // Reached the end of the string.
}

int32_t Unicode_సమితి_String_పరిధి::పరిధి_Not_వెనుక_UTF8(స్థిరరాశి uint8_t *s, int32_t length) స్థిరరాశి {
    int32_t pos=length;
    int32_t i, అక్షరశ్రేణులు_నిడివి=అక్షరశ్రేణులు.size();
    uint8_t *పరిధి_వెనుక_UTF8_Lengths=పరిధి_Lengths;
    ఒకవేళ(all) {
        పరిధి_వెనుక_UTF8_Lengths+=3*అక్షరశ్రేణులు_నిడివి;
    }
    చేయి {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos = p_పరిధి_Not_సమితి->పరిధి_వెనుక_UTF8(reinterpret_cast<స్థిరరాశి అక్షరం*>(s), pos, USET_పరిధి_NOT_CONTAINED);
        ఒకవేళ(pos==0) {
            తిరిగి 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_నిడివి=పరిధి_One_వెనుక_UTF8(పరిధి_సమితి, s, pos);
        ఒకవేళ(cp_నిడివి>0) {
            తిరిగి pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        స్థిరరాశి uint8_t *s8=utf8;
        int32_t length8;
        కొరకు(i=0; i<అక్షరశ్రేణులు_నిడివి; ++i) {
            length8=utf8Lengths[i];
            // ALL_CP_CONTAINED: Irrelevant string.
            ఒకవేళ(length8!=0 && పరిధి_వెనుక_UTF8_Lengths[i]!=ALL_CP_CONTAINED && length8<=pos && matches8(s+pos-length8, s8, length8)) {
                తిరిగి pos;  // There is a set element at pos.
            }
            s8+=length8;
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos+=cp_నిడివి;
    } అయితే(pos!=0);
    తిరిగి 0;  // Reached the start of the string.
}

U_NAMESPACE_చివర
