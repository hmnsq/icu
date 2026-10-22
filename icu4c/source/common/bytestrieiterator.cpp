// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  bytestrieiterator.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010nov03
*   created by: Markus W. Scherer
*/

#ضمن "unicode/utypes.h"
#ضمن "unicode/bytestrie.h"
#ضمن "unicode/stringpiece.h"
#ضمن "charstr.h"
#ضمن "uvectr32.h"

U_NAMESPACE_BEGIN

BytesTrie::Iterator::Iterator(ثابت فراغ* trieBytes U_LIFETIME_BOUND,
                              int32_t maxStringLength,
                              UErrorCode& errorCode)
        : bytes_(static_cast<ثابت uint8_t *>(trieBytes)),
          pos_(bytes_), initialPos_(bytes_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          str_(nullptr), maxLength_(maxStringLength), value_(0), stack_(nullptr) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=جديد CharString();
    stack_=جديد UVector32(errorCode);
    ܐܢ(U_SUCCESS(errorCode) && (str_==nullptr || stack_==nullptr)) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrie::Iterator::Iterator(ثابت BytesTrie &trie, int32_t maxStringLength,
                              UErrorCode &errorCode)
        : bytes_(trie.bytes_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          str_(nullptr), maxLength_(maxStringLength), value_(0), stack_(nullptr) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ;
    }
    str_=جديد CharString();
    stack_=جديد UVector32(errorCode);
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ;
    }
    ܐܢ(str_==nullptr || stack_==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ܐܢ(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        ܐܢ(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<ثابت حرف *>(pos_), length, errorCode);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

BytesTrie::Iterator::~Iterator() {
    delete str_;
    delete stack_;
}

BytesTrie::Iterator&
BytesTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    ܐܢ(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_->truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    stack_->setSize(0);
    ܦܢܐ *هذا;
}

UBool
BytesTrie::Iterator::hasNext() ثابت { ܦܢܐ pos_!=nullptr || !stack_->isEmpty(); }

UBool
BytesTrie::Iterator::next(UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ ܕܓܠܐ;
    }
    ثابت uint8_t *pos=pos_;
    ܐܢ(pos==nullptr) {
        ܐܢ(stack_->isEmpty()) {
            ܦܢܐ ܕܓܠܐ;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t stackSize=stack_->size();
        int32_t length=stack_->elementAti(stackSize-1);
        pos=bytes_+stack_->elementAti(stackSize-2);
        stack_->setSize(stackSize-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        ܐܢ(length>1) {
            pos=branchNext(pos, length, errorCode);
            ܐܢ(pos==nullptr) {
                ܦܢܐ ܫܪܪܐ;  // Reached a final value.
            }
        } ܐܚܪܢܐ {
            str_->append(static_cast<حرف>(*pos++), errorCode);
        }
    }
    ܐܢ(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        ܦܢܐ truncateAndStop();
    }
    لكل(;;) {
        int32_t node=*pos++;
        ܐܢ(node>=kMinValueLead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            value_=readValue(pos, node>>1);
            ܐܢ(isFinal || (maxLength_>0 && str_->length()==maxLength_)) {
                pos_=nullptr;
            } ܐܚܪܢܐ {
                pos_=skipValue(pos, node);
            }
            ܦܢܐ ܫܪܪܐ;
        }
        ܐܢ(maxLength_>0 && str_->length()==maxLength_) {
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
            // Linear-match node, append length bytes to str_.
            int32_t length=node-kMinLinearMatch+1;
            ܐܢ(maxLength_>0 && str_->length()+length>maxLength_) {
                str_->append(reinterpret_cast<ثابت حرف *>(pos),
                            maxLength_-str_->length(), errorCode);
                ܦܢܐ truncateAndStop();
            }
            str_->append(reinterpret_cast<ثابت حرف *>(pos), length, errorCode);
            pos+=length;
        }
    }
}

StringPiece
BytesTrie::Iterator::getString() ثابت U_LIFETIME_BOUND {
    ܦܢܐ str_ == nullptr ? StringPiece() : str_->toStringPiece();
}

UBool
BytesTrie::Iterator::truncateAndStop() {
    pos_=nullptr;
    value_=-1;  // no real value for str
    ܦܢܐ ܫܪܪܐ;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ثابت uint8_t*
BytesTrie::Iterator::branchNext(ثابت uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UErrorCode& errorCode) {
    ܟܡܐ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        // Push state for the greater-or-equal edge.
        stack_->addElement(static_cast<int32_t>(skipDelta(pos) - bytes_), errorCode);
        stack_->addElement(((length-(length>>1))<<16)|str_->length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    uint8_t trieByte=*pos++;
    int32_t node=*pos++;
    UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
    int32_t value=readValue(pos, node>>1);
    pos=skipValue(pos, node);
    stack_->addElement(static_cast<int32_t>(pos - bytes_), errorCode);
    stack_->addElement(((length-1)<<16)|str_->length(), errorCode);
    str_->append(static_cast<حرف>(trieByte), errorCode);
    ܐܢ(isFinal) {
        pos_=nullptr;
        value_=value;
        ܦܢܐ nullptr;
    } ܐܚܪܢܐ {
        ܦܢܐ pos+value;
    }
}

U_NAMESPACE_END
