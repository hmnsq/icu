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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/appendable.h"
#अन्तर्भाव "unicode/ucharstrie.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uassert.h"

U_NAMESPACE_BEGIN

UCharsTrie::~UCharsTrie() {
    uprv_free(ownedArray_);
}

UStringTrieResult
UCharsTrie::current() अचर {
    अचर char16_t *pos=pos_;
    यदि(pos==nullptr) {
        फर्काऊ USTRINGTRIE_NO_MATCH;
    } अन्यथा {
        int32_t गाँठो;
        फर्काऊ (remainingMatchLength_<0 && (गाँठो=*pos)>=k_Min_मान_Lead) ?
                मान_Result(गाँठो) : USTRINGTRIE_NO_मान;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    फर्काऊ cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    फर्काऊ cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(अचर char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    यदि(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    जबसम्म(length>k_Max_Branch_Linear_Sub_गाँठो_Length) {
        यदि(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } अन्यथा {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    गर {
        यदि(uchar==*pos++) {
            UStringTrieResult result;
            int32_t गाँठो=*pos;
            यदि(गाँठो&k_मान_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_मान;
            } अन्यथा {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                यदि(गाँठो<k_Min_Two_Unit_मान_Lead) {
                    delta=गाँठो;
                } अन्यथा यदि(गाँठो<k_Three_Unit_मान_Lead) {
                    delta=((गाँठो-k_Min_Two_Unit_मान_Lead)<<16)|*pos++;
                } अन्यथा {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                गाँठो=*pos;
                result= गाँठो>=k_Min_मान_Lead ? मान_Result(गाँठो) : USTRINGTRIE_NO_मान;
            }
            pos_=pos;
            फर्काऊ result;
        }
        --length;
        pos=skip_मान(pos);
    } जबसम्म(length>1);
    यदि(uchar==*pos++) {
        pos_=pos;
        int32_t गाँठो=*pos;
        फर्काऊ गाँठो>=k_Min_मान_Lead ? मान_Result(गाँठो) : USTRINGTRIE_NO_मान;
    } अन्यथा {
        stop();
        फर्काऊ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(अचर char16_t *pos, int32_t uchar) {
    int32_t गाँठो=*pos++;
    लागि(;;) {
        यदि(गाँठो<kMinLinearMatch) {
            फर्काऊ branchNext(pos, गाँठो, uchar);
        } अन्यथा यदि(गाँठो<k_Min_मान_Lead) {
            // Match the first of length+1 units.
            int32_t length=गाँठो-kMinLinearMatch;  // Actual match length minus 1.
            यदि(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                फर्काऊ (length<0 && (गाँठो=*pos)>=k_Min_मान_Lead) ?
                        मान_Result(गाँठो) : USTRINGTRIE_NO_मान;
            } अन्यथा {
                // No match.
                रोक;
            }
        } अन्यथा यदि(गाँठो&k_मान_Is_Final) {
            // No further matching units.
            रोक;
        } अन्यथा {
            // Skip intermediate value.
            pos=skip_गाँठो_मान(pos, गाँठो);
            गाँठो&=k_गाँठो_Type_Mask;
        }
    }
    stop();
    फर्काऊ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    अचर char16_t *pos=pos_;
    यदि(pos==nullptr) {
        फर्काऊ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    यदि(length>=0) {
        // Remaining part of a linear-match node.
        यदि(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t गाँठो;
            फर्काऊ (length<0 && (गाँठो=*pos)>=k_Min_मान_Lead) ?
                    मान_Result(गाँठो) : USTRINGTRIE_NO_मान;
        } अन्यथा {
            stop();
            फर्काऊ USTRINGTRIE_NO_MATCH;
        }
    }
    फर्काऊ nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    अचर char16_t *s=ptr;
    यदि(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        फर्काऊ current();
    }
    अचर char16_t *pos=pos_;
    यदि(pos==nullptr) {
        फर्काऊ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    लागि(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        यदि(sLength<0) {
            लागि(;;) {
                यदि((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t गाँठो;
                    फर्काऊ (length<0 && (गाँठो=*pos)>=k_Min_मान_Lead) ?
                            मान_Result(गाँठो) : USTRINGTRIE_NO_मान;
                }
                यदि(length<0) {
                    remainingMatchLength_=length;
                    रोक;
                }
                यदि(uchar!=*pos) {
                    stop();
                    फर्काऊ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } अन्यथा {
            लागि(;;) {
                यदि(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t गाँठो;
                    फर्काऊ (length<0 && (गाँठो=*pos)>=k_Min_मान_Lead) ?
                            मान_Result(गाँठो) : USTRINGTRIE_NO_मान;
                }
                uchar=*s++;
                --sLength;
                यदि(length<0) {
                    remainingMatchLength_=length;
                    रोक;
                }
                यदि(uchar!=*pos) {
                    stop();
                    फर्काऊ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t गाँठो=*pos++;
        लागि(;;) {
            यदि(गाँठो<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, गाँठो, uchar);
                यदि(result==USTRINGTRIE_NO_MATCH) {
                    फर्काऊ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                यदि(sLength<0) {
                    यदि((uchar=*s++)==0) {
                        फर्काऊ result;
                    }
                } अन्यथा {
                    यदि(sLength==0) {
                        फर्काऊ result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                यदि(result==USTRINGTRIE_FINAL_मान) {
                    // No further matching units.
                    stop();
                    फर्काऊ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                गाँठो=*pos++;
            } अन्यथा यदि(गाँठो<k_Min_मान_Lead) {
                // Match length+1 units.
                length=गाँठो-kMinLinearMatch;  // Actual match length minus 1.
                यदि(uchar!=*pos) {
                    stop();
                    फर्काऊ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                रोक;
            } अन्यथा यदि(गाँठो&k_मान_Is_Final) {
                // No further matching units.
                stop();
                फर्काऊ USTRINGTRIE_NO_MATCH;
            } अन्यथा {
                // Skip intermediate value.
                pos=skip_गाँठो_मान(pos, गाँठो);
                गाँठो&=k_गाँठो_Type_Mask;
            }
        }
    }
}

अचर char16_t*
UCharsTrie::find_Unique_मान_From_Branch(अचर char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_मान,
                                      int32_t& unique_मान) {
    जबसम्म(length>k_Max_Branch_Linear_Sub_गाँठो_Length) {
        ++pos;  // ignore the comparison unit
        यदि(nullptr==find_Unique_मान_From_Branch(jumpByDelta(pos), length>>1, have_Unique_मान, unique_मान)) {
            फर्काऊ nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    गर {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t गाँठो=*pos++;
        UBool isFinal = static_cast<UBool>(गाँठो >> 15);
        गाँठो&=0x7fff;
        int32_t value=read_मान(pos, गाँठो);
        pos=skip_मान(pos, गाँठो);
        यदि(isFinal) {
            यदि(have_Unique_मान) {
                यदि(value!=unique_मान) {
                    फर्काऊ nullptr;
                }
            } अन्यथा {
                unique_मान=value;
                have_Unique_मान=सत्य;
            }
        } अन्यथा {
            यदि(!find_Unique_मान(pos+value, have_Unique_मान, unique_मान)) {
                फर्काऊ nullptr;
            }
            have_Unique_मान=सत्य;
        }
    } जबसम्म(--length>1);
    फर्काऊ pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::find_Unique_मान(अचर char16_t *pos, UBool have_Unique_मान, int32_t &unique_मान) {
    int32_t गाँठो=*pos++;
    लागि(;;) {
        यदि(गाँठो<kMinLinearMatch) {
            यदि(गाँठो==0) {
                गाँठो=*pos++;
            }
            pos=find_Unique_मान_From_Branch(pos, गाँठो+1, have_Unique_मान, unique_मान);
            यदि(pos==nullptr) {
                फर्काऊ असत्य;
            }
            have_Unique_मान=सत्य;
            गाँठो=*pos++;
        } अन्यथा यदि(गाँठो<k_Min_मान_Lead) {
            // linear-match node
            pos+=गाँठो-kMinLinearMatch+1;  // Ignore the match units.
            गाँठो=*pos++;
        } अन्यथा {
            UBool isFinal = static_cast<UBool>(गाँठो >> 15);
            int32_t value;
            यदि(isFinal) {
                value=read_मान(pos, गाँठो&0x7fff);
            } अन्यथा {
                value=read_गाँठो_मान(pos, गाँठो);
            }
            यदि(have_Unique_मान) {
                यदि(value!=unique_मान) {
                    फर्काऊ असत्य;
                }
            } अन्यथा {
                unique_मान=value;
                have_Unique_मान=सत्य;
            }
            यदि(isFinal) {
                फर्काऊ सत्य;
            }
            pos=skip_गाँठो_मान(pos, गाँठो);
            गाँठो&=k_गाँठो_Type_Mask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) अचर {
    अचर char16_t *pos=pos_;
    यदि(pos==nullptr) {
        फर्काऊ 0;
    }
    यदि(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        फर्काऊ 1;
    }
    int32_t गाँठो=*pos++;
    यदि(गाँठो>=k_Min_मान_Lead) {
        यदि(गाँठो&k_मान_Is_Final) {
            फर्काऊ 0;
        } अन्यथा {
            pos=skip_गाँठो_मान(pos, गाँठो);
            गाँठो&=k_गाँठो_Type_Mask;
        }
    }
    यदि(गाँठो<kMinLinearMatch) {
        यदि(गाँठो==0) {
            गाँठो=*pos++;
        }
        out.reserveAppendCapacity(++गाँठो);
        getNextBranchUChars(pos, गाँठो, out);
        फर्काऊ गाँठो;
    } अन्यथा {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        फर्काऊ 1;
    }
}

शून्य
UCharsTrie::getNextBranchUChars(अचर char16_t *pos, int32_t length, Appendable &out) {
    जबसम्म(length>k_Max_Branch_Linear_Sub_गाँठो_Length) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    गर {
        out.appendCodeUnit(*pos++);
        pos=skip_मान(pos);
    } जबसम्म(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
