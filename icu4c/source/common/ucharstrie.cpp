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
    𐡄𐡍(pos==nullptr) {
        أرجع USTRINGTRIE_NO_MATCH;
    } وإلا {
        int32_t node;
        أرجع (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
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
    𐡄𐡍(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    طالما(length>kMaxBranchLinearSubNodeLength) {
        𐡄𐡍(uchar<*pos++) {
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
    𐡏𐡁𐡃 {
        𐡄𐡍(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            𐡄𐡍(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } وإلا {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                𐡄𐡍(node<kMinTwoUnitValueLead) {
                    delta=node;
                } وإلا 𐡄𐡍(node<kThreeUnitValueLead) {
                    delta=((node-kMinTwoUnitValueLead)<<16)|*pos++;
                } وإلا {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            أرجع result;
        }
        --length;
        pos=skipValue(pos);
    } طالما(length>1);
    𐡄𐡍(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        أرجع node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } وإلا {
        stop();
        أرجع USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(ثابت char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    لكل(;;) {
        𐡄𐡍(node<kMinLinearMatch) {
            أرجع branchNext(pos, node, uchar);
        } وإلا 𐡄𐡍(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            𐡄𐡍(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                أرجع (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } وإلا {
                // No match.
                اكسر;
            }
        } وإلا 𐡄𐡍(node&kValueIsFinal) {
            // No further matching units.
            اكسر;
        } وإلا {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    أرجع USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    ثابت char16_t *pos=pos_;
    𐡄𐡍(pos==nullptr) {
        أرجع USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    𐡄𐡍(length>=0) {
        // Remaining part of a linear-match node.
        𐡄𐡍(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            أرجع (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
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
    𐡄𐡍(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        أرجع current();
    }
    ثابت char16_t *pos=pos_;
    𐡄𐡍(pos==nullptr) {
        أرجع USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    لكل(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        𐡄𐡍(sLength<0) {
            لكل(;;) {
                𐡄𐡍((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    أرجع (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                𐡄𐡍(length<0) {
                    remainingMatchLength_=length;
                    اكسر;
                }
                𐡄𐡍(uchar!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } وإلا {
            لكل(;;) {
                𐡄𐡍(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    أرجع (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                𐡄𐡍(length<0) {
                    remainingMatchLength_=length;
                    اكسر;
                }
                𐡄𐡍(uchar!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        لكل(;;) {
            𐡄𐡍(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                𐡄𐡍(result==USTRINGTRIE_NO_MATCH) {
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                𐡄𐡍(sLength<0) {
                    𐡄𐡍((uchar=*s++)==0) {
                        أرجع result;
                    }
                } وإلا {
                    𐡄𐡍(sLength==0) {
                        أرجع result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                𐡄𐡍(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } وإلا 𐡄𐡍(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                𐡄𐡍(uchar!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                اكسر;
            } وإلا 𐡄𐡍(node&kValueIsFinal) {
                // No further matching units.
                stop();
                أرجع USTRINGTRIE_NO_MATCH;
            } وإلا {
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
    طالما(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        𐡄𐡍(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            أرجع nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    𐡏𐡁𐡃 {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t node=*pos++;
        UBool isFinal = static_cast<UBool>(node >> 15);
        node&=0x7fff;
        int32_t value=readValue(pos, node);
        pos=skipValue(pos, node);
        𐡄𐡍(isFinal) {
            𐡄𐡍(haveUniqueValue) {
                𐡄𐡍(value!=uniqueValue) {
                    أرجع nullptr;
                }
            } وإلا {
                uniqueValue=value;
                haveUniqueValue=𐡒𐡔𐡈;
            }
        } وإلا {
            𐡄𐡍(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                أرجع nullptr;
            }
            haveUniqueValue=𐡒𐡔𐡈;
        }
    } طالما(--length>1);
    أرجع pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(ثابت char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    لكل(;;) {
        𐡄𐡍(node<kMinLinearMatch) {
            𐡄𐡍(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            𐡄𐡍(pos==nullptr) {
                أرجع خطأ;
            }
            haveUniqueValue=𐡒𐡔𐡈;
            node=*pos++;
        } وإلا 𐡄𐡍(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } وإلا {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            𐡄𐡍(isFinal) {
                value=readValue(pos, node&0x7fff);
            } وإلا {
                value=readNodeValue(pos, node);
            }
            𐡄𐡍(haveUniqueValue) {
                𐡄𐡍(value!=uniqueValue) {
                    أرجع خطأ;
                }
            } وإلا {
                uniqueValue=value;
                haveUniqueValue=𐡒𐡔𐡈;
            }
            𐡄𐡍(isFinal) {
                أرجع 𐡒𐡔𐡈;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) ثابت {
    ثابت char16_t *pos=pos_;
    𐡄𐡍(pos==nullptr) {
        أرجع 0;
    }
    𐡄𐡍(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        أرجع 1;
    }
    int32_t node=*pos++;
    𐡄𐡍(node>=kMinValueLead) {
        𐡄𐡍(node&kValueIsFinal) {
            أرجع 0;
        } وإلا {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    𐡄𐡍(node<kMinLinearMatch) {
        𐡄𐡍(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        أرجع node;
    } وإلا {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        أرجع 1;
    }
}

فراغ
UCharsTrie::getNextBranchUChars(ثابت char16_t *pos, int32_t length, Appendable &out) {
    طالما(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    𐡏𐡁𐡃 {
        out.appendCodeUnit(*pos++);
        pos=skipValue(pos);
    } طالما(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
