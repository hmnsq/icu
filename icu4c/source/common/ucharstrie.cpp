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
    ܐܢ(pos==nullptr) {
        ܦܢܐ USTRINGTRIE_NO_MATCH;
    } ܐܚܪܢܐ {
        int32_t node;
        ܦܢܐ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    ܦܢܐ cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    ܦܢܐ cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(ثابت char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    ܐܢ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    ܟܡܐ(length>kMaxBranchLinearSubNodeLength) {
        ܐܢ(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ܐܚܪܢܐ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    ܥܒܕ {
        ܐܢ(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            ܐܢ(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } ܐܚܪܢܐ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                ܐܢ(node<kMinTwoUnitValueLead) {
                    delta=node;
                } ܐܚܪܢܐ ܐܢ(node<kThreeUnitValueLead) {
                    delta=((node-kMinTwoUnitValueLead)<<16)|*pos++;
                } ܐܚܪܢܐ {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ܦܢܐ result;
        }
        --length;
        pos=skipValue(pos);
    } ܟܡܐ(length>1);
    ܐܢ(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ܦܢܐ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } ܐܚܪܢܐ {
        stop();
        ܦܢܐ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(ثابت char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    لكل(;;) {
        ܐܢ(node<kMinLinearMatch) {
            ܦܢܐ branchNext(pos, node, uchar);
        } ܐܚܪܢܐ ܐܢ(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ܐܢ(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ܦܢܐ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } ܐܚܪܢܐ {
                // No match.
                اكسر;
            }
        } ܐܚܪܢܐ ܐܢ(node&kValueIsFinal) {
            // No further matching units.
            اكسر;
        } ܐܚܪܢܐ {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    ܦܢܐ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    ثابت char16_t *pos=pos_;
    ܐܢ(pos==nullptr) {
        ܦܢܐ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ܐܢ(length>=0) {
        // Remaining part of a linear-match node.
        ܐܢ(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ܦܢܐ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } ܐܚܪܢܐ {
            stop();
            ܦܢܐ USTRINGTRIE_NO_MATCH;
        }
    }
    ܦܢܐ nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    ثابت char16_t *s=ptr;
    ܐܢ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ܦܢܐ current();
    }
    ثابت char16_t *pos=pos_;
    ܐܢ(pos==nullptr) {
        ܦܢܐ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    لكل(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        ܐܢ(sLength<0) {
            لكل(;;) {
                ܐܢ((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ܦܢܐ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ܐܢ(length<0) {
                    remainingMatchLength_=length;
                    اكسر;
                }
                ܐܢ(uchar!=*pos) {
                    stop();
                    ܦܢܐ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ܐܚܪܢܐ {
            لكل(;;) {
                ܐܢ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ܦܢܐ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                ܐܢ(length<0) {
                    remainingMatchLength_=length;
                    اكسر;
                }
                ܐܢ(uchar!=*pos) {
                    stop();
                    ܦܢܐ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        لكل(;;) {
            ܐܢ(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                ܐܢ(result==USTRINGTRIE_NO_MATCH) {
                    ܦܢܐ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                ܐܢ(sLength<0) {
                    ܐܢ((uchar=*s++)==0) {
                        ܦܢܐ result;
                    }
                } ܐܚܪܢܐ {
                    ܐܢ(sLength==0) {
                        ܦܢܐ result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                ܐܢ(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    ܦܢܐ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } ܐܚܪܢܐ ܐܢ(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ܐܢ(uchar!=*pos) {
                    stop();
                    ܦܢܐ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                اكسر;
            } ܐܚܪܢܐ ܐܢ(node&kValueIsFinal) {
                // No further matching units.
                stop();
                ܦܢܐ USTRINGTRIE_NO_MATCH;
            } ܐܚܪܢܐ {
                // Skip intermediate value.
                pos=skipNodeValue(pos, node);
                node&=kNodeTypeMask;
            }
        }
    }
}

ثابت char16_t*
UCharsTrie::findUniqueValueFromBranch(ثابت char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool haveUniqueValue,
                                      int32_t& uniqueValue) {
    ܟܡܐ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        ܐܢ(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ܦܢܐ nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    ܥܒܕ {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t node=*pos++;
        UBool isFinal = static_cast<UBool>(node >> 15);
        node&=0x7fff;
        int32_t value=readValue(pos, node);
        pos=skipValue(pos, node);
        ܐܢ(isFinal) {
            ܐܢ(haveUniqueValue) {
                ܐܢ(value!=uniqueValue) {
                    ܦܢܐ nullptr;
                }
            } ܐܚܪܢܐ {
                uniqueValue=value;
                haveUniqueValue=ܫܪܪܐ;
            }
        } ܐܚܪܢܐ {
            ܐܢ(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ܦܢܐ nullptr;
            }
            haveUniqueValue=ܫܪܪܐ;
        }
    } ܟܡܐ(--length>1);
    ܦܢܐ pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(ثابت char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    لكل(;;) {
        ܐܢ(node<kMinLinearMatch) {
            ܐܢ(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ܐܢ(pos==nullptr) {
                ܦܢܐ ܕܓܠܐ;
            }
            haveUniqueValue=ܫܪܪܐ;
            node=*pos++;
        } ܐܚܪܢܐ ܐܢ(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } ܐܚܪܢܐ {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            ܐܢ(isFinal) {
                value=readValue(pos, node&0x7fff);
            } ܐܚܪܢܐ {
                value=readNodeValue(pos, node);
            }
            ܐܢ(haveUniqueValue) {
                ܐܢ(value!=uniqueValue) {
                    ܦܢܐ ܕܓܠܐ;
                }
            } ܐܚܪܢܐ {
                uniqueValue=value;
                haveUniqueValue=ܫܪܪܐ;
            }
            ܐܢ(isFinal) {
                ܦܢܐ ܫܪܪܐ;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) ثابت {
    ثابت char16_t *pos=pos_;
    ܐܢ(pos==nullptr) {
        ܦܢܐ 0;
    }
    ܐܢ(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        ܦܢܐ 1;
    }
    int32_t node=*pos++;
    ܐܢ(node>=kMinValueLead) {
        ܐܢ(node&kValueIsFinal) {
            ܦܢܐ 0;
        } ܐܚܪܢܐ {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    ܐܢ(node<kMinLinearMatch) {
        ܐܢ(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        ܦܢܐ node;
    } ܐܚܪܢܐ {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        ܦܢܐ 1;
    }
}

فراغ
UCharsTrie::getNextBranchUChars(ثابت char16_t *pos, int32_t length, Appendable &out) {
    ܟܡܐ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    ܥܒܕ {
        out.appendCodeUnit(*pos++);
        pos=skipValue(pos);
    } ܟܡܐ(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
