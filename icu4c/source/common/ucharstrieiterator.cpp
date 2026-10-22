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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucharstrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "uvectr32.h"

U_NAMESPACE_BEGIN

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          skip_ମୂଲ୍ୟ_2(ମିଥ୍ୟା),
          maxLength_(maxStringLength), ମୂଲ୍ୟ(0), ଗଦା(nullptr) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    ଗଦା=ନୂତନ UVector32(errorCode);
    ଯଦି(ଗଦା==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(ଅଚଳ UCharsTrie &trie, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          skip_ମୂଲ୍ୟ_2(ମିଥ୍ୟା),
          maxLength_(maxStringLength), ମୂଲ୍ୟ(0), ଗଦା(nullptr) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଗଦା=ନୂତନ UVector32(errorCode);
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି(ଗଦା==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ଯଦି(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        ଯଦି(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    delete ଗଦା;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    skip_ମୂଲ୍ୟ_2=ମିଥ୍ୟା;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    ଯଦି(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_.truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    ଗଦା->setSize(0);
    ଫେରନ୍ତୁ *ଏହି;
}

UBool
UCharsTrie::Iterator::hasNext() ଅଚଳ { ଫେରନ୍ତୁ pos_!=nullptr || !ଗଦା->isEmpty(); }

UBool
UCharsTrie::Iterator::next(UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଅଚଳ char16_t *pos=pos_;
    ଯଦି(pos==nullptr) {
        ଯଦି(ଗଦା->isEmpty()) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t ଗଦା_Size=ଗଦା->size();
        int32_t length=ଗଦା->elementAti(ଗଦା_Size-1);
        pos=uchars_+ଗଦା->elementAti(ଗଦା_Size-2);
        ଗଦା->setSize(ଗଦା_Size-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        ଯଦି(length>1) {
            pos=branchNext(pos, length, errorCode);
            ଯଦି(pos==nullptr) {
                ଫେରନ୍ତୁ ସତ୍ୟ;  // Reached a final value.
            }
        } ଅନ୍ୟଥା {
            str_.append(*pos++);
        }
    }
    ଯଦି(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        ଫେରନ୍ତୁ truncateAndStop();
    }
    ପାଇଁ(;;) {
        int32_t ଗଣ୍ଠି=*pos++;
        ଯଦି(ଗଣ୍ଠି>=k_Min_ମୂଲ୍ୟ_Lead) {
            ଯଦି(skip_ମୂଲ୍ୟ_2) {
                pos=skip_ଗଣ୍ଠି_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି);
                ଗଣ୍ଠି&=k_ଗଣ୍ଠି_Type_Mask;
                skip_ମୂଲ୍ୟ_2=ମିଥ୍ୟା;
            } ଅନ୍ୟଥା {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(ଗଣ୍ଠି >> 15);
                ଯଦି(isFinal) {
                    ମୂଲ୍ୟ=read_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି&0x7fff);
                } ଅନ୍ୟଥା {
                    ମୂଲ୍ୟ=read_ଗଣ୍ଠି_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି);
                }
                ଯଦି(isFinal || (maxLength_>0 && str_.length()==maxLength_)) {
                    pos_=nullptr;
                } ଅନ୍ୟଥା {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skip_ମୂଲ୍ୟ_2=ସତ୍ୟ;
                }
                ଫେରନ୍ତୁ ସତ୍ୟ;
            }
        }
        ଯଦି(maxLength_>0 && str_.length()==maxLength_) {
            ଫେରନ୍ତୁ truncateAndStop();
        }
        ଯଦି(ଗଣ୍ଠି<kMinLinearMatch) {
            ଯଦି(ଗଣ୍ଠି==0) {
                ଗଣ୍ଠି=*pos++;
            }
            pos=branchNext(pos, ଗଣ୍ଠି+1, errorCode);
            ଯଦି(pos==nullptr) {
                ଫେରନ୍ତୁ ସତ୍ୟ;  // Reached a final value.
            }
        } ଅନ୍ୟଥା {
            // Linear-match node, append length units to str_.
            int32_t length=ଗଣ୍ଠି-kMinLinearMatch+1;
            ଯଦି(maxLength_>0 && str_.length()+length>maxLength_) {
                str_.append(pos, maxLength_-str_.length());
                ଫେରନ୍ତୁ truncateAndStop();
            }
            str_.append(pos, length);
            pos+=length;
        }
    }
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ଅଚଳ char16_t*
UCharsTrie::Iterator::branchNext(ଅଚଳ char16_t* pos U_LIFETIME_BOUND,
                                 int32_t length,
                                 UErrorCode& errorCode) {
    ଯେତେବେଳେ(length>k_Max_Branch_Linear_Sub_ଗଣ୍ଠି_Length) {
        ++pos;  // ignore the comparison unit
        // Push state for the greater-or-equal edge.
        ଗଦା->addElement(static_cast<int32_t>(skipDelta(pos) - uchars_), errorCode);
        ଗଦା->addElement(((length-(length>>1))<<16)|str_.length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    char16_t trieUnit=*pos++;
    int32_t ଗଣ୍ଠି=*pos++;
    UBool isFinal = static_cast<UBool>(ଗଣ୍ଠି >> 15);
    int32_t value=read_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି&=0x7fff);
    pos=skip_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି);
    ଗଦା->addElement(static_cast<int32_t>(pos - uchars_), errorCode);
    ଗଦା->addElement(((length-1)<<16)|str_.length(), errorCode);
    str_.append(trieUnit);
    ଯଦି(isFinal) {
        pos_=nullptr;
        ମୂଲ୍ୟ=value;
        ଫେରନ୍ତୁ nullptr;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ pos+value;
    }
}

U_NAMESPACE_END
