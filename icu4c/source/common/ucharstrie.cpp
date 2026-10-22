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

UStringTrieResult
UCharsTrie::current() ثابت {
    ثابت char16_t *pos=pos_;
    إذا(pos==nullptr) {
        أرجع USTRINGTRIE_NO_MATCH;
    } وإلا {
        int32_t تۈگۈن;
        أرجع (remainingMatchLength_<0 && (تۈگۈن=*pos)>=k_Min_قىممەت_Lead) ?
                قىممەت_Result(تۈگۈن) : USTRINGTRIE_NO_قىممەت;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    أرجع cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    أرجع cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(ثابت char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    إذا(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    طالما(length>k_Max_Branch_Linear_Sub_تۈگۈن_Length) {
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
            UStringTrieResult result;
            int32_t تۈگۈن=*pos;
            إذا(تۈگۈن&k_قىممەت_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_قىممەت;
            } وإلا {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                إذا(تۈگۈن<k_Min_Two_Unit_قىممەت_Lead) {
                    delta=تۈگۈن;
                } وإلا إذا(تۈگۈن<k_Three_Unit_قىممەت_Lead) {
                    delta=((تۈگۈن-k_Min_Two_Unit_قىممەت_Lead)<<16)|*pos++;
                } وإلا {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                تۈگۈن=*pos;
                result= تۈگۈن>=k_Min_قىممەت_Lead ? قىممەت_Result(تۈگۈن) : USTRINGTRIE_NO_قىممەت;
            }
            pos_=pos;
            أرجع result;
        }
        --length;
        pos=skip_قىممەت(pos);
    } طالما(length>1);
    إذا(uchar==*pos++) {
        pos_=pos;
        int32_t تۈگۈن=*pos;
        أرجع تۈگۈن>=k_Min_قىممەت_Lead ? قىممەت_Result(تۈگۈن) : USTRINGTRIE_NO_قىممەت;
    } وإلا {
        stop();
        أرجع USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(ثابت char16_t *pos, int32_t uchar) {
    int32_t تۈگۈن=*pos++;
    لكل(;;) {
        إذا(تۈگۈن<kMinLinearMatch) {
            أرجع branchNext(pos, تۈگۈن, uchar);
        } وإلا إذا(تۈگۈن<k_Min_قىممەت_Lead) {
            // Match the first of length+1 units.
            int32_t length=تۈگۈن-kMinLinearMatch;  // Actual match length minus 1.
            إذا(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                أرجع (length<0 && (تۈگۈن=*pos)>=k_Min_قىممەت_Lead) ?
                        قىممەت_Result(تۈگۈن) : USTRINGTRIE_NO_قىممەت;
            } وإلا {
                // No match.
                اكسر;
            }
        } وإلا إذا(تۈگۈن&k_قىممەت_Is_Final) {
            // No further matching units.
            اكسر;
        } وإلا {
            // Skip intermediate value.
            pos=skip_تۈگۈن_قىممەت(pos, تۈگۈن);
            تۈگۈن&=k_تۈگۈن_Type_Mask;
        }
    }
    stop();
    أرجع USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    ثابت char16_t *pos=pos_;
    إذا(pos==nullptr) {
        أرجع USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    إذا(length>=0) {
        // Remaining part of a linear-match node.
        إذا(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t تۈگۈن;
            أرجع (length<0 && (تۈگۈن=*pos)>=k_Min_قىممەت_Lead) ?
                    قىممەت_Result(تۈگۈن) : USTRINGTRIE_NO_قىممەت;
        } وإلا {
            stop();
            أرجع USTRINGTRIE_NO_MATCH;
        }
    }
    أرجع nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    ثابت char16_t *s=ptr;
    إذا(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        أرجع current();
    }
    ثابت char16_t *pos=pos_;
    إذا(pos==nullptr) {
        أرجع USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    لكل(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        إذا(sLength<0) {
            لكل(;;) {
                إذا((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t تۈگۈن;
                    أرجع (length<0 && (تۈگۈن=*pos)>=k_Min_قىممەت_Lead) ?
                            قىممەت_Result(تۈگۈن) : USTRINGTRIE_NO_قىممەت;
                }
                إذا(length<0) {
                    remainingMatchLength_=length;
                    اكسر;
                }
                إذا(uchar!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } وإلا {
            لكل(;;) {
                إذا(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t تۈگۈن;
                    أرجع (length<0 && (تۈگۈن=*pos)>=k_Min_قىممەت_Lead) ?
                            قىممەت_Result(تۈگۈن) : USTRINGTRIE_NO_قىممەت;
                }
                uchar=*s++;
                --sLength;
                إذا(length<0) {
                    remainingMatchLength_=length;
                    اكسر;
                }
                إذا(uchar!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t تۈگۈن=*pos++;
        لكل(;;) {
            إذا(تۈگۈن<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, تۈگۈن, uchar);
                إذا(result==USTRINGTRIE_NO_MATCH) {
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                إذا(sLength<0) {
                    إذا((uchar=*s++)==0) {
                        أرجع result;
                    }
                } وإلا {
                    إذا(sLength==0) {
                        أرجع result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                إذا(result==USTRINGTRIE_FINAL_قىممەت) {
                    // No further matching units.
                    stop();
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                تۈگۈن=*pos++;
            } وإلا إذا(تۈگۈن<k_Min_قىممەت_Lead) {
                // Match length+1 units.
                length=تۈگۈن-kMinLinearMatch;  // Actual match length minus 1.
                إذا(uchar!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                اكسر;
            } وإلا إذا(تۈگۈن&k_قىممەت_Is_Final) {
                // No further matching units.
                stop();
                أرجع USTRINGTRIE_NO_MATCH;
            } وإلا {
                // Skip intermediate value.
                pos=skip_تۈگۈن_قىممەت(pos, تۈگۈن);
                تۈگۈن&=k_تۈگۈن_Type_Mask;
            }
        }
    }
}

ثابت char16_t*
UCharsTrie::find_Unique_قىممەت_From_Branch(ثابت char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_قىممەت,
                                      int32_t& unique_قىممەت) {
    طالما(length>k_Max_Branch_Linear_Sub_تۈگۈن_Length) {
        ++pos;  // ignore the comparison unit
        إذا(nullptr==find_Unique_قىممەت_From_Branch(jumpByDelta(pos), length>>1, have_Unique_قىممەت, unique_قىممەت)) {
            أرجع nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t تۈگۈن=*pos++;
        UBool isFinal = static_cast<UBool>(تۈگۈن >> 15);
        تۈگۈن&=0x7fff;
        int32_t value=read_قىممەت(pos, تۈگۈن);
        pos=skip_قىممەت(pos, تۈگۈن);
        إذا(isFinal) {
            إذا(have_Unique_قىممەت) {
                إذا(value!=unique_قىممەت) {
                    أرجع nullptr;
                }
            } وإلا {
                unique_قىممەت=value;
                have_Unique_قىممەت=true;
            }
        } وإلا {
            إذا(!find_Unique_قىممەت(pos+value, have_Unique_قىممەت, unique_قىممەت)) {
                أرجع nullptr;
            }
            have_Unique_قىممەت=true;
        }
    } طالما(--length>1);
    أرجع pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::find_Unique_قىممەت(ثابت char16_t *pos, UBool have_Unique_قىممەت, int32_t &unique_قىممەت) {
    int32_t تۈگۈن=*pos++;
    لكل(;;) {
        إذا(تۈگۈن<kMinLinearMatch) {
            إذا(تۈگۈن==0) {
                تۈگۈن=*pos++;
            }
            pos=find_Unique_قىممەت_From_Branch(pos, تۈگۈن+1, have_Unique_قىممەت, unique_قىممەت);
            إذا(pos==nullptr) {
                أرجع خطأ;
            }
            have_Unique_قىممەت=true;
            تۈگۈن=*pos++;
        } وإلا إذا(تۈگۈن<k_Min_قىممەت_Lead) {
            // linear-match node
            pos+=تۈگۈن-kMinLinearMatch+1;  // Ignore the match units.
            تۈگۈن=*pos++;
        } وإلا {
            UBool isFinal = static_cast<UBool>(تۈگۈن >> 15);
            int32_t value;
            إذا(isFinal) {
                value=read_قىممەت(pos, تۈگۈن&0x7fff);
            } وإلا {
                value=read_تۈگۈن_قىممەت(pos, تۈگۈن);
            }
            إذا(have_Unique_قىممەت) {
                إذا(value!=unique_قىممەت) {
                    أرجع خطأ;
                }
            } وإلا {
                unique_قىممەت=value;
                have_Unique_قىممەت=true;
            }
            إذا(isFinal) {
                أرجع true;
            }
            pos=skip_تۈگۈن_قىممەت(pos, تۈگۈن);
            تۈگۈن&=k_تۈگۈن_Type_Mask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) ثابت {
    ثابت char16_t *pos=pos_;
    إذا(pos==nullptr) {
        أرجع 0;
    }
    إذا(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        أرجع 1;
    }
    int32_t تۈگۈن=*pos++;
    إذا(تۈگۈن>=k_Min_قىممەت_Lead) {
        إذا(تۈگۈن&k_قىممەت_Is_Final) {
            أرجع 0;
        } وإلا {
            pos=skip_تۈگۈن_قىممەت(pos, تۈگۈن);
            تۈگۈن&=k_تۈگۈن_Type_Mask;
        }
    }
    إذا(تۈگۈن<kMinLinearMatch) {
        إذا(تۈگۈن==0) {
            تۈگۈن=*pos++;
        }
        out.reserveAppendCapacity(++تۈگۈن);
        getNextBranchUChars(pos, تۈگۈن, out);
        أرجع تۈگۈن;
    } وإلا {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        أرجع 1;
    }
}

فراغ
UCharsTrie::getNextBranchUChars(ثابت char16_t *pos, int32_t length, Appendable &out) {
    طالما(length>k_Max_Branch_Linear_Sub_تۈگۈن_Length) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        out.appendCodeUnit(*pos++);
        pos=skip_قىممەت(pos);
    } طالما(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
