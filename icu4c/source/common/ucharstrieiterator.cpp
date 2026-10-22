// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  ucharstrieiterator.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010nov15
*   created by: Markus W. Scherer
*/

#ضمن "unicode/utypes.h"
#ضمن "unicode/ucharstrie.h"
#ضمن "unicode/unistr.h"
#ضمن "uvectr32.h"

U_NAMESPACE_BEGIN

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t max_String_درازی,
                               UError_کد &error_کد)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          مانده_تطابق_درازی(-1), initial_مانده_تطابق_درازی(-1),
          skip_مقدار_2(خطأ),
          max_درازی(max_String_درازی), مقدار(0), پشته(اشاره_تهی) {
    إذا(U_FAILURE(error_کد)) {
        أرجع;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    پشته=جديد UVector32(error_کد);
    إذا(پشته==اشاره_تهی) {
        error_کد=U_حافظه_تخصیص_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(ثابت UCharsTrie &trie, int32_t max_String_درازی,
                               UError_کد &error_کد)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          مانده_تطابق_درازی(trie.مانده_تطابق_درازی),
          initial_مانده_تطابق_درازی(trie.مانده_تطابق_درازی),
          skip_مقدار_2(خطأ),
          max_درازی(max_String_درازی), مقدار(0), پشته(اشاره_تهی) {
    إذا(U_FAILURE(error_کد)) {
        أرجع;
    }
    پشته=جديد UVector32(error_کد);
    إذا(U_FAILURE(error_کد)) {
        أرجع;
    }
    إذا(پشته==اشاره_تهی) {
        error_کد=U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    int32_t length=مانده_تطابق_درازی;  // Actual remaining match length minus 1.
    إذا(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        إذا(max_درازی>0 && length>max_درازی) {
            length=max_درازی;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        مانده_تطابق_درازی-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    حذف پشته;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    مانده_تطابق_درازی=initial_مانده_تطابق_درازی;
    skip_مقدار_2=خطأ;
    int32_t length=مانده_تطابق_درازی+1;  // Remaining match length.
    إذا(max_درازی>0 && length>max_درازی) {
        length=max_درازی;
    }
    str_.truncate(length);
    pos_+=length;
    مانده_تطابق_درازی-=length;
    پشته->مجموعه_اندازه(0);
    أرجع *هذا;
}

UBool
UCharsTrie::Iterator::has_بعدی() ثابت { أرجع pos_!=اشاره_تهی || !پشته->is_خالی(); }

UBool
UCharsTrie::Iterator::next(UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع خطأ;
    }
    ثابت char16_t *pos=pos_;
    إذا(pos==اشاره_تهی) {
        إذا(پشته->is_خالی()) {
            أرجع خطأ;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t پشته_اندازه=پشته->size();
        int32_t length=پشته->elementAti(پشته_اندازه-1);
        pos=uchars_+پشته->elementAti(پشته_اندازه-2);
        پشته->مجموعه_اندازه(پشته_اندازه-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        إذا(length>1) {
            pos=branch_بعدی(pos, length, error_کد);
            إذا(pos==اشاره_تهی) {
                أرجع درست;  // Reached a final value.
            }
        } وإلا {
            str_.append(*pos++);
        }
    }
    إذا(مانده_تطابق_درازی>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        أرجع truncateAndStop();
    }
    لكل(;;) {
        int32_t گره=*pos++;
        إذا(گره>=k_Min_مقدار_Lead) {
            إذا(skip_مقدار_2) {
                pos=skip_گره_مقدار(pos, گره);
                گره&=k_گره_Type_ماسک;
                skip_مقدار_2=خطأ;
            } وإلا {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(گره >> 15);
                إذا(isFinal) {
                    مقدار=read_مقدار(pos, گره&0x7fff);
                } وإلا {
                    مقدار=read_گره_مقدار(pos, گره);
                }
                إذا(isFinal || (max_درازی>0 && str_.length()==max_درازی)) {
                    pos_=اشاره_تهی;
                } وإلا {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skip_مقدار_2=درست;
                }
                أرجع درست;
            }
        }
        إذا(max_درازی>0 && str_.length()==max_درازی) {
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
            // Linear-match node, append length units to str_.
            int32_t length=گره-k_Min_خطی_تطابق+1;
            إذا(max_درازی>0 && str_.length()+length>max_درازی) {
                str_.append(pos, max_درازی-str_.length());
                أرجع truncateAndStop();
            }
            str_.append(pos, length);
            pos+=length;
        }
    }
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ثابت char16_t*
UCharsTrie::Iterator::branch_بعدی(ثابت char16_t* pos U_LIFETIME_BOUND,
                                 int32_t length,
                                 UError_کد& error_کد) {
    طالما(length>k_Max_Branch_خطی_Sub_گره_درازی) {
        ++pos;  // ignore the comparison unit
        // Push state for the greater-or-equal edge.
        پشته->جمع_کن_Element(static_cast<int32_t>(skipDelta(pos) - uchars_), error_کد);
        پشته->جمع_کن_Element(((length-(length>>1))<<16)|str_.length(), error_کد);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    char16_t trieUnit=*pos++;
    int32_t گره=*pos++;
    UBool isFinal = static_cast<UBool>(گره >> 15);
    int32_t value=read_مقدار(pos, گره&=0x7fff);
    pos=skip_مقدار(pos, گره);
    پشته->جمع_کن_Element(static_cast<int32_t>(pos - uchars_), error_کد);
    پشته->جمع_کن_Element(((length-1)<<16)|str_.length(), error_کد);
    str_.append(trieUnit);
    إذا(isFinal) {
        pos_=اشاره_تهی;
        مقدار=value;
        أرجع اشاره_تهی;
    } وإلا {
        أرجع pos+value;
    }
}

U_NAMESPACE_انتها
