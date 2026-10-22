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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/bytestrie.h"
#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "uvectr32.h"

U_NAMESPACE_BEGIN

BytesTrie::Iterator::Iterator(अचर शून्य* trieBytes U_LIFETIME_BOUND,
                              int32_t maxStringLength,
                              UErrorCode& errorCode)
        : bytes_(static_cast<अचर uint8_t *>(trieBytes)),
          pos_(bytes_), initialPos_(bytes_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          str_(nullptr), maxLength_(maxStringLength), मान(0), थाक(nullptr) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=नूतन CharString();
    थाक=नूतन UVector32(errorCode);
    यदि(U_SUCCESS(errorCode) && (str_==nullptr || थाक==nullptr)) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrie::Iterator::Iterator(अचर BytesTrie &trie, int32_t maxStringLength,
                              UErrorCode &errorCode)
        : bytes_(trie.bytes_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          str_(nullptr), maxLength_(maxStringLength), मान(0), थाक(nullptr) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ;
    }
    str_=नूतन CharString();
    थाक=नूतन UVector32(errorCode);
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ;
    }
    यदि(str_==nullptr || थाक==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    यदि(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        यदि(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<अचर वर्ण *>(pos_), length, errorCode);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

BytesTrie::Iterator::~Iterator() {
    delete str_;
    delete थाक;
}

BytesTrie::Iterator&
BytesTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    यदि(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_->truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    थाक->setSize(0);
    फर्काऊ *एतत्;
}

UBool
BytesTrie::Iterator::hasNext() अचर { फर्काऊ pos_!=nullptr || !थाक->isEmpty(); }

UBool
BytesTrie::Iterator::next(UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ असत्य;
    }
    अचर uint8_t *pos=pos_;
    यदि(pos==nullptr) {
        यदि(थाक->isEmpty()) {
            फर्काऊ असत्य;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t थाक_Size=थाक->size();
        int32_t length=थाक->elementAti(थाक_Size-1);
        pos=bytes_+थाक->elementAti(थाक_Size-2);
        थाक->setSize(थाक_Size-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        यदि(length>1) {
            pos=branchNext(pos, length, errorCode);
            यदि(pos==nullptr) {
                फर्काऊ सत्य;  // Reached a final value.
            }
        } अन्यथा {
            str_->append(static_cast<वर्ण>(*pos++), errorCode);
        }
    }
    यदि(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        फर्काऊ truncateAndStop();
    }
    लागि(;;) {
        int32_t गाँठो=*pos++;
        यदि(गाँठो>=k_Min_मान_Lead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(गाँठो & k_मान_Is_Final);
            मान=read_मान(pos, गाँठो>>1);
            यदि(isFinal || (maxLength_>0 && str_->length()==maxLength_)) {
                pos_=nullptr;
            } अन्यथा {
                pos_=skip_मान(pos, गाँठो);
            }
            फर्काऊ सत्य;
        }
        यदि(maxLength_>0 && str_->length()==maxLength_) {
            फर्काऊ truncateAndStop();
        }
        यदि(गाँठो<kMinLinearMatch) {
            यदि(गाँठो==0) {
                गाँठो=*pos++;
            }
            pos=branchNext(pos, गाँठो+1, errorCode);
            यदि(pos==nullptr) {
                फर्काऊ सत्य;  // Reached a final value.
            }
        } अन्यथा {
            // Linear-match node, append length bytes to str_.
            int32_t length=गाँठो-kMinLinearMatch+1;
            यदि(maxLength_>0 && str_->length()+length>maxLength_) {
                str_->append(reinterpret_cast<अचर वर्ण *>(pos),
                            maxLength_-str_->length(), errorCode);
                फर्काऊ truncateAndStop();
            }
            str_->append(reinterpret_cast<अचर वर्ण *>(pos), length, errorCode);
            pos+=length;
        }
    }
}

StringPiece
BytesTrie::Iterator::getString() अचर U_LIFETIME_BOUND {
    फर्काऊ str_ == nullptr ? StringPiece() : str_->toStringPiece();
}

UBool
BytesTrie::Iterator::truncateAndStop() {
    pos_=nullptr;
    मान=-1;  // no real value for str
    फर्काऊ सत्य;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
अचर uint8_t*
BytesTrie::Iterator::branchNext(अचर uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UErrorCode& errorCode) {
    जबसम्म(length>k_Max_Branch_Linear_Sub_गाँठो_Length) {
        ++pos;  // ignore the comparison byte
        // Push state for the greater-or-equal edge.
        थाक->addElement(static_cast<int32_t>(skipDelta(pos) - bytes_), errorCode);
        थाक->addElement(((length-(length>>1))<<16)|str_->length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    uint8_t trieByte=*pos++;
    int32_t गाँठो=*pos++;
    UBool isFinal = static_cast<UBool>(गाँठो & k_मान_Is_Final);
    int32_t value=read_मान(pos, गाँठो>>1);
    pos=skip_मान(pos, गाँठो);
    थाक->addElement(static_cast<int32_t>(pos - bytes_), errorCode);
    थाक->addElement(((length-1)<<16)|str_->length(), errorCode);
    str_->append(static_cast<वर्ण>(trieByte), errorCode);
    यदि(isFinal) {
        pos_=nullptr;
        मान=value;
        फर्काऊ nullptr;
    } अन्यथा {
        फर्काऊ pos+value;
    }
}

U_NAMESPACE_END
