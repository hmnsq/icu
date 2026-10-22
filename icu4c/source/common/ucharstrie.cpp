// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  ucharstrie.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010nov14
*   created by: Markus W. Scherer
*/

#ضمن "unicode/utypes.h"
#ضمن "unicode/appendable.h"
#ضمن "unicode/ucharstrie.h"
#ضمن "unicode/uobject.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"

U_NAMESPACE_BEGIN

UCharsTrie::~UCharsTrie() {
    uprv_free(ownedArray_);
}

UString_Trie_نتیجہ
UCharsTrie::current() ثابت {
    ثابت char16_t *pos=pos_;
    إذا(pos==خالی_سوچک) {
        أرجع USTRINGTRIE_NO_میل;
    } وإلا {
        int32_t گرہ;
        أرجع (بچیا_میل_لمائی<0 && (گرہ=*pos)>=k_Min_قدر_Lead) ?
                قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
    }
}

UString_Trie_نتیجہ
UCharsTrie::پہلا_For_کوڈ_نقطہ(UChar32 cp) {
    أرجع cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_اگلا(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_میل);
}

UString_Trie_نتیجہ
UCharsTrie::اگلا_For_کوڈ_نقطہ(UChar32 cp) {
    أرجع cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_اگلا(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_میل);
}

