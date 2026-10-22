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

UString_Trie_نتیجه
UCharsTrie::current() ثابت {
    ثابت char16_t *pos=pos_;
    إذا(pos==اشاره_تهی) {
        أرجع USTRINGTRIE_NO_تطابق;
    } وإلا {
        int32_t گره;
        أرجع (مانده_تطابق_درازی<0 && (گره=*pos)>=k_Min_مقدار_Lead) ?
                مقدار_نتیجه(گره) : USTRINGTRIE_NO_مقدار;
    }
}

UString_Trie_نتیجه
UCharsTrie::اولی_For_کد_نقطه(UChar32 cp) {
    أرجع cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_بعدی(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_تطابق);
}

UString_Trie_نتیجه
UCharsTrie::بعدی_For_کد_نقطه(UChar32 cp) {
    أرجع cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_بعدی(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_تطابق);
}

UString_Trie_نتیجه
UCharsTrie::branch_بعدی(ثابت char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    إذا(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    طالما(length>k_Max_Branch_خطی_Sub_گره_درازی) {
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
            UString_Trie_نتیجه نتیجه;
            int32_t گره=*pos;
            إذا(گره&k_مقدار_Is_Final) {
                // Leave the final value for getValue() to read.
                نتیجه=USTRINGTRIE_FINAL_مقدار;
            } وإلا {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                إذا(گره<k_Min_Two_Unit_مقدار_Lead) {
                    delta=گره;
                } وإلا إذا(گره<k_Three_Unit_مقدار_Lead) {
                    delta=((گره-k_Min_Two_Unit_مقدار_Lead)<<16)|*pos++;
                } وإلا {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                گره=*pos;
                نتیجه= گره>=k_Min_مقدار_Lead ? مقدار_نتیجه(گره) : USTRINGTRIE_NO_مقدار;
            }
            pos_=pos;
            أرجع نتیجه;
        }
        --length;
        pos=skip_مقدار(pos);
    } طالما(length>1);
    إذا(uchar==*pos++) {
        pos_=pos;
        int32_t گره=*pos;
        أرجع گره>=k_Min_مقدار_Lead ? مقدار_نتیجه(گره) : USTRINGTRIE_NO_مقدار;
    } وإلا {
        stop();
        أرجع USTRINGTRIE_NO_تطابق;
    }
}

UString_Trie_نتیجه
UCharsTrie::بعدی_Impl(ثابت char16_t *pos, int32_t uchar) {
    int32_t گره=*pos++;
    لكل(;;) {
        إذا(گره<k_Min_خطی_تطابق) {
            أرجع branch_بعدی(pos, گره, uchar);
        } وإلا إذا(گره<k_Min_مقدار_Lead) {
            // Match the first of length+1 units.
            int32_t length=گره-k_Min_خطی_تطابق;  // Actual match length minus 1.
            إذا(uchar==*pos++) {
                مانده_تطابق_درازی=--length;
                pos_=pos;
                أرجع (length<0 && (گره=*pos)>=k_Min_مقدار_Lead) ?
                        مقدار_نتیجه(گره) : USTRINGTRIE_NO_مقدار;
            } وإلا {
                // No match.
                اكسر;
            }
        } وإلا إذا(گره&k_مقدار_Is_Final) {
            // No further matching units.
            اكسر;
        } وإلا {
            // Skip intermediate value.
            pos=skip_گره_مقدار(pos, گره);
            گره&=k_گره_Type_ماسک;
        }
    }
    stop();
    أرجع USTRINGTRIE_NO_تطابق;
}

UString_Trie_نتیجه
UCharsTrie::next(int32_t uchar) {
    ثابت char16_t *pos=pos_;
    إذا(pos==اشاره_تهی) {
        أرجع USTRINGTRIE_NO_تطابق;
    }
    int32_t length=مانده_تطابق_درازی;  // Actual remaining match length minus 1.
    إذا(length>=0) {
        // Remaining part of a linear-match node.
        إذا(uchar==*pos++) {
            مانده_تطابق_درازی=--length;
            pos_=pos;
            int32_t گره;
            أرجع (length<0 && (گره=*pos)>=k_Min_مقدار_Lead) ?
                    مقدار_نتیجه(گره) : USTRINGTRIE_NO_مقدار;
        } وإلا {
            stop();
            أرجع USTRINGTRIE_NO_تطابق;
        }
    }
    أرجع بعدی_Impl(pos, uchar);
}

UString_Trie_نتیجه
UCharsTrie::next(ConstChar16Ptr ptr, int32_t s_درازی) {
    ثابت char16_t *s=ptr;
    إذا(s_درازی<0 ? *s==0 : s_درازی==0) {
        // Empty input.
        أرجع current();
    }
    ثابت char16_t *pos=pos_;
    إذا(pos==اشاره_تهی) {
        أرجع USTRINGTRIE_NO_تطابق;
    }
    int32_t length=مانده_تطابق_درازی;  // Actual remaining match length minus 1.
    لكل(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        إذا(s_درازی<0) {
            لكل(;;) {
                إذا((uchar=*s++)==0) {
                    مانده_تطابق_درازی=length;
                    pos_=pos;
                    int32_t گره;
                    أرجع (length<0 && (گره=*pos)>=k_Min_مقدار_Lead) ?
                            مقدار_نتیجه(گره) : USTRINGTRIE_NO_مقدار;
                }
                إذا(length<0) {
                    مانده_تطابق_درازی=length;
                    اكسر;
                }
                إذا(uchar!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_تطابق;
                }
                ++pos;
                --length;
            }
        } وإلا {
            لكل(;;) {
                إذا(s_درازی==0) {
                    مانده_تطابق_درازی=length;
                    pos_=pos;
                    int32_t گره;
                    أرجع (length<0 && (گره=*pos)>=k_Min_مقدار_Lead) ?
                            مقدار_نتیجه(گره) : USTRINGTRIE_NO_مقدار;
                }
                uchar=*s++;
                --s_درازی;
                إذا(length<0) {
                    مانده_تطابق_درازی=length;
                    اكسر;
                }
                إذا(uchar!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_تطابق;
                }
                ++pos;
                --length;
            }
        }
        int32_t گره=*pos++;
        لكل(;;) {
            إذا(گره<k_Min_خطی_تطابق) {
                UString_Trie_نتیجه نتیجه=branch_بعدی(pos, گره, uchar);
                إذا(نتیجه==USTRINGTRIE_NO_تطابق) {
                    أرجع USTRINGTRIE_NO_تطابق;
                }
                // Fetch the next input unit, if there is one.
                إذا(s_درازی<0) {
                    إذا((uchar=*s++)==0) {
                        أرجع نتیجه;
                    }
                } وإلا {
                    إذا(s_درازی==0) {
                        أرجع نتیجه;
                    }
                    uchar=*s++;
                    --s_درازی;
                }
                إذا(نتیجه==USTRINGTRIE_FINAL_مقدار) {
                    // No further matching units.
                    stop();
                    أرجع USTRINGTRIE_NO_تطابق;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                گره=*pos++;
            } وإلا إذا(گره<k_Min_مقدار_Lead) {
                // Match length+1 units.
                length=گره-k_Min_خطی_تطابق;  // Actual match length minus 1.
                إذا(uchar!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_تطابق;
                }
                ++pos;
                --length;
                اكسر;
            } وإلا إذا(گره&k_مقدار_Is_Final) {
                // No further matching units.
                stop();
                أرجع USTRINGTRIE_NO_تطابق;
            } وإلا {
                // Skip intermediate value.
                pos=skip_گره_مقدار(pos, گره);
                گره&=k_گره_Type_ماسک;
            }
        }
    }
}

ثابت char16_t*
UCharsTrie::پیدا_کن_Unique_مقدار_From_Branch(ثابت char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_مقدار,
                                      int32_t& unique_مقدار) {
    طالما(length>k_Max_Branch_خطی_Sub_گره_درازی) {
        ++pos;  // ignore the comparison unit
        إذا(اشاره_تهی==پیدا_کن_Unique_مقدار_From_Branch(jumpByDelta(pos), length>>1, have_Unique_مقدار, unique_مقدار)) {
            أرجع اشاره_تهی;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t گره=*pos++;
        UBool isFinal = static_cast<UBool>(گره >> 15);
        گره&=0x7fff;
        int32_t value=read_مقدار(pos, گره);
        pos=skip_مقدار(pos, گره);
        إذا(isFinal) {
            إذا(have_Unique_مقدار) {
                إذا(value!=unique_مقدار) {
                    أرجع اشاره_تهی;
                }
            } وإلا {
                unique_مقدار=value;
                have_Unique_مقدار=درست;
            }
        } وإلا {
            إذا(!پیدا_کن_Unique_مقدار(pos+value, have_Unique_مقدار, unique_مقدار)) {
                أرجع اشاره_تهی;
            }
            have_Unique_مقدار=درست;
        }
    } طالما(--length>1);
    أرجع pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::پیدا_کن_Unique_مقدار(ثابت char16_t *pos, UBool have_Unique_مقدار, int32_t &unique_مقدار) {
    int32_t گره=*pos++;
    لكل(;;) {
        إذا(گره<k_Min_خطی_تطابق) {
            إذا(گره==0) {
                گره=*pos++;
            }
            pos=پیدا_کن_Unique_مقدار_From_Branch(pos, گره+1, have_Unique_مقدار, unique_مقدار);
            إذا(pos==اشاره_تهی) {
                أرجع خطأ;
            }
            have_Unique_مقدار=درست;
            گره=*pos++;
        } وإلا إذا(گره<k_Min_مقدار_Lead) {
            // linear-match node
            pos+=گره-k_Min_خطی_تطابق+1;  // Ignore the match units.
            گره=*pos++;
        } وإلا {
            UBool isFinal = static_cast<UBool>(گره >> 15);
            int32_t value;
            إذا(isFinal) {
                value=read_مقدار(pos, گره&0x7fff);
            } وإلا {
                value=read_گره_مقدار(pos, گره);
            }
            إذا(have_Unique_مقدار) {
                إذا(value!=unique_مقدار) {
                    أرجع خطأ;
                }
            } وإلا {
                unique_مقدار=value;
                have_Unique_مقدار=درست;
            }
            إذا(isFinal) {
                أرجع درست;
            }
            pos=skip_گره_مقدار(pos, گره);
            گره&=k_گره_Type_ماسک;
        }
    }
}

int32_t
UCharsTrie::get_بعدی_UChars(Appendable &out) ثابت {
    ثابت char16_t *pos=pos_;
    إذا(pos==اشاره_تهی) {
        أرجع 0;
    }
    إذا(مانده_تطابق_درازی>=0) {
        out.append_کد_Unit(*pos);  // Next unit of a pending linear-match node.
        أرجع 1;
    }
    int32_t گره=*pos++;
    إذا(گره>=k_Min_مقدار_Lead) {
        إذا(گره&k_مقدار_Is_Final) {
            أرجع 0;
        } وإلا {
            pos=skip_گره_مقدار(pos, گره);
            گره&=k_گره_Type_ماسک;
        }
    }
    إذا(گره<k_Min_خطی_تطابق) {
        إذا(گره==0) {
            گره=*pos++;
        }
        out.reserve_Append_ظرفیت(++گره);
        get_بعدی_Branch_UChars(pos, گره, out);
        أرجع گره;
    } وإلا {
        // First unit of the linear-match node.
        out.append_کد_Unit(*pos);
        أرجع 1;
    }
}

فراغ
UCharsTrie::get_بعدی_Branch_UChars(ثابت char16_t *pos, int32_t length, Appendable &out) {
    طالما(length>k_Max_Branch_خطی_Sub_گره_درازی) {
        ++pos;  // ignore the comparison unit
        get_بعدی_Branch_UChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        out.append_کد_Unit(*pos++);
        pos=skip_مقدار(pos);
    } طالما(--length>1);
    out.append_کد_Unit(*pos);
}

U_NAMESPACE_انتها
