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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/bytestrie.h"
#અન્તર્ભાવ "unicode/stringpiece.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "uvectr32.h"

U_NAMESPACE_BEGIN

BytesTrie::Iterator::Iterator(અચલ શૂન્ય* trieBytes U_LIFETIME_BOUND,
                              int32_t maxStringLength,
                              UErrorCode& errorCode)
        : bytes_(static_cast<અચલ uint8_t *>(trieBytes)),
          pos_(bytes_), initialPos_(bytes_),
          remainingMatchLength_(-1), initialRemainingMatchLength_(-1),
          str_(nullptr), maxLength_(maxStringLength), કિંમત(0), ઢગલો(nullptr) {
    જો(U_FAILURE(errorCode)) {
        પાછા;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=નવું CharString();
    ઢગલો=નવું UVector32(errorCode);
    જો(U_SUCCESS(errorCode) && (str_==nullptr || ઢગલો==nullptr)) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrie::Iterator::Iterator(અચલ BytesTrie &trie, int32_t maxStringLength,
                              UErrorCode &errorCode)
        : bytes_(trie.bytes_), pos_(trie.pos_), initialPos_(trie.pos_),
          remainingMatchLength_(trie.remainingMatchLength_),
          initialRemainingMatchLength_(trie.remainingMatchLength_),
          str_(nullptr), maxLength_(maxStringLength), કિંમત(0), ઢગલો(nullptr) {
    જો(U_FAILURE(errorCode)) {
        પાછા;
    }
    str_=નવું CharString();
    ઢગલો=નવું UVector32(errorCode);
    જો(U_FAILURE(errorCode)) {
        પાછા;
    }
    જો(str_==nullptr || ઢગલો==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    જો(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        જો(maxLength_>0 && length>maxLength_) {
            length=maxLength_;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<અચલ અક્ષર *>(pos_), length, errorCode);
        pos_+=length;
        remainingMatchLength_-=length;
    }
}

BytesTrie::Iterator::~Iterator() {
    delete str_;
    delete ઢગલો;
}

BytesTrie::Iterator&
BytesTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remainingMatchLength_=initialRemainingMatchLength_;
    int32_t length=remainingMatchLength_+1;  // Remaining match length.
    જો(maxLength_>0 && length>maxLength_) {
        length=maxLength_;
    }
    str_->truncate(length);
    pos_+=length;
    remainingMatchLength_-=length;
    ઢગલો->setSize(0);
    પાછા *આ;
}

UBool
BytesTrie::Iterator::hasNext() અચલ { પાછા pos_!=nullptr || !ઢગલો->isEmpty(); }

UBool
BytesTrie::Iterator::next(UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા ખોટું;
    }
    અચલ uint8_t *pos=pos_;
    જો(pos==nullptr) {
        જો(ઢગલો->isEmpty()) {
            પાછા ખોટું;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t ઢગલો_Size=ઢગલો->size();
        int32_t length=ઢગલો->elementAti(ઢગલો_Size-1);
        pos=bytes_+ઢગલો->elementAti(ઢગલો_Size-2);
        ઢગલો->setSize(ઢગલો_Size-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        જો(length>1) {
            pos=branchNext(pos, length, errorCode);
            જો(pos==nullptr) {
                પાછા સાચું;  // Reached a final value.
            }
        } નહીંતર {
            str_->append(static_cast<અક્ષર>(*pos++), errorCode);
        }
    }
    જો(remainingMatchLength_>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        પાછા truncateAndStop();
    }
    માટે(;;) {
        int32_t ગાંઠ=*pos++;
        જો(ગાંઠ>=k_Min_કિંમત_Lead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(ગાંઠ & k_કિંમત_Is_Final);
            કિંમત=read_કિંમત(pos, ગાંઠ>>1);
            જો(isFinal || (maxLength_>0 && str_->length()==maxLength_)) {
                pos_=nullptr;
            } નહીંતર {
                pos_=skip_કિંમત(pos, ગાંઠ);
            }
            પાછા સાચું;
        }
        જો(maxLength_>0 && str_->length()==maxLength_) {
            પાછા truncateAndStop();
        }
        જો(ગાંઠ<kMinLinearMatch) {
            જો(ગાંઠ==0) {
                ગાંઠ=*pos++;
            }
            pos=branchNext(pos, ગાંઠ+1, errorCode);
            જો(pos==nullptr) {
                પાછા સાચું;  // Reached a final value.
            }
        } નહીંતર {
            // Linear-match node, append length bytes to str_.
            int32_t length=ગાંઠ-kMinLinearMatch+1;
            જો(maxLength_>0 && str_->length()+length>maxLength_) {
                str_->append(reinterpret_cast<અચલ અક્ષર *>(pos),
                            maxLength_-str_->length(), errorCode);
                પાછા truncateAndStop();
            }
            str_->append(reinterpret_cast<અચલ અક્ષર *>(pos), length, errorCode);
            pos+=length;
        }
    }
}

StringPiece
BytesTrie::Iterator::getString() અચલ U_LIFETIME_BOUND {
    પાછા str_ == nullptr ? StringPiece() : str_->toStringPiece();
}

UBool
BytesTrie::Iterator::truncateAndStop() {
    pos_=nullptr;
    કિંમત=-1;  // no real value for str
    પાછા સાચું;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
અચલ uint8_t*
BytesTrie::Iterator::branchNext(અચલ uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UErrorCode& errorCode) {
    જ્યારે(length>k_Max_Branch_Linear_Sub_ગાંઠ_Length) {
        ++pos;  // ignore the comparison byte
        // Push state for the greater-or-equal edge.
        ઢગલો->addElement(static_cast<int32_t>(skipDelta(pos) - bytes_), errorCode);
        ઢગલો->addElement(((length-(length>>1))<<16)|str_->length(), errorCode);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    uint8_t trieByte=*pos++;
    int32_t ગાંઠ=*pos++;
    UBool isFinal = static_cast<UBool>(ગાંઠ & k_કિંમત_Is_Final);
    int32_t value=read_કિંમત(pos, ગાંઠ>>1);
    pos=skip_કિંમત(pos, ગાંઠ);
    ઢગલો->addElement(static_cast<int32_t>(pos - bytes_), errorCode);
    ઢગલો->addElement(((length-1)<<16)|str_->length(), errorCode);
    str_->append(static_cast<અક્ષર>(trieByte), errorCode);
    જો(isFinal) {
        pos_=nullptr;
        કિંમત=value;
        પાછા nullptr;
    } નહીંતર {
        પાછા pos+value;
    }
}

U_NAMESPACE_END
