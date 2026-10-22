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

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          skip_मानम्_2(मिच्छा),
          maxLength_(maxStringLength), मानम्(0), राशिः(nullptr) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    राशिः=नूतन UVector32(errorCode);
    सचे(राशिः==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(ध्रुव UCharsTrie &trie, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          skip_मानम्_2(मिच्छा),
          maxLength_(maxStringLength), मानम्(0), राशिः(nullptr) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    राशिः=नूतन UVector32(errorCode);
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    सचे(राशिः==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    सचे(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        सचे(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    delete राशिः;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    skip_मानम्_2=मिच्छा;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    सचे(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_.truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    राशिः->setSize(0);
    पटिनिवत्त *एतत्;
}

UBool
UCharsTrie::Iterator::hasNext() ध्रुव { पटिनिवत्त pos_!=nullptr || !राशिः->isEmpty(); }

UBool
UCharsTrie::Iterator::next(UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त मिच्छा;
    }
    ध्रुव char16_t *pos=pos_;
    सचे(pos==nullptr) {
        सचे(राशिः->isEmpty()) {
            पटिनिवत्त मिच्छा;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t राशिः_Size=राशिः->size();
        int32_t length=राशिः->elementAti(राशिः_Size-1);
        pos=uchars_+राशिः->elementAti(राशिः_Size-2);
        राशिः->setSize(राशिः_Size-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        सचे(length>1) {
            pos=branchNext(pos, length, errorCode);
            सचे(pos==nullptr) {
                पटिनिवत्त सच्च;  // Reached a final value.
            }
        } अञ्ञथा {
            str_.append(*pos++);
        }
    }
    सचे(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        पटिनिवत्त truncateAndStop();
    }
    अत्थाय(;;) {
        int32_t ग्रन्थिः=*pos++;
        सचे(ग्रन्थिः>=k_Min_मानम्_Lead) {
            सचे(skip_मानम्_2) {
                pos=skip_ग्रन्थिः_मानम्(pos, ग्रन्थिः);
                ग्रन्थिः&=k_ग्रन्थिः_Type_Mask;
                skip_मानम्_2=मिच्छा;
            } अञ्ञथा {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(ग्रन्थिः >> 15);
                सचे(isFinal) {
                    मानम्=read_मानम्(pos, ग्रन्थिः&0x7fff);
                } अञ्ञथा {
                    मानम्=read_ग्रन्थिः_मानम्(pos, ग्रन्थिः);
                }
                सचे(isFinal || (maxLength_>0 && str_.length()==maxLength_)) {
                    pos_=nullptr;
                } अञ्ञथा {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skip_मानम्_2=सच्च;
                }
                पटिनिवत्त सच्च;
            }
        }
        सचे(maxLength_>0 && str_.length()==maxLength_) {
            पटिनिवत्त truncateAndStop();
        }
        सचे(ग्रन्थिः<kMinLinearMatch) {
            सचे(ग्रन्थिः==0) {
                ग्रन्थिः=*pos++;
            }
            pos=branchNext(pos, ग्रन्थिः+1, errorCode);
            सचे(pos==nullptr) {
                पटिनिवत्त सच्च;  // Reached a final value.
            }
        } अञ्ञथा {
            // Linear-match node, append length units to str_.
            int32_t length=ग्रन्थिः-kMinLinearMatch+1;
            सचे(maxLength_>0 && str_.length()+length>maxLength_) {
                str_.append(pos, maxLength_-str_.length());
                पटिनिवत्त truncateAndStop();
            }
            str_.append(pos, length);
            pos+=length;
        }
    }
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ध्रुव char16_t*
UCharsTrie::Iterator::branchNext(ध्रुव char16_t* pos U_LIFETIME_BOUND,
                                 int32_t length,
                                 UErrorCode& errorCode) {
    याव(length>k_Max_Branch_Linear_Sub_ग्रन्थिः_Length) {
        ++pos;  // ignore the comparison unit
        // Push state for the greater-or-equal edge.
        राशिः->addElement(static_cast<int32_t>(skipDelta(pos) - uchars_), errorCode);
        राशिः->addElement(((length-(length>>1))<<16)|str_.length(), errorCode);
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
    राशिः->addElement(static_cast<int32_t>(pos - uchars_), errorCode);
    राशिः->addElement(((length-1)<<16)|str_.length(), errorCode);
    str_.append(trieUnit);
    सचे(isFinal) {
        pos_=nullptr;
        मानम्=value;
        पटिनिवत्त nullptr;
    } अञ्ञथा {
        पटिनिवत्त pos+value;
    }
}

U_NAMESPACE_END
