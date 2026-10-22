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

بایټونه_Trie::Iterator::Iterator(ثابت فراغ* trie_بایټونه U_LIFETIME_BOUND,
                              int32_t max_String_اوږدتیا,
                              UError_کوډ& error_کوډ)
        : بایټونه_2(static_cast<ثابت uint8_t *>(trie_بایټونه)),
          pos_(بایټونه_2), initialPos_(بایټونه_2),
          پاتې_شوی_سمون_اوږدتیا(-1), initial_پاتې_شوی_سمون_اوږدتیا(-1),
          str_(تش_اشاره), max_اوږدتیا(max_String_اوږدتیا), ارزښت(0), ډېری(تش_اشاره) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=جديد CharString();
    ډېری=جديد UVector32(error_کوډ);
    إذا(U_SUCCESS(error_کوډ) && (str_==تش_اشاره || ډېری==تش_اشاره)) {
        error_کوډ=U_حافظه_ویش_ERROR;
    }
}

بایټونه_Trie::Iterator::Iterator(ثابت بایټونه_Trie &trie, int32_t max_String_اوږدتیا,
                              UError_کوډ &error_کوډ)
        : بایټونه_2(trie.بایټونه_2), pos_(trie.pos_), initialPos_(trie.pos_),
          پاتې_شوی_سمون_اوږدتیا(trie.پاتې_شوی_سمون_اوږدتیا),
          initial_پاتې_شوی_سمون_اوږدتیا(trie.پاتې_شوی_سمون_اوږدتیا),
          str_(تش_اشاره), max_اوږدتیا(max_String_اوږدتیا), ارزښت(0), ډېری(تش_اشاره) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع;
    }
    str_=جديد CharString();
    ډېری=جديد UVector32(error_کوډ);
    إذا(U_FAILURE(error_کوډ)) {
        أرجع;
    }
    إذا(str_==تش_اشاره || ډېری==تش_اشاره) {
        error_کوډ=U_حافظه_ویش_ERROR;
        أرجع;
    }
    int32_t length=پاتې_شوی_سمون_اوږدتیا;  // Actual remaining match length minus 1.
    إذا(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        إذا(max_اوږدتیا>0 && length>max_اوږدتیا) {
            length=max_اوږدتیا;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<ثابت حرف *>(pos_), length, error_کوډ);
        pos_+=length;
        پاتې_شوی_سمون_اوږدتیا-=length;
    }
}

بایټونه_Trie::Iterator::~Iterator() {
    ړنګول str_;
    ړنګول ډېری;
}

بایټونه_Trie::Iterator&
بایټونه_Trie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    پاتې_شوی_سمون_اوږدتیا=initial_پاتې_شوی_سمون_اوږدتیا;
    int32_t length=پاتې_شوی_سمون_اوږدتیا+1;  // Remaining match length.
    إذا(max_اوږدتیا>0 && length>max_اوږدتیا) {
        length=max_اوږدتیا;
    }
    str_->truncate(length);
    pos_+=length;
    پاتې_شوی_سمون_اوږدتیا-=length;
    ډېری->سېټ_اندازه(0);
    أرجع *هذا;
}

UBool
بایټونه_Trie::Iterator::has_بل() ثابت { أرجع pos_!=تش_اشاره || !ډېری->is_تشه(); }

UBool
بایټونه_Trie::Iterator::next(UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع خطأ;
    }
    ثابت uint8_t *pos=pos_;
    إذا(pos==تش_اشاره) {
        إذا(ډېری->is_تشه()) {
            أرجع خطأ;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t ډېری_اندازه=ډېری->size();
        int32_t length=ډېری->elementAti(ډېری_اندازه-1);
        pos=بایټونه_2+ډېری->elementAti(ډېری_اندازه-2);
        ډېری->سېټ_اندازه(ډېری_اندازه-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        إذا(length>1) {
            pos=branch_بل(pos, length, error_کوډ);
            إذا(pos==تش_اشاره) {
                أرجع ریښتیا;  // Reached a final value.
            }
        } وإلا {
            str_->append(static_cast<حرف>(*pos++), error_کوډ);
        }
    }
    إذا(پاتې_شوی_سمون_اوږدتیا>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        أرجع truncateAndStop();
    }
    لكل(;;) {
        int32_t غوټه=*pos++;
        إذا(غوټه>=k_Min_ارزښت_Lead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(غوټه & k_ارزښت_Is_Final);
            ارزښت=read_ارزښت(pos, غوټه>>1);
            إذا(isFinal || (max_اوږدتیا>0 && str_->length()==max_اوږدتیا)) {
                pos_=تش_اشاره;
            } وإلا {
                pos_=skip_ارزښت(pos, غوټه);
            }
            أرجع ریښتیا;
        }
        إذا(max_اوږدتیا>0 && str_->length()==max_اوږدتیا) {
            أرجع truncateAndStop();
        }
        إذا(غوټه<k_Min_خطي_سمون) {
            إذا(غوټه==0) {
                غوټه=*pos++;
            }
            pos=branch_بل(pos, غوټه+1, error_کوډ);
            إذا(pos==تش_اشاره) {
                أرجع ریښتیا;  // Reached a final value.
            }
        } وإلا {
            // Linear-match node, append length bytes to str_.
            int32_t length=غوټه-k_Min_خطي_سمون+1;
            إذا(max_اوږدتیا>0 && str_->length()+length>max_اوږدتیا) {
                str_->append(reinterpret_cast<ثابت حرف *>(pos),
                            max_اوږدتیا-str_->length(), error_کوډ);
                أرجع truncateAndStop();
            }
            str_->append(reinterpret_cast<ثابت حرف *>(pos), length, error_کوډ);
            pos+=length;
        }
    }
}

StringPiece
بایټونه_Trie::Iterator::getString() ثابت U_LIFETIME_BOUND {
    أرجع str_ == تش_اشاره ? StringPiece() : str_->منزل_String_Piece();
}

UBool
بایټونه_Trie::Iterator::truncateAndStop() {
    pos_=تش_اشاره;
    ارزښت=-1;  // no real value for str
    أرجع ریښتیا;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ثابت uint8_t*
بایټونه_Trie::Iterator::branch_بل(ثابت uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UError_کوډ& error_کوډ) {
    طالما(length>k_Max_Branch_خطي_Sub_غوټه_اوږدتیا) {
        ++pos;  // ignore the comparison byte
        // Push state for the greater-or-equal edge.
        ډېری->جمع_کړه_Element(static_cast<int32_t>(skipDelta(pos) - بایټونه_2), error_کوډ);
        ډېری->جمع_کړه_Element(((length-(length>>1))<<16)|str_->length(), error_کوډ);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    uint8_t trieByte=*pos++;
    int32_t غوټه=*pos++;
    UBool isFinal = static_cast<UBool>(غوټه & k_ارزښت_Is_Final);
    int32_t value=read_ارزښت(pos, غوټه>>1);
    pos=skip_ارزښت(pos, غوټه);
    ډېری->جمع_کړه_Element(static_cast<int32_t>(pos - بایټونه_2), error_کوډ);
    ډېری->جمع_کړه_Element(((length-1)<<16)|str_->length(), error_کوډ);
    str_->append(static_cast<حرف>(trieByte), error_کوډ);
    إذا(isFinal) {
        pos_=تش_اشاره;
        ارزښت=value;
        أرجع تش_اشاره;
    } وإلا {
        أرجع pos+value;
    }
}

U_NAMESPACE_پای_ټکی
