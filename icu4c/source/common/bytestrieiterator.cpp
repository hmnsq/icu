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

بایتها_Trie::Iterator::Iterator(ثابت فراغ* trie_بایتها U_LIFETIME_BOUND,
                              int32_t max_String_درازی,
                              UError_کد& error_کد)
        : بایتها_2(static_cast<ثابت uint8_t *>(trie_بایتها)),
          pos_(بایتها_2), initialPos_(بایتها_2),
          مانده_تطابق_درازی(-1), initial_مانده_تطابق_درازی(-1),
          str_(اشاره_تهی), max_درازی(max_String_درازی), مقدار(0), پشته(اشاره_تهی) {
    إذا(U_FAILURE(error_کد)) {
        أرجع;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=جديد CharString();
    پشته=جديد UVector32(error_کد);
    إذا(U_SUCCESS(error_کد) && (str_==اشاره_تهی || پشته==اشاره_تهی)) {
        error_کد=U_حافظه_تخصیص_ERROR;
    }
}

بایتها_Trie::Iterator::Iterator(ثابت بایتها_Trie &trie, int32_t max_String_درازی,
                              UError_کد &error_کد)
        : بایتها_2(trie.بایتها_2), pos_(trie.pos_), initialPos_(trie.pos_),
          مانده_تطابق_درازی(trie.مانده_تطابق_درازی),
          initial_مانده_تطابق_درازی(trie.مانده_تطابق_درازی),
          str_(اشاره_تهی), max_درازی(max_String_درازی), مقدار(0), پشته(اشاره_تهی) {
    إذا(U_FAILURE(error_کد)) {
        أرجع;
    }
    str_=جديد CharString();
    پشته=جديد UVector32(error_کد);
    إذا(U_FAILURE(error_کد)) {
        أرجع;
    }
    إذا(str_==اشاره_تهی || پشته==اشاره_تهی) {
        error_کد=U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    int32_t length=مانده_تطابق_درازی;  // Actual remaining match length minus 1.
    إذا(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        إذا(max_درازی>0 && length>max_درازی) {
            length=max_درازی;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<ثابت حرف *>(pos_), length, error_کد);
        pos_+=length;
        مانده_تطابق_درازی-=length;
    }
}

بایتها_Trie::Iterator::~Iterator() {
    حذف str_;
    حذف پشته;
}

بایتها_Trie::Iterator&
بایتها_Trie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    مانده_تطابق_درازی=initial_مانده_تطابق_درازی;
    int32_t length=مانده_تطابق_درازی+1;  // Remaining match length.
    إذا(max_درازی>0 && length>max_درازی) {
        length=max_درازی;
    }
    str_->truncate(length);
    pos_+=length;
    مانده_تطابق_درازی-=length;
    پشته->مجموعه_اندازه(0);
    أرجع *هذا;
}

UBool
بایتها_Trie::Iterator::has_بعدی() ثابت { أرجع pos_!=اشاره_تهی || !پشته->is_خالی(); }

UBool
بایتها_Trie::Iterator::next(UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع خطأ;
    }
    ثابت uint8_t *pos=pos_;
    إذا(pos==اشاره_تهی) {
        إذا(پشته->is_خالی()) {
            أرجع خطأ;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t پشته_اندازه=پشته->size();
        int32_t length=پشته->elementAti(پشته_اندازه-1);
        pos=بایتها_2+پشته->elementAti(پشته_اندازه-2);
        پشته->مجموعه_اندازه(پشته_اندازه-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        إذا(length>1) {
            pos=branch_بعدی(pos, length, error_کد);
            إذا(pos==اشاره_تهی) {
                أرجع درست;  // Reached a final value.
            }
        } وإلا {
            str_->append(static_cast<حرف>(*pos++), error_کد);
        }
    }
    إذا(مانده_تطابق_درازی>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        أرجع truncateAndStop();
    }
    لكل(;;) {
        int32_t گره=*pos++;
        إذا(گره>=k_Min_مقدار_Lead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(گره & k_مقدار_Is_Final);
            مقدار=read_مقدار(pos, گره>>1);
            إذا(isFinal || (max_درازی>0 && str_->length()==max_درازی)) {
                pos_=اشاره_تهی;
            } وإلا {
                pos_=skip_مقدار(pos, گره);
            }
            أرجع درست;
        }
        إذا(max_درازی>0 && str_->length()==max_درازی) {
            أرجع truncateAndStop();
        }
        إذا(گره<k_Min_خطی_تطابق) {
            إذا(گره==0) {
                گره=*pos++;
            }
            pos=branch_بعدی(pos, گره+1, error_کد);
            إذا(pos==اشاره_تهی) {
                أرجع درست;  // Reached a final value.
            }
        } وإلا {
            // Linear-match node, append length bytes to str_.
            int32_t length=گره-k_Min_خطی_تطابق+1;
            إذا(max_درازی>0 && str_->length()+length>max_درازی) {
                str_->append(reinterpret_cast<ثابت حرف *>(pos),
                            max_درازی-str_->length(), error_کد);
                أرجع truncateAndStop();
            }
            str_->append(reinterpret_cast<ثابت حرف *>(pos), length, error_کد);
            pos+=length;
        }
    }
}

StringPiece
بایتها_Trie::Iterator::getString() ثابت U_LIFETIME_BOUND {
    أرجع str_ == اشاره_تهی ? StringPiece() : str_->مقصد_String_Piece();
}

UBool
بایتها_Trie::Iterator::truncateAndStop() {
    pos_=اشاره_تهی;
    مقدار=-1;  // no real value for str
    أرجع درست;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ثابت uint8_t*
بایتها_Trie::Iterator::branch_بعدی(ثابت uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UError_کد& error_کد) {
    طالما(length>k_Max_Branch_خطی_Sub_گره_درازی) {
        ++pos;  // ignore the comparison byte
        // Push state for the greater-or-equal edge.
        پشته->جمع_کن_Element(static_cast<int32_t>(skipDelta(pos) - بایتها_2), error_کد);
        پشته->جمع_کن_Element(((length-(length>>1))<<16)|str_->length(), error_کد);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    uint8_t trieByte=*pos++;
    int32_t گره=*pos++;
    UBool isFinal = static_cast<UBool>(گره & k_مقدار_Is_Final);
    int32_t value=read_مقدار(pos, گره>>1);
    pos=skip_مقدار(pos, گره);
    پشته->جمع_کن_Element(static_cast<int32_t>(pos - بایتها_2), error_کد);
    پشته->جمع_کن_Element(((length-1)<<16)|str_->length(), error_کد);
    str_->append(static_cast<حرف>(trieByte), error_کد);
    إذا(isFinal) {
        pos_=اشاره_تهی;
        مقدار=value;
        أرجع اشاره_تهی;
    } وإلا {
        أرجع pos+value;
    }
}

U_NAMESPACE_انتها
