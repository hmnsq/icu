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
    만약(pos==nullptr) {
        반환 USTRINGTRIE_NO_MATCH;
    } 아니면 {
        int32_t node;
        반환 (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    반환 cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    반환 cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(const char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    만약(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    동안(length>kMaxBranchLinearSubNodeLength) {
        만약(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } 아니면 {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        만약(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            만약(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } 아니면 {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                만약(node<kMinTwoUnitValueLead) {
                    delta=node;
                } 아니면 만약(node<kThreeUnitValueLead) {
                    delta=((node-kMinTwoUnitValueLead)<<16)|*pos++;
                } 아니면 {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            반환 result;
        }
        --length;
        pos=skipValue(pos);
    } 동안(length>1);
    만약(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        반환 node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } 아니면 {
        stop();
        반환 USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(const char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    위해(;;) {
        만약(node<kMinLinearMatch) {
            반환 branchNext(pos, node, uchar);
        } 아니면 만약(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            만약(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                반환 (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } 아니면 {
                // No match.
                break;
            }
        } 아니면 만약(node&kValueIsFinal) {
            // No further matching units.
            break;
        } 아니면 {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    반환 USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    const char16_t *pos=pos_;
    만약(pos==nullptr) {
        반환 USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    만약(length>=0) {
        // Remaining part of a linear-match node.
        만약(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            반환 (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } 아니면 {
            stop();
            반환 USTRINGTRIE_NO_MATCH;
        }
    }
    반환 nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    const char16_t *s=ptr;
    만약(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        반환 current();
    }
    const char16_t *pos=pos_;
    만약(pos==nullptr) {
        반환 USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    위해(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        만약(sLength<0) {
            위해(;;) {
                만약((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    반환 (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                만약(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                만약(uchar!=*pos) {
                    stop();
                    반환 USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } 아니면 {
            위해(;;) {
                만약(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    반환 (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                만약(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                만약(uchar!=*pos) {
                    stop();
                    반환 USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        위해(;;) {
            만약(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                만약(result==USTRINGTRIE_NO_MATCH) {
                    반환 USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                만약(sLength<0) {
                    만약((uchar=*s++)==0) {
                        반환 result;
                    }
                } 아니면 {
                    만약(sLength==0) {
                        반환 result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                만약(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    반환 USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } 아니면 만약(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                만약(uchar!=*pos) {
                    stop();
                    반환 USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                break;
            } 아니면 만약(node&kValueIsFinal) {
                // No further matching units.
                stop();
                반환 USTRINGTRIE_NO_MATCH;
            } 아니면 {
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
    동안(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        만약(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            반환 nullptr;
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
        만약(isFinal) {
            만약(haveUniqueValue) {
                만약(value!=uniqueValue) {
                    반환 nullptr;
                }
            } 아니면 {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } 아니면 {
            만약(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                반환 nullptr;
            }
            haveUniqueValue=true;
        }
    } 동안(--length>1);
    반환 pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(const char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    위해(;;) {
        만약(node<kMinLinearMatch) {
            만약(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            만약(pos==nullptr) {
                반환 false;
            }
            haveUniqueValue=true;
            node=*pos++;
        } 아니면 만약(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } 아니면 {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            만약(isFinal) {
                value=readValue(pos, node&0x7fff);
            } 아니면 {
                value=readNodeValue(pos, node);
            }
            만약(haveUniqueValue) {
                만약(value!=uniqueValue) {
                    반환 false;
                }
            } 아니면 {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            만약(isFinal) {
                반환 true;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) const {
    const char16_t *pos=pos_;
    만약(pos==nullptr) {
        반환 0;
    }
    만약(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        반환 1;
    }
    int32_t node=*pos++;
    만약(node>=kMinValueLead) {
        만약(node&kValueIsFinal) {
            반환 0;
        } 아니면 {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    만약(node<kMinLinearMatch) {
        만약(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        반환 node;
    } 아니면 {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        반환 1;
    }
}

void
UCharsTrie::getNextBranchUChars(const char16_t *pos, int32_t length, Appendable &out) {
    동안(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        out.appendCodeUnit(*pos++);
        pos=skipValue(pos);
    } 동안(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
