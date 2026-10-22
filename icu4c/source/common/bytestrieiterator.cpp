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

#ضمن "unicode/utypes.h"
#ضمن "unicode/bytestrie.h"
#ضمن "unicode/stringpiece.h"
#ضمن "charstr.h"
#ضمن "uvectr32.h"

U_NAMESPACE_BEGIN

بائيٽون_Trie::Iterator::Iterator(ثابت فراغ* trie_بائيٽون U_LIFETIME_BOUND,
                              int32_t max_String_ڊيگهائي,
                              UError_ڪوڊ& error_ڪوڊ)
        : بائيٽون_2(static_cast<ثابت uint8_t *>(trie_بائيٽون)),
          pos_(بائيٽون_2), initialPos_(بائيٽون_2),
          بچيل_ميلاپ_ڊيگهائي(-1), initial_بچيل_ميلاپ_ڊيگهائي(-1),
          str_(خالي_اشارو), max_ڊيگهائي(max_String_ڊيگهائي), قدر(0), ڍير(خالي_اشارو) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=جديد CharString();
    ڍير=جديد UVector32(error_ڪوڊ);
    إذا(U_SUCCESS(error_ڪوڊ) && (str_==خالي_اشارو || ڍير==خالي_اشارو)) {
        error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
    }
}

بائيٽون_Trie::Iterator::Iterator(ثابت بائيٽون_Trie &trie, int32_t max_String_ڊيگهائي,
                              UError_ڪوڊ &error_ڪوڊ)
        : بائيٽون_2(trie.بائيٽون_2), pos_(trie.pos_), initialPos_(trie.pos_),
          بچيل_ميلاپ_ڊيگهائي(trie.بچيل_ميلاپ_ڊيگهائي),
          initial_بچيل_ميلاپ_ڊيگهائي(trie.بچيل_ميلاپ_ڊيگهائي),
          str_(خالي_اشارو), max_ڊيگهائي(max_String_ڊيگهائي), قدر(0), ڍير(خالي_اشارو) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    str_=جديد CharString();
    ڍير=جديد UVector32(error_ڪوڊ);
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    إذا(str_==خالي_اشارو || ڍير==خالي_اشارو) {
        error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    int32_t length=بچيل_ميلاپ_ڊيگهائي;  // Actual remaining match length minus 1.
    إذا(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        إذا(max_ڊيگهائي>0 && length>max_ڊيگهائي) {
            length=max_ڊيگهائي;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<ثابت حرف *>(pos_), length, error_ڪوڊ);
        pos_+=length;
        بچيل_ميلاپ_ڊيگهائي-=length;
    }
}

بائيٽون_Trie::Iterator::~Iterator() {
    ميٽيو str_;
    ميٽيو ڍير;
}

بائيٽون_Trie::Iterator&
بائيٽون_Trie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    بچيل_ميلاپ_ڊيگهائي=initial_بچيل_ميلاپ_ڊيگهائي;
    int32_t length=بچيل_ميلاپ_ڊيگهائي+1;  // Remaining match length.
    إذا(max_ڊيگهائي>0 && length>max_ڊيگهائي) {
        length=max_ڊيگهائي;
    }
    str_->truncate(length);
    pos_+=length;
    بچيل_ميلاپ_ڊيگهائي-=length;
    ڍير->سيٽ_سائيز(0);
    أرجع *هذا;
}

UBool
بائيٽون_Trie::Iterator::has_ايندڙ() ثابت { أرجع pos_!=خالي_اشارو || !ڍير->is_خالي_جايون(); }

