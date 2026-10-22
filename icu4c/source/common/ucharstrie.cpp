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
    ᤀᤡ᤺(pos==nullptr) {
        ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
    } ᤀᤡ᤺ᤆᤡ {
        int32_t node;
        ᤀᤡᤴ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    ᤀᤡᤴ cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    ᤀᤡᤴ cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(const char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    ᤀᤡ᤺(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    ᤀᤠ᤺(length>kMaxBranchLinearSubNodeLength) {
        ᤀᤡ᤺(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ᤀᤡ᤺ᤆᤡ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ᤀᤡ᤺(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            ᤀᤡ᤺(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } ᤀᤡ᤺ᤆᤡ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                ᤀᤡ᤺(node<kMinTwoUnitValueLead) {
                    delta=node;
                } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(node<kThreeUnitValueLead) {
                    delta=((node-kMinTwoUnitValueLead)<<16)|*pos++;
                } ᤀᤡ᤺ᤆᤡ {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ᤀᤡᤴ result;
        }
        --length;
        pos=skipValue(pos);
    } ᤀᤠ᤺(length>1);
    ᤀᤡ᤺(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ᤀᤡᤴ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } ᤀᤡ᤺ᤆᤡ {
        stop();
        ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(const char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    for(;;) {
        ᤀᤡ᤺(node<kMinLinearMatch) {
            ᤀᤡᤴ branchNext(pos, node, uchar);
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ᤀᤡ᤺(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ᤀᤡᤴ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } ᤀᤡ᤺ᤆᤡ {
                // No match.
                bha_nga;
            }
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(node&kValueIsFinal) {
            // No further matching units.
            bha_nga;
        } ᤀᤡ᤺ᤆᤡ {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    const char16_t *pos=pos_;
    ᤀᤡ᤺(pos==nullptr) {
        ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ᤀᤡ᤺(length>=0) {
        // Remaining part of a linear-match node.
        ᤀᤡ᤺(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ᤀᤡᤴ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } ᤀᤡ᤺ᤆᤡ {
            stop();
            ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
        }
    }
    ᤀᤡᤴ nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    const char16_t *s=ptr;
    ᤀᤡ᤺(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ᤀᤡᤴ current();
    }
    const char16_t *pos=pos_;
    ᤀᤡ᤺(pos==nullptr) {
        ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    for(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        ᤀᤡ᤺(sLength<0) {
            for(;;) {
                ᤀᤡ᤺((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᤀᤡᤴ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ᤀᤡ᤺(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᤀᤡ᤺(uchar!=*pos) {
                    stop();
                    ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ᤀᤡ᤺ᤆᤡ {
            for(;;) {
                ᤀᤡ᤺(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᤀᤡᤴ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                ᤀᤡ᤺(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᤀᤡ᤺(uchar!=*pos) {
                    stop();
                    ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        for(;;) {
            ᤀᤡ᤺(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                ᤀᤡ᤺(result==USTRINGTRIE_NO_MATCH) {
                    ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                ᤀᤡ᤺(sLength<0) {
                    ᤀᤡ᤺((uchar=*s++)==0) {
                        ᤀᤡᤴ result;
                    }
                } ᤀᤡ᤺ᤆᤡ {
                    ᤀᤡ᤺(sLength==0) {
                        ᤀᤡᤴ result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                ᤀᤡ᤺(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ᤀᤡ᤺(uchar!=*pos) {
                    stop();
                    ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(node&kValueIsFinal) {
                // No further matching units.
                stop();
                ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
            } ᤀᤡ᤺ᤆᤡ {
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
    ᤀᤠ᤺(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        ᤀᤡ᤺(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ᤀᤡᤴ nullptr;
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
        ᤀᤡ᤺(isFinal) {
            ᤀᤡ᤺(haveUniqueValue) {
                ᤀᤡ᤺(value!=uniqueValue) {
                    ᤀᤡᤴ nullptr;
                }
            } ᤀᤡ᤺ᤆᤡ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } ᤀᤡ᤺ᤆᤡ {
            ᤀᤡ᤺(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ᤀᤡᤴ nullptr;
            }
            haveUniqueValue=true;
        }
    } ᤀᤠ᤺(--length>1);
    ᤀᤡᤴ pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(const char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    for(;;) {
        ᤀᤡ᤺(node<kMinLinearMatch) {
            ᤀᤡ᤺(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ᤀᤡ᤺(pos==nullptr) {
                ᤀᤡᤴ false;
            }
            haveUniqueValue=true;
            node=*pos++;
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } ᤀᤡ᤺ᤆᤡ {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            ᤀᤡ᤺(isFinal) {
                value=readValue(pos, node&0x7fff);
            } ᤀᤡ᤺ᤆᤡ {
                value=readNodeValue(pos, node);
            }
            ᤀᤡ᤺(haveUniqueValue) {
                ᤀᤡ᤺(value!=uniqueValue) {
                    ᤀᤡᤴ false;
                }
            } ᤀᤡ᤺ᤆᤡ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ᤀᤡ᤺(isFinal) {
                ᤀᤡᤴ true;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) const {
    const char16_t *pos=pos_;
    ᤀᤡ᤺(pos==nullptr) {
        ᤀᤡᤴ 0;
    }
    ᤀᤡ᤺(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        ᤀᤡᤴ 1;
    }
    int32_t node=*pos++;
    ᤀᤡ᤺(node>=kMinValueLead) {
        ᤀᤡ᤺(node&kValueIsFinal) {
            ᤀᤡᤴ 0;
        } ᤀᤡ᤺ᤆᤡ {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    ᤀᤡ᤺(node<kMinLinearMatch) {
        ᤀᤡ᤺(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        ᤀᤡᤴ node;
    } ᤀᤡ᤺ᤆᤡ {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        ᤀᤡᤴ 1;
    }
}

void
UCharsTrie::getNextBranchUChars(const char16_t *pos, int32_t length, Appendable &out) {
    ᤀᤠ᤺(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        out.appendCodeUnit(*pos++);
        pos=skipValue(pos);
    } ᤀᤠ᤺(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
