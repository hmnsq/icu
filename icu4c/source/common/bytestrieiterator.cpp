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

#समावेश "unicode/utypes.h"
#समावेश "unicode/bytestrie.h"
#समावेश "unicode/stringpiece.h"
#समावेश "charstr.h"
#समावेश "uvectr32.h"

U_NAMESPACE_BEGIN

BytesTrie::Iterator::Iterator(स्थिर व्योम* trieBytes U_LIFETIME_BOUND,
                              int32_t max_String_लंबाई,
                              UErrorCode& errorCode)
        : bytes_(static_cast<स्थिर uint8_t *>(trieBytes)),
          pos_(bytes_), initialPos_(bytes_),
          remaining_Match_लंबाई(-1), initial_Remaining_Match_लंबाई(-1),
          str_(nullptr), max_लंबाई(max_String_लंबाई), मूल्य(0), ढेर(nullptr) {
    अगर(U_FAILURE(errorCode)) {
        वापस;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=नया CharString();
    ढेर=नया UVector32(errorCode);
    अगर(U_SUCCESS(errorCode) && (str_==nullptr || ढेर==nullptr)) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrie::Iterator::Iterator(स्थिर BytesTrie &trie, int32_t max_String_लंबाई,
                              UErrorCode &errorCode)
        : bytes_(trie.bytes_), pos_(trie.pos_), initialPos_(trie.pos_),
          remaining_Match_लंबाई(trie.remaining_Match_लंबाई),
          initial_Remaining_Match_लंबाई(trie.remaining_Match_लंबाई),
          str_(nullptr), max_लंबाई(max_String_लंबाई), मूल्य(0), ढेर(nullptr) {
    अगर(U_FAILURE(errorCode)) {
        वापस;
    }
    str_=नया CharString();
    ढेर=नया UVector32(errorCode);
    अगर(U_FAILURE(errorCode)) {
        वापस;
    }
    अगर(str_==nullptr || ढेर==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    int32_t length=remaining_Match_लंबाई;  // Actual remaining match length minus 1.
    अगर(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        अगर(max_लंबाई>0 && length>max_लंबाई) {
            length=max_लंबाई;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<स्थिर अक्षर *>(pos_), length, errorCode);
        pos_+=length;
        remaining_Match_लंबाई-=length;
    }
}

BytesTrie::Iterator::~Iterator() {
    मिटाओ str_;
    मिटाओ ढेर;
}

BytesTrie::Iterator&
BytesTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remaining_Match_लंबाई=initial_Remaining_Match_लंबाई;
    int32_t length=remaining_Match_लंबाई+1;  // Remaining match length.
    अगर(max_लंबाई>0 && length>max_लंबाई) {
        length=max_लंबाई;
    }
    str_->truncate(length);
    pos_+=length;
    remaining_Match_लंबाई-=length;
    ढेर->set_आकार(0);
    वापस *यह;
}

UBool
BytesTrie::Iterator::has_अग्र() स्थिर { वापस pos_!=nullptr || !ढेर->isEmpty(); }

UBool
BytesTrie::Iterator::next(UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस false;
    }
    स्थिर uint8_t *pos=pos_;
    अगर(pos==nullptr) {
        अगर(ढेर->isEmpty()) {
            वापस false;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t ढेर_आकार=ढेर->size();
        int32_t length=ढेर->elementAti(ढेर_आकार-1);
        pos=bytes_+ढेर->elementAti(ढेर_आकार-2);
        ढेर->set_आकार(ढेर_आकार-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        अगर(length>1) {
            pos=branch_अग्र(pos, length, errorCode);
            अगर(pos==nullptr) {
                वापस true;  // Reached a final value.
            }
        } अन्यथा {
            str_->append(static_cast<अक्षर>(*pos++), errorCode);
        }
    }
    अगर(remaining_Match_लंबाई>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        वापस truncateAndStop();
    }
    क्रम(;;) {
        int32_t गाँठ=*pos++;
        अगर(गाँठ>=k_Min_मूल्य_Lead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(गाँठ & k_मूल्य_Is_Final);
            मूल्य=read_मूल्य(pos, गाँठ>>1);
            अगर(isFinal || (max_लंबाई>0 && str_->length()==max_लंबाई)) {
                pos_=nullptr;
            } अन्यथा {
                pos_=skip_मूल्य(pos, गाँठ);
            }
            वापस true;
        }
        अगर(max_लंबाई>0 && str_->length()==max_लंबाई) {
            वापस truncateAndStop();
        }
        अगर(गाँठ<kMinLinearMatch) {
            अगर(गाँठ==0) {
                गाँठ=*pos++;
            }
            pos=branch_अग्र(pos, गाँठ+1, errorCode);
            अगर(pos==nullptr) {
                वापस true;  // Reached a final value.
            }
        } अन्यथा {
            // Linear-match node, append length bytes to str_.
            int32_t length=गाँठ-kMinLinearMatch+1;
            अगर(max_लंबाई>0 && str_->length()+length>max_लंबाई) {
                str_->append(reinterpret_cast<स्थिर अक्षर *>(pos),
                            max_लंबाई-str_->length(), errorCode);
                वापस truncateAndStop();
            }
            str_->append(reinterpret_cast<स्थिर अक्षर *>(pos), length, errorCode);
            pos+=length;
        }
    }
}

StringPiece
BytesTrie::Iterator::getString() स्थिर U_LIFETIME_BOUND {
    वापस str_ == nullptr ? StringPiece() : str_->toStringPiece();
}

UBool
BytesTrie::Iterator::truncateAndStop() {
    pos_=nullptr;
    मूल्य=-1;  // no real value for str
    वापस true;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
स्थिर uint8_t*
BytesTrie::Iterator::branch_अग्र(स्थिर uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UErrorCode& errorCode) {
    जबतक(length>k_Max_Branch_Linear_Sub_गाँठ_लंबाई) {
        ++pos;  // ignore the comparison byte
        // Push state for the greater-or-equal edge.
        ढेर->addElement(static_cast<int32_t>(skipDelta(pos) - bytes_), errorCode);
        ढेर->addElement(((length-(length>>1))<<16)|str_->length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    uint8_t trieByte=*pos++;
    int32_t गाँठ=*pos++;
    UBool isFinal = static_cast<UBool>(गाँठ & k_मूल्य_Is_Final);
    int32_t value=read_मूल्य(pos, गाँठ>>1);
    pos=skip_मूल्य(pos, गाँठ);
    ढेर->addElement(static_cast<int32_t>(pos - bytes_), errorCode);
    ढेर->addElement(((length-1)<<16)|str_->length(), errorCode);
    str_->append(static_cast<अक्षर>(trieByte), errorCode);
    अगर(isFinal) {
        pos_=nullptr;
        मूल्य=value;
        वापस nullptr;
    } अन्यथा {
        वापस pos+value;
    }
}

U_NAMESPACE_END
