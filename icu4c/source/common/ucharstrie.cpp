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

UStringTrieResult
UCharsTrie::current() ثابت {
    ثابت char16_t *pos=pos_;
    اگر(pos==nullptr) {
        بازگشت USTRINGTRIE_NO_MATCH;
    } وگرنه {
        int32_t گره;
        بازگشت (remainingMatchLength_<0 && (گره=*pos)>=k_Min_مقدار_Lead) ?
                مقدار_Result(گره) : USTRINGTRIE_NO_مقدار;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    بازگشت cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    بازگشت cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(ثابت char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    اگر(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    وقتی(length>k_Max_Branch_Linear_Sub_گره_Length) {
        اگر(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } وگرنه {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    انجامبده {
        اگر(uchar==*pos++) {
            UStringTrieResult result;
            int32_t گره=*pos;
            اگر(گره&k_مقدار_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_مقدار;
            } وگرنه {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                اگر(گره<k_Min_Two_Unit_مقدار_Lead) {
                    delta=گره;
                } وگرنه اگر(گره<k_Three_Unit_مقدار_Lead) {
                    delta=((گره-k_Min_Two_Unit_مقدار_Lead)<<16)|*pos++;
                } وگرنه {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                گره=*pos;
                result= گره>=k_Min_مقدار_Lead ? مقدار_Result(گره) : USTRINGTRIE_NO_مقدار;
            }
            pos_=pos;
            بازگشت result;
        }
        --length;
        pos=skip_مقدار(pos);
    } وقتی(length>1);
    اگر(uchar==*pos++) {
        pos_=pos;
        int32_t گره=*pos;
        بازگشت گره>=k_Min_مقدار_Lead ? مقدار_Result(گره) : USTRINGTRIE_NO_مقدار;
    } وگرنه {
        stop();
        بازگشت USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(ثابت char16_t *pos, int32_t uchar) {
    int32_t گره=*pos++;
    برای(;;) {
        اگر(گره<kMinLinearMatch) {
            بازگشت branchNext(pos, گره, uchar);
        } وگرنه اگر(گره<k_Min_مقدار_Lead) {
            // Match the first of length+1 units.
            int32_t length=گره-kMinLinearMatch;  // Actual match length minus 1.
            اگر(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                بازگشت (length<0 && (گره=*pos)>=k_Min_مقدار_Lead) ?
                        مقدار_Result(گره) : USTRINGTRIE_NO_مقدار;
            } وگرنه {
                // No match.
                بشکن;
            }
        } وگرنه اگر(گره&k_مقدار_Is_Final) {
            // No further matching units.
            بشکن;
        } وگرنه {
            // Skip intermediate value.
            pos=skip_گره_مقدار(pos, گره);
            گره&=k_گره_Type_Mask;
        }
    }
    stop();
    بازگشت USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    ثابت char16_t *pos=pos_;
    اگر(pos==nullptr) {
        بازگشت USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    اگر(length>=0) {
        // Remaining part of a linear-match node.
        اگر(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t گره;
            بازگشت (length<0 && (گره=*pos)>=k_Min_مقدار_Lead) ?
                    مقدار_Result(گره) : USTRINGTRIE_NO_مقدار;
        } وگرنه {
            stop();
            بازگشت USTRINGTRIE_NO_MATCH;
        }
    }
    بازگشت nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    ثابت char16_t *s=ptr;
    اگر(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        بازگشت current();
    }
    ثابت char16_t *pos=pos_;
    اگر(pos==nullptr) {
        بازگشت USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    برای(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        اگر(sLength<0) {
            برای(;;) {
                اگر((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t گره;
                    بازگشت (length<0 && (گره=*pos)>=k_Min_مقدار_Lead) ?
                            مقدار_Result(گره) : USTRINGTRIE_NO_مقدار;
                }
                اگر(length<0) {
                    remainingMatchLength_=length;
                    بشکن;
                }
                اگر(uchar!=*pos) {
                    stop();
                    بازگشت USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } وگرنه {
            برای(;;) {
                اگر(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t گره;
                    بازگشت (length<0 && (گره=*pos)>=k_Min_مقدار_Lead) ?
                            مقدار_Result(گره) : USTRINGTRIE_NO_مقدار;
                }
                uchar=*s++;
                --sLength;
                اگر(length<0) {
                    remainingMatchLength_=length;
                    بشکن;
                }
                اگر(uchar!=*pos) {
                    stop();
                    بازگشت USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t گره=*pos++;
        برای(;;) {
            اگر(گره<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, گره, uchar);
                اگر(result==USTRINGTRIE_NO_MATCH) {
                    بازگشت USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                اگر(sLength<0) {
                    اگر((uchar=*s++)==0) {
                        بازگشت result;
                    }
                } وگرنه {
                    اگر(sLength==0) {
                        بازگشت result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                اگر(result==USTRINGTRIE_FINAL_مقدار) {
                    // No further matching units.
                    stop();
                    بازگشت USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                گره=*pos++;
            } وگرنه اگر(گره<k_Min_مقدار_Lead) {
                // Match length+1 units.
                length=گره-kMinLinearMatch;  // Actual match length minus 1.
                اگر(uchar!=*pos) {
                    stop();
                    بازگشت USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                بشکن;
            } وگرنه اگر(گره&k_مقدار_Is_Final) {
                // No further matching units.
                stop();
                بازگشت USTRINGTRIE_NO_MATCH;
            } وگرنه {
                // Skip intermediate value.
                pos=skip_گره_مقدار(pos, گره);
                گره&=k_گره_Type_Mask;
            }
        }
    }
}

ثابت char16_t*
UCharsTrie::find_Unique_مقدار_From_Branch(ثابت char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_مقدار,
                                      int32_t& unique_مقدار) {
    وقتی(length>k_Max_Branch_Linear_Sub_گره_Length) {
        ++pos;  // ignore the comparison unit
        اگر(nullptr==find_Unique_مقدار_From_Branch(jumpByDelta(pos), length>>1, have_Unique_مقدار, unique_مقدار)) {
            بازگشت nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    انجامبده {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t گره=*pos++;
        UBool isFinal = static_cast<UBool>(گره >> 15);
        گره&=0x7fff;
        int32_t value=read_مقدار(pos, گره);
        pos=skip_مقدار(pos, گره);
        اگر(isFinal) {
            اگر(have_Unique_مقدار) {
                اگر(value!=unique_مقدار) {
                    بازگشت nullptr;
                }
            } وگرنه {
                unique_مقدار=value;
                have_Unique_مقدار=درست;
            }
        } وگرنه {
            اگر(!find_Unique_مقدار(pos+value, have_Unique_مقدار, unique_مقدار)) {
                بازگشت nullptr;
            }
            have_Unique_مقدار=درست;
        }
    } وقتی(--length>1);
    بازگشت pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::find_Unique_مقدار(ثابت char16_t *pos, UBool have_Unique_مقدار, int32_t &unique_مقدار) {
    int32_t گره=*pos++;
    برای(;;) {
        اگر(گره<kMinLinearMatch) {
            اگر(گره==0) {
                گره=*pos++;
            }
            pos=find_Unique_مقدار_From_Branch(pos, گره+1, have_Unique_مقدار, unique_مقدار);
            اگر(pos==nullptr) {
                بازگشت نادرست;
            }
            have_Unique_مقدار=درست;
            گره=*pos++;
        } وگرنه اگر(گره<k_Min_مقدار_Lead) {
            // linear-match node
            pos+=گره-kMinLinearMatch+1;  // Ignore the match units.
            گره=*pos++;
        } وگرنه {
            UBool isFinal = static_cast<UBool>(گره >> 15);
            int32_t value;
            اگر(isFinal) {
                value=read_مقدار(pos, گره&0x7fff);
            } وگرنه {
                value=read_گره_مقدار(pos, گره);
            }
            اگر(have_Unique_مقدار) {
                اگر(value!=unique_مقدار) {
                    بازگشت نادرست;
                }
            } وگرنه {
                unique_مقدار=value;
                have_Unique_مقدار=درست;
            }
            اگر(isFinal) {
                بازگشت درست;
            }
            pos=skip_گره_مقدار(pos, گره);
            گره&=k_گره_Type_Mask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) ثابت {
    ثابت char16_t *pos=pos_;
    اگر(pos==nullptr) {
        بازگشت 0;
    }
    اگر(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        بازگشت 1;
    }
    int32_t گره=*pos++;
    اگر(گره>=k_Min_مقدار_Lead) {
        اگر(گره&k_مقدار_Is_Final) {
            بازگشت 0;
        } وگرنه {
            pos=skip_گره_مقدار(pos, گره);
            گره&=k_گره_Type_Mask;
        }
    }
    اگر(گره<kMinLinearMatch) {
        اگر(گره==0) {
            گره=*pos++;
        }
        out.reserveAppendCapacity(++گره);
        getNextBranchUChars(pos, گره, out);
        بازگشت گره;
    } وگرنه {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        بازگشت 1;
    }
}

تهی
UCharsTrie::getNextBranchUChars(ثابت char16_t *pos, int32_t length, Appendable &out) {
    وقتی(length>k_Max_Branch_Linear_Sub_گره_Length) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    انجامبده {
        out.appendCodeUnit(*pos++);
        pos=skip_مقدار(pos);
    } وقتی(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
