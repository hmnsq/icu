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
    ᯀᯮᯝ(pos==nullptr) {
        ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
    } ᯀᯮᯝᯂᯪ {
        int32_t node;
        ᯀᯮᯞ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    ᯀᯮᯞ cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    ᯀᯮᯞ cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(const char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    ᯀᯮᯝ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    ᯀᯬᯝ(length>kMaxBranchLinearSubNodeLength) {
        ᯀᯮᯝ(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ᯀᯮᯝᯂᯪ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ᯀᯮᯝ(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            ᯀᯮᯝ(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } ᯀᯮᯝᯂᯪ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                ᯀᯮᯝ(node<kMinTwoUnitValueLead) {
                    delta=node;
                } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(node<kThreeUnitValueLead) {
                    delta=((node-kMinTwoUnitValueLead)<<16)|*pos++;
                } ᯀᯮᯝᯂᯪ {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ᯀᯮᯞ result;
        }
        --length;
        pos=skipValue(pos);
    } ᯀᯬᯝ(length>1);
    ᯀᯮᯝ(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ᯀᯮᯞ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } ᯀᯮᯝᯂᯪ {
        stop();
        ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(const char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    for(;;) {
        ᯀᯮᯝ(node<kMinLinearMatch) {
            ᯀᯮᯞ branchNext(pos, node, uchar);
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ᯀᯮᯝ(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ᯀᯮᯞ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } ᯀᯮᯝᯂᯪ {
                // No match.
                bha_nga;
            }
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(node&kValueIsFinal) {
            // No further matching units.
            bha_nga;
        } ᯀᯮᯝᯂᯪ {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    const char16_t *pos=pos_;
    ᯀᯮᯝ(pos==nullptr) {
        ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ᯀᯮᯝ(length>=0) {
        // Remaining part of a linear-match node.
        ᯀᯮᯝ(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ᯀᯮᯞ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } ᯀᯮᯝᯂᯪ {
            stop();
            ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
        }
    }
    ᯀᯮᯞ nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    const char16_t *s=ptr;
    ᯀᯮᯝ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ᯀᯮᯞ current();
    }
    const char16_t *pos=pos_;
    ᯀᯮᯝ(pos==nullptr) {
        ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    for(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        ᯀᯮᯝ(sLength<0) {
            for(;;) {
                ᯀᯮᯝ((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᯀᯮᯞ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ᯀᯮᯝ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᯀᯮᯝ(uchar!=*pos) {
                    stop();
                    ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ᯀᯮᯝᯂᯪ {
            for(;;) {
                ᯀᯮᯝ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᯀᯮᯞ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                ᯀᯮᯝ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᯀᯮᯝ(uchar!=*pos) {
                    stop();
                    ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        for(;;) {
            ᯀᯮᯝ(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                ᯀᯮᯝ(result==USTRINGTRIE_NO_MATCH) {
                    ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                ᯀᯮᯝ(sLength<0) {
                    ᯀᯮᯝ((uchar=*s++)==0) {
                        ᯀᯮᯞ result;
                    }
                } ᯀᯮᯝᯂᯪ {
                    ᯀᯮᯝ(sLength==0) {
                        ᯀᯮᯞ result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                ᯀᯮᯝ(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ᯀᯮᯝ(uchar!=*pos) {
                    stop();
                    ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(node&kValueIsFinal) {
                // No further matching units.
                stop();
                ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
            } ᯀᯮᯝᯂᯪ {
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
    ᯀᯬᯝ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        ᯀᯮᯝ(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ᯀᯮᯞ nullptr;
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
        ᯀᯮᯝ(isFinal) {
            ᯀᯮᯝ(haveUniqueValue) {
                ᯀᯮᯝ(value!=uniqueValue) {
                    ᯀᯮᯞ nullptr;
                }
            } ᯀᯮᯝᯂᯪ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } ᯀᯮᯝᯂᯪ {
            ᯀᯮᯝ(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ᯀᯮᯞ nullptr;
            }
            haveUniqueValue=true;
        }
    } ᯀᯬᯝ(--length>1);
    ᯀᯮᯞ pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(const char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    for(;;) {
        ᯀᯮᯝ(node<kMinLinearMatch) {
            ᯀᯮᯝ(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ᯀᯮᯝ(pos==nullptr) {
                ᯀᯮᯞ false;
            }
            haveUniqueValue=true;
            node=*pos++;
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } ᯀᯮᯝᯂᯪ {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            ᯀᯮᯝ(isFinal) {
                value=readValue(pos, node&0x7fff);
            } ᯀᯮᯝᯂᯪ {
                value=readNodeValue(pos, node);
            }
            ᯀᯮᯝ(haveUniqueValue) {
                ᯀᯮᯝ(value!=uniqueValue) {
                    ᯀᯮᯞ false;
                }
            } ᯀᯮᯝᯂᯪ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ᯀᯮᯝ(isFinal) {
                ᯀᯮᯞ true;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) const {
    const char16_t *pos=pos_;
    ᯀᯮᯝ(pos==nullptr) {
        ᯀᯮᯞ 0;
    }
    ᯀᯮᯝ(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        ᯀᯮᯞ 1;
    }
    int32_t node=*pos++;
    ᯀᯮᯝ(node>=kMinValueLead) {
        ᯀᯮᯝ(node&kValueIsFinal) {
            ᯀᯮᯞ 0;
        } ᯀᯮᯝᯂᯪ {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    ᯀᯮᯝ(node<kMinLinearMatch) {
        ᯀᯮᯝ(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        ᯀᯮᯞ node;
    } ᯀᯮᯝᯂᯪ {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        ᯀᯮᯞ 1;
    }
}

void
UCharsTrie::getNextBranchUChars(const char16_t *pos, int32_t length, Appendable &out) {
    ᯀᯬᯝ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        out.appendCodeUnit(*pos++);
        pos=skipValue(pos);
    } ᯀᯬᯝ(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
