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
    ᰀᰧ(pos==nullptr) {
        ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
    } ᰀᰧᰀ {
        int32_t node;
        ᰀᰧᰌ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    ᰀᰧᰌ cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    ᰀᰧᰌ cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(const char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    ᰀᰧ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    ᰀᰬ(length>kMaxBranchLinearSubNodeLength) {
        ᰀᰧ(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ᰀᰧᰀ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ᰀᰧ(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            ᰀᰧ(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } ᰀᰧᰀ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                ᰀᰧ(node<kMinTwoUnitValueLead) {
                    delta=node;
                } ᰀᰧᰀ ᰀᰧ(node<kThreeUnitValueLead) {
                    delta=((node-kMinTwoUnitValueLead)<<16)|*pos++;
                } ᰀᰧᰀ {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ᰀᰧᰌ result;
        }
        --length;
        pos=skipValue(pos);
    } ᰀᰬ(length>1);
    ᰀᰧ(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ᰀᰧᰌ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } ᰀᰧᰀ {
        stop();
        ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(const char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    for(;;) {
        ᰀᰧ(node<kMinLinearMatch) {
            ᰀᰧᰌ branchNext(pos, node, uchar);
        } ᰀᰧᰀ ᰀᰧ(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ᰀᰧ(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ᰀᰧᰌ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } ᰀᰧᰀ {
                // No match.
                bha_nga;
            }
        } ᰀᰧᰀ ᰀᰧ(node&kValueIsFinal) {
            // No further matching units.
            bha_nga;
        } ᰀᰧᰀ {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    const char16_t *pos=pos_;
    ᰀᰧ(pos==nullptr) {
        ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ᰀᰧ(length>=0) {
        // Remaining part of a linear-match node.
        ᰀᰧ(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ᰀᰧᰌ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } ᰀᰧᰀ {
            stop();
            ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
        }
    }
    ᰀᰧᰌ nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    const char16_t *s=ptr;
    ᰀᰧ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ᰀᰧᰌ current();
    }
    const char16_t *pos=pos_;
    ᰀᰧ(pos==nullptr) {
        ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    for(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        ᰀᰧ(sLength<0) {
            for(;;) {
                ᰀᰧ((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᰀᰧᰌ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ᰀᰧ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᰀᰧ(uchar!=*pos) {
                    stop();
                    ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ᰀᰧᰀ {
            for(;;) {
                ᰀᰧ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᰀᰧᰌ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                ᰀᰧ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᰀᰧ(uchar!=*pos) {
                    stop();
                    ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        for(;;) {
            ᰀᰧ(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                ᰀᰧ(result==USTRINGTRIE_NO_MATCH) {
                    ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                ᰀᰧ(sLength<0) {
                    ᰀᰧ((uchar=*s++)==0) {
                        ᰀᰧᰌ result;
                    }
                } ᰀᰧᰀ {
                    ᰀᰧ(sLength==0) {
                        ᰀᰧᰌ result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                ᰀᰧ(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } ᰀᰧᰀ ᰀᰧ(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ᰀᰧ(uchar!=*pos) {
                    stop();
                    ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } ᰀᰧᰀ ᰀᰧ(node&kValueIsFinal) {
                // No further matching units.
                stop();
                ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
            } ᰀᰧᰀ {
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
    ᰀᰬ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        ᰀᰧ(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ᰀᰧᰌ nullptr;
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
        ᰀᰧ(isFinal) {
            ᰀᰧ(haveUniqueValue) {
                ᰀᰧ(value!=uniqueValue) {
                    ᰀᰧᰌ nullptr;
                }
            } ᰀᰧᰀ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } ᰀᰧᰀ {
            ᰀᰧ(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ᰀᰧᰌ nullptr;
            }
            haveUniqueValue=true;
        }
    } ᰀᰬ(--length>1);
    ᰀᰧᰌ pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(const char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    for(;;) {
        ᰀᰧ(node<kMinLinearMatch) {
            ᰀᰧ(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ᰀᰧ(pos==nullptr) {
                ᰀᰧᰌ false;
            }
            haveUniqueValue=true;
            node=*pos++;
        } ᰀᰧᰀ ᰀᰧ(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } ᰀᰧᰀ {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            ᰀᰧ(isFinal) {
                value=readValue(pos, node&0x7fff);
            } ᰀᰧᰀ {
                value=readNodeValue(pos, node);
            }
            ᰀᰧ(haveUniqueValue) {
                ᰀᰧ(value!=uniqueValue) {
                    ᰀᰧᰌ false;
                }
            } ᰀᰧᰀ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ᰀᰧ(isFinal) {
                ᰀᰧᰌ true;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) const {
    const char16_t *pos=pos_;
    ᰀᰧ(pos==nullptr) {
        ᰀᰧᰌ 0;
    }
    ᰀᰧ(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        ᰀᰧᰌ 1;
    }
    int32_t node=*pos++;
    ᰀᰧ(node>=kMinValueLead) {
        ᰀᰧ(node&kValueIsFinal) {
            ᰀᰧᰌ 0;
        } ᰀᰧᰀ {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    ᰀᰧ(node<kMinLinearMatch) {
        ᰀᰧ(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        ᰀᰧᰌ node;
    } ᰀᰧᰀ {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        ᰀᰧᰌ 1;
    }
}

void
UCharsTrie::getNextBranchUChars(const char16_t *pos, int32_t length, Appendable &out) {
    ᰀᰬ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        out.appendCodeUnit(*pos++);
        pos=skipValue(pos);
    } ᰀᰬ(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
