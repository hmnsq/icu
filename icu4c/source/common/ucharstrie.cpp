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

UString_Trie_پایله
UCharsTrie::current() ثابت {
    ثابت char16_t *pos=pos_;
    إذا(pos==تش_اشاره) {
        أرجع USTRINGTRIE_NO_سمون;
    } وإلا {
        int32_t غوټه;
        أرجع (پاتې_شوی_سمون_اوږدتیا<0 && (غوټه=*pos)>=k_Min_ارزښت_Lead) ?
                ارزښت_پایله(غوټه) : USTRINGTRIE_NO_ارزښت;
    }
}

UString_Trie_پایله
UCharsTrie::لومړی_For_کوډ_ټکی(UChar32 cp) {
    أرجع cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_بل(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_سمون);
}

UString_Trie_پایله
UCharsTrie::بل_For_کوډ_ټکی(UChar32 cp) {
    أرجع cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_بل(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_سمون);
}

UString_Trie_پایله
UCharsTrie::branch_بل(ثابت char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    إذا(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    طالما(length>k_Max_Branch_خطي_Sub_غوټه_اوږدتیا) {
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
            UString_Trie_پایله پایله;
            int32_t غوټه=*pos;
            إذا(غوټه&k_ارزښت_Is_Final) {
                // Leave the final value for getValue() to read.
                پایله=USTRINGTRIE_FINAL_ارزښت;
            } وإلا {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                إذا(غوټه<k_Min_Two_Unit_ارزښت_Lead) {
                    delta=غوټه;
                } وإلا إذا(غوټه<k_Three_Unit_ارزښت_Lead) {
                    delta=((غوټه-k_Min_Two_Unit_ارزښت_Lead)<<16)|*pos++;
                } وإلا {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                غوټه=*pos;
                پایله= غوټه>=k_Min_ارزښت_Lead ? ارزښت_پایله(غوټه) : USTRINGTRIE_NO_ارزښت;
            }
            pos_=pos;
            أرجع پایله;
        }
        --length;
        pos=skip_ارزښت(pos);
    } طالما(length>1);
    إذا(uchar==*pos++) {
        pos_=pos;
        int32_t غوټه=*pos;
        أرجع غوټه>=k_Min_ارزښت_Lead ? ارزښت_پایله(غوټه) : USTRINGTRIE_NO_ارزښت;
    } وإلا {
        stop();
        أرجع USTRINGTRIE_NO_سمون;
    }
}

UString_Trie_پایله
UCharsTrie::بل_Impl(ثابت char16_t *pos, int32_t uchar) {
    int32_t غوټه=*pos++;
    لكل(;;) {
        إذا(غوټه<k_Min_خطي_سمون) {
            أرجع branch_بل(pos, غوټه, uchar);
        } وإلا إذا(غوټه<k_Min_ارزښت_Lead) {
            // Match the first of length+1 units.
            int32_t length=غوټه-k_Min_خطي_سمون;  // Actual match length minus 1.
            إذا(uchar==*pos++) {
                پاتې_شوی_سمون_اوږدتیا=--length;
                pos_=pos;
                أرجع (length<0 && (غوټه=*pos)>=k_Min_ارزښت_Lead) ?
                        ارزښت_پایله(غوټه) : USTRINGTRIE_NO_ارزښت;
            } وإلا {
                // No match.
                اكسر;
            }
        } وإلا إذا(غوټه&k_ارزښت_Is_Final) {
            // No further matching units.
            اكسر;
        } وإلا {
            // Skip intermediate value.
            pos=skip_غوټه_ارزښت(pos, غوټه);
            غوټه&=k_غوټه_Type_ماسک;
        }
    }
    stop();
    أرجع USTRINGTRIE_NO_سمون;
}

UString_Trie_پایله
UCharsTrie::next(int32_t uchar) {
    ثابت char16_t *pos=pos_;
    إذا(pos==تش_اشاره) {
        أرجع USTRINGTRIE_NO_سمون;
    }
    int32_t length=پاتې_شوی_سمون_اوږدتیا;  // Actual remaining match length minus 1.
    إذا(length>=0) {
        // Remaining part of a linear-match node.
        إذا(uchar==*pos++) {
            پاتې_شوی_سمون_اوږدتیا=--length;
            pos_=pos;
            int32_t غوټه;
            أرجع (length<0 && (غوټه=*pos)>=k_Min_ارزښت_Lead) ?
                    ارزښت_پایله(غوټه) : USTRINGTRIE_NO_ارزښت;
        } وإلا {
            stop();
            أرجع USTRINGTRIE_NO_سمون;
        }
    }
    أرجع بل_Impl(pos, uchar);
}

UString_Trie_پایله
UCharsTrie::next(ConstChar16Ptr ptr, int32_t s_اوږدتیا) {
    ثابت char16_t *s=ptr;
    إذا(s_اوږدتیا<0 ? *s==0 : s_اوږدتیا==0) {
        // Empty input.
        أرجع current();
    }
    ثابت char16_t *pos=pos_;
    إذا(pos==تش_اشاره) {
        أرجع USTRINGTRIE_NO_سمون;
    }
    int32_t length=پاتې_شوی_سمون_اوږدتیا;  // Actual remaining match length minus 1.
    لكل(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        إذا(s_اوږدتیا<0) {
            لكل(;;) {
                إذا((uchar=*s++)==0) {
                    پاتې_شوی_سمون_اوږدتیا=length;
                    pos_=pos;
                    int32_t غوټه;
                    أرجع (length<0 && (غوټه=*pos)>=k_Min_ارزښت_Lead) ?
                            ارزښت_پایله(غوټه) : USTRINGTRIE_NO_ارزښت;
                }
                إذا(length<0) {
                    پاتې_شوی_سمون_اوږدتیا=length;
                    اكسر;
                }
                إذا(uchar!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_سمون;
                }
                ++pos;
                --length;
            }
        } وإلا {
            لكل(;;) {
                إذا(s_اوږدتیا==0) {
                    پاتې_شوی_سمون_اوږدتیا=length;
                    pos_=pos;
                    int32_t غوټه;
                    أرجع (length<0 && (غوټه=*pos)>=k_Min_ارزښت_Lead) ?
                            ارزښت_پایله(غوټه) : USTRINGTRIE_NO_ارزښت;
                }
                uchar=*s++;
                --s_اوږدتیا;
                إذا(length<0) {
                    پاتې_شوی_سمون_اوږدتیا=length;
                    اكسر;
                }
                إذا(uchar!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_سمون;
                }
                ++pos;
                --length;
            }
        }
        int32_t غوټه=*pos++;
        لكل(;;) {
            إذا(غوټه<k_Min_خطي_سمون) {
                UString_Trie_پایله پایله=branch_بل(pos, غوټه, uchar);
                إذا(پایله==USTRINGTRIE_NO_سمون) {
                    أرجع USTRINGTRIE_NO_سمون;
                }
                // Fetch the next input unit, if there is one.
                إذا(s_اوږدتیا<0) {
                    إذا((uchar=*s++)==0) {
                        أرجع پایله;
                    }
                } وإلا {
                    إذا(s_اوږدتیا==0) {
                        أرجع پایله;
                    }
                    uchar=*s++;
                    --s_اوږدتیا;
                }
                إذا(پایله==USTRINGTRIE_FINAL_ارزښت) {
                    // No further matching units.
                    stop();
                    أرجع USTRINGTRIE_NO_سمون;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                غوټه=*pos++;
            } وإلا إذا(غوټه<k_Min_ارزښت_Lead) {
                // Match length+1 units.
                length=غوټه-k_Min_خطي_سمون;  // Actual match length minus 1.
                إذا(uchar!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_سمون;
                }
                ++pos;
                --length;
                اكسر;
            } وإلا إذا(غوټه&k_ارزښت_Is_Final) {
                // No further matching units.
                stop();
                أرجع USTRINGTRIE_NO_سمون;
            } وإلا {
                // Skip intermediate value.
                pos=skip_غوټه_ارزښت(pos, غوټه);
                غوټه&=k_غوټه_Type_ماسک;
            }
        }
    }
}

ثابت char16_t*
UCharsTrie::پیدا_کړه_Unique_ارزښت_From_Branch(ثابت char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_ارزښت,
                                      int32_t& unique_ارزښت) {
    طالما(length>k_Max_Branch_خطي_Sub_غوټه_اوږدتیا) {
        ++pos;  // ignore the comparison unit
        إذا(تش_اشاره==پیدا_کړه_Unique_ارزښت_From_Branch(jumpByDelta(pos), length>>1, have_Unique_ارزښت, unique_ارزښت)) {
            أرجع تش_اشاره;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t غوټه=*pos++;
        UBool isFinal = static_cast<UBool>(غوټه >> 15);
        غوټه&=0x7fff;
        int32_t value=read_ارزښت(pos, غوټه);
        pos=skip_ارزښت(pos, غوټه);
        إذا(isFinal) {
            إذا(have_Unique_ارزښت) {
                إذا(value!=unique_ارزښت) {
                    أرجع تش_اشاره;
                }
            } وإلا {
                unique_ارزښت=value;
                have_Unique_ارزښت=ریښتیا;
            }
        } وإلا {
            إذا(!پیدا_کړه_Unique_ارزښت(pos+value, have_Unique_ارزښت, unique_ارزښت)) {
                أرجع تش_اشاره;
            }
            have_Unique_ارزښت=ریښتیا;
        }
    } طالما(--length>1);
    أرجع pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::پیدا_کړه_Unique_ارزښت(ثابت char16_t *pos, UBool have_Unique_ارزښت, int32_t &unique_ارزښت) {
    int32_t غوټه=*pos++;
    لكل(;;) {
        إذا(غوټه<k_Min_خطي_سمون) {
            إذا(غوټه==0) {
                غوټه=*pos++;
            }
            pos=پیدا_کړه_Unique_ارزښت_From_Branch(pos, غوټه+1, have_Unique_ارزښت, unique_ارزښت);
            إذا(pos==تش_اشاره) {
                أرجع خطأ;
            }
            have_Unique_ارزښت=ریښتیا;
            غوټه=*pos++;
        } وإلا إذا(غوټه<k_Min_ارزښت_Lead) {
            // linear-match node
            pos+=غوټه-k_Min_خطي_سمون+1;  // Ignore the match units.
            غوټه=*pos++;
        } وإلا {
            UBool isFinal = static_cast<UBool>(غوټه >> 15);
            int32_t value;
            إذا(isFinal) {
                value=read_ارزښت(pos, غوټه&0x7fff);
            } وإلا {
                value=read_غوټه_ارزښت(pos, غوټه);
            }
            إذا(have_Unique_ارزښت) {
                إذا(value!=unique_ارزښت) {
                    أرجع خطأ;
                }
            } وإلا {
                unique_ارزښت=value;
                have_Unique_ارزښت=ریښتیا;
            }
            إذا(isFinal) {
                أرجع ریښتیا;
            }
            pos=skip_غوټه_ارزښت(pos, غوټه);
            غوټه&=k_غوټه_Type_ماسک;
        }
    }
}

int32_t
UCharsTrie::get_بل_UChars(Appendable &out) ثابت {
    ثابت char16_t *pos=pos_;
    إذا(pos==تش_اشاره) {
        أرجع 0;
    }
    إذا(پاتې_شوی_سمون_اوږدتیا>=0) {
        out.append_کوډ_Unit(*pos);  // Next unit of a pending linear-match node.
        أرجع 1;
    }
    int32_t غوټه=*pos++;
    إذا(غوټه>=k_Min_ارزښت_Lead) {
        إذا(غوټه&k_ارزښت_Is_Final) {
            أرجع 0;
        } وإلا {
            pos=skip_غوټه_ارزښت(pos, غوټه);
            غوټه&=k_غوټه_Type_ماسک;
        }
    }
    إذا(غوټه<k_Min_خطي_سمون) {
        إذا(غوټه==0) {
            غوټه=*pos++;
        }
        out.reserve_Append_ظرفیت(++غوټه);
        get_بل_Branch_UChars(pos, غوټه, out);
        أرجع غوټه;
    } وإلا {
        // First unit of the linear-match node.
        out.append_کوډ_Unit(*pos);
        أرجع 1;
    }
}

فراغ
UCharsTrie::get_بل_Branch_UChars(ثابت char16_t *pos, int32_t length, Appendable &out) {
    طالما(length>k_Max_Branch_خطي_Sub_غوټه_اوږدتیا) {
        ++pos;  // ignore the comparison unit
        get_بل_Branch_UChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        out.append_کوډ_Unit(*pos++);
        pos=skip_ارزښت(pos);
    } طالما(--length>1);
    out.append_کوډ_Unit(*pos);
}

U_NAMESPACE_پای_ټکی