UBool
بائيٽون_Trie::Iterator::next(UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خطأ;
    }
    ثابت uint8_t *pos=pos_;
    إذا(pos==خالي_اشارو) {
        إذا(ڍير->is_خالي_جايون()) {
            أرجع خطأ;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t ڍير_سائيز=ڍير->size();
        int32_t length=ڍير->elementAti(ڍير_سائيز-1);
        pos=بائيٽون_2+ڍير->elementAti(ڍير_سائيز-2);
        ڍير->سيٽ_سائيز(ڍير_سائيز-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        إذا(length>1) {
            pos=branch_ايندڙ(pos, length, error_ڪوڊ);
            إذا(pos==خالي_اشارو) {
                أرجع سچ;  // Reached a final value.
            }
        } وإلا {
            str_->append(static_cast<حرف>(*pos++), error_ڪوڊ);
        }
    }
    إذا(بچيل_ميلاپ_ڊيگهائي>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        أرجع truncateAndStop();
    }
    لكل(;;) {
        int32_t ڳنڍ=*pos++;
        إذا(ڳنڍ>=k_Min_قدر_Lead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(ڳنڍ & k_قدر_Is_Final);
            قدر=read_قدر(pos, ڳنڍ>>1);
            إذا(isFinal || (max_ڊيگهائي>0 && str_->length()==max_ڊيگهائي)) {
                pos_=خالي_اشارو;
            } وإلا {
                pos_=skip_قدر(pos, ڳنڍ);
            }
            أرجع سچ;
        }
        إذا(max_ڊيگهائي>0 && str_->length()==max_ڊيگهائي) {
            أرجع truncateAndStop();
        }
        إذا(ڳنڍ<k_Min_لڪيري_ميلاپ) {
            إذا(ڳنڍ==0) {
                ڳنڍ=*pos++;
            }
            pos=branch_ايندڙ(pos, ڳنڍ+1, error_ڪوڊ);
            إذا(pos==خالي_اشارو) {
                أرجع سچ;  // Reached a final value.
            }
        } وإلا {
            // Linear-match node, append length bytes to str_.
            int32_t length=ڳنڍ-k_Min_لڪيري_ميلاپ+1;
            إذا(max_ڊيگهائي>0 && str_->length()+length>max_ڊيگهائي) {
                str_->append(reinterpret_cast<ثابت حرف *>(pos),
                            max_ڊيگهائي-str_->length(), error_ڪوڊ);
                أرجع truncateAndStop();
            }
            str_->append(reinterpret_cast<ثابت حرف *>(pos), length, error_ڪوڊ);
            pos+=length;
        }
    }
}

StringPiece
بائيٽون_Trie::Iterator::getString() ثابت U_LIFETIME_BOUND {
    أرجع str_ == خالي_اشارو ? StringPiece() : str_->منزل_String_Piece();
}

UBool
بائيٽون_Trie::Iterator::truncateAndStop() {
    pos_=خالي_اشارو;
    قدر=-1;  // no real value for str
    أرجع سچ;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ثابت uint8_t*
بائيٽون_Trie::Iterator::branch_ايندڙ(ثابت uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UError_ڪوڊ& error_ڪوڊ) {
    طالما(length>k_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي) {
        ++pos;  // ignore the comparison byte
        // Push state for the greater-or-equal edge.
        ڍير->جوڙيو_Element(static_cast<int32_t>(skipDelta(pos) - بائيٽون_2), error_ڪوڊ);
        ڍير->جوڙيو_Element(((length-(length>>1))<<16)|str_->length(), error_ڪوڊ);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    uint8_t trieByte=*pos++;
    int32_t ڳنڍ=*pos++;
    UBool isFinal = static_cast<UBool>(ڳنڍ & k_قدر_Is_Final);
    int32_t value=read_قدر(pos, ڳنڍ>>1);
    pos=skip_قدر(pos, ڳنڍ);
    ڍير->جوڙيو_Element(static_cast<int32_t>(pos - بائيٽون_2), error_ڪوڊ);
    ڍير->جوڙيو_Element(((length-1)<<16)|str_->length(), error_ڪوڊ);
    str_->append(static_cast<حرف>(trieByte), error_ڪوڊ);
    إذا(isFinal) {
        pos_=خالي_اشارو;
        قدر=value;
        أرجع خالي_اشارو;
    } وإلا {
        أرجع pos+value;
    }
}

U_NAMESPACE_پڇاڙي
