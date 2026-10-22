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

BytesTrie::Iterator::Iterator(ध्रुव शून्य* trieBytes U_LIFETIME_BOUND,
                              int32_t maxStringLength,
                              UErrorCode& errorCode)
        : bytes_(static_cast<ध्रुव uint8_t *>(trieBytes)),
          pos_(bytes_), initialPos_(bytes_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          str_(nullptr), maxLength_(maxStringLength), मानम्(0), राशिः(nullptr) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=नूतन CharString();
    राशिः=नूतन UVector32(errorCode);
    सचे(U_SUCCESS(errorCode) && (str_==nullptr || राशिः==nullptr)) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrie::Iterator::Iterator(ध्रुव BytesTrie &trie, int32_t maxStringLength,
                              UErrorCode &errorCode)
        : bytes_(trie.bytes_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          str_(nullptr), maxLength_(maxStringLength), मानम्(0), राशिः(nullptr) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    str_=नूतन CharString();
    राशिः=नूतन UVector32(errorCode);
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    सचे(str_==nullptr || राशिः==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    सचे(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        सचे(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<ध्रुव वर्ण *>(pos_), length, errorCode);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

BytesTrie::Iterator::~Iterator() {
    delete str_;
    delete राशिः;
}

BytesTrie::Iterator&
BytesTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    सचे(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_->truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    राशिः->setSize(0);
    पटिनिवत्त *एतत्;
}

UBool
BytesTrie::Iterator::hasNext() ध्रुव { पटिनिवत्त pos_!=nullptr || !राशिः->isEmpty(); }

UBool
BytesTrie::Iterator::next(UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त मिच्छा;
    }
    ध्रुव uint8_t *pos=pos_;
    सचे(pos==nullptr) {
        सचे(राशिः->isEmpty()) {
            पटिनिवत्त मिच्छा;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t राशिः_Size=राशिः->size();
        int32_t length=राशिः->elementAti(राशिः_Size-1);
        pos=bytes_+राशिः->elementAti(राशिः_Size-2);
        राशिः->setSize(राशिः_Size-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        सचे(length>1) {
            pos=branchNext(pos, length, errorCode);
            सचे(pos==nullptr) {
                पटिनिवत्त सच्च;  // Reached a final value.
            }
        } अञ्ञथा {
            str_->append(static_cast<वर्ण>(*pos++), errorCode);
        }
    }
    सचे(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        पटिनिवत्त truncateAndStop();
    }
    अत्थाय(;;) {
        int32_t ग्रन्थिः=*pos++;
        सचे(ग्रन्थिः>=k_Min_मानम्_Lead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(ग्रन्थिः & k_मानम्_Is_Final);
            मानम्=read_मानम्(pos, ग्रन्थिः>>1);
            सचे(isFinal || (maxLength_>0 && str_->length()==maxLength_)) {
                pos_=nullptr;
            } अञ्ञथा {
                pos_=skip_मानम्(pos, ग्रन्थिः);
            }
            पटिनिवत्त सच्च;
        }
        सचे(maxLength_>0 && str_->length()==maxLength_) {
            पटिनिवत्त truncateAndStop();
        }
        सचे(ग्रन्थिः<kMinLinearMatch) {
            सचे(ग्रन्थिः==0) {
                ग्रन्थिः=*pos++;
            }
            pos=branchNext(pos, ग्रन्थिः+1, errorCode);
            सचे(pos==nullptr) {
                पटिनिवत्त सच्च;  // Reached a final value.
            }
        } अञ्ञथा {
            // Linear-match node, append length bytes to str_.
            int32_t length=ग्रन्थिः-kMinLinearMatch+1;
            सचे(maxLength_>0 && str_->length()+length>maxLength_) {
                str_->append(reinterpret_cast<ध्रुव वर्ण *>(pos),
                            maxLength_-str_->length(), errorCode);
                पटिनिवत्त truncateAndStop();
            }
            str_->append(reinterpret_cast<ध्रुव वर्ण *>(pos), length, errorCode);
            pos+=length;
        }
    }
}

StringPiece
BytesTrie::Iterator::getString() ध्रुव U_LIFETIME_BOUND {
    पटिनिवत्त str_ == nullptr ? StringPiece() : str_->toStringPiece();
}

UBool
BytesTrie::Iterator::truncateAndStop() {
    pos_=nullptr;
    मानम्=-1;  // no real value for str
    पटिनिवत्त सच्च;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ध्रुव uint8_t*
BytesTrie::Iterator::branchNext(ध्रुव uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UErrorCode& errorCode) {
    याव(length>k_Max_Branch_Linear_Sub_ग्रन्थिः_Length) {
        ++pos;  // ignore the comparison byte
        // Push state for the greater-or-equal edge.
        राशिः->addElement(static_cast<int32_t>(skipDelta(pos) - bytes_), errorCode);
        राशिः->addElement(((length-(length>>1))<<16)|str_->length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    uint8_t trieByte=*pos++;
    int32_t ग्रन्थिः=*pos++;
    UBool isFinal = static_cast<UBool>(ग्रन्थिः & k_मानम्_Is_Final);
    int32_t value=read_मानम्(pos, ग्रन्थिः>>1);
    pos=skip_मानम्(pos, ग्रन्थिः);
    राशिः->addElement(static_cast<int32_t>(pos - bytes_), errorCode);
    राशिः->addElement(((length-1)<<16)|str_->length(), errorCode);
    str_->append(static_cast<वर्ण>(trieByte), errorCode);
    सचे(isFinal) {
        pos_=nullptr;
        मानम्=value;
        पटिनिवत्त nullptr;
    } अञ्ञथा {
        पटिनिवत्त pos+value;
    }
}

U_NAMESPACE_END
