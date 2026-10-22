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

BytesTrie::Iterator::Iterator(स्थिरांक शून्य* trieBytes U_LIFETIME_BOUND,
                              int32_t maxStringLength,
                              UErrorCode& errorCode)
        : bytes_(static_cast<स्थिरांक uint8_t *>(trieBytes)),
          pos_(bytes_), initialPos_(bytes_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          str_(nullptr), maxLength_(maxStringLength), मूल्य(0), चळत(nullptr) {
    जर(U_FAILURE(errorCode)) {
        परत;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=नवीन CharString();
    चळत=नवीन UVector32(errorCode);
    जर(U_SUCCESS(errorCode) && (str_==nullptr || चळत==nullptr)) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrie::Iterator::Iterator(स्थिरांक BytesTrie &trie, int32_t maxStringLength,
                              UErrorCode &errorCode)
        : bytes_(trie.bytes_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          str_(nullptr), maxLength_(maxStringLength), मूल्य(0), चळत(nullptr) {
    जर(U_FAILURE(errorCode)) {
        परत;
    }
    str_=नवीन CharString();
    चळत=नवीन UVector32(errorCode);
    जर(U_FAILURE(errorCode)) {
        परत;
    }
    जर(str_==nullptr || चळत==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    जर(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        जर(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<स्थिरांक अक्षर *>(pos_), length, errorCode);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

BytesTrie::Iterator::~Iterator() {
    delete str_;
    delete चळत;
}

BytesTrie::Iterator&
BytesTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    जर(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_->truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    चळत->setSize(0);
    परत *हे;
}

UBool
BytesTrie::Iterator::hasNext() स्थिरांक { परत pos_!=nullptr || !चळत->isEmpty(); }

UBool
BytesTrie::Iterator::next(UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत असत्य;
    }
    स्थिरांक uint8_t *pos=pos_;
    जर(pos==nullptr) {
        जर(चळत->isEmpty()) {
            परत असत्य;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t चळत_Size=चळत->size();
        int32_t length=चळत->elementAti(चळत_Size-1);
        pos=bytes_+चळत->elementAti(चळत_Size-2);
        चळत->setSize(चळत_Size-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        जर(length>1) {
            pos=branchNext(pos, length, errorCode);
            जर(pos==nullptr) {
                परत सत्य;  // Reached a final value.
            }
        } अन्यथा {
            str_->append(static_cast<अक्षर>(*pos++), errorCode);
        }
    }
    जर(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        परत truncateAndStop();
    }
    साठी(;;) {
        int32_t गाठ=*pos++;
        जर(गाठ>=k_Min_मूल्य_Lead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(गाठ & k_मूल्य_Is_Final);
            मूल्य=read_मूल्य(pos, गाठ>>1);
            जर(isFinal || (maxLength_>0 && str_->length()==maxLength_)) {
                pos_=nullptr;
            } अन्यथा {
                pos_=skip_मूल्य(pos, गाठ);
            }
            परत सत्य;
        }
        जर(maxLength_>0 && str_->length()==maxLength_) {
            परत truncateAndStop();
        }
        जर(गाठ<kMinLinearMatch) {
            जर(गाठ==0) {
                गाठ=*pos++;
            }
            pos=branchNext(pos, गाठ+1, errorCode);
            जर(pos==nullptr) {
                परत सत्य;  // Reached a final value.
            }
        } अन्यथा {
            // Linear-match node, append length bytes to str_.
            int32_t length=गाठ-kMinLinearMatch+1;
            जर(maxLength_>0 && str_->length()+length>maxLength_) {
                str_->append(reinterpret_cast<स्थिरांक अक्षर *>(pos),
                            maxLength_-str_->length(), errorCode);
                परत truncateAndStop();
            }
            str_->append(reinterpret_cast<स्थिरांक अक्षर *>(pos), length, errorCode);
            pos+=length;
        }
    }
}

StringPiece
BytesTrie::Iterator::getString() स्थिरांक U_LIFETIME_BOUND {
    परत str_ == nullptr ? StringPiece() : str_->toStringPiece();
}

UBool
BytesTrie::Iterator::truncateAndStop() {
    pos_=nullptr;
    मूल्य=-1;  // no real value for str
    परत सत्य;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
स्थिरांक uint8_t*
BytesTrie::Iterator::branchNext(स्थिरांक uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UErrorCode& errorCode) {
    पर्यंत(length>k_Max_Branch_Linear_Sub_गाठ_Length) {
        ++pos;  // ignore the comparison byte
        // Push state for the greater-or-equal edge.
        चळत->addElement(static_cast<int32_t>(skipDelta(pos) - bytes_), errorCode);
        चळत->addElement(((length-(length>>1))<<16)|str_->length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    uint8_t trieByte=*pos++;
    int32_t गाठ=*pos++;
    UBool isFinal = static_cast<UBool>(गाठ & k_मूल्य_Is_Final);
    int32_t value=read_मूल्य(pos, गाठ>>1);
    pos=skip_मूल्य(pos, गाठ);
    चळत->addElement(static_cast<int32_t>(pos - bytes_), errorCode);
    चळत->addElement(((length-1)<<16)|str_->length(), errorCode);
    str_->append(static_cast<अक्षर>(trieByte), errorCode);
    जर(isFinal) {
        pos_=nullptr;
        मूल्य=value;
        परत nullptr;
    } अन्यथा {
        परत pos+value;
    }
}

U_NAMESPACE_END
