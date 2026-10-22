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

BytesTrie::Iterator::Iterator(ধ্রুবক শূন্য* trieBytes U_LIFETIME_BOUND,
                              int32_t max_String_দৈর্ঘ্য,
                              UErrorCode& errorCode)
        : bytes_(static_cast<ধ্রুবক uint8_t *>(trieBytes)),
          pos_(bytes_), initialPos_(bytes_),
          remaining_Match_দৈর্ঘ্য(-1), initial_Remaining_Match_দৈর্ঘ্য(-1),
          str_(nullptr), max_দৈর্ঘ্য(max_String_দৈর্ঘ্য), মান(0), স্তূপ(nullptr) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=নতুন CharString();
    স্তূপ=নতুন UVector32(errorCode);
    যদি(U_SUCCESS(errorCode) && (str_==nullptr || স্তূপ==nullptr)) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrie::Iterator::Iterator(ধ্রুবক BytesTrie &trie, int32_t max_String_দৈর্ঘ্য,
                              UErrorCode &errorCode)
        : bytes_(trie.bytes_), pos_(trie.pos_), initialPos_(trie.pos_),
          remaining_Match_দৈর্ঘ্য(trie.remaining_Match_দৈর্ঘ্য),
          initial_Remaining_Match_দৈর্ঘ্য(trie.remaining_Match_দৈর্ঘ্য),
          str_(nullptr), max_দৈর্ঘ্য(max_String_দৈর্ঘ্য), মান(0), স্তূপ(nullptr) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত;
    }
    str_=নতুন CharString();
    স্তূপ=নতুন UVector32(errorCode);
    যদি(U_FAILURE(errorCode)) {
        ফেরত;
    }
    যদি(str_==nullptr || স্তূপ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    int32_t length=remaining_Match_দৈর্ঘ্য;  // Actual remaining match length minus 1.
    যদি(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        যদি(max_দৈর্ঘ্য>0 && length>max_দৈর্ঘ্য) {
            length=max_দৈর্ঘ্য;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<ধ্রুবক অক্ষর *>(pos_), length, errorCode);
        pos_+=length;
        remaining_Match_দৈর্ঘ্য-=length;
    }
}

BytesTrie::Iterator::~Iterator() {
    delete str_;
    delete স্তূপ;
}

BytesTrie::Iterator&
BytesTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remaining_Match_দৈর্ঘ্য=initial_Remaining_Match_দৈর্ঘ্য;
    int32_t length=remaining_Match_দৈর্ঘ্য+1;  // Remaining match length.
    যদি(max_দৈর্ঘ্য>0 && length>max_দৈর্ঘ্য) {
        length=max_দৈর্ঘ্য;
    }
    str_->truncate(length);
    pos_+=length;
    remaining_Match_দৈর্ঘ্য-=length;
    স্তূপ->set_আকার(0);
    ফেরত *এই;
}

UBool
BytesTrie::Iterator::has_পরবর্তী() ধ্রুবক { ফেরত pos_!=nullptr || !স্তূপ->isEmpty(); }

UBool
BytesTrie::Iterator::next(UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত মিথ্যা;
    }
    ধ্রুবক uint8_t *pos=pos_;
    যদি(pos==nullptr) {
        যদি(স্তূপ->isEmpty()) {
            ফেরত মিথ্যা;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t স্তূপ_আকার=স্তূপ->size();
        int32_t length=স্তূপ->elementAti(স্তূপ_আকার-1);
        pos=bytes_+স্তূপ->elementAti(স্তূপ_আকার-2);
        স্তূপ->set_আকার(স্তূপ_আকার-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        যদি(length>1) {
            pos=branch_পরবর্তী(pos, length, errorCode);
            যদি(pos==nullptr) {
                ফেরত সত্য;  // Reached a final value.
            }
        } অন্যথা {
            str_->append(static_cast<অক্ষর>(*pos++), errorCode);
        }
    }
    যদি(remaining_Match_দৈর্ঘ্য>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        ফেরত truncateAndStop();
    }
    জন্য(;;) {
        int32_t গ্রন্থি=*pos++;
        যদি(গ্রন্থি>=k_Min_মান_Lead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(গ্রন্থি & k_মান_Is_Final);
            মান=read_মান(pos, গ্রন্থি>>1);
            যদি(isFinal || (max_দৈর্ঘ্য>0 && str_->length()==max_দৈর্ঘ্য)) {
                pos_=nullptr;
            } অন্যথা {
                pos_=skip_মান(pos, গ্রন্থি);
            }
            ফেরত সত্য;
        }
        যদি(max_দৈর্ঘ্য>0 && str_->length()==max_দৈর্ঘ্য) {
            ফেরত truncateAndStop();
        }
        যদি(গ্রন্থি<kMinLinearMatch) {
            যদি(গ্রন্থি==0) {
                গ্রন্থি=*pos++;
            }
            pos=branch_পরবর্তী(pos, গ্রন্থি+1, errorCode);
            যদি(pos==nullptr) {
                ফেরত সত্য;  // Reached a final value.
            }
        } অন্যথা {
            // Linear-match node, append length bytes to str_.
            int32_t length=গ্রন্থি-kMinLinearMatch+1;
            যদি(max_দৈর্ঘ্য>0 && str_->length()+length>max_দৈর্ঘ্য) {
                str_->append(reinterpret_cast<ধ্রুবক অক্ষর *>(pos),
                            max_দৈর্ঘ্য-str_->length(), errorCode);
                ফেরত truncateAndStop();
            }
            str_->append(reinterpret_cast<ধ্রুবক অক্ষর *>(pos), length, errorCode);
            pos+=length;
        }
    }
}

StringPiece
BytesTrie::Iterator::getString() ধ্রুবক U_LIFETIME_BOUND {
    ফেরত str_ == nullptr ? StringPiece() : str_->toStringPiece();
}

UBool
BytesTrie::Iterator::truncateAndStop() {
    pos_=nullptr;
    মান=-1;  // no real value for str
    ফেরত সত্য;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ধ্রুবক uint8_t*
BytesTrie::Iterator::branch_পরবর্তী(ধ্রুবক uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UErrorCode& errorCode) {
    যখন(length>k_Max_Branch_Linear_Sub_গ্রন্থি_দৈর্ঘ্য) {
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
    str_->append(static_cast<অক্ষর>(trieByte), errorCode);
    যদি(isFinal) {
        pos_=nullptr;
        মান=value;
        ফেরত nullptr;
    } অন্যথা {
        ফেরত pos+value;
    }
}

U_NAMESPACE_END
