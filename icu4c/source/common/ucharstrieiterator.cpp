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

#include "unicode/utypes.h"
#include "unicode/ucharstrie.h"
#include "unicode/unistr.h"
#include "uvectr32.h"

U_NAMESPACE_BEGIN

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          skipValue_(false),
          maxLength_(maxStringLength), value_(0), stack_(nullptr) {
    ᨀᨘ(U_FAILURE(errorCode)) {
        ᨀᨘᨒ;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    stack_=new UVector32(errorCode);
    ᨀᨘ(stack_==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(const UCharsTrie &trie, int32_t maxStringLength,
                               UErrorCode &errorCode)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          skipValue_(false),
          maxLength_(maxStringLength), value_(0), stack_(nullptr) {
    ᨀᨘ(U_FAILURE(errorCode)) {
        ᨀᨘᨒ;
    }
    stack_=new UVector32(errorCode);
    ᨀᨘ(U_FAILURE(errorCode)) {
        ᨀᨘᨒ;
    }
    ᨀᨘ(stack_==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ᨀᨘᨒ;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ᨀᨘ(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        ᨀᨘ(maxLength_>0 && length>maxLength_) {
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
    skipValue_=false;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    ᨀᨘ(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_.truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    stack_->setSize(0);
    ᨀᨘᨒ *this;
}

UBool
UCharsTrie::Iterator::hasNext() const { ᨀᨘᨒ pos_!=nullptr || !stack_->isEmpty(); }

UBool
UCharsTrie::Iterator::next(UErrorCode &errorCode) {
    ᨀᨘ(U_FAILURE(errorCode)) {
        ᨀᨘᨒ false;
    }
    const char16_t *pos=pos_;
    ᨀᨘ(pos==nullptr) {
        ᨀᨘ(stack_->isEmpty()) {
            ᨀᨘᨒ false;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t stackSize=stack_->size();
        int32_t length=stack_->elementAti(stackSize-1);
        pos=uchars_+stack_->elementAti(stackSize-2);
        stack_->setSize(stackSize-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        ᨀᨘ(length>1) {
            pos=branchNext(pos, length, errorCode);
            ᨀᨘ(pos==nullptr) {
                ᨀᨘᨒ true;  // Reached a final value.
            }
        } ᨀᨘᨁ {
            str_.append(*pos++);
        }
    }
    ᨀᨘ(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        ᨀᨘᨒ truncateAndStop();
    }
    for(;;) {
        int32_t node=*pos++;
        ᨀᨘ(node>=kMinValueLead) {
            ᨀᨘ(skipValue_) {
                pos=skipNodeValue(pos, node);
                node&=kNodeTypeMask;
                skipValue_=false;
            } ᨀᨘᨁ {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(node >> 15);
                ᨀᨘ(isFinal) {
                    value_=readValue(pos, node&0x7fff);
                } ᨀᨘᨁ {
                    value_=readNodeValue(pos, node);
                }
                ᨀᨘ(isFinal || (maxLength_>0 && str_.length()==maxLength_)) {
                    pos_=nullptr;
                } ᨀᨘᨁ {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skipValue_=true;
                }
                ᨀᨘᨒ true;
            }
        }
        ᨀᨘ(maxLength_>0 && str_.length()==maxLength_) {
            ᨀᨘᨒ truncateAndStop();
        }
        ᨀᨘ(node<kMinLinearMatch) {
            ᨀᨘ(node==0) {
                node=*pos++;
            }
            pos=branchNext(pos, node+1, errorCode);
            ᨀᨘ(pos==nullptr) {
                ᨀᨘᨒ true;  // Reached a final value.
            }
        } ᨀᨘᨁ {
            // Linear-match node, append length units to str_.
            int32_t length=node-kMinLinearMatch+1;
            ᨀᨘ(maxLength_>0 && str_.length()+length>maxLength_) {
                str_.append(pos, maxLength_-str_.length());
                ᨀᨘᨒ truncateAndStop();
            }
            str_.append(pos, length);
            pos+=length;
        }
    }
}

// Branch node, needs to take the first outbound edge and push state for the rest.
const char16_t*
UCharsTrie::Iterator::branchNext(const char16_t* pos U_LIFETIME_BOUND,
                                 int32_t length,
                                 UErrorCode& errorCode) {
    ᨀᨛ(length>kMaxBranchLinearSubNodeLength) {
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
    ᨀᨘ(isFinal) {
        pos_=nullptr;
        value_=value;
        ᨀᨘᨒ nullptr;
    } ᨀᨘᨁ {
        ᨀᨘᨒ pos+value;
    }
}

U_NAMESPACE_END
