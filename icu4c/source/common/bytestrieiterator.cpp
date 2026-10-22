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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/bytestrie.h"
#அந்தர்பாவ "unicode/stringpiece.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "uvectr32.h"

U_NAMESPACE_BEGIN

BytesTrie::Iterator::Iterator(மாறிலி வெறுமை* trieBytes U_LIFETIME_BOUND,
                              int32_t maxStringLength,
                              UErrorCode& errorCode)
        : bytes_(static_cast<மாறிலி uint8_t *>(trieBytes)),
          pos_(bytes_), initialPos_(bytes_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          str_(nullptr), maxLength_(maxStringLength), மதிப்பு(0), அடுக்கு(nullptr) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=புதிய CharString();
    அடுக்கு=புதிய UVector32(errorCode);
    எனில்(U_SUCCESS(errorCode) && (str_==nullptr || அடுக்கு==nullptr)) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrie::Iterator::Iterator(மாறிலி BytesTrie &trie, int32_t maxStringLength,
                              UErrorCode &errorCode)
        : bytes_(trie.bytes_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          str_(nullptr), maxLength_(maxStringLength), மதிப்பு(0), அடுக்கு(nullptr) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு;
    }
    str_=புதிய CharString();
    அடுக்கு=புதிய UVector32(errorCode);
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு;
    }
    எனில்(str_==nullptr || அடுக்கு==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    எனில்(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        எனில்(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<மாறிலி எழுத்து *>(pos_), length, errorCode);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

BytesTrie::Iterator::~Iterator() {
    delete str_;
    delete அடுக்கு;
}

BytesTrie::Iterator&
BytesTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    எனில்(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_->truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    அடுக்கு->setSize(0);
    திரும்பு *இது;
}

UBool
BytesTrie::Iterator::hasNext() மாறிலி { திரும்பு pos_!=nullptr || !அடுக்கு->isEmpty(); }

UBool
BytesTrie::Iterator::next(UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு பொய்;
    }
    மாறிலி uint8_t *pos=pos_;
    எனில்(pos==nullptr) {
        எனில்(அடுக்கு->isEmpty()) {
            திரும்பு பொய்;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t அடுக்கு_Size=அடுக்கு->size();
        int32_t length=அடுக்கு->elementAti(அடுக்கு_Size-1);
        pos=bytes_+அடுக்கு->elementAti(அடுக்கு_Size-2);
        அடுக்கு->setSize(அடுக்கு_Size-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        எனில்(length>1) {
            pos=branchNext(pos, length, errorCode);
            எனில்(pos==nullptr) {
                திரும்பு உண்மை;  // Reached a final value.
            }
        } இல்லையெனில் {
            str_->append(static_cast<எழுத்து>(*pos++), errorCode);
        }
    }
    எனில்(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        திரும்பு truncateAndStop();
    }
    க்கு(;;) {
        int32_t முனை=*pos++;
        எனில்(முனை>=k_Min_மதிப்பு_Lead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(முனை & k_மதிப்பு_Is_Final);
            மதிப்பு=read_மதிப்பு(pos, முனை>>1);
            எனில்(isFinal || (maxLength_>0 && str_->length()==maxLength_)) {
                pos_=nullptr;
            } இல்லையெனில் {
                pos_=skip_மதிப்பு(pos, முனை);
            }
            திரும்பு உண்மை;
        }
        எனில்(maxLength_>0 && str_->length()==maxLength_) {
            திரும்பு truncateAndStop();
        }
        எனில்(முனை<kMinLinearMatch) {
            எனில்(முனை==0) {
                முனை=*pos++;
            }
            pos=branchNext(pos, முனை+1, errorCode);
            எனில்(pos==nullptr) {
                திரும்பு உண்மை;  // Reached a final value.
            }
        } இல்லையெனில் {
            // Linear-match node, append length bytes to str_.
            int32_t length=முனை-kMinLinearMatch+1;
            எனில்(maxLength_>0 && str_->length()+length>maxLength_) {
                str_->append(reinterpret_cast<மாறிலி எழுத்து *>(pos),
                            maxLength_-str_->length(), errorCode);
                திரும்பு truncateAndStop();
            }
            str_->append(reinterpret_cast<மாறிலி எழுத்து *>(pos), length, errorCode);
            pos+=length;
        }
    }
}

StringPiece
BytesTrie::Iterator::getString() மாறிலி U_LIFETIME_BOUND {
    திரும்பு str_ == nullptr ? StringPiece() : str_->toStringPiece();
}

UBool
BytesTrie::Iterator::truncateAndStop() {
    pos_=nullptr;
    மதிப்பு=-1;  // no real value for str
    திரும்பு உண்மை;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
மாறிலி uint8_t*
BytesTrie::Iterator::branchNext(மாறிலி uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UErrorCode& errorCode) {
    போது(length>k_Max_Branch_Linear_Sub_முனை_Length) {
        ++pos;  // ignore the comparison byte
        // Push state for the greater-or-equal edge.
        அடுக்கு->addElement(static_cast<int32_t>(skipDelta(pos) - bytes_), errorCode);
        அடுக்கு->addElement(((length-(length>>1))<<16)|str_->length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    uint8_t trieByte=*pos++;
    int32_t முனை=*pos++;
    UBool isFinal = static_cast<UBool>(முனை & k_மதிப்பு_Is_Final);
    int32_t value=read_மதிப்பு(pos, முனை>>1);
    pos=skip_மதிப்பு(pos, முனை);
    அடுக்கு->addElement(static_cast<int32_t>(pos - bytes_), errorCode);
    அடுக்கு->addElement(((length-1)<<16)|str_->length(), errorCode);
    str_->append(static_cast<எழுத்து>(trieByte), errorCode);
    எனில்(isFinal) {
        pos_=nullptr;
        மதிப்பு=value;
        திரும்பு nullptr;
    } இல்லையெனில் {
        திரும்பு pos+value;
    }
}

U_NAMESPACE_END
