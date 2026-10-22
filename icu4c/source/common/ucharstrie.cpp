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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/appendable.h"
#அந்தர்பாவ "unicode/ucharstrie.h"
#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "uassert.h"

U_NAMESPACE_BEGIN

UCharsTrie::~UCharsTrie() {
    uprv_free(ownedArray_);
}

UStringTrieResult
UCharsTrie::current() மாறிலி {
    மாறிலி char16_t *pos=pos_;
    எனில்(pos==nullptr) {
        திரும்பு USTRINGTRIE_NO_MATCH;
    } இல்லையெனில் {
        int32_t முனை;
        திரும்பு (remainingMatchLength_<0 && (முனை=*pos)>=k_Min_மதிப்பு_Lead) ?
                மதிப்பு_Result(முனை) : USTRINGTRIE_NO_மதிப்பு;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    திரும்பு cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    திரும்பு cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(மாறிலி char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    எனில்(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    போது(length>k_Max_Branch_Linear_Sub_முனை_Length) {
        எனில்(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } இல்லையெனில் {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    செய் {
        எனில்(uchar==*pos++) {
            UStringTrieResult result;
            int32_t முனை=*pos;
            எனில்(முனை&k_மதிப்பு_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_மதிப்பு;
            } இல்லையெனில் {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                எனில்(முனை<k_Min_Two_Unit_மதிப்பு_Lead) {
                    delta=முனை;
                } இல்லையெனில் எனில்(முனை<k_Three_Unit_மதிப்பு_Lead) {
                    delta=((முனை-k_Min_Two_Unit_மதிப்பு_Lead)<<16)|*pos++;
                } இல்லையெனில் {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                முனை=*pos;
                result= முனை>=k_Min_மதிப்பு_Lead ? மதிப்பு_Result(முனை) : USTRINGTRIE_NO_மதிப்பு;
            }
            pos_=pos;
            திரும்பு result;
        }
        --length;
        pos=skip_மதிப்பு(pos);
    } போது(length>1);
    எனில்(uchar==*pos++) {
        pos_=pos;
        int32_t முனை=*pos;
        திரும்பு முனை>=k_Min_மதிப்பு_Lead ? மதிப்பு_Result(முனை) : USTRINGTRIE_NO_மதிப்பு;
    } இல்லையெனில் {
        stop();
        திரும்பு USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(மாறிலி char16_t *pos, int32_t uchar) {
    int32_t முனை=*pos++;
    க்கு(;;) {
        எனில்(முனை<kMinLinearMatch) {
            திரும்பு branchNext(pos, முனை, uchar);
        } இல்லையெனில் எனில்(முனை<k_Min_மதிப்பு_Lead) {
            // Match the first of length+1 units.
            int32_t length=முனை-kMinLinearMatch;  // Actual match length minus 1.
            எனில்(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                திரும்பு (length<0 && (முனை=*pos)>=k_Min_மதிப்பு_Lead) ?
                        மதிப்பு_Result(முனை) : USTRINGTRIE_NO_மதிப்பு;
            } இல்லையெனில் {
                // No match.
                நிறுத்து;
            }
        } இல்லையெனில் எனில்(முனை&k_மதிப்பு_Is_Final) {
            // No further matching units.
            நிறுத்து;
        } இல்லையெனில் {
            // Skip intermediate value.
            pos=skip_முனை_மதிப்பு(pos, முனை);
            முனை&=k_முனை_Type_Mask;
        }
    }
    stop();
    திரும்பு USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    மாறிலி char16_t *pos=pos_;
    எனில்(pos==nullptr) {
        திரும்பு USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    எனில்(length>=0) {
        // Remaining part of a linear-match node.
        எனில்(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t முனை;
            திரும்பு (length<0 && (முனை=*pos)>=k_Min_மதிப்பு_Lead) ?
                    மதிப்பு_Result(முனை) : USTRINGTRIE_NO_மதிப்பு;
        } இல்லையெனில் {
            stop();
            திரும்பு USTRINGTRIE_NO_MATCH;
        }
    }
    திரும்பு nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    மாறிலி char16_t *s=ptr;
    எனில்(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        திரும்பு current();
    }
    மாறிலி char16_t *pos=pos_;
    எனில்(pos==nullptr) {
        திரும்பு USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    க்கு(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        எனில்(sLength<0) {
            க்கு(;;) {
                எனில்((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t முனை;
                    திரும்பு (length<0 && (முனை=*pos)>=k_Min_மதிப்பு_Lead) ?
                            மதிப்பு_Result(முனை) : USTRINGTRIE_NO_மதிப்பு;
                }
                எனில்(length<0) {
                    remainingMatchLength_=length;
                    நிறுத்து;
                }
                எனில்(uchar!=*pos) {
                    stop();
                    திரும்பு USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } இல்லையெனில் {
            க்கு(;;) {
                எனில்(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t முனை;
                    திரும்பு (length<0 && (முனை=*pos)>=k_Min_மதிப்பு_Lead) ?
                            மதிப்பு_Result(முனை) : USTRINGTRIE_NO_மதிப்பு;
                }
                uchar=*s++;
                --sLength;
                எனில்(length<0) {
                    remainingMatchLength_=length;
                    நிறுத்து;
                }
                எனில்(uchar!=*pos) {
                    stop();
                    திரும்பு USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t முனை=*pos++;
        க்கு(;;) {
            எனில்(முனை<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, முனை, uchar);
                எனில்(result==USTRINGTRIE_NO_MATCH) {
                    திரும்பு USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                எனில்(sLength<0) {
                    எனில்((uchar=*s++)==0) {
                        திரும்பு result;
                    }
                } இல்லையெனில் {
                    எனில்(sLength==0) {
                        திரும்பு result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                எனில்(result==USTRINGTRIE_FINAL_மதிப்பு) {
                    // No further matching units.
                    stop();
                    திரும்பு USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                முனை=*pos++;
            } இல்லையெனில் எனில்(முனை<k_Min_மதிப்பு_Lead) {
                // Match length+1 units.
                length=முனை-kMinLinearMatch;  // Actual match length minus 1.
                எனில்(uchar!=*pos) {
                    stop();
                    திரும்பு USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                நிறுத்து;
            } இல்லையெனில் எனில்(முனை&k_மதிப்பு_Is_Final) {
                // No further matching units.
                stop();
                திரும்பு USTRINGTRIE_NO_MATCH;
            } இல்லையெனில் {
                // Skip intermediate value.
                pos=skip_முனை_மதிப்பு(pos, முனை);
                முனை&=k_முனை_Type_Mask;
            }
        }
    }
}

மாறிலி char16_t*
UCharsTrie::find_Unique_மதிப்பு_From_Branch(மாறிலி char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_மதிப்பு,
                                      int32_t& unique_மதிப்பு) {
    போது(length>k_Max_Branch_Linear_Sub_முனை_Length) {
        ++pos;  // ignore the comparison unit
        எனில்(nullptr==find_Unique_மதிப்பு_From_Branch(jumpByDelta(pos), length>>1, have_Unique_மதிப்பு, unique_மதிப்பு)) {
            திரும்பு nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    செய் {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t முனை=*pos++;
        UBool isFinal = static_cast<UBool>(முனை >> 15);
        முனை&=0x7fff;
        int32_t value=read_மதிப்பு(pos, முனை);
        pos=skip_மதிப்பு(pos, முனை);
        எனில்(isFinal) {
            எனில்(have_Unique_மதிப்பு) {
                எனில்(value!=unique_மதிப்பு) {
                    திரும்பு nullptr;
                }
            } இல்லையெனில் {
                unique_மதிப்பு=value;
                have_Unique_மதிப்பு=உண்மை;
            }
        } இல்லையெனில் {
            எனில்(!find_Unique_மதிப்பு(pos+value, have_Unique_மதிப்பு, unique_மதிப்பு)) {
                திரும்பு nullptr;
            }
            have_Unique_மதிப்பு=உண்மை;
        }
    } போது(--length>1);
    திரும்பு pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::find_Unique_மதிப்பு(மாறிலி char16_t *pos, UBool have_Unique_மதிப்பு, int32_t &unique_மதிப்பு) {
    int32_t முனை=*pos++;
    க்கு(;;) {
        எனில்(முனை<kMinLinearMatch) {
            எனில்(முனை==0) {
                முனை=*pos++;
            }
            pos=find_Unique_மதிப்பு_From_Branch(pos, முனை+1, have_Unique_மதிப்பு, unique_மதிப்பு);
            எனில்(pos==nullptr) {
                திரும்பு பொய்;
            }
            have_Unique_மதிப்பு=உண்மை;
            முனை=*pos++;
        } இல்லையெனில் எனில்(முனை<k_Min_மதிப்பு_Lead) {
            // linear-match node
            pos+=முனை-kMinLinearMatch+1;  // Ignore the match units.
            முனை=*pos++;
        } இல்லையெனில் {
            UBool isFinal = static_cast<UBool>(முனை >> 15);
            int32_t value;
            எனில்(isFinal) {
                value=read_மதிப்பு(pos, முனை&0x7fff);
            } இல்லையெனில் {
                value=read_முனை_மதிப்பு(pos, முனை);
            }
            எனில்(have_Unique_மதிப்பு) {
                எனில்(value!=unique_மதிப்பு) {
                    திரும்பு பொய்;
                }
            } இல்லையெனில் {
                unique_மதிப்பு=value;
                have_Unique_மதிப்பு=உண்மை;
            }
            எனில்(isFinal) {
                திரும்பு உண்மை;
            }
            pos=skip_முனை_மதிப்பு(pos, முனை);
            முனை&=k_முனை_Type_Mask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) மாறிலி {
    மாறிலி char16_t *pos=pos_;
    எனில்(pos==nullptr) {
        திரும்பு 0;
    }
    எனில்(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        திரும்பு 1;
    }
    int32_t முனை=*pos++;
    எனில்(முனை>=k_Min_மதிப்பு_Lead) {
        எனில்(முனை&k_மதிப்பு_Is_Final) {
            திரும்பு 0;
        } இல்லையெனில் {
            pos=skip_முனை_மதிப்பு(pos, முனை);
            முனை&=k_முனை_Type_Mask;
        }
    }
    எனில்(முனை<kMinLinearMatch) {
        எனில்(முனை==0) {
            முனை=*pos++;
        }
        out.reserveAppendCapacity(++முனை);
        getNextBranchUChars(pos, முனை, out);
        திரும்பு முனை;
    } இல்லையெனில் {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        திரும்பு 1;
    }
}

வெறுமை
UCharsTrie::getNextBranchUChars(மாறிலி char16_t *pos, int32_t length, Appendable &out) {
    போது(length>k_Max_Branch_Linear_Sub_முனை_Length) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    செய் {
        out.appendCodeUnit(*pos++);
        pos=skip_மதிப்பு(pos);
    } போது(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
