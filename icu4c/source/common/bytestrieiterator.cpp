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

بائٹاں_Trie::Iterator::Iterator(ثابت فراغ* trie_بائٹاں U_LIFETIME_BOUND,
                              int32_t max_String_لمائی,
                              UError_کوڈ& error_کوڈ)
        : بائٹاں_2(static_cast<ثابت uint8_t *>(trie_بائٹاں)),
          pos_(بائٹاں_2), initialPos_(بائٹاں_2),
          بچیا_میل_لمائی(-1), initial_بچیا_میل_لمائی(-1),
          str_(خالی_سوچک), max_لمائی(max_String_لمائی), قدر(0), ڈھیر(خالی_سوچک) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=جديد CharString();
    ڈھیر=جديد UVector32(error_کوڈ);
    إذا(U_SUCCESS(error_کوڈ) && (str_==خالی_سوچک || ڈھیر==خالی_سوچک)) {
        error_کوڈ=U_حافظہ_ونڈ_ERROR;
    }
}

بائٹاں_Trie::Iterator::Iterator(ثابت بائٹاں_Trie &trie, int32_t max_String_لمائی,
                              UError_کوڈ &error_کوڈ)
        : بائٹاں_2(trie.بائٹاں_2), pos_(trie.pos_), initialPos_(trie.pos_),
          بچیا_میل_لمائی(trie.بچیا_میل_لمائی),
          initial_بچیا_میل_لمائی(trie.بچیا_میل_لمائی),
          str_(خالی_سوچک), max_لمائی(max_String_لمائی), قدر(0), ڈھیر(خالی_سوچک) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    str_=جديد CharString();
    ڈھیر=جديد UVector32(error_کوڈ);
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    إذا(str_==خالی_سوچک || ڈھیر==خالی_سوچک) {
        error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    int32_t length=بچیا_میل_لمائی;  // Actual remaining match length minus 1.
    إذا(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        إذا(max_لمائی>0 && length>max_لمائی) {
            length=max_لمائی;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<ثابت حرف *>(pos_), length, error_کوڈ);
        pos_+=length;
        بچیا_میل_لمائی-=length;
    }
}

بائٹاں_Trie::Iterator::~Iterator() {
    مٹاؤ str_;
    مٹاؤ ڈھیر;
}

بائٹاں_Trie::Iterator&
بائٹاں_Trie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    بچیا_میل_لمائی=initial_بچیا_میل_لمائی;
    int32_t length=بچیا_میل_لمائی+1;  // Remaining match length.
    إذا(max_لمائی>0 && length>max_لمائی) {
        length=max_لمائی;
    }
    str_->truncate(length);
    pos_+=length;
    بچیا_میل_لمائی-=length;
    ڈھیر->سیٹ_ناپ(0);
    أرجع *هذا;
}

UBool
بائٹاں_Trie::Iterator::has_اگلا() ثابت { أرجع pos_!=خالی_سوچک || !ڈھیر->is_خالی_تھاواں(); }

UBool
بائٹاں_Trie::Iterator::next(UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع خطأ;
    }
    ثابت uint8_t *pos=pos_;
    إذا(pos==خالی_سوچک) {
        إذا(ڈھیر->is_خالی_تھاواں()) {
            أرجع خطأ;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t ڈھیر_ناپ=ڈھیر->size();
        int32_t length=ڈھیر->elementAti(ڈھیر_ناپ-1);
        pos=بائٹاں_2+ڈھیر->elementAti(ڈھیر_ناپ-2);
        ڈھیر->سیٹ_ناپ(ڈھیر_ناپ-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        إذا(length>1) {
            pos=branch_اگلا(pos, length, error_کوڈ);
            إذا(pos==خالی_سوچک) {
                أرجع سچ;  // Reached a final value.
            }
        } وإلا {
            str_->append(static_cast<حرف>(*pos++), error_کوڈ);
        }
    }
    إذا(بچیا_میل_لمائی>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        أرجع truncateAndStop();
    }
    لكل(;;) {
        int32_t گرہ=*pos++;
        إذا(گرہ>=k_Min_قدر_Lead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(گرہ & k_قدر_Is_Final);
            قدر=read_قدر(pos, گرہ>>1);
            إذا(isFinal || (max_لمائی>0 && str_->length()==max_لمائی)) {
                pos_=خالی_سوچک;
            } وإلا {
                pos_=skip_قدر(pos, گرہ);
            }
            أرجع سچ;
        }
        إذا(max_لمائی>0 && str_->length()==max_لمائی) {
            أرجع truncateAndStop();
        }
        إذا(گرہ<k_Min_خطی_میل) {
            إذا(گرہ==0) {
                گرہ=*pos++;
            }
            pos=branch_اگلا(pos, گرہ+1, error_کوڈ);
            إذا(pos==خالی_سوچک) {
                أرجع سچ;  // Reached a final value.
            }
        } وإلا {
            // Linear-match node, append length bytes to str_.
            int32_t length=گرہ-k_Min_خطی_میل+1;
            إذا(max_لمائی>0 && str_->length()+length>max_لمائی) {
                str_->append(reinterpret_cast<ثابت حرف *>(pos),
                            max_لمائی-str_->length(), error_کوڈ);
                أرجع truncateAndStop();
            }
            str_->append(reinterpret_cast<ثابت حرف *>(pos), length, error_کوڈ);
            pos+=length;
        }
    }
}

StringPiece
بائٹاں_Trie::Iterator::getString() ثابت U_LIFETIME_BOUND {
    أرجع str_ == خالی_سوچک ? StringPiece() : str_->منزل_String_Piece();
}

UBool
بائٹاں_Trie::Iterator::truncateAndStop() {
    pos_=خالی_سوچک;
    قدر=-1;  // no real value for str
    أرجع سچ;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ثابت uint8_t*
بائٹاں_Trie::Iterator::branch_اگلا(ثابت uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UError_کوڈ& error_کوڈ) {
    طالما(length>k_Max_Branch_خطی_Sub_گرہ_لمائی) {
        ++pos;  // ignore the comparison byte
        // Push state for the greater-or-equal edge.
        ڈھیر->جوڑو_Element(static_cast<int32_t>(skipDelta(pos) - بائٹاں_2), error_کوڈ);
        ڈھیر->جوڑو_Element(((length-(length>>1))<<16)|str_->length(), error_کوڈ);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    uint8_t trieByte=*pos++;
    int32_t گرہ=*pos++;
    UBool isFinal = static_cast<UBool>(گرہ & k_قدر_Is_Final);
    int32_t value=read_قدر(pos, گرہ>>1);
    pos=skip_قدر(pos, گرہ);
    ڈھیر->جوڑو_Element(static_cast<int32_t>(pos - بائٹاں_2), error_کوڈ);
    ڈھیر->جوڑو_Element(((length-1)<<16)|str_->length(), error_کوڈ);
    str_->append(static_cast<حرف>(trieByte), error_کوڈ);
    إذا(isFinal) {
        pos_=خالی_سوچک;
        قدر=value;
        أرجع خالی_سوچک;
    } وإلا {
        أرجع pos+value;
    }
}

U_NAMESPACE_سرا
