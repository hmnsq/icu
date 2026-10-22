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

#ضمن "unicode/utypes.h"
#ضمن "unicode/ucharstrie.h"
#ضمن "unicode/unistr.h"
#ضمن "uvectr32.h"

U_NAMESPACE_BEGIN

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          skip_قدر_2(خطأ),
          maxLength_(maxStringLength), قدر(0), ڈھیر(nullptr) {
    إذا(U_FAILURE(errorCode)) {
        أرجع;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    ڈھیر=جديد UVector32(errorCode);
    إذا(ڈھیر==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(ثابت UCharsTrie &trie, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          skip_قدر_2(خطأ),
          maxLength_(maxStringLength), قدر(0), ڈھیر(nullptr) {
    إذا(U_FAILURE(errorCode)) {
        أرجع;
    }
    ڈھیر=جديد UVector32(errorCode);
    إذا(U_FAILURE(errorCode)) {
        أرجع;
    }
    إذا(ڈھیر==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    إذا(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        إذا(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    delete ڈھیر;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    skip_قدر_2=خطأ;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    إذا(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_.truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    ڈھیر->setSize(0);
    أرجع *هذا;
}

UBool
UCharsTrie::Iterator::hasNext() ثابت { أرجع pos_!=nullptr || !ڈھیر->isEmpty(); }

UBool
UCharsTrie::Iterator::next(UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        أرجع خطأ;
    }
    ثابت char16_t *pos=pos_;
    إذا(pos==nullptr) {
        إذا(ڈھیر->isEmpty()) {
            أرجع خطأ;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t ڈھیر_Size=ڈھیر->size();
        int32_t length=ڈھیر->elementAti(ڈھیر_Size-1);
        pos=uchars_+ڈھیر->elementAti(ڈھیر_Size-2);
        ڈھیر->setSize(ڈھیر_Size-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        إذا(length>1) {
            pos=branchNext(pos, length, errorCode);
            إذا(pos==nullptr) {
                أرجع true;  // Reached a final value.
            }
        } وإلا {
            str_.append(*pos++);
        }
    }
    إذا(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        أرجع truncateAndStop();
    }
    لكل(;;) {
        int32_t گرہ=*pos++;
        إذا(گرہ>=k_Min_قدر_Lead) {
            إذا(skip_قدر_2) {
                pos=skip_گرہ_قدر(pos, گرہ);
                گرہ&=k_گرہ_Type_Mask;
                skip_قدر_2=خطأ;
            } وإلا {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(گرہ >> 15);
                إذا(isFinal) {
                    قدر=read_قدر(pos, گرہ&0x7fff);
                } وإلا {
                    قدر=read_گرہ_قدر(pos, گرہ);
                }
                إذا(isFinal || (maxLength_>0 && str_.length()==maxLength_)) {
                    pos_=nullptr;
                } وإلا {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skip_قدر_2=true;
                }
                أرجع true;
            }
        }
        إذا(maxLength_>0 && str_.length()==maxLength_) {
            أرجع truncateAndStop();
        }
        إذا(گرہ<kMinLinearMatch) {
            إذا(گرہ==0) {
                گرہ=*pos++;
            }
            pos=branchNext(pos, گرہ+1, errorCode);
            إذا(pos==nullptr) {
                أرجع true;  // Reached a final value.
            }
        } وإلا {
            // Linear-match node, append length units to str_.
            int32_t length=گرہ-kMinLinearMatch+1;
            إذا(maxLength_>0 && str_.length()+length>maxLength_) {
                str_.append(pos, maxLength_-str_.length());
                أرجع truncateAndStop();
            }
            str_.append(pos, length);
            pos+=length;
        }
    }
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ثابت char16_t*
UCharsTrie::Iterator::branchNext(ثابت char16_t* pos U_LIFETIME_BOUND,
                                 int32_t length,
                                 UErrorCode& errorCode) {
    طالما(length>k_Max_Branch_Linear_Sub_گرہ_Length) {
        ++pos;  // ignore the comparison unit
        // Push state for the greater-or-equal edge.
        ڈھیر->addElement(static_cast<int32_t>(skipDelta(pos) - uchars_), errorCode);
        ڈھیر->addElement(((length-(length>>1))<<16)|str_.length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    char16_t trieUnit=*pos++;
    int32_t گرہ=*pos++;
    UBool isFinal = static_cast<UBool>(گرہ >> 15);
    int32_t value=read_قدر(pos, گرہ&=0x7fff);
    pos=skip_قدر(pos, گرہ);
    ڈھیر->addElement(static_cast<int32_t>(pos - uchars_), errorCode);
    ڈھیر->addElement(((length-1)<<16)|str_.length(), errorCode);
    str_.append(trieUnit);
    إذا(isFinal) {
        pos_=nullptr;
        قدر=value;
        أرجع nullptr;
    } وإلا {
        أرجع pos+value;
    }
}

U_NAMESPACE_END
