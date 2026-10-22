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
          skipValue_(ܕܓܠܐ),
          maxLength_(maxStringLength), value_(0), stack_(nullptr) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    stack_=جديد UVector32(errorCode);
    ܐܢ(stack_==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(ثابت UCharsTrie &trie, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          skipValue_(ܕܓܠܐ),
          maxLength_(maxStringLength), value_(0), stack_(nullptr) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ;
    }
    stack_=جديد UVector32(errorCode);
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ;
    }
    ܐܢ(stack_==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ܐܢ(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        ܐܢ(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    delete stack_;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    skipValue_=ܕܓܠܐ;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    ܐܢ(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_.truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    stack_->setSize(0);
    ܦܢܐ *هذا;
}

UBool
UCharsTrie::Iterator::hasNext() ثابت { ܦܢܐ pos_!=nullptr || !stack_->isEmpty(); }

UBool
UCharsTrie::Iterator::next(UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ ܕܓܠܐ;
    }
    ثابت char16_t *pos=pos_;
    ܐܢ(pos==nullptr) {
        ܐܢ(stack_->isEmpty()) {
            ܦܢܐ ܕܓܠܐ;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t stackSize=stack_->size();
        int32_t length=stack_->elementAti(stackSize-1);
        pos=uchars_+stack_->elementAti(stackSize-2);
        stack_->setSize(stackSize-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        ܐܢ(length>1) {
            pos=branchNext(pos, length, errorCode);
            ܐܢ(pos==nullptr) {
                ܦܢܐ ܫܪܪܐ;  // Reached a final value.
            }
        } ܐܚܪܢܐ {
            str_.append(*pos++);
        }
    }
    ܐܢ(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        ܦܢܐ truncateAndStop();
    }
    لكل(;;) {
        int32_t node=*pos++;
        ܐܢ(node>=kMinValueLead) {
            ܐܢ(skipValue_) {
                pos=skipNodeValue(pos, node);
                node&=kNodeTypeMask;
                skipValue_=ܕܓܠܐ;
            } ܐܚܪܢܐ {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(node >> 15);
                ܐܢ(isFinal) {
                    value_=readValue(pos, node&0x7fff);
                } ܐܚܪܢܐ {
                    value_=readNodeValue(pos, node);
                }
                ܐܢ(isFinal || (maxLength_>0 && str_.length()==maxLength_)) {
                    pos_=nullptr;
                } ܐܚܪܢܐ {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skipValue_=ܫܪܪܐ;
                }
                ܦܢܐ ܫܪܪܐ;
            }
        }
        ܐܢ(maxLength_>0 && str_.length()==maxLength_) {
            ܦܢܐ truncateAndStop();
        }
        ܐܢ(node<kMinLinearMatch) {
            ܐܢ(node==0) {
                node=*pos++;
            }
            pos=branchNext(pos, node+1, errorCode);
            ܐܢ(pos==nullptr) {
                ܦܢܐ ܫܪܪܐ;  // Reached a final value.
            }
        } ܐܚܪܢܐ {
            // Linear-match node, append length units to str_.
            int32_t length=node-kMinLinearMatch+1;
            ܐܢ(maxLength_>0 && str_.length()+length>maxLength_) {
                str_.append(pos, maxLength_-str_.length());
                ܦܢܐ truncateAndStop();
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
    ܟܡܐ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        // Push state for the greater-or-equal edge.
        stack_->addElement(static_cast<int32_t>(skipDelta(pos) - uchars_), errorCode);
        stack_->addElement(((length-(length>>1))<<16)|str_.length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    char16_t trieUnit=*pos++;
    int32_t node=*pos++;
    UBool isFinal = static_cast<UBool>(node >> 15);
    int32_t value=readValue(pos, node&=0x7fff);
    pos=skipValue(pos, node);
    stack_->addElement(static_cast<int32_t>(pos - uchars_), errorCode);
    stack_->addElement(((length-1)<<16)|str_.length(), errorCode);
    str_.append(trieUnit);
    ܐܢ(isFinal) {
        pos_=nullptr;
        value_=value;
        ܦܢܐ nullptr;
    } ܐܚܪܢܐ {
        ܦܢܐ pos+value;
    }
}

U_NAMESPACE_END
