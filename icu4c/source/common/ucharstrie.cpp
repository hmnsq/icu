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
    ᨀᨘ(pos==nullptr) {
        ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
    } ᨀᨘᨁ {
        int32_t node;
        ᨀᨘᨒ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    ᨀᨘᨒ cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    ᨀᨘᨒ cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(const char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    ᨀᨘ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    ᨀᨛ(length>kMaxBranchLinearSubNodeLength) {
        ᨀᨘ(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ᨀᨘᨁ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ᨀᨘ(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            ᨀᨘ(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } ᨀᨘᨁ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                ᨀᨘ(node<kMinTwoUnitValueLead) {
                    delta=node;
                } ᨀᨘᨁ ᨀᨘ(node<kThreeUnitValueLead) {
                    delta=((node-kMinTwoUnitValueLead)<<16)|*pos++;
                } ᨀᨘᨁ {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ᨀᨘᨒ result;
        }
        --length;
        pos=skipValue(pos);
    } ᨀᨛ(length>1);
    ᨀᨘ(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ᨀᨘᨒ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } ᨀᨘᨁ {
        stop();
        ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(const char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    for(;;) {
        ᨀᨘ(node<kMinLinearMatch) {
            ᨀᨘᨒ branchNext(pos, node, uchar);
        } ᨀᨘᨁ ᨀᨘ(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ᨀᨘ(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ᨀᨘᨒ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } ᨀᨘᨁ {
                // No match.
                bha_nga;
            }
        } ᨀᨘᨁ ᨀᨘ(node&kValueIsFinal) {
            // No further matching units.
            bha_nga;
        } ᨀᨘᨁ {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    const char16_t *pos=pos_;
    ᨀᨘ(pos==nullptr) {
        ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ᨀᨘ(length>=0) {
        // Remaining part of a linear-match node.
        ᨀᨘ(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ᨀᨘᨒ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } ᨀᨘᨁ {
            stop();
            ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
        }
    }
    ᨀᨘᨒ nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    const char16_t *s=ptr;
    ᨀᨘ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ᨀᨘᨒ current();
    }
    const char16_t *pos=pos_;
    ᨀᨘ(pos==nullptr) {
        ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    for(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        ᨀᨘ(sLength<0) {
            for(;;) {
                ᨀᨘ((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᨀᨘᨒ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ᨀᨘ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᨀᨘ(uchar!=*pos) {
                    stop();
                    ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ᨀᨘᨁ {
            for(;;) {
                ᨀᨘ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᨀᨘᨒ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                ᨀᨘ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᨀᨘ(uchar!=*pos) {
                    stop();
                    ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        for(;;) {
            ᨀᨘ(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                ᨀᨘ(result==USTRINGTRIE_NO_MATCH) {
                    ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                ᨀᨘ(sLength<0) {
                    ᨀᨘ((uchar=*s++)==0) {
                        ᨀᨘᨒ result;
                    }
                } ᨀᨘᨁ {
                    ᨀᨘ(sLength==0) {
                        ᨀᨘᨒ result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                ᨀᨘ(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } ᨀᨘᨁ ᨀᨘ(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ᨀᨘ(uchar!=*pos) {
                    stop();
                    ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } ᨀᨘᨁ ᨀᨘ(node&kValueIsFinal) {
                // No further matching units.
                stop();
                ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
            } ᨀᨘᨁ {
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
    ᨀᨛ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        ᨀᨘ(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ᨀᨘᨒ nullptr;
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
        ᨀᨘ(isFinal) {
            ᨀᨘ(haveUniqueValue) {
                ᨀᨘ(value!=uniqueValue) {
                    ᨀᨘᨒ nullptr;
                }
            } ᨀᨘᨁ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } ᨀᨘᨁ {
            ᨀᨘ(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ᨀᨘᨒ nullptr;
            }
            haveUniqueValue=true;
        }
    } ᨀᨛ(--length>1);
    ᨀᨘᨒ pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(const char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    for(;;) {
        ᨀᨘ(node<kMinLinearMatch) {
            ᨀᨘ(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ᨀᨘ(pos==nullptr) {
                ᨀᨘᨒ false;
            }
            haveUniqueValue=true;
            node=*pos++;
        } ᨀᨘᨁ ᨀᨘ(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } ᨀᨘᨁ {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            ᨀᨘ(isFinal) {
                value=readValue(pos, node&0x7fff);
            } ᨀᨘᨁ {
                value=readNodeValue(pos, node);
            }
            ᨀᨘ(haveUniqueValue) {
                ᨀᨘ(value!=uniqueValue) {
                    ᨀᨘᨒ false;
                }
            } ᨀᨘᨁ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ᨀᨘ(isFinal) {
                ᨀᨘᨒ true;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) const {
    const char16_t *pos=pos_;
    ᨀᨘ(pos==nullptr) {
        ᨀᨘᨒ 0;
    }
    ᨀᨘ(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        ᨀᨘᨒ 1;
    }
    int32_t node=*pos++;
    ᨀᨘ(node>=kMinValueLead) {
        ᨀᨘ(node&kValueIsFinal) {
            ᨀᨘᨒ 0;
        } ᨀᨘᨁ {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    ᨀᨘ(node<kMinLinearMatch) {
        ᨀᨘ(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        ᨀᨘᨒ node;
    } ᨀᨘᨁ {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        ᨀᨘᨒ 1;
    }
}

void
UCharsTrie::getNextBranchUChars(const char16_t *pos, int32_t length, Appendable &out) {
    ᨀᨛ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        out.appendCodeUnit(*pos++);
        pos=skipValue(pos);
    } ᨀᨛ(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
