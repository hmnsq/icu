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

UString_Trie_نتيجو
UCharsTrie::current() ثابت {
    ثابت char16_t *pos=pos_;
    إذا(pos==خالي_اشارو) {
        أرجع USTRINGTRIE_NO_ميلاپ;
    } وإلا {
        int32_t ڳنڍ;
        أرجع (بچيل_ميلاپ_ڊيگهائي<0 && (ڳنڍ=*pos)>=k_Min_قدر_Lead) ?
                قدر_نتيجو(ڳنڍ) : USTRINGTRIE_NO_قدر;
    }
}

UString_Trie_نتيجو
UCharsTrie::پهريون_For_ڪوڊ_نقطو(UChar32 cp) {
    أرجع cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_ايندڙ(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_ميلاپ);
}

UString_Trie_نتيجو
UCharsTrie::ايندڙ_For_ڪوڊ_نقطو(UChar32 cp) {
    أرجع cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_ايندڙ(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_ميلاپ);
}

UString_Trie_نتيجو
UCharsTrie::branch_ايندڙ(ثابت char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    إذا(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    طالما(length>k_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي) {
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
            UString_Trie_نتيجو نتيجو;
            int32_t ڳنڍ=*pos;
            إذا(ڳنڍ&k_قدر_Is_Final) {
                // Leave the final value for getValue() to read.
                نتيجو=USTRINGTRIE_FINAL_قدر;
            } وإلا {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                إذا(ڳنڍ<k_Min_Two_Unit_قدر_Lead) {
                    delta=ڳنڍ;
                } وإلا إذا(ڳنڍ<k_Three_Unit_قدر_Lead) {
                    delta=((ڳنڍ-k_Min_Two_Unit_قدر_Lead)<<16)|*pos++;
                } وإلا {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                ڳنڍ=*pos;
                نتيجو= ڳنڍ>=k_Min_قدر_Lead ? قدر_نتيجو(ڳنڍ) : USTRINGTRIE_NO_قدر;
            }
            pos_=pos;
            أرجع نتيجو;
        }
        --length;
        pos=skip_قدر(pos);
    } طالما(length>1);
    إذا(uchar==*pos++) {
        pos_=pos;
        int32_t ڳنڍ=*pos;
        أرجع ڳنڍ>=k_Min_قدر_Lead ? قدر_نتيجو(ڳنڍ) : USTRINGTRIE_NO_قدر;
    } وإلا {
        stop();
        أرجع USTRINGTRIE_NO_ميلاپ;
    }
}

UString_Trie_نتيجو
UCharsTrie::ايندڙ_Impl(ثابت char16_t *pos, int32_t uchar) {
    int32_t ڳنڍ=*pos++;
    لكل(;;) {
        إذا(ڳنڍ<k_Min_لڪيري_ميلاپ) {
            أرجع branch_ايندڙ(pos, ڳنڍ, uchar);
        } وإلا إذا(ڳنڍ<k_Min_قدر_Lead) {
            // Match the first of length+1 units.
            int32_t length=ڳنڍ-k_Min_لڪيري_ميلاپ;  // Actual match length minus 1.
            إذا(uchar==*pos++) {
                بچيل_ميلاپ_ڊيگهائي=--length;
                pos_=pos;
                أرجع (length<0 && (ڳنڍ=*pos)>=k_Min_قدر_Lead) ?
                        قدر_نتيجو(ڳنڍ) : USTRINGTRIE_NO_قدر;
            } وإلا {
                // No match.
                اكسر;
            }
        } وإلا إذا(ڳنڍ&k_قدر_Is_Final) {
            // No further matching units.
            اكسر;
        } وإلا {
            // Skip intermediate value.
            pos=skip_ڳنڍ_قدر(pos, ڳنڍ);
            ڳنڍ&=k_ڳنڍ_Type_نقاب;
        }
    }
    stop();
    أرجع USTRINGTRIE_NO_ميلاپ;
}

UString_Trie_نتيجو
UCharsTrie::next(int32_t uchar) {
    ثابت char16_t *pos=pos_;
    إذا(pos==خالي_اشارو) {
        أرجع USTRINGTRIE_NO_ميلاپ;
    }
    int32_t length=بچيل_ميلاپ_ڊيگهائي;  // Actual remaining match length minus 1.
    إذا(length>=0) {
        // Remaining part of a linear-match node.
        إذا(uchar==*pos++) {
            بچيل_ميلاپ_ڊيگهائي=--length;
            pos_=pos;
            int32_t ڳنڍ;
            أرجع (length<0 && (ڳنڍ=*pos)>=k_Min_قدر_Lead) ?
                    قدر_نتيجو(ڳنڍ) : USTRINGTRIE_NO_قدر;
        } وإلا {
            stop();
            أرجع USTRINGTRIE_NO_ميلاپ;
        }
    }
    أرجع ايندڙ_Impl(pos, uchar);
}

UString_Trie_نتيجو
UCharsTrie::next(ConstChar16Ptr ptr, int32_t s_ڊيگهائي) {
    ثابت char16_t *s=ptr;
    إذا(s_ڊيگهائي<0 ? *s==0 : s_ڊيگهائي==0) {
        // Empty input.
        أرجع current();
    }
    ثابت char16_t *pos=pos_;
    إذا(pos==خالي_اشارو) {
        أرجع USTRINGTRIE_NO_ميلاپ;
    }
    int32_t length=بچيل_ميلاپ_ڊيگهائي;  // Actual remaining match length minus 1.
    لكل(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        إذا(s_ڊيگهائي<0) {
            لكل(;;) {
                إذا((uchar=*s++)==0) {
                    بچيل_ميلاپ_ڊيگهائي=length;
                    pos_=pos;
                    int32_t ڳنڍ;
                    أرجع (length<0 && (ڳنڍ=*pos)>=k_Min_قدر_Lead) ?
                            قدر_نتيجو(ڳنڍ) : USTRINGTRIE_NO_قدر;
                }
                إذا(length<0) {
                    بچيل_ميلاپ_ڊيگهائي=length;
                    اكسر;
                }
                إذا(uchar!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_ميلاپ;
                }
                ++pos;
                --length;
            }
        } وإلا {
            لكل(;;) {
                إذا(s_ڊيگهائي==0) {
                    بچيل_ميلاپ_ڊيگهائي=length;
                    pos_=pos;
                    int32_t ڳنڍ;
                    أرجع (length<0 && (ڳنڍ=*pos)>=k_Min_قدر_Lead) ?
                            قدر_نتيجو(ڳنڍ) : USTRINGTRIE_NO_قدر;
                }
                uchar=*s++;
                --s_ڊيگهائي;
                إذا(length<0) {
                    بچيل_ميلاپ_ڊيگهائي=length;
                    اكسر;
                }
                إذا(uchar!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_ميلاپ;
                }
                ++pos;
                --length;
            }
        }
        int32_t ڳنڍ=*pos++;
        لكل(;;) {
            إذا(ڳنڍ<k_Min_لڪيري_ميلاپ) {
                UString_Trie_نتيجو نتيجو=branch_ايندڙ(pos, ڳنڍ, uchar);
                إذا(نتيجو==USTRINGTRIE_NO_ميلاپ) {
                    أرجع USTRINGTRIE_NO_ميلاپ;
                }
                // Fetch the next input unit, if there is one.
                إذا(s_ڊيگهائي<0) {
                    إذا((uchar=*s++)==0) {
                        أرجع نتيجو;
                    }
                } وإلا {
                    إذا(s_ڊيگهائي==0) {
                        أرجع نتيجو;
                    }
                    uchar=*s++;
                    --s_ڊيگهائي;
                }
                إذا(نتيجو==USTRINGTRIE_FINAL_قدر) {
                    // No further matching units.
                    stop();
                    أرجع USTRINGTRIE_NO_ميلاپ;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                ڳنڍ=*pos++;
            } وإلا إذا(ڳنڍ<k_Min_قدر_Lead) {
                // Match length+1 units.
                length=ڳنڍ-k_Min_لڪيري_ميلاپ;  // Actual match length minus 1.
                إذا(uchar!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_ميلاپ;
                }
                ++pos;
                --length;
                اكسر;
            } وإلا إذا(ڳنڍ&k_قدر_Is_Final) {
                // No further matching units.
                stop();
                أرجع USTRINGTRIE_NO_ميلاپ;
            } وإلا {
                // Skip intermediate value.
                pos=skip_ڳنڍ_قدر(pos, ڳنڍ);
                ڳنڍ&=k_ڳنڍ_Type_نقاب;
            }
        }
    }
}

ثابت char16_t*
UCharsTrie::ڳوليو_Unique_قدر_From_Branch(ثابت char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_قدر,
                                      int32_t& unique_قدر) {
    طالما(length>k_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي) {
        ++pos;  // ignore the comparison unit
        إذا(خالي_اشارو==ڳوليو_Unique_قدر_From_Branch(jumpByDelta(pos), length>>1, have_Unique_قدر, unique_قدر)) {
            أرجع خالي_اشارو;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t ڳنڍ=*pos++;
        UBool isFinal = static_cast<UBool>(ڳنڍ >> 15);
        ڳنڍ&=0x7fff;
        int32_t value=read_قدر(pos, ڳنڍ);
        pos=skip_قدر(pos, ڳنڍ);
        إذا(isFinal) {
            إذا(have_Unique_قدر) {
                إذا(value!=unique_قدر) {
                    أرجع خالي_اشارو;
                }
            } وإلا {
                unique_قدر=value;
                have_Unique_قدر=سچ;
            }
        } وإلا {
            إذا(!ڳوليو_Unique_قدر(pos+value, have_Unique_قدر, unique_قدر)) {
                أرجع خالي_اشارو;
            }
            have_Unique_قدر=سچ;
        }
    } طالما(--length>1);
    أرجع pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::ڳوليو_Unique_قدر(ثابت char16_t *pos, UBool have_Unique_قدر, int32_t &unique_قدر) {
    int32_t ڳنڍ=*pos++;
    لكل(;;) {
        إذا(ڳنڍ<k_Min_لڪيري_ميلاپ) {
            إذا(ڳنڍ==0) {
                ڳنڍ=*pos++;
            }
            pos=ڳوليو_Unique_قدر_From_Branch(pos, ڳنڍ+1, have_Unique_قدر, unique_قدر);
            إذا(pos==خالي_اشارو) {
                أرجع خطأ;
            }
            have_Unique_قدر=سچ;
            ڳنڍ=*pos++;
        } وإلا إذا(ڳنڍ<k_Min_قدر_Lead) {
            // linear-match node
            pos+=ڳنڍ-k_Min_لڪيري_ميلاپ+1;  // Ignore the match units.
            ڳنڍ=*pos++;
        } وإلا {
            UBool isFinal = static_cast<UBool>(ڳنڍ >> 15);
            int32_t value;
            إذا(isFinal) {
                value=read_قدر(pos, ڳنڍ&0x7fff);
            } وإلا {
                value=read_ڳنڍ_قدر(pos, ڳنڍ);
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
            pos=skip_ڳنڍ_قدر(pos, ڳنڍ);
            ڳنڍ&=k_ڳنڍ_Type_نقاب;
        }
    }
}

int32_t
UCharsTrie::get_ايندڙ_UChars(Appendable &out) ثابت {
    ثابت char16_t *pos=pos_;
    إذا(pos==خالي_اشارو) {
        أرجع 0;
    }
    إذا(بچيل_ميلاپ_ڊيگهائي>=0) {
        out.append_ڪوڊ_Unit(*pos);  // Next unit of a pending linear-match node.
        أرجع 1;
    }
    int32_t ڳنڍ=*pos++;
    إذا(ڳنڍ>=k_Min_قدر_Lead) {
        إذا(ڳنڍ&k_قدر_Is_Final) {
            أرجع 0;
        } وإلا {
            pos=skip_ڳنڍ_قدر(pos, ڳنڍ);
            ڳنڍ&=k_ڳنڍ_Type_نقاب;
        }
    }
    إذا(ڳنڍ<k_Min_لڪيري_ميلاپ) {
        إذا(ڳنڍ==0) {
            ڳنڍ=*pos++;
        }
        out.reserve_Append_صلاحيت(++ڳنڍ);
        get_ايندڙ_Branch_UChars(pos, ڳنڍ, out);
        أرجع ڳنڍ;
    } وإلا {
        // First unit of the linear-match node.
        out.append_ڪوڊ_Unit(*pos);
        أرجع 1;
    }
}

فراغ
UCharsTrie::get_ايندڙ_Branch_UChars(ثابت char16_t *pos, int32_t length, Appendable &out) {
    طالما(length>k_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي) {
        ++pos;  // ignore the comparison unit
        get_ايندڙ_Branch_UChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        out.append_ڪوڊ_Unit(*pos++);
        pos=skip_قدر(pos);
    } طالما(--length>1);
    out.append_ڪوڊ_Unit(*pos);
}

U_NAMESPACE_پڇاڙي
