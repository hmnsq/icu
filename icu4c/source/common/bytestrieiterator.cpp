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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "uvectr32.h"

U_NAMESPACE_BEGIN

BytesTrie::Iterator::Iterator(ಧ್ರುವ ಶೂನ್ಯ* trieBytes U_LIFETIME_BOUND,
                              int32_t maxStringLength,
                              UErrorCode& errorCode)
        : bytes_(static_cast<ಧ್ರುವ uint8_t *>(trieBytes)),
          pos_(bytes_), initialPos_(bytes_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          str_(nullptr), maxLength_(maxStringLength), ಮೌಲ್ಯ(0), ರಾಶಿ(nullptr) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=ನೂತನ CharString();
    ರಾಶಿ=ನೂತನ UVector32(errorCode);
    ಯದಿ(U_SUCCESS(errorCode) && (str_==nullptr || ರಾಶಿ==nullptr)) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrie::Iterator::Iterator(ಧ್ರುವ BytesTrie &trie, int32_t maxStringLength,
                              UErrorCode &errorCode)
        : bytes_(trie.bytes_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          str_(nullptr), maxLength_(maxStringLength), ಮೌಲ್ಯ(0), ರಾಶಿ(nullptr) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    str_=ನೂತನ CharString();
    ರಾಶಿ=ನೂತನ UVector32(errorCode);
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ(str_==nullptr || ರಾಶಿ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ಯದಿ(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        ಯದಿ(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<ಧ್ರುವ ವರ್ಣ *>(pos_), length, errorCode);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

BytesTrie::Iterator::~Iterator() {
    delete str_;
    delete ರಾಶಿ;
}

BytesTrie::Iterator&
BytesTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    ಯದಿ(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_->truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    ರಾಶಿ->setSize(0);
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

UBool
BytesTrie::Iterator::hasNext() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ pos_!=nullptr || !ರಾಶಿ->isEmpty(); }

UBool
BytesTrie::Iterator::next(UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಧ್ರುವ uint8_t *pos=pos_;
    ಯದಿ(pos==nullptr) {
        ಯದಿ(ರಾಶಿ->isEmpty()) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t ರಾಶಿ_Size=ರಾಶಿ->size();
        int32_t length=ರಾಶಿ->elementAti(ರಾಶಿ_Size-1);
        pos=bytes_+ರಾಶಿ->elementAti(ರಾಶಿ_Size-2);
        ರಾಶಿ->setSize(ರಾಶಿ_Size-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        ಯದಿ(length>1) {
            pos=branchNext(pos, length, errorCode);
            ಯದಿ(pos==nullptr) {
                ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;  // Reached a final value.
            }
        } ಅನ್ಯಥಾ {
            str_->append(static_cast<ವರ್ಣ>(*pos++), errorCode);
        }
    }
    ಯದಿ(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        ಪ್ರತ್ಯಾಗಮ truncateAndStop();
    }
    ಪ್ರತಿ(;;) {
        int32_t ಗಂಟು=*pos++;
        ಯದಿ(ಗಂಟು>=k_Min_ಮೌಲ್ಯ_Lead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(ಗಂಟು & k_ಮೌಲ್ಯ_Is_Final);
            ಮೌಲ್ಯ=read_ಮೌಲ್ಯ(pos, ಗಂಟು>>1);
            ಯದಿ(isFinal || (maxLength_>0 && str_->length()==maxLength_)) {
                pos_=nullptr;
            } ಅನ್ಯಥಾ {
                pos_=skip_ಮೌಲ್ಯ(pos, ಗಂಟು);
            }
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }
        ಯದಿ(maxLength_>0 && str_->length()==maxLength_) {
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
            // Linear-match node, append length bytes to str_.
            int32_t length=ಗಂಟು-kMinLinearMatch+1;
            ಯದಿ(maxLength_>0 && str_->length()+length>maxLength_) {
                str_->append(reinterpret_cast<ಧ್ರುವ ವರ್ಣ *>(pos),
                            maxLength_-str_->length(), errorCode);
                ಪ್ರತ್ಯಾಗಮ truncateAndStop();
            }
            str_->append(reinterpret_cast<ಧ್ರುವ ವರ್ಣ *>(pos), length, errorCode);
            pos+=length;
        }
    }
}

StringPiece
BytesTrie::Iterator::getString() ಧ್ರುವ U_LIFETIME_BOUND {
    ಪ್ರತ್ಯಾಗಮ str_ == nullptr ? StringPiece() : str_->toStringPiece();
}

UBool
BytesTrie::Iterator::truncateAndStop() {
    pos_=nullptr;
    ಮೌಲ್ಯ=-1;  // no real value for str
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ಧ್ರುವ uint8_t*
BytesTrie::Iterator::branchNext(ಧ್ರುವ uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UErrorCode& errorCode) {
    ಯಾವತ್(length>k_Max_Branch_Linear_Sub_ಗಂಟು_Length) {
        ++pos;  // ignore the comparison byte
        // Push state for the greater-or-equal edge.
        ರಾಶಿ->addElement(static_cast<int32_t>(skipDelta(pos) - bytes_), errorCode);
        ರಾಶಿ->addElement(((length-(length>>1))<<16)|str_->length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    uint8_t trieByte=*pos++;
    int32_t ಗಂಟು=*pos++;
    UBool isFinal = static_cast<UBool>(ಗಂಟು & k_ಮೌಲ್ಯ_Is_Final);
    int32_t value=read_ಮೌಲ್ಯ(pos, ಗಂಟು>>1);
    pos=skip_ಮೌಲ್ಯ(pos, ಗಂಟು);
    ರಾಶಿ->addElement(static_cast<int32_t>(pos - bytes_), errorCode);
    ರಾಶಿ->addElement(((length-1)<<16)|str_->length(), errorCode);
    str_->append(static_cast<ವರ್ಣ>(trieByte), errorCode);
    ಯದಿ(isFinal) {
        pos_=nullptr;
        ಮೌಲ್ಯ=value;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ pos+value;
    }
}

U_NAMESPACE_END
