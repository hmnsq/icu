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

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t max_String_لمائی,
                               UError_کوڈ &error_کوڈ)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          بچیا_میل_لمائی(-1), initial_بچیا_میل_لمائی(-1),
          skip_قدر_2(خطأ),
          max_لمائی(max_String_لمائی), قدر(0), ڈھیر(خالی_سوچک) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    ڈھیر=جديد UVector32(error_کوڈ);
    إذا(ڈھیر==خالی_سوچک) {
        error_کوڈ=U_حافظہ_ونڈ_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(ثابت UCharsTrie &trie, int32_t max_String_لمائی,
                               UError_کوڈ &error_کوڈ)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          بچیا_میل_لمائی(trie.بچیا_میل_لمائی),
          initial_بچیا_میل_لمائی(trie.بچیا_میل_لمائی),
          skip_قدر_2(خطأ),
          max_لمائی(max_String_لمائی), قدر(0), ڈھیر(خالی_سوچک) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    ڈھیر=جديد UVector32(error_کوڈ);
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    إذا(ڈھیر==خالی_سوچک) {
        error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    int32_t length=بچیا_میل_لمائی;  // Actual remaining match length minus 1.
    إذا(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        إذا(max_لمائی>0 && length>max_لمائی) {
            length=max_لمائی;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        بچیا_میل_لمائی-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    مٹاؤ ڈھیر;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    بچیا_میل_لمائی=initial_بچیا_میل_لمائی;
    skip_قدر_2=خطأ;
    int32_t length=بچیا_میل_لمائی+1;  // Remaining match length.
    إذا(max_لمائی>0 && length>max_لمائی) {
        length=max_لمائی;
    }
    str_.truncate(length);
    pos_+=length;
    بچیا_میل_لمائی-=length;
    ڈھیر->سیٹ_ناپ(0);
    أرجع *هذا;
}

UBool
UCharsTrie::Iterator::has_اگلا() ثابت { أرجع pos_!=خالی_سوچک || !ڈھیر->is_خالی_تھاواں(); }

UBool
UCharsTrie::Iterator::next(UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع خطأ;
    }
    ثابت char16_t *pos=pos_;
    إذا(pos==خالی_سوچک) {
        إذا(ڈھیر->is_خالی_تھاواں()) {
            أرجع خطأ;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t ڈھیر_ناپ=ڈھیر->size();
        int32_t length=ڈھیر->elementAti(ڈھیر_ناپ-1);
        pos=uchars_+ڈھیر->elementAti(ڈھیر_ناپ-2);
        ڈھیر->سیٹ_ناپ(ڈھیر_ناپ-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        إذا(length>1) {
            pos=branch_اگلا(pos, length, error_کوڈ);
            إذا(pos==خالی_سوچک) {
                أرجع سچ;  // Reached a final value.
            }
        } وإلا {
            str_.append(*pos++);
        }
    }
    إذا(بچیا_میل_لمائی>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        أرجع truncateAndStop();
    }
    لكل(;;) {
        int32_t گرہ=*pos++;
        إذا(گرہ>=k_Min_قدر_Lead) {
            إذا(skip_قدر_2) {
                pos=skip_گرہ_قدر(pos, گرہ);
                گرہ&=k_گرہ_Type_نقاب;
                skip_قدر_2=خطأ;
            } وإلا {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(گرہ >> 15);
                إذا(isFinal) {
                    قدر=read_قدر(pos, گرہ&0x7fff);
                } وإلا {
                    قدر=read_گرہ_قدر(pos, گرہ);
                }
                إذا(isFinal || (max_لمائی>0 && str_.length()==max_لمائی)) {
                    pos_=خالی_سوچک;
                } وإلا {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skip_قدر_2=سچ;
                }
                أرجع سچ;
            }
        }
        إذا(max_لمائی>0 && str_.length()==max_لمائی) {
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
            // Linear-match node, append length units to str_.
            int32_t length=گرہ-k_Min_خطی_میل+1;
            إذا(max_لمائی>0 && str_.length()+length>max_لمائی) {
                str_.append(pos, max_لمائی-str_.length());
                أرجع truncateAndStop();
            }
            str_.append(pos, length);
            pos+=length;
        }
    }
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ثابت char16_t*
UCharsTrie::Iterator::branch_اگلا(ثابت char16_t* pos U_LIFETIME_BOUND,
                                 int32_t length,
                                 UError_کوڈ& error_کوڈ) {
    طالما(length>k_Max_Branch_خطی_Sub_گرہ_لمائی) {
        ++pos;  // ignore the comparison unit
        // Push state for the greater-or-equal edge.
        ڈھیر->جوڑو_Element(static_cast<int32_t>(skipDelta(pos) - uchars_), error_کوڈ);
        ڈھیر->جوڑو_Element(((length-(length>>1))<<16)|str_.length(), error_کوڈ);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    char16_t trieUnit=*pos++;
    int32_t گرہ=*pos++;
    UBool isFinal = static_cast<UBool>(گرہ >> 15);
    int32_t value=read_قدر(pos, گرہ&=0x7fff);
    pos=skip_قدر(pos, گرہ);
    ڈھیر->جوڑو_Element(static_cast<int32_t>(pos - uchars_), error_کوڈ);
    ڈھیر->جوڑو_Element(((length-1)<<16)|str_.length(), error_کوڈ);
    str_.append(trieUnit);
    إذا(isFinal) {
        pos_=خالی_سوچک;
        قدر=value;
        أرجع خالی_سوچک;
    } وإلا {
        أرجع pos+value;
    }
}

U_NAMESPACE_سرا
