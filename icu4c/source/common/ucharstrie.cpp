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

#شامل "unicode/utypes.h"
#شامل "unicode/appendable.h"
#شامل "unicode/ucharstrie.h"
#شامل "unicode/uobject.h"
#شامل "unicode/utf16.h"
#شامل "cmemory.h"
#شامل "uassert.h"

U_NAMESPACE_BEGIN

UCharsTrie::~UCharsTrie() {
    uprv_free(ownedArray_);
}

UString_Trie_نتیجہ
UCharsTrie::current() ثابت {
    ثابت char16_t *pos=pos_;
    اگر(pos==خالی_اشاریہ) {
        واپس USTRINGTRIE_NO_مطابقت;
    } ورنہ {
        int32_t گرہ;
        واپس (بقایا_مطابقت_لمبائی<0 && (گرہ=*pos)>=k_Min_قدر_Lead) ?
                قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
    }
}

UString_Trie_نتیجہ
UCharsTrie::پہلا_For_رمز_نقطہ(UChar32 cp) {
    واپس cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_اگلا(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_مطابقت);
}

UString_Trie_نتیجہ
UCharsTrie::اگلا_For_رمز_نقطہ(UChar32 cp) {
    واپس cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_اگلا(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_مطابقت);
}

UString_Trie_نتیجہ
UCharsTrie::branch_اگلا(ثابت char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    اگر(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    جب(length>k_Max_Branch_خطی_Sub_گرہ_لمبائی) {
        اگر(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ورنہ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    کرو {
        اگر(uchar==*pos++) {
            UString_Trie_نتیجہ نتیجہ;
            int32_t گرہ=*pos;
            اگر(گرہ&k_قدر_Is_Final) {
                // Leave the final value for getValue() to read.
                نتیجہ=USTRINGTRIE_FINAL_قدر;
            } ورنہ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                اگر(گرہ<k_Min_Two_Unit_قدر_Lead) {
                    delta=گرہ;
                } ورنہ اگر(گرہ<k_Three_Unit_قدر_Lead) {
                    delta=((گرہ-k_Min_Two_Unit_قدر_Lead)<<16)|*pos++;
                } ورنہ {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                گرہ=*pos;
                نتیجہ= گرہ>=k_Min_قدر_Lead ? قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
            }
            pos_=pos;
            واپس نتیجہ;
        }
        --length;
        pos=skip_قدر(pos);
    } جب(length>1);
    اگر(uchar==*pos++) {
        pos_=pos;
        int32_t گرہ=*pos;
        واپس گرہ>=k_Min_قدر_Lead ? قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
    } ورنہ {
        stop();
        واپس USTRINGTRIE_NO_مطابقت;
    }
}

UString_Trie_نتیجہ
UCharsTrie::اگلا_Impl(ثابت char16_t *pos, int32_t uchar) {
    int32_t گرہ=*pos++;
    برائے(;;) {
        اگر(گرہ<k_Min_خطی_مطابقت) {
            واپس branch_اگلا(pos, گرہ, uchar);
        } ورنہ اگر(گرہ<k_Min_قدر_Lead) {
            // Match the first of length+1 units.
            int32_t length=گرہ-k_Min_خطی_مطابقت;  // Actual match length minus 1.
            اگر(uchar==*pos++) {
                بقایا_مطابقت_لمبائی=--length;
                pos_=pos;
                واپس (length<0 && (گرہ=*pos)>=k_Min_قدر_Lead) ?
                        قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
            } ورنہ {
                // No match.
                توڑو;
            }
        } ورنہ اگر(گرہ&k_قدر_Is_Final) {
            // No further matching units.
            توڑو;
        } ورنہ {
            // Skip intermediate value.
            pos=skip_گرہ_قدر(pos, گرہ);
            گرہ&=k_گرہ_Type_نقاب;
        }
    }
    stop();
    واپس USTRINGTRIE_NO_مطابقت;
}

UString_Trie_نتیجہ
UCharsTrie::next(int32_t uchar) {
    ثابت char16_t *pos=pos_;
    اگر(pos==خالی_اشاریہ) {
        واپس USTRINGTRIE_NO_مطابقت;
    }
    int32_t length=بقایا_مطابقت_لمبائی;  // Actual remaining match length minus 1.
    اگر(length>=0) {
        // Remaining part of a linear-match node.
        اگر(uchar==*pos++) {
            بقایا_مطابقت_لمبائی=--length;
            pos_=pos;
            int32_t گرہ;
            واپس (length<0 && (گرہ=*pos)>=k_Min_قدر_Lead) ?
                    قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
        } ورنہ {
            stop();
            واپس USTRINGTRIE_NO_مطابقت;
        }
    }
    واپس اگلا_Impl(pos, uchar);
}

UString_Trie_نتیجہ
UCharsTrie::next(ConstChar16Ptr ptr, int32_t s_لمبائی) {
    ثابت char16_t *s=ptr;
    اگر(s_لمبائی<0 ? *s==0 : s_لمبائی==0) {
        // Empty input.
        واپس current();
    }
    ثابت char16_t *pos=pos_;
    اگر(pos==خالی_اشاریہ) {
        واپس USTRINGTRIE_NO_مطابقت;
    }
    int32_t length=بقایا_مطابقت_لمبائی;  // Actual remaining match length minus 1.
    برائے(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        اگر(s_لمبائی<0) {
            برائے(;;) {
                اگر((uchar=*s++)==0) {
                    بقایا_مطابقت_لمبائی=length;
                    pos_=pos;
                    int32_t گرہ;
                    واپس (length<0 && (گرہ=*pos)>=k_Min_قدر_Lead) ?
                            قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
                }
                اگر(length<0) {
                    بقایا_مطابقت_لمبائی=length;
                    توڑو;
                }
                اگر(uchar!=*pos) {
                    stop();
                    واپس USTRINGTRIE_NO_مطابقت;
                }
                ++pos;
                --length;
            }
        } ورنہ {
            برائے(;;) {
                اگر(s_لمبائی==0) {
                    بقایا_مطابقت_لمبائی=length;
                    pos_=pos;
                    int32_t گرہ;
                    واپس (length<0 && (گرہ=*pos)>=k_Min_قدر_Lead) ?
                            قدر_نتیجہ(گرہ) : USTRINGTRIE_NO_قدر;
                }
                uchar=*s++;
                --s_لمبائی;
                اگر(length<0) {
                    بقایا_مطابقت_لمبائی=length;
                    توڑو;
                }
                اگر(uchar!=*pos) {
                    stop();
                    واپس USTRINGTRIE_NO_مطابقت;
                }
                ++pos;
                --length;
            }
        }
        int32_t گرہ=*pos++;
        برائے(;;) {
            اگر(گرہ<k_Min_خطی_مطابقت) {
                UString_Trie_نتیجہ نتیجہ=branch_اگلا(pos, گرہ, uchar);
                اگر(نتیجہ==USTRINGTRIE_NO_مطابقت) {
                    واپس USTRINGTRIE_NO_مطابقت;
                }
                // Fetch the next input unit, if there is one.
                اگر(s_لمبائی<0) {
                    اگر((uchar=*s++)==0) {
                        واپس نتیجہ;
                    }
                } ورنہ {
                    اگر(s_لمبائی==0) {
                        واپس نتیجہ;
                    }
                    uchar=*s++;
                    --s_لمبائی;
                }
                اگر(نتیجہ==USTRINGTRIE_FINAL_قدر) {
                    // No further matching units.
                    stop();
                    واپس USTRINGTRIE_NO_مطابقت;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                گرہ=*pos++;
            } ورنہ اگر(گرہ<k_Min_قدر_Lead) {
                // Match length+1 units.
                length=گرہ-k_Min_خطی_مطابقت;  // Actual match length minus 1.
                اگر(uchar!=*pos) {
                    stop();
                    واپس USTRINGTRIE_NO_مطابقت;
                }
                ++pos;
                --length;
                توڑو;
            } ورنہ اگر(گرہ&k_قدر_Is_Final) {
                // No further matching units.
                stop();
                واپس USTRINGTRIE_NO_مطابقت;
            } ورنہ {
                // Skip intermediate value.
                pos=skip_گرہ_قدر(pos, گرہ);
                گرہ&=k_گرہ_Type_نقاب;
            }
        }
    }
}

ثابت char16_t*
UCharsTrie::ڈھونڈو_Unique_قدر_From_Branch(ثابت char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_قدر,
                                      int32_t& unique_قدر) {
    جب(length>k_Max_Branch_خطی_Sub_گرہ_لمبائی) {
        ++pos;  // ignore the comparison unit
        اگر(خالی_اشاریہ==ڈھونڈو_Unique_قدر_From_Branch(jumpByDelta(pos), length>>1, have_Unique_قدر, unique_قدر)) {
            واپس خالی_اشاریہ;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    کرو {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t گرہ=*pos++;
        UBool isFinal = static_cast<UBool>(گرہ >> 15);
        گرہ&=0x7fff;
        int32_t value=read_قدر(pos, گرہ);
        pos=skip_قدر(pos, گرہ);
        اگر(isFinal) {
            اگر(have_Unique_قدر) {
                اگر(value!=unique_قدر) {
                    واپس خالی_اشاریہ;
                }
            } ورنہ {
                unique_قدر=value;
                have_Unique_قدر=سچ;
            }
        } ورنہ {
            اگر(!ڈھونڈو_Unique_قدر(pos+value, have_Unique_قدر, unique_قدر)) {
                واپس خالی_اشاریہ;
            }
            have_Unique_قدر=سچ;
        }
    } جب(--length>1);
    واپس pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::ڈھونڈو_Unique_قدر(ثابت char16_t *pos, UBool have_Unique_قدر, int32_t &unique_قدر) {
    int32_t گرہ=*pos++;
    برائے(;;) {
        اگر(گرہ<k_Min_خطی_مطابقت) {
            اگر(گرہ==0) {
                گرہ=*pos++;
            }
            pos=ڈھونڈو_Unique_قدر_From_Branch(pos, گرہ+1, have_Unique_قدر, unique_قدر);
            اگر(pos==خالی_اشاریہ) {
                واپس جھوٹ;
            }
            have_Unique_قدر=سچ;
            گرہ=*pos++;
        } ورنہ اگر(گرہ<k_Min_قدر_Lead) {
            // linear-match node
            pos+=گرہ-k_Min_خطی_مطابقت+1;  // Ignore the match units.
            گرہ=*pos++;
        } ورنہ {
            UBool isFinal = static_cast<UBool>(گرہ >> 15);
            int32_t value;
            اگر(isFinal) {
                value=read_قدر(pos, گرہ&0x7fff);
            } ورنہ {
                value=read_گرہ_قدر(pos, گرہ);
            }
            اگر(have_Unique_قدر) {
                اگر(value!=unique_قدر) {
                    واپس جھوٹ;
                }
            } ورنہ {
                unique_قدر=value;
                have_Unique_قدر=سچ;
            }
            اگر(isFinal) {
                واپس سچ;
            }
            pos=skip_گرہ_قدر(pos, گرہ);
            گرہ&=k_گرہ_Type_نقاب;
        }
    }
}

int32_t
UCharsTrie::get_اگلا_UChars(Appendable &out) ثابت {
    ثابت char16_t *pos=pos_;
    اگر(pos==خالی_اشاریہ) {
        واپس 0;
    }
    اگر(بقایا_مطابقت_لمبائی>=0) {
        out.append_رمز_Unit(*pos);  // Next unit of a pending linear-match node.
        واپس 1;
    }
    int32_t گرہ=*pos++;
    اگر(گرہ>=k_Min_قدر_Lead) {
        اگر(گرہ&k_قدر_Is_Final) {
            واپس 0;
        } ورنہ {
            pos=skip_گرہ_قدر(pos, گرہ);
            گرہ&=k_گرہ_Type_نقاب;
        }
    }
    اگر(گرہ<k_Min_خطی_مطابقت) {
        اگر(گرہ==0) {
            گرہ=*pos++;
        }
        out.reserve_Append_استعداد(++گرہ);
        get_اگلا_Branch_UChars(pos, گرہ, out);
        واپس گرہ;
    } ورنہ {
        // First unit of the linear-match node.
        out.append_رمز_Unit(*pos);
        واپس 1;
    }
}

خالی
UCharsTrie::get_اگلا_Branch_UChars(ثابت char16_t *pos, int32_t length, Appendable &out) {
    جب(length>k_Max_Branch_خطی_Sub_گرہ_لمبائی) {
        ++pos;  // ignore the comparison unit
        get_اگلا_Branch_UChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    کرو {
        out.append_رمز_Unit(*pos++);
        pos=skip_قدر(pos);
    } جب(--length>1);
    out.append_رمز_Unit(*pos);
}

U_NAMESPACE_انتہا
