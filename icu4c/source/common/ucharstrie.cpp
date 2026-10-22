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
        إرجاع USTRINGTRIE_NO_MATCH;
    } وإلا {
        int32_t عقدة;
        إرجاع (remainingMatchLength_<0 && (عقدة=*pos)>=k_Min_قيمة_Lead) ?
                قيمة_Result(عقدة) : USTRINGTRIE_NO_قيمة;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    إرجاع cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    إرجاع cp<=0xffff ?
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
    بينما(length>k_Max_Branch_Linear_Sub_عقدة_Length) {
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
            int32_t عقدة=*pos;
            إذا(عقدة&k_قيمة_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_قيمة;
            } وإلا {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                إذا(عقدة<k_Min_Two_Unit_قيمة_Lead) {
                    delta=عقدة;
                } وإلا إذا(عقدة<k_Three_Unit_قيمة_Lead) {
                    delta=((عقدة-k_Min_Two_Unit_قيمة_Lead)<<16)|*pos++;
                } وإلا {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                عقدة=*pos;
                result= عقدة>=k_Min_قيمة_Lead ? قيمة_Result(عقدة) : USTRINGTRIE_NO_قيمة;
            }
            pos_=pos;
            إرجاع result;
        }
        --length;
        pos=skip_قيمة(pos);
    } بينما(length>1);
    إذا(uchar==*pos++) {
        pos_=pos;
        int32_t عقدة=*pos;
        إرجاع عقدة>=k_Min_قيمة_Lead ? قيمة_Result(عقدة) : USTRINGTRIE_NO_قيمة;
    } وإلا {
        stop();
        إرجاع USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(ثابت char16_t *pos, int32_t uchar) {
    int32_t عقدة=*pos++;
    لكل(;;) {
        إذا(عقدة<kMinLinearMatch) {
            إرجاع branchNext(pos, عقدة, uchar);
        } وإلا إذا(عقدة<k_Min_قيمة_Lead) {
            // Match the first of length+1 units.
            int32_t length=عقدة-kMinLinearMatch;  // Actual match length minus 1.
            إذا(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                إرجاع (length<0 && (عقدة=*pos)>=k_Min_قيمة_Lead) ?
                        قيمة_Result(عقدة) : USTRINGTRIE_NO_قيمة;
            } وإلا {
                // No match.
                اكسر;
            }
        } وإلا إذا(عقدة&k_قيمة_Is_Final) {
            // No further matching units.
            اكسر;
        } وإلا {
            // Skip intermediate value.
            pos=skip_عقدة_قيمة(pos, عقدة);
            عقدة&=k_عقدة_Type_Mask;
        }
    }
    stop();
    إرجاع USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    ثابت char16_t *pos=pos_;
    إذا(pos==nullptr) {
        إرجاع USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    إذا(length>=0) {
        // Remaining part of a linear-match node.
        إذا(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t عقدة;
            إرجاع (length<0 && (عقدة=*pos)>=k_Min_قيمة_Lead) ?
                    قيمة_Result(عقدة) : USTRINGTRIE_NO_قيمة;
        } وإلا {
            stop();
            إرجاع USTRINGTRIE_NO_MATCH;
        }
    }
    إرجاع nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    ثابت char16_t *s=ptr;
    إذا(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        إرجاع current();
    }
    ثابت char16_t *pos=pos_;
    إذا(pos==nullptr) {
        إرجاع USTRINGTRIE_NO_MATCH;
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
                    int32_t عقدة;
                    إرجاع (length<0 && (عقدة=*pos)>=k_Min_قيمة_Lead) ?
                            قيمة_Result(عقدة) : USTRINGTRIE_NO_قيمة;
                }
                إذا(length<0) {
                    remainingMatchLength_=length;
                    اكسر;
                }
                إذا(uchar!=*pos) {
                    stop();
                    إرجاع USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } وإلا {
            لكل(;;) {
                إذا(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t عقدة;
                    إرجاع (length<0 && (عقدة=*pos)>=k_Min_قيمة_Lead) ?
                            قيمة_Result(عقدة) : USTRINGTRIE_NO_قيمة;
                }
                uchar=*s++;
                --sLength;
                إذا(length<0) {
                    remainingMatchLength_=length;
                    اكسر;
                }
                إذا(uchar!=*pos) {
                    stop();
                    إرجاع USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t عقدة=*pos++;
        لكل(;;) {
            إذا(عقدة<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, عقدة, uchar);
                إذا(result==USTRINGTRIE_NO_MATCH) {
                    إرجاع USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                إذا(sLength<0) {
                    إذا((uchar=*s++)==0) {
                        إرجاع result;
                    }
                } وإلا {
                    إذا(sLength==0) {
                        إرجاع result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                إذا(result==USTRINGTRIE_FINAL_قيمة) {
                    // No further matching units.
                    stop();
                    إرجاع USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                عقدة=*pos++;
            } وإلا إذا(عقدة<k_Min_قيمة_Lead) {
                // Match length+1 units.
                length=عقدة-kMinLinearMatch;  // Actual match length minus 1.
                إذا(uchar!=*pos) {
                    stop();
                    إرجاع USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                اكسر;
            } وإلا إذا(عقدة&k_قيمة_Is_Final) {
                // No further matching units.
                stop();
                إرجاع USTRINGTRIE_NO_MATCH;
            } وإلا {
                // Skip intermediate value.
                pos=skip_عقدة_قيمة(pos, عقدة);
                عقدة&=k_عقدة_Type_Mask;
            }
        }
    }
}

ثابت char16_t*
UCharsTrie::find_Unique_قيمة_From_Branch(ثابت char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_قيمة,
                                      int32_t& unique_قيمة) {
    بينما(length>k_Max_Branch_Linear_Sub_عقدة_Length) {
        ++pos;  // ignore the comparison unit
        إذا(nullptr==find_Unique_قيمة_From_Branch(jumpByDelta(pos), length>>1, have_Unique_قيمة, unique_قيمة)) {
            إرجاع nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t عقدة=*pos++;
        UBool isFinal = static_cast<UBool>(عقدة >> 15);
        عقدة&=0x7fff;
        int32_t value=read_قيمة(pos, عقدة);
        pos=skip_قيمة(pos, عقدة);
        إذا(isFinal) {
            إذا(have_Unique_قيمة) {
                إذا(value!=unique_قيمة) {
                    إرجاع nullptr;
                }
            } وإلا {
                unique_قيمة=value;
                have_Unique_قيمة=true;
            }
        } وإلا {
            إذا(!find_Unique_قيمة(pos+value, have_Unique_قيمة, unique_قيمة)) {
                إرجاع nullptr;
            }
            have_Unique_قيمة=true;
        }
    } بينما(--length>1);
    إرجاع pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::find_Unique_قيمة(ثابت char16_t *pos, UBool have_Unique_قيمة, int32_t &unique_قيمة) {
    int32_t عقدة=*pos++;
    لكل(;;) {
        إذا(عقدة<kMinLinearMatch) {
            إذا(عقدة==0) {
                عقدة=*pos++;
            }
            pos=find_Unique_قيمة_From_Branch(pos, عقدة+1, have_Unique_قيمة, unique_قيمة);
            إذا(pos==nullptr) {
                إرجاع خطأ;
            }
            have_Unique_قيمة=true;
            عقدة=*pos++;
        } وإلا إذا(عقدة<k_Min_قيمة_Lead) {
            // linear-match node
            pos+=عقدة-kMinLinearMatch+1;  // Ignore the match units.
            عقدة=*pos++;
        } وإلا {
            UBool isFinal = static_cast<UBool>(عقدة >> 15);
            int32_t value;
            إذا(isFinal) {
                value=read_قيمة(pos, عقدة&0x7fff);
            } وإلا {
                value=read_عقدة_قيمة(pos, عقدة);
            }
            إذا(have_Unique_قيمة) {
                إذا(value!=unique_قيمة) {
                    إرجاع خطأ;
                }
            } وإلا {
                unique_قيمة=value;
                have_Unique_قيمة=true;
            }
            إذا(isFinal) {
                إرجاع true;
            }
            pos=skip_عقدة_قيمة(pos, عقدة);
            عقدة&=k_عقدة_Type_Mask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) ثابت {
    ثابت char16_t *pos=pos_;
    إذا(pos==nullptr) {
        إرجاع 0;
    }
    إذا(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        إرجاع 1;
    }
    int32_t عقدة=*pos++;
    إذا(عقدة>=k_Min_قيمة_Lead) {
        إذا(عقدة&k_قيمة_Is_Final) {
            إرجاع 0;
        } وإلا {
            pos=skip_عقدة_قيمة(pos, عقدة);
            عقدة&=k_عقدة_Type_Mask;
        }
    }
    إذا(عقدة<kMinLinearMatch) {
        إذا(عقدة==0) {
            عقدة=*pos++;
        }
        out.reserveAppendCapacity(++عقدة);
        getNextBranchUChars(pos, عقدة, out);
        إرجاع عقدة;
    } وإلا {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        إرجاع 1;
    }
}

فراغ
UCharsTrie::getNextBranchUChars(ثابت char16_t *pos, int32_t length, Appendable &out) {
    بينما(length>k_Max_Branch_Linear_Sub_عقدة_Length) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        out.appendCodeUnit(*pos++);
        pos=skip_قيمة(pos);
    } بينما(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
