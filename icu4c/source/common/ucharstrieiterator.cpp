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

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t max_String_اوږدتیا,
                               UError_کوډ &error_کوډ)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          پاتې_شوی_سمون_اوږدتیا(-1), initial_پاتې_شوی_سمون_اوږدتیا(-1),
          skip_ارزښت_2(خطأ),
          max_اوږدتیا(max_String_اوږدتیا), ارزښت(0), ډېری(تش_اشاره) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    ډېری=جديد UVector32(error_کوډ);
    إذا(ډېری==تش_اشاره) {
        error_کوډ=U_حافظه_ویش_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(ثابت UCharsTrie &trie, int32_t max_String_اوږدتیا,
                               UError_کوډ &error_کوډ)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          پاتې_شوی_سمون_اوږدتیا(trie.پاتې_شوی_سمون_اوږدتیا),
          initial_پاتې_شوی_سمون_اوږدتیا(trie.پاتې_شوی_سمون_اوږدتیا),
          skip_ارزښت_2(خطأ),
          max_اوږدتیا(max_String_اوږدتیا), ارزښت(0), ډېری(تش_اشاره) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع;
    }
    ډېری=جديد UVector32(error_کوډ);
    إذا(U_FAILURE(error_کوډ)) {
        أرجع;
    }
    إذا(ډېری==تش_اشاره) {
        error_کوډ=U_حافظه_ویش_ERROR;
        أرجع;
    }
    int32_t length=پاتې_شوی_سمون_اوږدتیا;  // Actual remaining match length minus 1.
    إذا(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        إذا(max_اوږدتیا>0 && length>max_اوږدتیا) {
            length=max_اوږدتیا;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        پاتې_شوی_سمون_اوږدتیا-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    ړنګول ډېری;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    پاتې_شوی_سمون_اوږدتیا=initial_پاتې_شوی_سمون_اوږدتیا;
    skip_ارزښت_2=خطأ;
    int32_t length=پاتې_شوی_سمون_اوږدتیا+1;  // Remaining match length.
    إذا(max_اوږدتیا>0 && length>max_اوږدتیا) {
        length=max_اوږدتیا;
    }
    str_.truncate(length);
    pos_+=length;
    پاتې_شوی_سمون_اوږدتیا-=length;
    ډېری->سېټ_اندازه(0);
    أرجع *هذا;
}

UBool
UCharsTrie::Iterator::has_بل() ثابت { أرجع pos_!=تش_اشاره || !ډېری->is_تشه(); }

UBool
UCharsTrie::Iterator::next(UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع خطأ;
    }
    ثابت char16_t *pos=pos_;
    إذا(pos==تش_اشاره) {
        إذا(ډېری->is_تشه()) {
            أرجع خطأ;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t ډېری_اندازه=ډېری->size();
        int32_t length=ډېری->elementAti(ډېری_اندازه-1);
        pos=uchars_+ډېری->elementAti(ډېری_اندازه-2);
        ډېری->سېټ_اندازه(ډېری_اندازه-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        إذا(length>1) {
            pos=branch_بل(pos, length, error_کوډ);
            إذا(pos==تش_اشاره) {
                أرجع ریښتیا;  // Reached a final value.
            }
        } وإلا {
            str_.append(*pos++);
        }
    }
    إذا(پاتې_شوی_سمون_اوږدتیا>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        أرجع truncateAndStop();
    }
    لكل(;;) {
        int32_t غوټه=*pos++;
        إذا(غوټه>=k_Min_ارزښت_Lead) {
            إذا(skip_ارزښت_2) {
                pos=skip_غوټه_ارزښت(pos, غوټه);
                غوټه&=k_غوټه_Type_ماسک;
                skip_ارزښت_2=خطأ;
            } وإلا {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(غوټه >> 15);
                إذا(isFinal) {
                    ارزښت=read_ارزښت(pos, غوټه&0x7fff);
                } وإلا {
                    ارزښت=read_غوټه_ارزښت(pos, غوټه);
                }
                إذا(isFinal || (max_اوږدتیا>0 && str_.length()==max_اوږدتیا)) {
                    pos_=تش_اشاره;
                } وإلا {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skip_ارزښت_2=ریښتیا;
                }
                أرجع ریښتیا;
            }
        }
        إذا(max_اوږدتیا>0 && str_.length()==max_اوږدتیا) {
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
            // Linear-match node, append length units to str_.
            int32_t length=غوټه-k_Min_خطي_سمون+1;
            إذا(max_اوږدتیا>0 && str_.length()+length>max_اوږدتیا) {
                str_.append(pos, max_اوږدتیا-str_.length());
                أرجع truncateAndStop();
            }
            str_.append(pos, length);
            pos+=length;
        }
    }
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ثابت char16_t*
UCharsTrie::Iterator::branch_بل(ثابت char16_t* pos U_LIFETIME_BOUND,
                                 int32_t length,
                                 UError_کوډ& error_کوډ) {
    طالما(length>k_Max_Branch_خطي_Sub_غوټه_اوږدتیا) {
        ++pos;  // ignore the comparison unit
        // Push state for the greater-or-equal edge.
        ډېری->جمع_کړه_Element(static_cast<int32_t>(skipDelta(pos) - uchars_), error_کوډ);
        ډېری->جمع_کړه_Element(((length-(length>>1))<<16)|str_.length(), error_کوډ);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    char16_t trieUnit=*pos++;
    int32_t غوټه=*pos++;
    UBool isFinal = static_cast<UBool>(غوټه >> 15);
    int32_t value=read_ارزښت(pos, غوټه&=0x7fff);
    pos=skip_ارزښت(pos, غوټه);
    ډېری->جمع_کړه_Element(static_cast<int32_t>(pos - uchars_), error_کوډ);
    ډېری->جمع_کړه_Element(((length-1)<<16)|str_.length(), error_کوډ);
    str_.append(trieUnit);
    إذا(isFinal) {
        pos_=تش_اشاره;
        ارزښت=value;
        أرجع تش_اشاره;
    } وإلا {
        أرجع pos+value;
    }
}

U_NAMESPACE_پای_ټکی
