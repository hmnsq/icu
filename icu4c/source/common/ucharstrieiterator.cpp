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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucharstrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "uvectr32.h"

U_NAMESPACE_BEGIN

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          skip_ಮೌಲ್ಯ_2(ಅಸತ್ಯ),
          maxLength_(maxStringLength), ಮೌಲ್ಯ(0), ರಾಶಿ(nullptr) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    ರಾಶಿ=ನೂತನ UVector32(errorCode);
    ಯದಿ(ರಾಶಿ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(ಧ್ರುವ UCharsTrie &trie, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          skip_ಮೌಲ್ಯ_2(ಅಸತ್ಯ),
          maxLength_(maxStringLength), ಮೌಲ್ಯ(0), ರಾಶಿ(nullptr) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ರಾಶಿ=ನೂತನ UVector32(errorCode);
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ(ರಾಶಿ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ಯದಿ(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        ಯದಿ(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    delete ರಾಶಿ;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    skip_ಮೌಲ್ಯ_2=ಅಸತ್ಯ;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    ಯದಿ(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_.truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    ರಾಶಿ->setSize(0);
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

UBool
UCharsTrie::Iterator::hasNext() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ pos_!=nullptr || !ರಾಶಿ->isEmpty(); }

UBool
UCharsTrie::Iterator::next(UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಧ್ರುವ char16_t *pos=pos_;
    ಯದಿ(pos==nullptr) {
        ಯದಿ(ರಾಶಿ->isEmpty()) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t ರಾಶಿ_Size=ರಾಶಿ->size();
        int32_t length=ರಾಶಿ->elementAti(ರಾಶಿ_Size-1);
        pos=uchars_+ರಾಶಿ->elementAti(ರಾಶಿ_Size-2);
        ರಾಶಿ->setSize(ರಾಶಿ_Size-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        ಯದಿ(length>1) {
            pos=branchNext(pos, length, errorCode);
            ಯದಿ(pos==nullptr) {
                ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;  // Reached a final value.
            }
        } ಅನ್ಯಥಾ {
            str_.append(*pos++);
        }
    }
    ಯದಿ(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        ಪ್ರತ್ಯಾಗಮ truncateAndStop();
    }
    ಪ್ರತಿ(;;) {
        int32_t ಗಂಟು=*pos++;
        ಯದಿ(ಗಂಟು>=k_Min_ಮೌಲ್ಯ_Lead) {
            ಯದಿ(skip_ಮೌಲ್ಯ_2) {
                pos=skip_ಗಂಟು_ಮೌಲ್ಯ(pos, ಗಂಟು);
                ಗಂಟು&=k_ಗಂಟು_Type_Mask;
                skip_ಮೌಲ್ಯ_2=ಅಸತ್ಯ;
            } ಅನ್ಯಥಾ {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(ಗಂಟು >> 15);
                ಯದಿ(isFinal) {
                    ಮೌಲ್ಯ=read_ಮೌಲ್ಯ(pos, ಗಂಟು&0x7fff);
                } ಅನ್ಯಥಾ {
                    ಮೌಲ್ಯ=read_ಗಂಟು_ಮೌಲ್ಯ(pos, ಗಂಟು);
                }
                ಯದಿ(isFinal || (maxLength_>0 && str_.length()==maxLength_)) {
                    pos_=nullptr;
                } ಅನ್ಯಥಾ {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skip_ಮೌಲ್ಯ_2=ಸತ್ಯ;
                }
                ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
            }
        }
        ಯದಿ(maxLength_>0 && str_.length()==maxLength_) {
            ಪ್ರತ್ಯಾಗಮ truncateAndStop();
        }
        ಯದಿ(ಗಂಟು<kMinLinearMatch) {
            ಯದಿ(ಗಂಟು==0) {
                ಗಂಟು=*pos++;
            }
            pos=branchNext(pos, ಗಂಟು+1, errorCode);
            ಯದಿ(pos==nullptr) {
                ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;  // Reached a final value.
            }
        } ಅನ್ಯಥಾ {
            // Linear-match node, append length units to str_.
            int32_t length=ಗಂಟು-kMinLinearMatch+1;
            ಯದಿ(maxLength_>0 && str_.length()+length>maxLength_) {
                str_.append(pos, maxLength_-str_.length());
                ಪ್ರತ್ಯಾಗಮ truncateAndStop();
            }
            str_.append(pos, length);
            pos+=length;
        }
    }
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ಧ್ರುವ char16_t*
UCharsTrie::Iterator::branchNext(ಧ್ರುವ char16_t* pos U_LIFETIME_BOUND,
                                 int32_t length,
                                 UErrorCode& errorCode) {
    ಯಾವತ್(length>k_Max_Branch_Linear_Sub_ಗಂಟು_Length) {
        ++pos;  // ignore the comparison unit
        // Push state for the greater-or-equal edge.
        ರಾಶಿ->addElement(static_cast<int32_t>(skipDelta(pos) - uchars_), errorCode);
        ರಾಶಿ->addElement(((length-(length>>1))<<16)|str_.length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    char16_t trieUnit=*pos++;
    int32_t ಗಂಟು=*pos++;
    UBool isFinal = static_cast<UBool>(ಗಂಟು >> 15);
    int32_t value=read_ಮೌಲ್ಯ(pos, ಗಂಟು&=0x7fff);
    pos=skip_ಮೌಲ್ಯ(pos, ಗಂಟು);
    ರಾಶಿ->addElement(static_cast<int32_t>(pos - uchars_), errorCode);
    ರಾಶಿ->addElement(((length-1)<<16)|str_.length(), errorCode);
    str_.append(trieUnit);
    ಯದಿ(isFinal) {
        pos_=nullptr;
        ಮೌಲ್ಯ=value;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ pos+value;
    }
}

U_NAMESPACE_END
