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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/appendable.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucharstrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

U_NAMESPACE_BEGIN

UCharsTrie::~UCharsTrie() {
    uprv_free(ownedArray_);
}

UStringTrieResult
UCharsTrie::current() ಧ್ರುವ {
    ಧ್ರುವ char16_t *pos=pos_;
    ಯದಿ(pos==nullptr) {
        ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
    } ಅನ್ಯಥಾ {
        int32_t ಗಂಟು;
        ಪ್ರತ್ಯಾಗಮ (remainingMatchLength_<0 && (ಗಂಟು=*pos)>=k_Min_ಮೌಲ್ಯ_Lead) ?
                ಮೌಲ್ಯ_Result(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    ಪ್ರತ್ಯಾಗಮ cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    ಪ್ರತ್ಯಾಗಮ cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(ಧ್ರುವ char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    ಯದಿ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    ಯಾವತ್(length>k_Max_Branch_Linear_Sub_ಗಂಟು_Length) {
        ಯದಿ(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ಅನ್ಯಥಾ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    ಕುರು {
        ಯದಿ(uchar==*pos++) {
            UStringTrieResult result;
            int32_t ಗಂಟು=*pos;
            ಯದಿ(ಗಂಟು&k_ಮೌಲ್ಯ_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_ಮೌಲ್ಯ;
            } ಅನ್ಯಥಾ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                ಯದಿ(ಗಂಟು<k_Min_Two_Unit_ಮೌಲ್ಯ_Lead) {
                    delta=ಗಂಟು;
                } ಅನ್ಯಥಾ ಯದಿ(ಗಂಟು<k_Three_Unit_ಮೌಲ್ಯ_Lead) {
                    delta=((ಗಂಟು-k_Min_Two_Unit_ಮೌಲ್ಯ_Lead)<<16)|*pos++;
                } ಅನ್ಯಥಾ {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                ಗಂಟು=*pos;
                result= ಗಂಟು>=k_Min_ಮೌಲ್ಯ_Lead ? ಮೌಲ್ಯ_Result(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
            }
            pos_=pos;
            ಪ್ರತ್ಯಾಗಮ result;
        }
        --length;
        pos=skip_ಮೌಲ್ಯ(pos);
    } ಯಾವತ್(length>1);
    ಯದಿ(uchar==*pos++) {
        pos_=pos;
        int32_t ಗಂಟು=*pos;
        ಪ್ರತ್ಯಾಗಮ ಗಂಟು>=k_Min_ಮೌಲ್ಯ_Lead ? ಮೌಲ್ಯ_Result(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
    } ಅನ್ಯಥಾ {
        stop();
        ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(ಧ್ರುವ char16_t *pos, int32_t uchar) {
    int32_t ಗಂಟು=*pos++;
    ಪ್ರತಿ(;;) {
        ಯದಿ(ಗಂಟು<kMinLinearMatch) {
            ಪ್ರತ್ಯಾಗಮ branchNext(pos, ಗಂಟು, uchar);
        } ಅನ್ಯಥಾ ಯದಿ(ಗಂಟು<k_Min_ಮೌಲ್ಯ_Lead) {
            // Match the first of length+1 units.
            int32_t length=ಗಂಟು-kMinLinearMatch;  // Actual match length minus 1.
            ಯದಿ(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ಪ್ರತ್ಯಾಗಮ (length<0 && (ಗಂಟು=*pos)>=k_Min_ಮೌಲ್ಯ_Lead) ?
                        ಮೌಲ್ಯ_Result(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
            } ಅನ್ಯಥಾ {
                // No match.
                ಭಙ್ಗ;
            }
        } ಅನ್ಯಥಾ ಯದಿ(ಗಂಟು&k_ಮೌಲ್ಯ_Is_Final) {
            // No further matching units.
            ಭಙ್ಗ;
        } ಅನ್ಯಥಾ {
            // Skip intermediate value.
            pos=skip_ಗಂಟು_ಮೌಲ್ಯ(pos, ಗಂಟು);
            ಗಂಟು&=k_ಗಂಟು_Type_Mask;
        }
    }
    stop();
    ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    ಧ್ರುವ char16_t *pos=pos_;
    ಯದಿ(pos==nullptr) {
        ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ಯದಿ(length>=0) {
        // Remaining part of a linear-match node.
        ಯದಿ(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t ಗಂಟು;
            ಪ್ರತ್ಯಾಗಮ (length<0 && (ಗಂಟು=*pos)>=k_Min_ಮೌಲ್ಯ_Lead) ?
                    ಮೌಲ್ಯ_Result(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
        } ಅನ್ಯಥಾ {
            stop();
            ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
        }
    }
    ಪ್ರತ್ಯಾಗಮ nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    ಧ್ರುವ char16_t *s=ptr;
    ಯದಿ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ಪ್ರತ್ಯಾಗಮ current();
    }
    ಧ್ರುವ char16_t *pos=pos_;
    ಯದಿ(pos==nullptr) {
        ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ಪ್ರತಿ(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        ಯದಿ(sLength<0) {
            ಪ್ರತಿ(;;) {
                ಯದಿ((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t ಗಂಟು;
                    ಪ್ರತ್ಯಾಗಮ (length<0 && (ಗಂಟು=*pos)>=k_Min_ಮೌಲ್ಯ_Lead) ?
                            ಮೌಲ್ಯ_Result(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
                }
                ಯದಿ(length<0) {
                    remainingMatchLength_=length;
                    ಭಙ್ಗ;
                }
                ಯದಿ(uchar!=*pos) {
                    stop();
                    ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ಅನ್ಯಥಾ {
            ಪ್ರತಿ(;;) {
                ಯದಿ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t ಗಂಟು;
                    ಪ್ರತ್ಯಾಗಮ (length<0 && (ಗಂಟು=*pos)>=k_Min_ಮೌಲ್ಯ_Lead) ?
                            ಮೌಲ್ಯ_Result(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
                }
                uchar=*s++;
                --sLength;
                ಯದಿ(length<0) {
                    remainingMatchLength_=length;
                    ಭಙ್ಗ;
                }
                ಯದಿ(uchar!=*pos) {
                    stop();
                    ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t ಗಂಟು=*pos++;
        ಪ್ರತಿ(;;) {
            ಯದಿ(ಗಂಟು<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, ಗಂಟು, uchar);
                ಯದಿ(result==USTRINGTRIE_NO_MATCH) {
                    ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                ಯದಿ(sLength<0) {
                    ಯದಿ((uchar=*s++)==0) {
                        ಪ್ರತ್ಯಾಗಮ result;
                    }
                } ಅನ್ಯಥಾ {
                    ಯದಿ(sLength==0) {
                        ಪ್ರತ್ಯಾಗಮ result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                ಯದಿ(result==USTRINGTRIE_FINAL_ಮೌಲ್ಯ) {
                    // No further matching units.
                    stop();
                    ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                ಗಂಟು=*pos++;
            } ಅನ್ಯಥಾ ಯದಿ(ಗಂಟು<k_Min_ಮೌಲ್ಯ_Lead) {
                // Match length+1 units.
                length=ಗಂಟು-kMinLinearMatch;  // Actual match length minus 1.
                ಯದಿ(uchar!=*pos) {
                    stop();
                    ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                ಭಙ್ಗ;
            } ಅನ್ಯಥಾ ಯದಿ(ಗಂಟು&k_ಮೌಲ್ಯ_Is_Final) {
                // No further matching units.
                stop();
                ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_NO_MATCH;
            } ಅನ್ಯಥಾ {
                // Skip intermediate value.
                pos=skip_ಗಂಟು_ಮೌಲ್ಯ(pos, ಗಂಟು);
                ಗಂಟು&=k_ಗಂಟು_Type_Mask;
            }
        }
    }
}

ಧ್ರುವ char16_t*
UCharsTrie::find_Unique_ಮೌಲ್ಯ_From_Branch(ಧ್ರುವ char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_ಮೌಲ್ಯ,
                                      int32_t& unique_ಮೌಲ್ಯ) {
    ಯಾವತ್(length>k_Max_Branch_Linear_Sub_ಗಂಟು_Length) {
        ++pos;  // ignore the comparison unit
        ಯದಿ(nullptr==find_Unique_ಮೌಲ್ಯ_From_Branch(jumpByDelta(pos), length>>1, have_Unique_ಮೌಲ್ಯ, unique_ಮೌಲ್ಯ)) {
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    ಕುರು {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t ಗಂಟು=*pos++;
        UBool isFinal = static_cast<UBool>(ಗಂಟು >> 15);
        ಗಂಟು&=0x7fff;
        int32_t value=read_ಮೌಲ್ಯ(pos, ಗಂಟು);
        pos=skip_ಮೌಲ್ಯ(pos, ಗಂಟು);
        ಯದಿ(isFinal) {
            ಯದಿ(have_Unique_ಮೌಲ್ಯ) {
                ಯದಿ(value!=unique_ಮೌಲ್ಯ) {
                    ಪ್ರತ್ಯಾಗಮ nullptr;
                }
            } ಅನ್ಯಥಾ {
                unique_ಮೌಲ್ಯ=value;
                have_Unique_ಮೌಲ್ಯ=ಸತ್ಯ;
            }
        } ಅನ್ಯಥಾ {
            ಯದಿ(!find_Unique_ಮೌಲ್ಯ(pos+value, have_Unique_ಮೌಲ್ಯ, unique_ಮೌಲ್ಯ)) {
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
            have_Unique_ಮೌಲ್ಯ=ಸತ್ಯ;
        }
    } ಯಾವತ್(--length>1);
    ಪ್ರತ್ಯಾಗಮ pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::find_Unique_ಮೌಲ್ಯ(ಧ್ರುವ char16_t *pos, UBool have_Unique_ಮೌಲ್ಯ, int32_t &unique_ಮೌಲ್ಯ) {
    int32_t ಗಂಟು=*pos++;
    ಪ್ರತಿ(;;) {
        ಯದಿ(ಗಂಟು<kMinLinearMatch) {
            ಯದಿ(ಗಂಟು==0) {
                ಗಂಟು=*pos++;
            }
            pos=find_Unique_ಮೌಲ್ಯ_From_Branch(pos, ಗಂಟು+1, have_Unique_ಮೌಲ್ಯ, unique_ಮೌಲ್ಯ);
            ಯದಿ(pos==nullptr) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            have_Unique_ಮೌಲ್ಯ=ಸತ್ಯ;
            ಗಂಟು=*pos++;
        } ಅನ್ಯಥಾ ಯದಿ(ಗಂಟು<k_Min_ಮೌಲ್ಯ_Lead) {
            // linear-match node
            pos+=ಗಂಟು-kMinLinearMatch+1;  // Ignore the match units.
            ಗಂಟು=*pos++;
        } ಅನ್ಯಥಾ {
            UBool isFinal = static_cast<UBool>(ಗಂಟು >> 15);
            int32_t value;
            ಯದಿ(isFinal) {
                value=read_ಮೌಲ್ಯ(pos, ಗಂಟು&0x7fff);
            } ಅನ್ಯಥಾ {
                value=read_ಗಂಟು_ಮೌಲ್ಯ(pos, ಗಂಟು);
            }
            ಯದಿ(have_Unique_ಮೌಲ್ಯ) {
                ಯದಿ(value!=unique_ಮೌಲ್ಯ) {
                    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                }
            } ಅನ್ಯಥಾ {
                unique_ಮೌಲ್ಯ=value;
                have_Unique_ಮೌಲ್ಯ=ಸತ್ಯ;
            }
            ಯದಿ(isFinal) {
                ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
            }
            pos=skip_ಗಂಟು_ಮೌಲ್ಯ(pos, ಗಂಟು);
            ಗಂಟು&=k_ಗಂಟು_Type_Mask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) ಧ್ರುವ {
    ಧ್ರುವ char16_t *pos=pos_;
    ಯದಿ(pos==nullptr) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        ಪ್ರತ್ಯಾಗಮ 1;
    }
    int32_t ಗಂಟು=*pos++;
    ಯದಿ(ಗಂಟು>=k_Min_ಮೌಲ್ಯ_Lead) {
        ಯದಿ(ಗಂಟು&k_ಮೌಲ್ಯ_Is_Final) {
            ಪ್ರತ್ಯಾಗಮ 0;
        } ಅನ್ಯಥಾ {
            pos=skip_ಗಂಟು_ಮೌಲ್ಯ(pos, ಗಂಟು);
            ಗಂಟು&=k_ಗಂಟು_Type_Mask;
        }
    }
    ಯದಿ(ಗಂಟು<kMinLinearMatch) {
        ಯದಿ(ಗಂಟು==0) {
            ಗಂಟು=*pos++;
        }
        out.reserveAppendCapacity(++ಗಂಟು);
        getNextBranchUChars(pos, ಗಂಟು, out);
        ಪ್ರತ್ಯಾಗಮ ಗಂಟು;
    } ಅನ್ಯಥಾ {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        ಪ್ರತ್ಯಾಗಮ 1;
    }
}

ಶೂನ್ಯ
UCharsTrie::getNextBranchUChars(ಧ್ರುವ char16_t *pos, int32_t length, Appendable &out) {
    ಯಾವತ್(length>k_Max_Branch_Linear_Sub_ಗಂಟು_Length) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    ಕುರು {
        out.appendCodeUnit(*pos++);
        pos=skip_ಮೌಲ್ಯ(pos);
    } ಯಾವತ್(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
