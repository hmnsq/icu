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

#شامل "unicode/utypes.h"
#شامل "unicode/bytestrie.h"
#شامل "unicode/stringpiece.h"
#شامل "charstr.h"
#شامل "uvectr32.h"

U_NAMESPACE_BEGIN

بائٹیں_Trie::Iterator::Iterator(ثابت خالی* trie_بائٹیں U_LIFETIME_BOUND,
                              int32_t max_String_لمبائی,
                              UError_رمز& error_رمز)
        : بائٹیں_2(static_cast<ثابت uint8_t *>(trie_بائٹیں)),
          pos_(بائٹیں_2), initialPos_(بائٹیں_2),
          بقایا_مطابقت_لمبائی(-1), initial_بقایا_مطابقت_لمبائی(-1),
          str_(خالی_اشاریہ), max_لمبائی(max_String_لمبائی), قدر(0), ڈھیر(خالی_اشاریہ) {
    اگر(U_FAILURE(error_رمز)) {
        واپس;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=نیا CharString();
    ڈھیر=نیا UVector32(error_رمز);
    اگر(U_SUCCESS(error_رمز) && (str_==خالی_اشاریہ || ڈھیر==خالی_اشاریہ)) {
        error_رمز=U_حافظہ_تخصیص_ERROR;
    }
}

بائٹیں_Trie::Iterator::Iterator(ثابت بائٹیں_Trie &trie, int32_t max_String_لمبائی,
                              UError_رمز &error_رمز)
        : بائٹیں_2(trie.بائٹیں_2), pos_(trie.pos_), initialPos_(trie.pos_),
          بقایا_مطابقت_لمبائی(trie.بقایا_مطابقت_لمبائی),
          initial_بقایا_مطابقت_لمبائی(trie.بقایا_مطابقت_لمبائی),
          str_(خالی_اشاریہ), max_لمبائی(max_String_لمبائی), قدر(0), ڈھیر(خالی_اشاریہ) {
    اگر(U_FAILURE(error_رمز)) {
        واپس;
    }
    str_=نیا CharString();
    ڈھیر=نیا UVector32(error_رمز);
    اگر(U_FAILURE(error_رمز)) {
        واپس;
    }
    اگر(str_==خالی_اشاریہ || ڈھیر==خالی_اشاریہ) {
        error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    int32_t length=بقایا_مطابقت_لمبائی;  // Actual remaining match length minus 1.
    اگر(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        اگر(max_لمبائی>0 && length>max_لمبائی) {
            length=max_لمبائی;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<ثابت حرف *>(pos_), length, error_رمز);
        pos_+=length;
        بقایا_مطابقت_لمبائی-=length;
    }
}

بائٹیں_Trie::Iterator::~Iterator() {
    حذف str_;
    حذف ڈھیر;
}

بائٹیں_Trie::Iterator&
بائٹیں_Trie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    بقایا_مطابقت_لمبائی=initial_بقایا_مطابقت_لمبائی;
    int32_t length=بقایا_مطابقت_لمبائی+1;  // Remaining match length.
    اگر(max_لمبائی>0 && length>max_لمبائی) {
        length=max_لمبائی;
    }
    str_->truncate(length);
    pos_+=length;
    بقایا_مطابقت_لمبائی-=length;
    ڈھیر->سیٹ_سائز(0);
    واپس *یہ;
}

UBool
بائٹیں_Trie::Iterator::has_اگلا() ثابت { واپس pos_!=خالی_اشاریہ || !ڈھیر->is_خالی_خانے(); }

UBool
بائٹیں_Trie::Iterator::next(UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس جھوٹ;
    }
    ثابت uint8_t *pos=pos_;
    اگر(pos==خالی_اشاریہ) {
        اگر(ڈھیر->is_خالی_خانے()) {
            واپس جھوٹ;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t ڈھیر_سائز=ڈھیر->size();
        int32_t length=ڈھیر->elementAti(ڈھیر_سائز-1);
        pos=بائٹیں_2+ڈھیر->elementAti(ڈھیر_سائز-2);
        ڈھیر->سیٹ_سائز(ڈھیر_سائز-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        اگر(length>1) {
            pos=branch_اگلا(pos, length, error_رمز);
            اگر(pos==خالی_اشاریہ) {
                واپس سچ;  // Reached a final value.
            }
        } ورنہ {
            str_->append(static_cast<حرف>(*pos++), error_رمز);
        }
    }
    اگر(بقایا_مطابقت_لمبائی>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        واپس truncateAndStop();
    }
    برائے(;;) {
        int32_t گرہ=*pos++;
        اگر(گرہ>=k_Min_قدر_Lead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(گرہ & k_قدر_Is_Final);
            قدر=read_قدر(pos, گرہ>>1);
            اگر(isFinal || (max_لمبائی>0 && str_->length()==max_لمبائی)) {
                pos_=خالی_اشاریہ;
            } ورنہ {
                pos_=skip_قدر(pos, گرہ);
            }
            واپس سچ;
        }
        اگر(max_لمبائی>0 && str_->length()==max_لمبائی) {
            واپس truncateAndStop();
        }
        اگر(گرہ<k_Min_خطی_مطابقت) {
            اگر(گرہ==0) {
                گرہ=*pos++;
            }
            pos=branch_اگلا(pos, گرہ+1, error_رمز);
            اگر(pos==خالی_اشاریہ) {
                واپس سچ;  // Reached a final value.
            }
        } ورنہ {
            // Linear-match node, append length bytes to str_.
            int32_t length=گرہ-k_Min_خطی_مطابقت+1;
            اگر(max_لمبائی>0 && str_->length()+length>max_لمبائی) {
                str_->append(reinterpret_cast<ثابت حرف *>(pos),
                            max_لمبائی-str_->length(), error_رمز);
                واپس truncateAndStop();
            }
            str_->append(reinterpret_cast<ثابت حرف *>(pos), length, error_رمز);
            pos+=length;
        }
    }
}

StringPiece
بائٹیں_Trie::Iterator::getString() ثابت U_LIFETIME_BOUND {
    واپس str_ == خالی_اشاریہ ? StringPiece() : str_->مقصد_String_Piece();
}

UBool
بائٹیں_Trie::Iterator::truncateAndStop() {
    pos_=خالی_اشاریہ;
    قدر=-1;  // no real value for str
    واپس سچ;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ثابت uint8_t*
بائٹیں_Trie::Iterator::branch_اگلا(ثابت uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UError_رمز& error_رمز) {
    جب(length>k_Max_Branch_خطی_Sub_گرہ_لمبائی) {
        ++pos;  // ignore the comparison byte
        // Push state for the greater-or-equal edge.
        ڈھیر->جوڑو_Element(static_cast<int32_t>(skipDelta(pos) - بائٹیں_2), error_رمز);
        ڈھیر->جوڑو_Element(((length-(length>>1))<<16)|str_->length(), error_رمز);
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
    ڈھیر->جوڑو_Element(static_cast<int32_t>(pos - بائٹیں_2), error_رمز);
    ڈھیر->جوڑو_Element(((length-1)<<16)|str_->length(), error_رمز);
    str_->append(static_cast<حرف>(trieByte), error_رمز);
    اگر(isFinal) {
        pos_=خالی_اشاریہ;
        قدر=value;
        واپس خالی_اشاریہ;
    } ورنہ {
        واپس pos+value;
    }
}

U_NAMESPACE_انتہا
