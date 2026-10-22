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

#include "unicode/utypes.h"
#include "unicode/appendable.h"
#include "unicode/ucharstrie.h"
#include "unicode/uobject.h"
#include "unicode/utf16.h"
#include "cmemory.h"
#include "uassert.h"

U_NAMESPACE_BEGIN

UCharsTrie::~UCharsTrie() {
    uprv_free(ownedArray_);
}

UStringTrieResult
UCharsTrie::current() const {
    const char16_t *pos=pos_;
    如果(pos==nullptr) {
        返回 USTRINGTRIE_NO_MATCH;
    } 否则 {
        int32_t node;
        返回 (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    返回 cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    返回 cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(const char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    如果(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    当(length>kMaxBranchLinearSubNodeLength) {
        如果(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } 否则 {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        如果(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            如果(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } 否则 {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                如果(node<kMinTwoUnitValueLead) {
                    delta=node;
                } 否则 如果(node<kThreeUnitValueLead) {
                    delta=((node-kMinTwoUnitValueLead)<<16)|*pos++;
                } 否则 {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            返回 result;
        }
        --length;
        pos=skipValue(pos);
    } 当(length>1);
    如果(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        返回 node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } 否则 {
        stop();
        返回 USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(const char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    为(;;) {
        如果(node<kMinLinearMatch) {
            返回 branchNext(pos, node, uchar);
        } 否则 如果(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            如果(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                返回 (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } 否则 {
                // No match.
                break;
            }
        } 否则 如果(node&kValueIsFinal) {
            // No further matching units.
            break;
        } 否则 {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    返回 USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    const char16_t *pos=pos_;
    如果(pos==nullptr) {
        返回 USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    如果(length>=0) {
        // Remaining part of a linear-match node.
        如果(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            返回 (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } 否则 {
            stop();
            返回 USTRINGTRIE_NO_MATCH;
        }
    }
    返回 nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    const char16_t *s=ptr;
    如果(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        返回 current();
    }
    const char16_t *pos=pos_;
    如果(pos==nullptr) {
        返回 USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    为(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        如果(sLength<0) {
            为(;;) {
                如果((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    返回 (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                如果(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                如果(uchar!=*pos) {
                    stop();
                    返回 USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } 否则 {
            为(;;) {
                如果(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    返回 (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                如果(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                如果(uchar!=*pos) {
                    stop();
                    返回 USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        为(;;) {
            如果(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                如果(result==USTRINGTRIE_NO_MATCH) {
                    返回 USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                如果(sLength<0) {
                    如果((uchar=*s++)==0) {
                        返回 result;
                    }
                } 否则 {
                    如果(sLength==0) {
                        返回 result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                如果(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    返回 USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } 否则 如果(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                如果(uchar!=*pos) {
                    stop();
                    返回 USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                break;
            } 否则 如果(node&kValueIsFinal) {
                // No further matching units.
                stop();
                返回 USTRINGTRIE_NO_MATCH;
            } 否则 {
                // Skip intermediate value.
                pos=skipNodeValue(pos, node);
                node&=kNodeTypeMask;
            }
        }
    }
}

const char16_t*
UCharsTrie::findUniqueValueFromBranch(const char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool haveUniqueValue,
                                      int32_t& uniqueValue) {
    当(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        如果(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            返回 nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t node=*pos++;
        UBool isFinal = static_cast<UBool>(node >> 15);
        node&=0x7fff;
        int32_t value=readValue(pos, node);
        pos=skipValue(pos, node);
        如果(isFinal) {
            如果(haveUniqueValue) {
                如果(value!=uniqueValue) {
                    返回 nullptr;
                }
            } 否则 {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } 否则 {
            如果(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                返回 nullptr;
            }
            haveUniqueValue=true;
        }
    } 当(--length>1);
    返回 pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(const char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    为(;;) {
        如果(node<kMinLinearMatch) {
            如果(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            如果(pos==nullptr) {
                返回 false;
            }
            haveUniqueValue=true;
            node=*pos++;
        } 否则 如果(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } 否则 {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            如果(isFinal) {
                value=readValue(pos, node&0x7fff);
            } 否则 {
                value=readNodeValue(pos, node);
            }
            如果(haveUniqueValue) {
                如果(value!=uniqueValue) {
                    返回 false;
                }
            } 否则 {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            如果(isFinal) {
                返回 true;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) const {
    const char16_t *pos=pos_;
    如果(pos==nullptr) {
        返回 0;
    }
    如果(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        返回 1;
    }
    int32_t node=*pos++;
    如果(node>=kMinValueLead) {
        如果(node&kValueIsFinal) {
            返回 0;
        } 否则 {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    如果(node<kMinLinearMatch) {
        如果(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        返回 node;
    } 否则 {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        返回 1;
    }
}

void
UCharsTrie::getNextBranchUChars(const char16_t *pos, int32_t length, Appendable &out) {
    当(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        out.appendCodeUnit(*pos++);
        pos=skipValue(pos);
    } 当(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
