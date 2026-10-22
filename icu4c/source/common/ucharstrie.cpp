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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/appendable.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucharstrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"

U_NAMESPACE_BEGIN

UCharsTrie::~UCharsTrie() {
    uprv_free(ownedArray_);
}

UStringTrieResult
UCharsTrie::current() ଅଚଳ {
    ଅଚଳ char16_t *pos=pos_;
    ଯଦି(pos==nullptr) {
        ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
    } ଅନ୍ୟଥା {
        int32_t ଗଣ୍ଠି;
        ଫେରନ୍ତୁ (remainingMatchLength_<0 && (ଗଣ୍ଠି=*pos)>=k_Min_ମୂଲ୍ୟ_Lead) ?
                ମୂଲ୍ୟ_Result(ଗଣ୍ଠି) : USTRINGTRIE_NO_ମୂଲ୍ୟ;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    ଫେରନ୍ତୁ cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    ଫେରନ୍ତୁ cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(ଅଚଳ char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    ଯଦି(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    ଯେତେବେଳେ(length>k_Max_Branch_Linear_Sub_ଗଣ୍ଠି_Length) {
        ଯଦି(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ଅନ୍ୟଥା {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    କର {
        ଯଦି(uchar==*pos++) {
            UStringTrieResult result;
            int32_t ଗଣ୍ଠି=*pos;
            ଯଦି(ଗଣ୍ଠି&k_ମୂଲ୍ୟ_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_ମୂଲ୍ୟ;
            } ଅନ୍ୟଥା {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                ଯଦି(ଗଣ୍ଠି<k_Min_Two_Unit_ମୂଲ୍ୟ_Lead) {
                    delta=ଗଣ୍ଠି;
                } ଅନ୍ୟଥା ଯଦି(ଗଣ୍ଠି<k_Three_Unit_ମୂଲ୍ୟ_Lead) {
                    delta=((ଗଣ୍ଠି-k_Min_Two_Unit_ମୂଲ୍ୟ_Lead)<<16)|*pos++;
                } ଅନ୍ୟଥା {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                ଗଣ୍ଠି=*pos;
                result= ଗଣ୍ଠି>=k_Min_ମୂଲ୍ୟ_Lead ? ମୂଲ୍ୟ_Result(ଗଣ୍ଠି) : USTRINGTRIE_NO_ମୂଲ୍ୟ;
            }
            pos_=pos;
            ଫେରନ୍ତୁ result;
        }
        --length;
        pos=skip_ମୂଲ୍ୟ(pos);
    } ଯେତେବେଳେ(length>1);
    ଯଦି(uchar==*pos++) {
        pos_=pos;
        int32_t ଗଣ୍ଠି=*pos;
        ଫେରନ୍ତୁ ଗଣ୍ଠି>=k_Min_ମୂଲ୍ୟ_Lead ? ମୂଲ୍ୟ_Result(ଗଣ୍ଠି) : USTRINGTRIE_NO_ମୂଲ୍ୟ;
    } ଅନ୍ୟଥା {
        stop();
        ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(ଅଚଳ char16_t *pos, int32_t uchar) {
    int32_t ଗଣ୍ଠି=*pos++;
    ପାଇଁ(;;) {
        ଯଦି(ଗଣ୍ଠି<kMinLinearMatch) {
            ଫେରନ୍ତୁ branchNext(pos, ଗଣ୍ଠି, uchar);
        } ଅନ୍ୟଥା ଯଦି(ଗଣ୍ଠି<k_Min_ମୂଲ୍ୟ_Lead) {
            // Match the first of length+1 units.
            int32_t length=ଗଣ୍ଠି-kMinLinearMatch;  // Actual match length minus 1.
            ଯଦି(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ଫେରନ୍ତୁ (length<0 && (ଗଣ୍ଠି=*pos)>=k_Min_ମୂଲ୍ୟ_Lead) ?
                        ମୂଲ୍ୟ_Result(ଗଣ୍ଠି) : USTRINGTRIE_NO_ମୂଲ୍ୟ;
            } ଅନ୍ୟଥା {
                // No match.
                ଭାଙ୍ଗ;
            }
        } ଅନ୍ୟଥା ଯଦି(ଗଣ୍ଠି&k_ମୂଲ୍ୟ_Is_Final) {
            // No further matching units.
            ଭାଙ୍ଗ;
        } ଅନ୍ୟଥା {
            // Skip intermediate value.
            pos=skip_ଗଣ୍ଠି_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି);
            ଗଣ୍ଠି&=k_ଗଣ୍ଠି_Type_Mask;
        }
    }
    stop();
    ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    ଅଚଳ char16_t *pos=pos_;
    ଯଦି(pos==nullptr) {
        ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ଯଦି(length>=0) {
        // Remaining part of a linear-match node.
        ଯଦି(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t ଗଣ୍ଠି;
            ଫେରନ୍ତୁ (length<0 && (ଗଣ୍ଠି=*pos)>=k_Min_ମୂଲ୍ୟ_Lead) ?
                    ମୂଲ୍ୟ_Result(ଗଣ୍ଠି) : USTRINGTRIE_NO_ମୂଲ୍ୟ;
        } ଅନ୍ୟଥା {
            stop();
            ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
        }
    }
    ଫେରନ୍ତୁ nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    ଅଚଳ char16_t *s=ptr;
    ଯଦି(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ଫେରନ୍ତୁ current();
    }
    ଅଚଳ char16_t *pos=pos_;
    ଯଦି(pos==nullptr) {
        ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ପାଇଁ(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        ଯଦି(sLength<0) {
            ପାଇଁ(;;) {
                ଯଦି((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t ଗଣ୍ଠି;
                    ଫେରନ୍ତୁ (length<0 && (ଗଣ୍ଠି=*pos)>=k_Min_ମୂଲ୍ୟ_Lead) ?
                            ମୂଲ୍ୟ_Result(ଗଣ୍ଠି) : USTRINGTRIE_NO_ମୂଲ୍ୟ;
                }
                ଯଦି(length<0) {
                    remainingMatchLength_=length;
                    ଭାଙ୍ଗ;
                }
                ଯଦି(uchar!=*pos) {
                    stop();
                    ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ଅନ୍ୟଥା {
            ପାଇଁ(;;) {
                ଯଦି(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t ଗଣ୍ଠି;
                    ଫେରନ୍ତୁ (length<0 && (ଗଣ୍ଠି=*pos)>=k_Min_ମୂଲ୍ୟ_Lead) ?
                            ମୂଲ୍ୟ_Result(ଗଣ୍ଠି) : USTRINGTRIE_NO_ମୂଲ୍ୟ;
                }
                uchar=*s++;
                --sLength;
                ଯଦି(length<0) {
                    remainingMatchLength_=length;
                    ଭାଙ୍ଗ;
                }
                ଯଦି(uchar!=*pos) {
                    stop();
                    ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t ଗଣ୍ଠି=*pos++;
        ପାଇଁ(;;) {
            ଯଦି(ଗଣ୍ଠି<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, ଗଣ୍ଠି, uchar);
                ଯଦି(result==USTRINGTRIE_NO_MATCH) {
                    ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                ଯଦି(sLength<0) {
                    ଯଦି((uchar=*s++)==0) {
                        ଫେରନ୍ତୁ result;
                    }
                } ଅନ୍ୟଥା {
                    ଯଦି(sLength==0) {
                        ଫେରନ୍ତୁ result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                ଯଦି(result==USTRINGTRIE_FINAL_ମୂଲ୍ୟ) {
                    // No further matching units.
                    stop();
                    ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                ଗଣ୍ଠି=*pos++;
            } ଅନ୍ୟଥା ଯଦି(ଗଣ୍ଠି<k_Min_ମୂଲ୍ୟ_Lead) {
                // Match length+1 units.
                length=ଗଣ୍ଠି-kMinLinearMatch;  // Actual match length minus 1.
                ଯଦି(uchar!=*pos) {
                    stop();
                    ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                ଭାଙ୍ଗ;
            } ଅନ୍ୟଥା ଯଦି(ଗଣ୍ଠି&k_ମୂଲ୍ୟ_Is_Final) {
                // No further matching units.
                stop();
                ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
            } ଅନ୍ୟଥା {
                // Skip intermediate value.
                pos=skip_ଗଣ୍ଠି_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି);
                ଗଣ୍ଠି&=k_ଗଣ୍ଠି_Type_Mask;
            }
        }
    }
}

ଅଚଳ char16_t*
UCharsTrie::find_Unique_ମୂଲ୍ୟ_From_Branch(ଅଚଳ char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_ମୂଲ୍ୟ,
                                      int32_t& unique_ମୂଲ୍ୟ) {
    ଯେତେବେଳେ(length>k_Max_Branch_Linear_Sub_ଗଣ୍ଠି_Length) {
        ++pos;  // ignore the comparison unit
        ଯଦି(nullptr==find_Unique_ମୂଲ୍ୟ_From_Branch(jumpByDelta(pos), length>>1, have_Unique_ମୂଲ୍ୟ, unique_ମୂଲ୍ୟ)) {
            ଫେରନ୍ତୁ nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    କର {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t ଗଣ୍ଠି=*pos++;
        UBool isFinal = static_cast<UBool>(ଗଣ୍ଠି >> 15);
        ଗଣ୍ଠି&=0x7fff;
        int32_t value=read_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି);
        pos=skip_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି);
        ଯଦି(isFinal) {
            ଯଦି(have_Unique_ମୂଲ୍ୟ) {
                ଯଦି(value!=unique_ମୂଲ୍ୟ) {
                    ଫେରନ୍ତୁ nullptr;
                }
            } ଅନ୍ୟଥା {
                unique_ମୂଲ୍ୟ=value;
                have_Unique_ମୂଲ୍ୟ=ସତ୍ୟ;
            }
        } ଅନ୍ୟଥା {
            ଯଦି(!find_Unique_ମୂଲ୍ୟ(pos+value, have_Unique_ମୂଲ୍ୟ, unique_ମୂଲ୍ୟ)) {
                ଫେରନ୍ତୁ nullptr;
            }
            have_Unique_ମୂଲ୍ୟ=ସତ୍ୟ;
        }
    } ଯେତେବେଳେ(--length>1);
    ଫେରନ୍ତୁ pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::find_Unique_ମୂଲ୍ୟ(ଅଚଳ char16_t *pos, UBool have_Unique_ମୂଲ୍ୟ, int32_t &unique_ମୂଲ୍ୟ) {
    int32_t ଗଣ୍ଠି=*pos++;
    ପାଇଁ(;;) {
        ଯଦି(ଗଣ୍ଠି<kMinLinearMatch) {
            ଯଦି(ଗଣ୍ଠି==0) {
                ଗଣ୍ଠି=*pos++;
            }
            pos=find_Unique_ମୂଲ୍ୟ_From_Branch(pos, ଗଣ୍ଠି+1, have_Unique_ମୂଲ୍ୟ, unique_ମୂଲ୍ୟ);
            ଯଦି(pos==nullptr) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
            have_Unique_ମୂଲ୍ୟ=ସତ୍ୟ;
            ଗଣ୍ଠି=*pos++;
        } ଅନ୍ୟଥା ଯଦି(ଗଣ୍ଠି<k_Min_ମୂଲ୍ୟ_Lead) {
            // linear-match node
            pos+=ଗଣ୍ଠି-kMinLinearMatch+1;  // Ignore the match units.
            ଗଣ୍ଠି=*pos++;
        } ଅନ୍ୟଥା {
            UBool isFinal = static_cast<UBool>(ଗଣ୍ଠି >> 15);
            int32_t value;
            ଯଦି(isFinal) {
                value=read_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି&0x7fff);
            } ଅନ୍ୟଥା {
                value=read_ଗଣ୍ଠି_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି);
            }
            ଯଦି(have_Unique_ମୂଲ୍ୟ) {
                ଯଦି(value!=unique_ମୂଲ୍ୟ) {
                    ଫେରନ୍ତୁ ମିଥ୍ୟା;
                }
            } ଅନ୍ୟଥା {
                unique_ମୂଲ୍ୟ=value;
                have_Unique_ମୂଲ୍ୟ=ସତ୍ୟ;
            }
            ଯଦି(isFinal) {
                ଫେରନ୍ତୁ ସତ୍ୟ;
            }
            pos=skip_ଗଣ୍ଠି_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି);
            ଗଣ୍ଠି&=k_ଗଣ୍ଠି_Type_Mask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) ଅଚଳ {
    ଅଚଳ char16_t *pos=pos_;
    ଯଦି(pos==nullptr) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        ଫେରନ୍ତୁ 1;
    }
    int32_t ଗଣ୍ଠି=*pos++;
    ଯଦି(ଗଣ୍ଠି>=k_Min_ମୂଲ୍ୟ_Lead) {
        ଯଦି(ଗଣ୍ଠି&k_ମୂଲ୍ୟ_Is_Final) {
            ଫେରନ୍ତୁ 0;
        } ଅନ୍ୟଥା {
            pos=skip_ଗଣ୍ଠି_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି);
            ଗଣ୍ଠି&=k_ଗଣ୍ଠି_Type_Mask;
        }
    }
    ଯଦି(ଗଣ୍ଠି<kMinLinearMatch) {
        ଯଦି(ଗଣ୍ଠି==0) {
            ଗଣ୍ଠି=*pos++;
        }
        out.reserveAppendCapacity(++ଗଣ୍ଠି);
        getNextBranchUChars(pos, ଗଣ୍ଠି, out);
        ଫେରନ୍ତୁ ଗଣ୍ଠି;
    } ଅନ୍ୟଥା {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        ଫେରନ୍ତୁ 1;
    }
}

ଶୂନ୍ୟ
UCharsTrie::getNextBranchUChars(ଅଚଳ char16_t *pos, int32_t length, Appendable &out) {
    ଯେତେବେଳେ(length>k_Max_Branch_Linear_Sub_ଗଣ୍ଠି_Length) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    କର {
        out.appendCodeUnit(*pos++);
        pos=skip_ମୂଲ୍ୟ(pos);
    } ଯେତେବେଳେ(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
