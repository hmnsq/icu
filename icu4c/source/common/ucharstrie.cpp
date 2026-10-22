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
        int32_t كلي;
        أرجع (remainingMatchLength_<0 && (كلي=*pos)>=k_Min_درج_Lead) ?
                درج_Result(كلي) : USTRINGTRIE_NO_درج;
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
    طالما(length>k_Max_Branch_Linear_Sub_كلي_Length) {
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
            int32_t كلي=*pos;
            إذا(كلي&k_درج_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_درج;
            } وإلا {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                إذا(كلي<k_Min_Two_Unit_درج_Lead) {
                    delta=كلي;
                } وإلا إذا(كلي<k_Three_Unit_درج_Lead) {
                    delta=((كلي-k_Min_Two_Unit_درج_Lead)<<16)|*pos++;
                } وإلا {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                كلي=*pos;
                result= كلي>=k_Min_درج_Lead ? درج_Result(كلي) : USTRINGTRIE_NO_درج;
            }
            pos_=pos;
            أرجع result;
        }
        --length;
        pos=skip_درج(pos);
    } طالما(length>1);
    إذا(uchar==*pos++) {
        pos_=pos;
        int32_t كلي=*pos;
        أرجع كلي>=k_Min_درج_Lead ? درج_Result(كلي) : USTRINGTRIE_NO_درج;
    } وإلا {
        stop();
        أرجع USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(ثابت char16_t *pos, int32_t uchar) {
    int32_t كلي=*pos++;
    لكل(;;) {
        إذا(كلي<kMinLinearMatch) {
            أرجع branchNext(pos, كلي, uchar);
        } وإلا إذا(كلي<k_Min_درج_Lead) {
            // Match the first of length+1 units.
            int32_t length=كلي-kMinLinearMatch;  // Actual match length minus 1.
            إذا(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                أرجع (length<0 && (كلي=*pos)>=k_Min_درج_Lead) ?
                        درج_Result(كلي) : USTRINGTRIE_NO_درج;
            } وإلا {
                // No match.
                اكسر;
            }
        } وإلا إذا(كلي&k_درج_Is_Final) {
            // No further matching units.
            اكسر;
        } وإلا {
            // Skip intermediate value.
            pos=skip_كلي_درج(pos, كلي);
            كلي&=k_كلي_Type_Mask;
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
            int32_t كلي;
            أرجع (length<0 && (كلي=*pos)>=k_Min_درج_Lead) ?
                    درج_Result(كلي) : USTRINGTRIE_NO_درج;
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
                    int32_t كلي;
                    أرجع (length<0 && (كلي=*pos)>=k_Min_درج_Lead) ?
                            درج_Result(كلي) : USTRINGTRIE_NO_درج;
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
                    int32_t كلي;
                    أرجع (length<0 && (كلي=*pos)>=k_Min_درج_Lead) ?
                            درج_Result(كلي) : USTRINGTRIE_NO_درج;
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
        int32_t كلي=*pos++;
        لكل(;;) {
            إذا(كلي<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, كلي, uchar);
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
                إذا(result==USTRINGTRIE_FINAL_درج) {
                    // No further matching units.
                    stop();
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                كلي=*pos++;
            } وإلا إذا(كلي<k_Min_درج_Lead) {
                // Match length+1 units.
                length=كلي-kMinLinearMatch;  // Actual match length minus 1.
                إذا(uchar!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                اكسر;
            } وإلا إذا(كلي&k_درج_Is_Final) {
                // No further matching units.
                stop();
                أرجع USTRINGTRIE_NO_MATCH;
            } وإلا {
                // Skip intermediate value.
                pos=skip_كلي_درج(pos, كلي);
                كلي&=k_كلي_Type_Mask;
            }
        }
    }
}

ثابت char16_t*
UCharsTrie::find_Unique_درج_From_Branch(ثابت char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_درج,
                                      int32_t& unique_درج) {
    طالما(length>k_Max_Branch_Linear_Sub_كلي_Length) {
        ++pos;  // ignore the comparison unit
        إذا(nullptr==find_Unique_درج_From_Branch(jumpByDelta(pos), length>>1, have_Unique_درج, unique_درج)) {
            أرجع nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t كلي=*pos++;
        UBool isFinal = static_cast<UBool>(كلي >> 15);
        كلي&=0x7fff;
        int32_t value=read_درج(pos, كلي);
        pos=skip_درج(pos, كلي);
        إذا(isFinal) {
            إذا(have_Unique_درج) {
                إذا(value!=unique_درج) {
                    أرجع nullptr;
                }
            } وإلا {
                unique_درج=value;
                have_Unique_درج=true;
            }
        } وإلا {
            إذا(!find_Unique_درج(pos+value, have_Unique_درج, unique_درج)) {
                أرجع nullptr;
            }
            have_Unique_درج=true;
        }
    } طالما(--length>1);
    أرجع pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::find_Unique_درج(ثابت char16_t *pos, UBool have_Unique_درج, int32_t &unique_درج) {
    int32_t كلي=*pos++;
    لكل(;;) {
        إذا(كلي<kMinLinearMatch) {
            إذا(كلي==0) {
                كلي=*pos++;
            }
            pos=find_Unique_درج_From_Branch(pos, كلي+1, have_Unique_درج, unique_درج);
            إذا(pos==nullptr) {
                أرجع خطأ;
            }
            have_Unique_درج=true;
            كلي=*pos++;
        } وإلا إذا(كلي<k_Min_درج_Lead) {
            // linear-match node
            pos+=كلي-kMinLinearMatch+1;  // Ignore the match units.
            كلي=*pos++;
        } وإلا {
            UBool isFinal = static_cast<UBool>(كلي >> 15);
            int32_t value;
            إذا(isFinal) {
                value=read_درج(pos, كلي&0x7fff);
            } وإلا {
                value=read_كلي_درج(pos, كلي);
            }
            إذا(have_Unique_درج) {
                إذا(value!=unique_درج) {
                    أرجع خطأ;
                }
            } وإلا {
                unique_درج=value;
                have_Unique_درج=true;
            }
            إذا(isFinal) {
                أرجع true;
            }
            pos=skip_كلي_درج(pos, كلي);
            كلي&=k_كلي_Type_Mask;
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
    int32_t كلي=*pos++;
    إذا(كلي>=k_Min_درج_Lead) {
        إذا(كلي&k_درج_Is_Final) {
            أرجع 0;
        } وإلا {
            pos=skip_كلي_درج(pos, كلي);
            كلي&=k_كلي_Type_Mask;
        }
    }
    إذا(كلي<kMinLinearMatch) {
        إذا(كلي==0) {
            كلي=*pos++;
        }
        out.reserveAppendCapacity(++كلي);
        getNextBranchUChars(pos, كلي, out);
        أرجع كلي;
    } وإلا {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        أرجع 1;
    }
}

فراغ
UCharsTrie::getNextBranchUChars(ثابت char16_t *pos, int32_t length, Appendable &out) {
    طالما(length>k_Max_Branch_Linear_Sub_كلي_Length) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    افعل {
        out.appendCodeUnit(*pos++);
        pos=skip_درج(pos);
    } طالما(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
