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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/bytestrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/stringpiece.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "uvectr32.h"

U_NAMESPACE_BEGIN

BytesTrie::Iterator::Iterator(ଅଚଳ ଶୂନ୍ୟ* trieBytes U_LIFETIME_BOUND,
                              int32_t maxStringLength,
                              UErrorCode& errorCode)
        : bytes_(static_cast<ଅଚଳ uint8_t *>(trieBytes)),
          pos_(bytes_), initialPos_(bytes_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          str_(nullptr), maxLength_(maxStringLength), ମୂଲ୍ୟ(0), ଗଦା(nullptr) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=ନୂତନ CharString();
    ଗଦା=ନୂତନ UVector32(errorCode);
    ଯଦି(U_SUCCESS(errorCode) && (str_==nullptr || ଗଦା==nullptr)) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrie::Iterator::Iterator(ଅଚଳ BytesTrie &trie, int32_t maxStringLength,
                              UErrorCode &errorCode)
        : bytes_(trie.bytes_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          str_(nullptr), maxLength_(maxStringLength), ମୂଲ୍ୟ(0), ଗଦା(nullptr) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    str_=ନୂତନ CharString();
    ଗଦା=ନୂତନ UVector32(errorCode);
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି(str_==nullptr || ଗଦା==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ଯଦି(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        ଯଦି(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<ଅଚଳ ଅକ୍ଷର *>(pos_), length, errorCode);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

BytesTrie::Iterator::~Iterator() {
    delete str_;
    delete ଗଦା;
}

BytesTrie::Iterator&
BytesTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    ଯଦି(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_->truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    ଗଦା->setSize(0);
    ଫେରନ୍ତୁ *ଏହି;
}

UBool
BytesTrie::Iterator::hasNext() ଅଚଳ { ଫେରନ୍ତୁ pos_!=nullptr || !ଗଦା->isEmpty(); }

UBool
BytesTrie::Iterator::next(UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଅଚଳ uint8_t *pos=pos_;
    ଯଦି(pos==nullptr) {
        ଯଦି(ଗଦା->isEmpty()) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t ଗଦା_Size=ଗଦା->size();
        int32_t length=ଗଦା->elementAti(ଗଦା_Size-1);
        pos=bytes_+ଗଦା->elementAti(ଗଦା_Size-2);
        ଗଦା->setSize(ଗଦା_Size-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        ଯଦି(length>1) {
            pos=branchNext(pos, length, errorCode);
            ଯଦି(pos==nullptr) {
                ଫେରନ୍ତୁ ସତ୍ୟ;  // Reached a final value.
            }
        } ଅନ୍ୟଥା {
            str_->append(static_cast<ଅକ୍ଷର>(*pos++), errorCode);
        }
    }
    ଯଦି(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        ଫେରନ୍ତୁ truncateAndStop();
    }
    ପାଇଁ(;;) {
        int32_t ଗଣ୍ଠି=*pos++;
        ଯଦି(ଗଣ୍ଠି>=k_Min_ମୂଲ୍ୟ_Lead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(ଗଣ୍ଠି & k_ମୂଲ୍ୟ_Is_Final);
            ମୂଲ୍ୟ=read_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି>>1);
            ଯଦି(isFinal || (maxLength_>0 && str_->length()==maxLength_)) {
                pos_=nullptr;
            } ଅନ୍ୟଥା {
                pos_=skip_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି);
            }
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }
        ଯଦି(maxLength_>0 && str_->length()==maxLength_) {
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
            // Linear-match node, append length bytes to str_.
            int32_t length=ଗଣ୍ଠି-kMinLinearMatch+1;
            ଯଦି(maxLength_>0 && str_->length()+length>maxLength_) {
                str_->append(reinterpret_cast<ଅଚଳ ଅକ୍ଷର *>(pos),
                            maxLength_-str_->length(), errorCode);
                ଫେରନ୍ତୁ truncateAndStop();
            }
            str_->append(reinterpret_cast<ଅଚଳ ଅକ୍ଷର *>(pos), length, errorCode);
            pos+=length;
        }
    }
}

StringPiece
BytesTrie::Iterator::getString() ଅଚଳ U_LIFETIME_BOUND {
    ଫେରନ୍ତୁ str_ == nullptr ? StringPiece() : str_->toStringPiece();
}

UBool
BytesTrie::Iterator::truncateAndStop() {
    pos_=nullptr;
    ମୂଲ୍ୟ=-1;  // no real value for str
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ଅଚଳ uint8_t*
BytesTrie::Iterator::branchNext(ଅଚଳ uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UErrorCode& errorCode) {
    ଯେତେବେଳେ(length>k_Max_Branch_Linear_Sub_ଗଣ୍ଠି_Length) {
        ++pos;  // ignore the comparison byte
        // Push state for the greater-or-equal edge.
        ଗଦା->addElement(static_cast<int32_t>(skipDelta(pos) - bytes_), errorCode);
        ଗଦା->addElement(((length-(length>>1))<<16)|str_->length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    uint8_t trieByte=*pos++;
    int32_t ଗଣ୍ଠି=*pos++;
    UBool isFinal = static_cast<UBool>(ଗଣ୍ଠି & k_ମୂଲ୍ୟ_Is_Final);
    int32_t value=read_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି>>1);
    pos=skip_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି);
    ଗଦା->addElement(static_cast<int32_t>(pos - bytes_), errorCode);
    ଗଦା->addElement(((length-1)<<16)|str_->length(), errorCode);
    str_->append(static_cast<ଅକ୍ଷର>(trieByte), errorCode);
    ଯଦି(isFinal) {
        pos_=nullptr;
        ମୂଲ୍ୟ=value;
        ଫେରନ୍ତୁ nullptr;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ pos+value;
    }
}

U_NAMESPACE_END
