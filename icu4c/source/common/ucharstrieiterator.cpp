// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  ucharstrieiterator.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010nov15
*   created by: Markus W. Scherer
*/

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ucharstrie.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "uvectr32.h"

U_NAMESPACE_BEGIN

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t max_String_दीर्घता,
                               UError_संहिता &error_संहिता)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          remaining_मेलनम्_दीर्घता(-1), initial_Remaining_मेलनम्_दीर्घता(-1),
          skip_मानम्_2(असत्य),
          max_दीर्घता(max_String_दीर्घता), मानम्(0), राशिः(शून्यसूचकः) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    राशिः=नूतन UVector32(error_संहिता);
    यदि(राशिः==शून्यसूचकः) {
        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(ध्रुव UCharsTrie &trie, int32_t max_String_दीर्घता,
                               UError_संहिता &error_संहिता)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          remaining_मेलनम्_दीर्घता(trie.remaining_मेलनम्_दीर्घता),
          initial_Remaining_मेलनम्_दीर्घता(trie.remaining_मेलनम्_दीर्घता),
          skip_मानम्_2(असत्य),
          max_दीर्घता(max_String_दीर्घता), मानम्(0), राशिः(शून्यसूचकः) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    राशिः=नूतन UVector32(error_संहिता);
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    यदि(राशिः==शून्यसूचकः) {
        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    int32_t length=remaining_मेलनम्_दीर्घता;  // Actual remaining match length minus 1.
    यदि(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        यदि(max_दीर्घता>0 && length>max_दीर्घता) {
            length=max_दीर्घता;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        remaining_मेलनम्_दीर्घता-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    विलोपय राशिः;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remaining_मेलनम्_दीर्घता=initial_Remaining_मेलनम्_दीर्घता;
    skip_मानम्_2=असत्य;
    int32_t length=remaining_मेलनम्_दीर्घता+1;  // Remaining match length.
    यदि(max_दीर्घता>0 && length>max_दीर्घता) {
        length=max_दीर्घता;
    }
    str_.truncate(length);
    pos_+=length;
    remaining_मेलनम्_दीर्घता-=length;
    राशिः->समुच्चयः_आकार(0);
    प्रत्यागम *एतत्;
}

UBool
UCharsTrie::Iterator::has_अग्र() ध्रुव { प्रत्यागम pos_!=शून्यसूचकः || !राशिः->is_रिक्तानि(); }

UBool
UCharsTrie::Iterator::next(UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम असत्य;
    }
    ध्रुव char16_t *pos=pos_;
    यदि(pos==शून्यसूचकः) {
        यदि(राशिः->is_रिक्तानि()) {
            प्रत्यागम असत्य;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t राशिः_आकार=राशिः->size();
        int32_t length=राशिः->elementAti(राशिः_आकार-1);
        pos=uchars_+राशिः->elementAti(राशिः_आकार-2);
        राशिः->समुच्चयः_आकार(राशिः_आकार-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        यदि(length>1) {
            pos=branch_अग्र(pos, length, error_संहिता);
            यदि(pos==शून्यसूचकः) {
                प्रत्यागम सत्य;  // Reached a final value.
            }
        } अन्यथा {
            str_.append(*pos++);
        }
    }
    यदि(remaining_मेलनम्_दीर्घता>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        प्रत्यागम truncateAndStop();
    }
    प्रति(;;) {
        int32_t ग्रन्थिः=*pos++;
        यदि(ग्रन्थिः>=k_Min_मानम्_Lead) {
            यदि(skip_मानम्_2) {
                pos=skip_ग्रन्थिः_मानम्(pos, ग्रन्थिः);
                ग्रन्थिः&=k_ग्रन्थिः_Type_आवरणम्;
                skip_मानम्_2=असत्य;
            } अन्यथा {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(ग्रन्थिः >> 15);
                यदि(isFinal) {
                    मानम्=read_मानम्(pos, ग्रन्थिः&0x7fff);
                } अन्यथा {
                    मानम्=read_ग्रन्थिः_मानम्(pos, ग्रन्थिः);
                }
                यदि(isFinal || (max_दीर्घता>0 && str_.length()==max_दीर्घता)) {
                    pos_=शून्यसूचकः;
                } अन्यथा {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skip_मानम्_2=सत्य;
                }
                प्रत्यागम सत्य;
            }
        }
        यदि(max_दीर्घता>0 && str_.length()==max_दीर्घता) {
            प्रत्यागम truncateAndStop();
        }
        यदि(ग्रन्थिः<k_Min_रैखिकम्_मेलनम्) {
            यदि(ग्रन्थिः==0) {
                ग्रन्थिः=*pos++;
            }
            pos=branch_अग्र(pos, ग्रन्थिः+1, error_संहिता);
            यदि(pos==शून्यसूचकः) {
                प्रत्यागम सत्य;  // Reached a final value.
            }
        } अन्यथा {
            // Linear-match node, append length units to str_.
            int32_t length=ग्रन्थिः-k_Min_रैखिकम्_मेलनम्+1;
            यदि(max_दीर्घता>0 && str_.length()+length>max_दीर्घता) {
                str_.append(pos, max_दीर्घता-str_.length());
                प्रत्यागम truncateAndStop();
            }
            str_.append(pos, length);
            pos+=length;
        }
    }
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ध्रुव char16_t*
UCharsTrie::Iterator::branch_अग्र(ध्रुव char16_t* pos U_LIFETIME_BOUND,
                                 int32_t length,
                                 UError_संहिता& error_संहिता) {
    यावत्(length>k_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता) {
        ++pos;  // ignore the comparison unit
        // Push state for the greater-or-equal edge.
        राशिः->सङ्कलय_Element(static_cast<int32_t>(skipDelta(pos) - uchars_), error_संहिता);
        राशिः->सङ्कलय_Element(((length-(length>>1))<<16)|str_.length(), error_संहिता);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    char16_t trieUnit=*pos++;
    int32_t ग्रन्थिः=*pos++;
    UBool isFinal = static_cast<UBool>(ग्रन्थिः >> 15);
    int32_t value=read_मानम्(pos, ग्रन्थिः&=0x7fff);
    pos=skip_मानम्(pos, ग्रन्थिः);
    राशिः->सङ्कलय_Element(static_cast<int32_t>(pos - uchars_), error_संहिता);
    राशिः->सङ्कलय_Element(((length-1)<<16)|str_.length(), error_संहिता);
    str_.append(trieUnit);
    यदि(isFinal) {
        pos_=शून्यसूचकः;
        मानम्=value;
        प्रत्यागम शून्यसूचकः;
    } अन्यथा {
        प्रत्यागम pos+value;
    }
}

U_NAMESPACE_अन्तः
