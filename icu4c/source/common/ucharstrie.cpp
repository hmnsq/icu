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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/appendable.h"
#അന്തര്ഭാവ "unicode/ucharstrie.h"
#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "uassert.h"

U_NAMESPACE_BEGIN

UCharsTrie::~UCharsTrie() {
    uprv_free(ownedArray_);
}

UStringTrieResult
UCharsTrie::current() സ്ഥിരം {
    സ്ഥിരം char16_t *pos=pos_;
    എങ്കിൽ(pos==nullptr) {
        തിരികെ USTRINGTRIE_NO_MATCH;
    } അല്ലെങ്കിൽ {
        int32_t കണ്ണി;
        തിരികെ (remainingMatchLength_<0 && (കണ്ണി=*pos)>=k_Min_മൂല്യം_Lead) ?
                മൂല്യം_Result(കണ്ണി) : USTRINGTRIE_NO_മൂല്യം;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    തിരികെ cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    തിരികെ cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(സ്ഥിരം char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    എങ്കിൽ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    ആകുമ്പോൾ(length>k_Max_Branch_Linear_Sub_കണ്ണി_Length) {
        എങ്കിൽ(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } അല്ലെങ്കിൽ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    ചെയ്യുക {
        എങ്കിൽ(uchar==*pos++) {
            UStringTrieResult result;
            int32_t കണ്ണി=*pos;
            എങ്കിൽ(കണ്ണി&k_മൂല്യം_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_മൂല്യം;
            } അല്ലെങ്കിൽ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                എങ്കിൽ(കണ്ണി<k_Min_Two_Unit_മൂല്യം_Lead) {
                    delta=കണ്ണി;
                } അല്ലെങ്കിൽ എങ്കിൽ(കണ്ണി<k_Three_Unit_മൂല്യം_Lead) {
                    delta=((കണ്ണി-k_Min_Two_Unit_മൂല്യം_Lead)<<16)|*pos++;
                } അല്ലെങ്കിൽ {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                കണ്ണി=*pos;
                result= കണ്ണി>=k_Min_മൂല്യം_Lead ? മൂല്യം_Result(കണ്ണി) : USTRINGTRIE_NO_മൂല്യം;
            }
            pos_=pos;
            തിരികെ result;
        }
        --length;
        pos=skip_മൂല്യം(pos);
    } ആകുമ്പോൾ(length>1);
    എങ്കിൽ(uchar==*pos++) {
        pos_=pos;
        int32_t കണ്ണി=*pos;
        തിരികെ കണ്ണി>=k_Min_മൂല്യം_Lead ? മൂല്യം_Result(കണ്ണി) : USTRINGTRIE_NO_മൂല്യം;
    } അല്ലെങ്കിൽ {
        stop();
        തിരികെ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(സ്ഥിരം char16_t *pos, int32_t uchar) {
    int32_t കണ്ണി=*pos++;
    വേണ്ടി(;;) {
        എങ്കിൽ(കണ്ണി<kMinLinearMatch) {
            തിരികെ branchNext(pos, കണ്ണി, uchar);
        } അല്ലെങ്കിൽ എങ്കിൽ(കണ്ണി<k_Min_മൂല്യം_Lead) {
            // Match the first of length+1 units.
            int32_t length=കണ്ണി-kMinLinearMatch;  // Actual match length minus 1.
            എങ്കിൽ(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                തിരികെ (length<0 && (കണ്ണി=*pos)>=k_Min_മൂല്യം_Lead) ?
                        മൂല്യം_Result(കണ്ണി) : USTRINGTRIE_NO_മൂല്യം;
            } അല്ലെങ്കിൽ {
                // No match.
                നിർത്തുക;
            }
        } അല്ലെങ്കിൽ എങ്കിൽ(കണ്ണി&k_മൂല്യം_Is_Final) {
            // No further matching units.
            നിർത്തുക;
        } അല്ലെങ്കിൽ {
            // Skip intermediate value.
            pos=skip_കണ്ണി_മൂല്യം(pos, കണ്ണി);
            കണ്ണി&=k_കണ്ണി_Type_Mask;
        }
    }
    stop();
    തിരികെ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    സ്ഥിരം char16_t *pos=pos_;
    എങ്കിൽ(pos==nullptr) {
        തിരികെ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    എങ്കിൽ(length>=0) {
        // Remaining part of a linear-match node.
        എങ്കിൽ(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t കണ്ണി;
            തിരികെ (length<0 && (കണ്ണി=*pos)>=k_Min_മൂല്യം_Lead) ?
                    മൂല്യം_Result(കണ്ണി) : USTRINGTRIE_NO_മൂല്യം;
        } അല്ലെങ്കിൽ {
            stop();
            തിരികെ USTRINGTRIE_NO_MATCH;
        }
    }
    തിരികെ nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    സ്ഥിരം char16_t *s=ptr;
    എങ്കിൽ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        തിരികെ current();
    }
    സ്ഥിരം char16_t *pos=pos_;
    എങ്കിൽ(pos==nullptr) {
        തിരികെ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    വേണ്ടി(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        എങ്കിൽ(sLength<0) {
            വേണ്ടി(;;) {
                എങ്കിൽ((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t കണ്ണി;
                    തിരികെ (length<0 && (കണ്ണി=*pos)>=k_Min_മൂല്യം_Lead) ?
                            മൂല്യം_Result(കണ്ണി) : USTRINGTRIE_NO_മൂല്യം;
                }
                എങ്കിൽ(length<0) {
                    remainingMatchLength_=length;
                    നിർത്തുക;
                }
                എങ്കിൽ(uchar!=*pos) {
                    stop();
                    തിരികെ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } അല്ലെങ്കിൽ {
            വേണ്ടി(;;) {
                എങ്കിൽ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t കണ്ണി;
                    തിരികെ (length<0 && (കണ്ണി=*pos)>=k_Min_മൂല്യം_Lead) ?
                            മൂല്യം_Result(കണ്ണി) : USTRINGTRIE_NO_മൂല്യം;
                }
                uchar=*s++;
                --sLength;
                എങ്കിൽ(length<0) {
                    remainingMatchLength_=length;
                    നിർത്തുക;
                }
                എങ്കിൽ(uchar!=*pos) {
                    stop();
                    തിരികെ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t കണ്ണി=*pos++;
        വേണ്ടി(;;) {
            എങ്കിൽ(കണ്ണി<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, കണ്ണി, uchar);
                എങ്കിൽ(result==USTRINGTRIE_NO_MATCH) {
                    തിരികെ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                എങ്കിൽ(sLength<0) {
                    എങ്കിൽ((uchar=*s++)==0) {
                        തിരികെ result;
                    }
                } അല്ലെങ്കിൽ {
                    എങ്കിൽ(sLength==0) {
                        തിരികെ result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                എങ്കിൽ(result==USTRINGTRIE_FINAL_മൂല്യം) {
                    // No further matching units.
                    stop();
                    തിരികെ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                കണ്ണി=*pos++;
            } അല്ലെങ്കിൽ എങ്കിൽ(കണ്ണി<k_Min_മൂല്യം_Lead) {
                // Match length+1 units.
                length=കണ്ണി-kMinLinearMatch;  // Actual match length minus 1.
                എങ്കിൽ(uchar!=*pos) {
                    stop();
                    തിരികെ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                നിർത്തുക;
            } അല്ലെങ്കിൽ എങ്കിൽ(കണ്ണി&k_മൂല്യം_Is_Final) {
                // No further matching units.
                stop();
                തിരികെ USTRINGTRIE_NO_MATCH;
            } അല്ലെങ്കിൽ {
                // Skip intermediate value.
                pos=skip_കണ്ണി_മൂല്യം(pos, കണ്ണി);
                കണ്ണി&=k_കണ്ണി_Type_Mask;
            }
        }
    }
}

സ്ഥിരം char16_t*
UCharsTrie::find_Unique_മൂല്യം_From_Branch(സ്ഥിരം char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_മൂല്യം,
                                      int32_t& unique_മൂല്യം) {
    ആകുമ്പോൾ(length>k_Max_Branch_Linear_Sub_കണ്ണി_Length) {
        ++pos;  // ignore the comparison unit
        എങ്കിൽ(nullptr==find_Unique_മൂല്യം_From_Branch(jumpByDelta(pos), length>>1, have_Unique_മൂല്യം, unique_മൂല്യം)) {
            തിരികെ nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    ചെയ്യുക {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t കണ്ണി=*pos++;
        UBool isFinal = static_cast<UBool>(കണ്ണി >> 15);
        കണ്ണി&=0x7fff;
        int32_t value=read_മൂല്യം(pos, കണ്ണി);
        pos=skip_മൂല്യം(pos, കണ്ണി);
        എങ്കിൽ(isFinal) {
            എങ്കിൽ(have_Unique_മൂല്യം) {
                എങ്കിൽ(value!=unique_മൂല്യം) {
                    തിരികെ nullptr;
                }
            } അല്ലെങ്കിൽ {
                unique_മൂല്യം=value;
                have_Unique_മൂല്യം=സത്യം;
            }
        } അല്ലെങ്കിൽ {
            എങ്കിൽ(!find_Unique_മൂല്യം(pos+value, have_Unique_മൂല്യം, unique_മൂല്യം)) {
                തിരികെ nullptr;
            }
            have_Unique_മൂല്യം=സത്യം;
        }
    } ആകുമ്പോൾ(--length>1);
    തിരികെ pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::find_Unique_മൂല്യം(സ്ഥിരം char16_t *pos, UBool have_Unique_മൂല്യം, int32_t &unique_മൂല്യം) {
    int32_t കണ്ണി=*pos++;
    വേണ്ടി(;;) {
        എങ്കിൽ(കണ്ണി<kMinLinearMatch) {
            എങ്കിൽ(കണ്ണി==0) {
                കണ്ണി=*pos++;
            }
            pos=find_Unique_മൂല്യം_From_Branch(pos, കണ്ണി+1, have_Unique_മൂല്യം, unique_മൂല്യം);
            എങ്കിൽ(pos==nullptr) {
                തിരികെ അസത്യം;
            }
            have_Unique_മൂല്യം=സത്യം;
            കണ്ണി=*pos++;
        } അല്ലെങ്കിൽ എങ്കിൽ(കണ്ണി<k_Min_മൂല്യം_Lead) {
            // linear-match node
            pos+=കണ്ണി-kMinLinearMatch+1;  // Ignore the match units.
            കണ്ണി=*pos++;
        } അല്ലെങ്കിൽ {
            UBool isFinal = static_cast<UBool>(കണ്ണി >> 15);
            int32_t value;
            എങ്കിൽ(isFinal) {
                value=read_മൂല്യം(pos, കണ്ണി&0x7fff);
            } അല്ലെങ്കിൽ {
                value=read_കണ്ണി_മൂല്യം(pos, കണ്ണി);
            }
            എങ്കിൽ(have_Unique_മൂല്യം) {
                എങ്കിൽ(value!=unique_മൂല്യം) {
                    തിരികെ അസത്യം;
                }
            } അല്ലെങ്കിൽ {
                unique_മൂല്യം=value;
                have_Unique_മൂല്യം=സത്യം;
            }
            എങ്കിൽ(isFinal) {
                തിരികെ സത്യം;
            }
            pos=skip_കണ്ണി_മൂല്യം(pos, കണ്ണി);
            കണ്ണി&=k_കണ്ണി_Type_Mask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) സ്ഥിരം {
    സ്ഥിരം char16_t *pos=pos_;
    എങ്കിൽ(pos==nullptr) {
        തിരികെ 0;
    }
    എങ്കിൽ(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        തിരികെ 1;
    }
    int32_t കണ്ണി=*pos++;
    എങ്കിൽ(കണ്ണി>=k_Min_മൂല്യം_Lead) {
        എങ്കിൽ(കണ്ണി&k_മൂല്യം_Is_Final) {
            തിരികെ 0;
        } അല്ലെങ്കിൽ {
            pos=skip_കണ്ണി_മൂല്യം(pos, കണ്ണി);
            കണ്ണി&=k_കണ്ണി_Type_Mask;
        }
    }
    എങ്കിൽ(കണ്ണി<kMinLinearMatch) {
        എങ്കിൽ(കണ്ണി==0) {
            കണ്ണി=*pos++;
        }
        out.reserveAppendCapacity(++കണ്ണി);
        getNextBranchUChars(pos, കണ്ണി, out);
        തിരികെ കണ്ണി;
    } അല്ലെങ്കിൽ {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        തിരികെ 1;
    }
}

ശൂന്യം
UCharsTrie::getNextBranchUChars(സ്ഥിരം char16_t *pos, int32_t length, Appendable &out) {
    ആകുമ്പോൾ(length>k_Max_Branch_Linear_Sub_കണ്ണി_Length) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    ചെയ്യുക {
        out.appendCodeUnit(*pos++);
        pos=skip_മൂല്യം(pos);
    } ആകുമ്പോൾ(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
