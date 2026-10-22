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

#شامل "unicode/utypes.h"
#شامل "unicode/ucharstrie.h"
#شامل "unicode/unistr.h"
#شامل "uvectr32.h"

U_NAMESPACE_BEGIN

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t max_String_لمبائی,
                               UError_رمز &error_رمز)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          بقایا_مطابقت_لمبائی(-1), initial_بقایا_مطابقت_لمبائی(-1),
          skip_قدر_2(جھوٹ),
          max_لمبائی(max_String_لمبائی), قدر(0), ڈھیر(خالی_اشاریہ) {
    اگر(U_FAILURE(error_رمز)) {
        واپس;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    ڈھیر=نیا UVector32(error_رمز);
    اگر(ڈھیر==خالی_اشاریہ) {
        error_رمز=U_حافظہ_تخصیص_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(ثابت UCharsTrie &trie, int32_t max_String_لمبائی,
                               UError_رمز &error_رمز)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          بقایا_مطابقت_لمبائی(trie.بقایا_مطابقت_لمبائی),
          initial_بقایا_مطابقت_لمبائی(trie.بقایا_مطابقت_لمبائی),
          skip_قدر_2(جھوٹ),
          max_لمبائی(max_String_لمبائی), قدر(0), ڈھیر(خالی_اشاریہ) {
    اگر(U_FAILURE(error_رمز)) {
        واپس;
    }
    ڈھیر=نیا UVector32(error_رمز);
    اگر(U_FAILURE(error_رمز)) {
        واپس;
    }
    اگر(ڈھیر==خالی_اشاریہ) {
        error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    int32_t length=بقایا_مطابقت_لمبائی;  // Actual remaining match length minus 1.
    اگر(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        اگر(max_لمبائی>0 && length>max_لمبائی) {
            length=max_لمبائی;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        بقایا_مطابقت_لمبائی-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    حذف ڈھیر;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    بقایا_مطابقت_لمبائی=initial_بقایا_مطابقت_لمبائی;
    skip_قدر_2=جھوٹ;
    int32_t length=بقایا_مطابقت_لمبائی+1;  // Remaining match length.
    اگر(max_لمبائی>0 && length>max_لمبائی) {
        length=max_لمبائی;
    }
    str_.truncate(length);
    pos_+=length;
    بقایا_مطابقت_لمبائی-=length;
    ڈھیر->سیٹ_سائز(0);
    واپس *یہ;
}

UBool
UCharsTrie::Iterator::has_اگلا() ثابت { واپس pos_!=خالی_اشاریہ || !ڈھیر->is_خالی_خانے(); }

UBool
UCharsTrie::Iterator::next(UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس جھوٹ;
    }
    ثابت char16_t *pos=pos_;
    اگر(pos==خالی_اشاریہ) {
        اگر(ڈھیر->is_خالی_خانے()) {
            واپس جھوٹ;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t ڈھیر_سائز=ڈھیر->size();
        int32_t length=ڈھیر->elementAti(ڈھیر_سائز-1);
        pos=uchars_+ڈھیر->elementAti(ڈھیر_سائز-2);
        ڈھیر->سیٹ_سائز(ڈھیر_سائز-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        اگر(length>1) {
            pos=branch_اگلا(pos, length, error_رمز);
            اگر(pos==خالی_اشاریہ) {
                واپس سچ;  // Reached a final value.
            }
        } ورنہ {
            str_.append(*pos++);
        }
    }
    اگر(بقایا_مطابقت_لمبائی>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        واپس truncateAndStop();
    }
    برائے(;;) {
        int32_t گرہ=*pos++;
        اگر(گرہ>=k_Min_قدر_Lead) {
            اگر(skip_قدر_2) {
                pos=skip_گرہ_قدر(pos, گرہ);
                گرہ&=k_گرہ_Type_نقاب;
                skip_قدر_2=جھوٹ;
            } ورنہ {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(گرہ >> 15);
                اگر(isFinal) {
                    قدر=read_قدر(pos, گرہ&0x7fff);
                } ورنہ {
                    قدر=read_گرہ_قدر(pos, گرہ);
                }
                اگر(isFinal || (max_لمبائی>0 && str_.length()==max_لمبائی)) {
                    pos_=خالی_اشاریہ;
                } ورنہ {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skip_قدر_2=سچ;
                }
                واپس سچ;
            }
        }
        اگر(max_لمبائی>0 && str_.length()==max_لمبائی) {
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
            // Linear-match node, append length units to str_.
            int32_t length=گرہ-k_Min_خطی_مطابقت+1;
            اگر(max_لمبائی>0 && str_.length()+length>max_لمبائی) {
                str_.append(pos, max_لمبائی-str_.length());
                واپس truncateAndStop();
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
                                 UError_رمز& error_رمز) {
    جب(length>k_Max_Branch_خطی_Sub_گرہ_لمبائی) {
        ++pos;  // ignore the comparison unit
        // Push state for the greater-or-equal edge.
        ڈھیر->جوڑو_Element(static_cast<int32_t>(skipDelta(pos) - uchars_), error_رمز);
        ڈھیر->جوڑو_Element(((length-(length>>1))<<16)|str_.length(), error_رمز);
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
    ڈھیر->جوڑو_Element(static_cast<int32_t>(pos - uchars_), error_رمز);
    ڈھیر->جوڑو_Element(((length-1)<<16)|str_.length(), error_رمز);
    str_.append(trieUnit);
    اگر(isFinal) {
        pos_=خالی_اشاریہ;
        قدر=value;
        واپس خالی_اشاریہ;
    } ورنہ {
        واپس pos+value;
    }
}

U_NAMESPACE_انتہا
