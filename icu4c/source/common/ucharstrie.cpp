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
    אם(pos==nullptr) {
        החזר USTRINGTRIE_NO_MATCH;
    } אחרת {
        int32_t node;
        החזר (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    החזר cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    החזר cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(ثابت char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    אם(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    כלעוד(length>kMaxBranchLinearSubNodeLength) {
        אם(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } אחרת {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    עשה {
        אם(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            אם(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } אחרת {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                אם(node<kMinTwoUnitValueLead) {
                    delta=node;
                } אחרת אם(node<kThreeUnitValueLead) {
                    delta=((node-kMinTwoUnitValueLead)<<16)|*pos++;
                } אחרת {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            החזר result;
        }
        --length;
        pos=skipValue(pos);
    } כלעוד(length>1);
    אם(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        החזר node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } אחרת {
        stop();
        החזר USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(ثابت char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    עבור(;;) {
        אם(node<kMinLinearMatch) {
            החזר branchNext(pos, node, uchar);
        } אחרת אם(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            אם(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                החזר (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } אחרת {
                // No match.
                اكسر;
            }
        } אחרת אם(node&kValueIsFinal) {
            // No further matching units.
            اكسر;
        } אחרת {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    החזר USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    ثابت char16_t *pos=pos_;
    אם(pos==nullptr) {
        החזר USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    אם(length>=0) {
        // Remaining part of a linear-match node.
        אם(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            החזר (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } אחרת {
            stop();
            החזר USTRINGTRIE_NO_MATCH;
        }
    }
    החזר nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    ثابت char16_t *s=ptr;
    אם(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        החזר current();
    }
    ثابت char16_t *pos=pos_;
    אם(pos==nullptr) {
        החזר USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    עבור(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        אם(sLength<0) {
            עבור(;;) {
                אם((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    החזר (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                אם(length<0) {
                    remainingMatchLength_=length;
                    اكسر;
                }
                אם(uchar!=*pos) {
                    stop();
                    החזר USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } אחרת {
            עבור(;;) {
                אם(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    החזר (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                אם(length<0) {
                    remainingMatchLength_=length;
                    اكسر;
                }
                אם(uchar!=*pos) {
                    stop();
                    החזר USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        עבור(;;) {
            אם(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                אם(result==USTRINGTRIE_NO_MATCH) {
                    החזר USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                אם(sLength<0) {
                    אם((uchar=*s++)==0) {
                        החזר result;
                    }
                } אחרת {
                    אם(sLength==0) {
                        החזר result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                אם(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    החזר USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } אחרת אם(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                אם(uchar!=*pos) {
                    stop();
                    החזר USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                اكسر;
            } אחרת אם(node&kValueIsFinal) {
                // No further matching units.
                stop();
                החזר USTRINGTRIE_NO_MATCH;
            } אחרת {
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
    כלעוד(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        אם(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            החזר nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    עשה {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t node=*pos++;
        UBool isFinal = static_cast<UBool>(node >> 15);
        node&=0x7fff;
        int32_t value=readValue(pos, node);
        pos=skipValue(pos, node);
        אם(isFinal) {
            אם(haveUniqueValue) {
                אם(value!=uniqueValue) {
                    החזר nullptr;
                }
            } אחרת {
                uniqueValue=value;
                haveUniqueValue=אמת;
            }
        } אחרת {
            אם(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                החזר nullptr;
            }
            haveUniqueValue=אמת;
        }
    } כלעוד(--length>1);
    החזר pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(ثابت char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    עבור(;;) {
        אם(node<kMinLinearMatch) {
            אם(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            אם(pos==nullptr) {
                החזר שקר;
            }
            haveUniqueValue=אמת;
            node=*pos++;
        } אחרת אם(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } אחרת {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            אם(isFinal) {
                value=readValue(pos, node&0x7fff);
            } אחרת {
                value=readNodeValue(pos, node);
            }
            אם(haveUniqueValue) {
                אם(value!=uniqueValue) {
                    החזר שקר;
                }
            } אחרת {
                uniqueValue=value;
                haveUniqueValue=אמת;
            }
            אם(isFinal) {
                החזר אמת;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) ثابت {
    ثابت char16_t *pos=pos_;
    אם(pos==nullptr) {
        החזר 0;
    }
    אם(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        החזר 1;
    }
    int32_t node=*pos++;
    אם(node>=kMinValueLead) {
        אם(node&kValueIsFinal) {
            החזר 0;
        } אחרת {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    אם(node<kMinLinearMatch) {
        אם(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        החזר node;
    } אחרת {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        החזר 1;
    }
}

فراغ
UCharsTrie::getNextBranchUChars(ثابت char16_t *pos, int32_t length, Appendable &out) {
    כלעוד(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    עשה {
        out.appendCodeUnit(*pos++);
        pos=skipValue(pos);
    } כלעוד(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
