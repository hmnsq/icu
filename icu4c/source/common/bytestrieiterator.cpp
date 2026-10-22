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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/bytestrie.h"
#অন্তর্ভাব "unicode/stringpiece.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "uvectr32.h"

U_NAMESPACE_BEGIN

BytesTrie::Iterator::Iterator(ধ্রুব শূন্য* trieBytes U_LIFETIME_BOUND,
                              int32_t maxStringLength,
                              UErrorCode& errorCode)
        : bytes_(static_cast<ধ্রুব uint8_t *>(trieBytes)),
          pos_(bytes_), initialPos_(bytes_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          str_(nullptr), maxLength_(maxStringLength), মান(0), স্তূপ(nullptr) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=নূতন CharString();
    স্তূপ=নূতন UVector32(errorCode);
    যদি(U_SUCCESS(errorCode) && (str_==nullptr || স্তূপ==nullptr)) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrie::Iterator::Iterator(ধ্রুব BytesTrie &trie, int32_t maxStringLength,
                              UErrorCode &errorCode)
        : bytes_(trie.bytes_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          str_(nullptr), maxLength_(maxStringLength), মান(0), স্তূপ(nullptr) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম;
    }
    str_=নূতন CharString();
    স্তূপ=নূতন UVector32(errorCode);
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম;
    }
    যদি(str_==nullptr || স্তূপ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    যদি(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        যদি(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<ধ্রুব বর্ণ *>(pos_), length, errorCode);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

BytesTrie::Iterator::~Iterator() {
    delete str_;
    delete স্তূপ;
}

BytesTrie::Iterator&
BytesTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    যদি(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_->truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    স্তূপ->setSize(0);
    প্রত্যাগম *এতত্;
}

UBool
BytesTrie::Iterator::hasNext() ধ্রুব { প্রত্যাগম pos_!=nullptr || !স্তূপ->isEmpty(); }

UBool
BytesTrie::Iterator::next(UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম অসত্য;
    }
    ধ্রুব uint8_t *pos=pos_;
    যদি(pos==nullptr) {
        যদি(স্তূপ->isEmpty()) {
            প্রত্যাগম অসত্য;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t স্তূপ_Size=স্তূপ->size();
        int32_t length=স্তূপ->elementAti(স্তূপ_Size-1);
        pos=bytes_+স্তূপ->elementAti(স্তূপ_Size-2);
        স্তূপ->setSize(স্তূপ_Size-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        যদি(length>1) {
            pos=branchNext(pos, length, errorCode);
            যদি(pos==nullptr) {
                প্রত্যাগম সত্য;  // Reached a final value.
            }
        } অন্যথা {
            str_->append(static_cast<বর্ণ>(*pos++), errorCode);
        }
    }
    যদি(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        প্রত্যাগম truncateAndStop();
    }
    প্রতি(;;) {
        int32_t গ্রন্থি=*pos++;
        যদি(গ্রন্থি>=k_Min_মান_Lead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(গ্রন্থি & k_মান_Is_Final);
            মান=read_মান(pos, গ্রন্থি>>1);
            যদি(isFinal || (maxLength_>0 && str_->length()==maxLength_)) {
                pos_=nullptr;
            } অন্যথা {
                pos_=skip_মান(pos, গ্রন্থি);
            }
            প্রত্যাগম সত্য;
        }
        যদি(maxLength_>0 && str_->length()==maxLength_) {
            প্রত্যাগম truncateAndStop();
        }
        যদি(গ্রন্থি<kMinLinearMatch) {
            যদি(গ্রন্থি==0) {
                গ্রন্থি=*pos++;
            }
            pos=branchNext(pos, গ্রন্থি+1, errorCode);
            যদি(pos==nullptr) {
                প্রত্যাগম সত্য;  // Reached a final value.
            }
        } অন্যথা {
            // Linear-match node, append length bytes to str_.
            int32_t length=গ্রন্থি-kMinLinearMatch+1;
            যদি(maxLength_>0 && str_->length()+length>maxLength_) {
                str_->append(reinterpret_cast<ধ্রুব বর্ণ *>(pos),
                            maxLength_-str_->length(), errorCode);
                প্রত্যাগম truncateAndStop();
            }
            str_->append(reinterpret_cast<ধ্রুব বর্ণ *>(pos), length, errorCode);
            pos+=length;
        }
    }
}

StringPiece
BytesTrie::Iterator::getString() ধ্রুব U_LIFETIME_BOUND {
    প্রত্যাগম str_ == nullptr ? StringPiece() : str_->toStringPiece();
}

UBool
BytesTrie::Iterator::truncateAndStop() {
    pos_=nullptr;
    মান=-1;  // no real value for str
    প্রত্যাগম সত্য;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ধ্রুব uint8_t*
BytesTrie::Iterator::branchNext(ধ্রুব uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UErrorCode& errorCode) {
    যাবত্(length>k_Max_Branch_Linear_Sub_গ্রন্থি_Length) {
        ++pos;  // ignore the comparison byte
        // Push state for the greater-or-equal edge.
        স্তূপ->addElement(static_cast<int32_t>(skipDelta(pos) - bytes_), errorCode);
        স্তূপ->addElement(((length-(length>>1))<<16)|str_->length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    uint8_t trieByte=*pos++;
    int32_t গ্রন্থি=*pos++;
    UBool isFinal = static_cast<UBool>(গ্রন্থি & k_মান_Is_Final);
    int32_t value=read_মান(pos, গ্রন্থি>>1);
    pos=skip_মান(pos, গ্রন্থি);
    স্তূপ->addElement(static_cast<int32_t>(pos - bytes_), errorCode);
    স্তূপ->addElement(((length-1)<<16)|str_->length(), errorCode);
    str_->append(static_cast<বর্ণ>(trieByte), errorCode);
    যদি(isFinal) {
        pos_=nullptr;
        মান=value;
        প্রত্যাগম nullptr;
    } অন্যথা {
        প্রত্যাগম pos+value;
    }
}

U_NAMESPACE_END
