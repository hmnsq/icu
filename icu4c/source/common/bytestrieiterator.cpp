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

#شامل "unicode/utypes.h"
#شامل "unicode/bytestrie.h"
#شامل "unicode/stringpiece.h"
#شامل "charstr.h"
#شامل "uvectr32.h"

U_NAMESPACE_BEGIN

BytesTrie::Iterator::Iterator(ثابت تهی* trieBytes U_LIFETIME_BOUND,
                              int32_t maxStringLength,
                              UErrorCode& errorCode)
        : bytes_(static_cast<ثابت uint8_t *>(trieBytes)),
          pos_(bytes_), initialPos_(bytes_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          str_(nullptr), maxLength_(maxStringLength), مقدار(0), پشته(nullptr) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=جدید CharString();
    پشته=جدید UVector32(errorCode);
    اگر(U_SUCCESS(errorCode) && (str_==nullptr || پشته==nullptr)) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrie::Iterator::Iterator(ثابت BytesTrie &trie, int32_t maxStringLength,
                              UErrorCode &errorCode)
        : bytes_(trie.bytes_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          str_(nullptr), maxLength_(maxStringLength), مقدار(0), پشته(nullptr) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت;
    }
    str_=جدید CharString();
    پشته=جدید UVector32(errorCode);
    اگر(U_FAILURE(errorCode)) {
        بازگشت;
    }
    اگر(str_==nullptr || پشته==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    اگر(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        اگر(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<ثابت نویسه *>(pos_), length, errorCode);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

BytesTrie::Iterator::~Iterator() {
    delete str_;
    delete پشته;
}

BytesTrie::Iterator&
BytesTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    اگر(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_->truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    پشته->setSize(0);
    بازگشت *این;
}

UBool
BytesTrie::Iterator::hasNext() ثابت { بازگشت pos_!=nullptr || !پشته->isEmpty(); }

UBool
BytesTrie::Iterator::next(UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت نادرست;
    }
    ثابت uint8_t *pos=pos_;
    اگر(pos==nullptr) {
        اگر(پشته->isEmpty()) {
            بازگشت نادرست;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t پشته_Size=پشته->size();
        int32_t length=پشته->elementAti(پشته_Size-1);
        pos=bytes_+پشته->elementAti(پشته_Size-2);
        پشته->setSize(پشته_Size-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        اگر(length>1) {
            pos=branchNext(pos, length, errorCode);
            اگر(pos==nullptr) {
                بازگشت درست;  // Reached a final value.
            }
        } وگرنه {
            str_->append(static_cast<نویسه>(*pos++), errorCode);
        }
    }
    اگر(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        بازگشت truncateAndStop();
    }
    برای(;;) {
        int32_t گره=*pos++;
        اگر(گره>=k_Min_مقدار_Lead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(گره & k_مقدار_Is_Final);
            مقدار=read_مقدار(pos, گره>>1);
            اگر(isFinal || (maxLength_>0 && str_->length()==maxLength_)) {
                pos_=nullptr;
            } وگرنه {
                pos_=skip_مقدار(pos, گره);
            }
            بازگشت درست;
        }
        اگر(maxLength_>0 && str_->length()==maxLength_) {
            بازگشت truncateAndStop();
        }
        اگر(گره<kMinLinearMatch) {
            اگر(گره==0) {
                گره=*pos++;
            }
            pos=branchNext(pos, گره+1, errorCode);
            اگر(pos==nullptr) {
                بازگشت درست;  // Reached a final value.
            }
        } وگرنه {
            // Linear-match node, append length bytes to str_.
            int32_t length=گره-kMinLinearMatch+1;
            اگر(maxLength_>0 && str_->length()+length>maxLength_) {
                str_->append(reinterpret_cast<ثابت نویسه *>(pos),
                            maxLength_-str_->length(), errorCode);
                بازگشت truncateAndStop();
            }
            str_->append(reinterpret_cast<ثابت نویسه *>(pos), length, errorCode);
            pos+=length;
        }
    }
}

StringPiece
BytesTrie::Iterator::getString() ثابت U_LIFETIME_BOUND {
    بازگشت str_ == nullptr ? StringPiece() : str_->toStringPiece();
}

UBool
BytesTrie::Iterator::truncateAndStop() {
    pos_=nullptr;
    مقدار=-1;  // no real value for str
    بازگشت درست;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ثابت uint8_t*
BytesTrie::Iterator::branchNext(ثابت uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UErrorCode& errorCode) {
    وقتی(length>k_Max_Branch_Linear_Sub_گره_Length) {
        ++pos;  // ignore the comparison byte
        // Push state for the greater-or-equal edge.
        پشته->addElement(static_cast<int32_t>(skipDelta(pos) - bytes_), errorCode);
        پشته->addElement(((length-(length>>1))<<16)|str_->length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    uint8_t trieByte=*pos++;
    int32_t گره=*pos++;
    UBool isFinal = static_cast<UBool>(گره & k_مقدار_Is_Final);
    int32_t value=read_مقدار(pos, گره>>1);
    pos=skip_مقدار(pos, گره);
    پشته->addElement(static_cast<int32_t>(pos - bytes_), errorCode);
    پشته->addElement(((length-1)<<16)|str_->length(), errorCode);
    str_->append(static_cast<نویسه>(trieByte), errorCode);
    اگر(isFinal) {
        pos_=nullptr;
        مقدار=value;
        بازگشت nullptr;
    } وگرنه {
        بازگشت pos+value;
    }
}

U_NAMESPACE_END
