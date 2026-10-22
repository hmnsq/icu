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
    အကယ်၍(U_FAILURE(errorCode)) {
        ပြန်ပေး;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    stack_=new UVector32(errorCode);
    အကယ်၍(stack_==nullptr) {
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
    အကယ်၍(U_FAILURE(errorCode)) {
        ပြန်ပေး;
    }
    stack_=new UVector32(errorCode);
    အကယ်၍(U_FAILURE(errorCode)) {
        ပြန်ပေး;
    }
    အကယ်၍(stack_==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ပြန်ပေး;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    အကယ်၍(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        အကယ်၍(maxLength_>0 && length>maxLength_) {
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
    အကယ်၍(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_.truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    stack_->setSize(0);
    ပြန်ပေး *this;
}

UBool
UCharsTrie::Iterator::hasNext() const { ပြန်ပေး pos_!=nullptr || !stack_->isEmpty(); }

UBool
UCharsTrie::Iterator::next(UErrorCode &errorCode) {
    အကယ်၍(U_FAILURE(errorCode)) {
        ပြန်ပေး false;
    }
    const char16_t *pos=pos_;
    အကယ်၍(pos==nullptr) {
        အကယ်၍(stack_->isEmpty()) {
            ပြန်ပေး false;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t stackSize=stack_->size();
        int32_t length=stack_->elementAti(stackSize-1);
        pos=uchars_+stack_->elementAti(stackSize-2);
        stack_->setSize(stackSize-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        အကယ်၍(length>1) {
            pos=branchNext(pos, length, errorCode);
            အကယ်၍(pos==nullptr) {
                ပြန်ပေး true;  // Reached a final value.
            }
        } မဟုတ်ပါက {
            str_.append(*pos++);
        }
    }
    အကယ်၍(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        ပြန်ပေး truncateAndStop();
    }
    အတွက်(;;) {
        int32_t node=*pos++;
        အကယ်၍(node>=kMinValueLead) {
            အကယ်၍(skipValue_) {
                pos=skipNodeValue(pos, node);
                node&=kNodeTypeMask;
                skipValue_=false;
            } မဟုတ်ပါက {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(node >> 15);
                အကယ်၍(isFinal) {
                    value_=readValue(pos, node&0x7fff);
                } မဟုတ်ပါက {
                    value_=readNodeValue(pos, node);
                }
                အကယ်၍(isFinal || (maxLength_>0 && str_.length()==maxLength_)) {
                    pos_=nullptr;
                } မဟုတ်ပါက {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skipValue_=true;
                }
                ပြန်ပေး true;
            }
        }
        အကယ်၍(maxLength_>0 && str_.length()==maxLength_) {
            ပြန်ပေး truncateAndStop();
        }
        အကယ်၍(node<kMinLinearMatch) {
            အကယ်၍(node==0) {
                node=*pos++;
            }
            pos=branchNext(pos, node+1, errorCode);
            အကယ်၍(pos==nullptr) {
                ပြန်ပေး true;  // Reached a final value.
            }
        } မဟုတ်ပါက {
            // Linear-match node, append length units to str_.
            int32_t length=node-kMinLinearMatch+1;
            အကယ်၍(maxLength_>0 && str_.length()+length>maxLength_) {
                str_.append(pos, maxLength_-str_.length());
                ပြန်ပေး truncateAndStop();
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
    နေစဉ်(length>kMaxBranchLinearSubNodeLength) {
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
    အကယ်၍(isFinal) {
        pos_=nullptr;
        value_=value;
        ပြန်ပေး nullptr;
    } မဟုတ်ပါက {
        ပြန်ပေး pos+value;
    }
}

U_NAMESPACE_END