UString_Trie_نتیجہ
UCharsTrie::branch_اگلا(ثابت char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    إذا(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    طالما(length>k_Max_Branch_خطی_Sub_گرہ_لمائی) {
        إذا(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } وإلا {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    افعل {
        إذا(uchar==*pos++) {
            UString_Trie_نتیجہ نتیجہ;
            int32_t گرہ=*pos;
            إذا(گرہ&k_قدر_Is_Final) {
                // Leave the final value for getValue() to read.
                نتیجہ=USTRINGTRIE_FINAL_قدر;
            } وإلا {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                إذا(گرہ<k_Min_Two_Unit_قدر_Lead) {
                    delta=گرہ;
                } وإلا إذا(گرہ<k_Three_Unit_قدر_Lead) {
                    delta=((گرہ-k_Min_Two_Unit_قدر_Lead)<<16)|*pos++;
                } وإلا {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                گرہ=*pos;
                نتیجہ= گرہ>=k_Min_قدر_Lead ? قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
            }
            pos_=pos;
            أرجع نتیجہ;
        }
        --length;
        pos=skip_قدر(pos);
    } طالما(length>1);
    إذا(uchar==*pos++) {
        pos_=pos;
        int32_t گرہ=*pos;
        أرجع گرہ>=k_Min_قدر_Lead ? قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
    } وإلا {
        stop();
        أرجع USTRINGTRIE_NO_میل;
    }
}

UString_Trie_نتیجہ
UCharsTrie::اگلا_Impl(ثابت char16_t *pos, int32_t uchar) {
    int32_t گرہ=*pos++;
    لكل(;;) {
        إذا(گرہ<k_Min_خطی_میل) {
            أرجع branch_اگلا(pos, گرہ, uchar);
        } وإلا إذا(گرہ<k_Min_قدر_Lead) {
            // Match the first of length+1 units.
            int32_t length=گرہ-k_Min_خطی_میل;  // Actual match length minus 1.
            إذا(uchar==*pos++) {
                بچیا_میل_لمائی=--length;
                pos_=pos;
                أرجع (length<0 && (گرہ=*pos)>=k_Min_قدر_Lead) ?
                        قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
            } وإلا {
                // No match.
                اكسر;
            }
        } وإلا إذا(گرہ&k_قدر_Is_Final) {
            // No further matching units.
            اكسر;
        } وإلا {
            // Skip intermediate value.
            pos=skip_گرہ_قدر(pos, گرہ);
            گرہ&=k_گرہ_Type_نقاب;
        }
    }
    stop();
    أرجع USTRINGTRIE_NO_میل;
}

UString_Trie_نتیجہ
UCharsTrie::next(int32_t uchar) {
    ثابت char16_t *pos=pos_;
    إذا(pos==خالی_سوچک) {
        أرجع USTRINGTRIE_NO_میل;
    }
    int32_t length=بچیا_میل_لمائی;  // Actual remaining match length minus 1.
    إذا(length>=0) {
        // Remaining part of a linear-match node.
        إذا(uchar==*pos++) {
            بچیا_میل_لمائی=--length;
            pos_=pos;
            int32_t گرہ;
            أرجع (length<0 && (گرہ=*pos)>=k_Min_قدر_Lead) ?
                    قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
        } وإلا {
            stop();
            أرجع USTRINGTRIE_NO_میل;
        }
    }
    أرجع اگلا_Impl(pos, uchar);
}

UString_Trie_نتیجہ
UCharsTrie::next(ConstChar16Ptr ptr, int32_t s_لمائی) {
    ثابت char16_t *s=ptr;
    إذا(s_لمائی<0 ? *s==0 : s_لمائی==0) {
        // Empty input.
        أرجع current();
    }
    ثابت char16_t *pos=pos_;
    إذا(pos==خالی_سوچک) {
        أرجع USTRINGTRIE_NO_میل;
    }
    int32_t length=بچیا_میل_لمائی;  // Actual remaining match length minus 1.
    لكل(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        إذا(s_لمائی<0) {
            لكل(;;) {
                إذا((uchar=*s++)==0) {
                    بچیا_میل_لمائی=length;
                    pos_=pos;
                    int32_t گرہ;
                    أرجع (length<0 && (گرہ=*pos)>=k_Min_قدر_Lead) ?
                            قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
                }
                إذا(length<0) {
                    بچیا_میل_لمائی=length;
                    اكسر;
                }
                إذا(uchar!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_میل;
                }
                ++pos;
                --length;
            }
        } وإلا {
            لكل(;;) {
                إذا(s_لمائی==0) {
                    بچیا_میل_لمائی=length;
                    pos_=pos;
                    int32_t گرہ;
                    أرجع (length<0 && (گرہ=*pos)>=k_Min_قدر_Lead) ?
                            قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
                }
                uchar=*s++;
                --s_لمائی;
                إذا(length<0) {
                    بچیا_میل_لمائی=length;
                    اكسر;
                }
                إذا(uchar!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_میل;
                }
                ++pos;
                --length;
            }
        }
        int32_t گرہ=*pos++;
        لكل(;;) {
            إذا(گرہ<k_Min_خطی_میل) {
                UString_Trie_نتیجہ نتیجہ=branch_اگلا(pos, گرہ, uchar);
                إذا(نتیجہ==USTRINGTRIE_NO_میل) {
                    أرجع USTRINGTRIE_NO_میل;
                }
                // Fetch the next input unit, if there is one.
                إذا(s_لمائی<0) {
                    إذا((uchar=*s++)==0) {
                        أرجع نتیجہ;
                    }
                } وإلا {
                    إذا(s_لمائی==0) {
                        أرجع نتیجہ;
                    }
                    uchar=*s++;
                    --s_لمائی;
                }
                إذا(نتیجہ==USTRINGTRIE_FINAL_قدر) {
                    // No further matching units.
                    stop();
                    أرجع USTRINGTRIE_NO_میل;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                گرہ=*pos++;
            } وإلا إذا(گرہ<k_Min_قدر_Lead) {
                // Match length+1 units.
                length=گرہ-k_Min_خطی_میل;  // Actual match length minus 1.
                إذا(uchar!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_میل;
                }
                ++pos;
                --length;
                اكسر;
            } وإلا إذا(گرہ&k_قدر_Is_Final) {
                // No further matching units.
                stop();
                أرجع USTRINGTRIE_NO_میل;
            } وإلا {
                // Skip intermediate value.
                pos=skip_گرہ_قدر(pos, گرہ);
                گرہ&=k_گرہ_Type_نقاب;
            }
        }
    }
}

ثابت char16_t*
UCharsTrie::لبھو_Unique_قدر_From_Branch(ثابت char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_قدر,
                                      int32_t& unique_قدر) {
    طالما(length>k_Max_Branch_خطی_Sub_گرہ_لمائی) {
        ++pos;  // ignore the comparison unit
        إذا(خالی_سوچک==لبھو_Unique_قدر_From_Branch(jumpByDelta(pos), length>>1, have_Unique_قدر, unique_قدر)) {
            أرجع خالی_سوچک;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t گرہ=*pos++;
        UBool isFinal = static_cast<UBool>(گرہ >> 15);
        گرہ&=0x7fff;
        int32_t value=read_قدر(pos, گرہ);
        pos=skip_قدر(pos, گرہ);
        إذا(isFinal) {
            إذا(have_Unique_قدر) {
                إذا(value!=unique_قدر) {
                    أرجع خالی_سوچک;
                }
            } وإلا {
                unique_قدر=value;
                have_Unique_قدر=سچ;
            }
        } وإلا {
            إذا(!لبھو_Unique_قدر(pos+value, have_Unique_قدر, unique_قدر)) {
                أرجع خالی_سوچک;
            }
            have_Unique_قدر=سچ;
        }
    } طالما(--length>1);
    أرجع pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::لبھو_Unique_قدر(ثابت char16_t *pos, UBool have_Unique_قدر, int32_t &unique_قدر) {
    int32_t گرہ=*pos++;
    لكل(;;) {
        إذا(گرہ<k_Min_خطی_میل) {
            إذا(گرہ==0) {
                گرہ=*pos++;
            }
            pos=لبھو_Unique_قدر_From_Branch(pos, گرہ+1, have_Unique_قدر, unique_قدر);
            إذا(pos==خالی_سوچک) {
                أرجع خطأ;
            }
            have_Unique_قدر=سچ;
            گرہ=*pos++;
        } وإلا إذا(گرہ<k_Min_قدر_Lead) {
            // linear-match node
            pos+=گرہ-k_Min_خطی_میل+1;  // Ignore the match units.
            گرہ=*pos++;
        } وإلا {
            UBool isFinal = static_cast<UBool>(گرہ >> 15);
            int32_t value;
            إذا(isFinal) {
                value=read_قدر(pos, گرہ&0x7fff);
            } وإلا {
                value=read_گرہ_قدر(pos, گرہ);
            }
            إذا(have_Unique_قدر) {
                إذا(value!=unique_قدر) {
                    أرجع خطأ;
                }
            } وإلا {
                unique_قدر=value;
                have_Unique_قدر=سچ;
            }
            إذا(isFinal) {
                أرجع سچ;
            }
            pos=skip_گرہ_قدر(pos, گرہ);
            گرہ&=k_گرہ_Type_نقاب;
        }
    }
}

int32_t
UCharsTrie::get_اگلا_UChars(Appendable &out) ثابت {
    ثابت char16_t *pos=pos_;
    إذا(pos==خالی_سوچک) {
        أرجع 0;
    }
    إذا(بچیا_میل_لمائی>=0) {
        out.append_کوڈ_Unit(*pos);  // Next unit of a pending linear-match node.
        أرجع 1;
    }
    int32_t گرہ=*pos++;
    إذا(گرہ>=k_Min_قدر_Lead) {
        إذا(گرہ&k_قدر_Is_Final) {
            أرجع 0;
        } وإلا {
            pos=skip_گرہ_قدر(pos, گرہ);
            گرہ&=k_گرہ_Type_نقاب;
        }
    }
    إذا(گرہ<k_Min_خطی_میل) {
        إذا(گرہ==0) {
            گرہ=*pos++;
        }
        out.reserve_Append_سمرتھا(++گرہ);
        get_اگلا_Branch_UChars(pos, گرہ, out);
        أرجع گرہ;
    } وإلا {
        // First unit of the linear-match node.
        out.append_کوڈ_Unit(*pos);
        أرجع 1;
    }
}

فراغ
UCharsTrie::get_اگلا_Branch_UChars(ثابت char16_t *pos, int32_t length, Appendable &out) {
    طالما(length>k_Max_Branch_خطی_Sub_گرہ_لمائی) {
        ++pos;  // ignore the comparison unit
        get_اگلا_Branch_UChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        out.append_کوڈ_Unit(*pos++);
        pos=skip_قدر(pos);
    } طالما(--length>1);
    out.append_کوڈ_Unit(*pos);
}

U_NAMESPACE_سرا
