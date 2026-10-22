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

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t max_String_ڊيگهائي,
                               UError_ڪوڊ &error_ڪوڊ)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          بچيل_ميلاپ_ڊيگهائي(-1), initial_بچيل_ميلاپ_ڊيگهائي(-1),
          skip_قدر_2(خطأ),
          max_ڊيگهائي(max_String_ڊيگهائي), قدر(0), ڍير(خالي_اشارو) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    ڍير=جديد UVector32(error_ڪوڊ);
    إذا(ڍير==خالي_اشارو) {
        error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(ثابت UCharsTrie &trie, int32_t max_String_ڊيگهائي,
                               UError_ڪوڊ &error_ڪوڊ)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          بچيل_ميلاپ_ڊيگهائي(trie.بچيل_ميلاپ_ڊيگهائي),
          initial_بچيل_ميلاپ_ڊيگهائي(trie.بچيل_ميلاپ_ڊيگهائي),
          skip_قدر_2(خطأ),
          max_ڊيگهائي(max_String_ڊيگهائي), قدر(0), ڍير(خالي_اشارو) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    ڍير=جديد UVector32(error_ڪوڊ);
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    إذا(ڍير==خالي_اشارو) {
        error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    int32_t length=بچيل_ميلاپ_ڊيگهائي;  // Actual remaining match length minus 1.
    إذا(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        إذا(max_ڊيگهائي>0 && length>max_ڊيگهائي) {
            length=max_ڊيگهائي;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        بچيل_ميلاپ_ڊيگهائي-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    ميٽيو ڍير;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    بچيل_ميلاپ_ڊيگهائي=initial_بچيل_ميلاپ_ڊيگهائي;
    skip_قدر_2=خطأ;
    int32_t length=بچيل_ميلاپ_ڊيگهائي+1;  // Remaining match length.
    إذا(max_ڊيگهائي>0 && length>max_ڊيگهائي) {
        length=max_ڊيگهائي;
    }
    str_.truncate(length);
    pos_+=length;
    بچيل_ميلاپ_ڊيگهائي-=length;
    ڍير->سيٽ_سائيز(0);
    أرجع *هذا;
}

UBool
UCharsTrie::Iterator::has_ايندڙ() ثابت { أرجع pos_!=خالي_اشارو || !ڍير->is_خالي_جايون(); }

UBool
UCharsTrie::Iterator::next(UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خطأ;
    }
    ثابت char16_t *pos=pos_;
    إذا(pos==خالي_اشارو) {
        إذا(ڍير->is_خالي_جايون()) {
            أرجع خطأ;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t ڍير_سائيز=ڍير->size();
        int32_t length=ڍير->elementAti(ڍير_سائيز-1);
        pos=uchars_+ڍير->elementAti(ڍير_سائيز-2);
        ڍير->سيٽ_سائيز(ڍير_سائيز-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        إذا(length>1) {
            pos=branch_ايندڙ(pos, length, error_ڪوڊ);
            إذا(pos==خالي_اشارو) {
                أرجع سچ;  // Reached a final value.
            }
        } وإلا {
            str_.append(*pos++);
        }
    }
    إذا(بچيل_ميلاپ_ڊيگهائي>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        أرجع truncateAndStop();
    }
    لكل(;;) {
        int32_t ڳنڍ=*pos++;
        إذا(ڳنڍ>=k_Min_قدر_Lead) {
            إذا(skip_قدر_2) {
                pos=skip_ڳنڍ_قدر(pos, ڳنڍ);
                ڳنڍ&=k_ڳنڍ_Type_نقاب;
                skip_قدر_2=خطأ;
            } وإلا {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(ڳنڍ >> 15);
                إذا(isFinal) {
                    قدر=read_قدر(pos, ڳنڍ&0x7fff);
                } وإلا {
                    قدر=read_ڳنڍ_قدر(pos, ڳنڍ);
                }
                إذا(isFinal || (max_ڊيگهائي>0 && str_.length()==max_ڊيگهائي)) {
                    pos_=خالي_اشارو;
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
        إذا(max_ڊيگهائي>0 && str_.length()==max_ڊيگهائي) {
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
            // Linear-match node, append length units to str_.
            int32_t length=ڳنڍ-k_Min_لڪيري_ميلاپ+1;
            إذا(max_ڊيگهائي>0 && str_.length()+length>max_ڊيگهائي) {
                str_.append(pos, max_ڊيگهائي-str_.length());
                أرجع truncateAndStop();
            }
            str_.append(pos, length);
            pos+=length;
        }
    }
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ثابت char16_t*
UCharsTrie::Iterator::branch_ايندڙ(ثابت char16_t* pos U_LIFETIME_BOUND,
                                 int32_t length,
                                 UError_ڪوڊ& error_ڪوڊ) {
    طالما(length>k_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي) {
        ++pos;  // ignore the comparison unit
        // Push state for the greater-or-equal edge.
        ڍير->جوڙيو_Element(static_cast<int32_t>(skipDelta(pos) - uchars_), error_ڪوڊ);
        ڍير->جوڙيو_Element(((length-(length>>1))<<16)|str_.length(), error_ڪوڊ);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    char16_t trieUnit=*pos++;
    int32_t ڳنڍ=*pos++;
    UBool isFinal = static_cast<UBool>(ڳنڍ >> 15);
    int32_t value=read_قدر(pos, ڳنڍ&=0x7fff);
    pos=skip_قدر(pos, ڳنڍ);
    ڍير->جوڙيو_Element(static_cast<int32_t>(pos - uchars_), error_ڪوڊ);
    ڍير->جوڙيو_Element(((length-1)<<16)|str_.length(), error_ڪوڊ);
    str_.append(trieUnit);
    إذا(isFinal) {
        pos_=خالي_اشارو;
        قدر=value;
        أرجع خالي_اشارو;
    } وإلا {
        أرجع pos+value;
    }
}

U_NAMESPACE_پڇاڙي
