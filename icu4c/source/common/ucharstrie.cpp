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
    ᱡᱩᱫᱤ(pos==nullptr) {
        ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
    } ᱵᱟᱝ {
        int32_t node;
        ᱨᱩᱣᱟᱹᱲ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    ᱨᱩᱣᱟᱹᱲ cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    ᱨᱩᱣᱟᱹᱲ cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(const char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    ᱡᱩᱫᱤ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    ᱡᱚᱠᱷᱚᱱ(length>kMaxBranchLinearSubNodeLength) {
        ᱡᱩᱫᱤ(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ᱵᱟᱝ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ᱡᱩᱫᱤ(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            ᱡᱩᱫᱤ(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } ᱵᱟᱝ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                ᱡᱩᱫᱤ(node<kMinTwoUnitValueLead) {
                    delta=node;
                } ᱵᱟᱝ ᱡᱩᱫᱤ(node<kThreeUnitValueLead) {
                    delta=((node-kMinTwoUnitValueLead)<<16)|*pos++;
                } ᱵᱟᱝ {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ᱨᱩᱣᱟᱹᱲ result;
        }
        --length;
        pos=skipValue(pos);
    } ᱡᱚᱠᱷᱚᱱ(length>1);
    ᱡᱩᱫᱤ(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ᱨᱩᱣᱟᱹᱲ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } ᱵᱟᱝ {
        stop();
        ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(const char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    for(;;) {
        ᱡᱩᱫᱤ(node<kMinLinearMatch) {
            ᱨᱩᱣᱟᱹᱲ branchNext(pos, node, uchar);
        } ᱵᱟᱝ ᱡᱩᱫᱤ(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ᱡᱩᱫᱤ(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ᱨᱩᱣᱟᱹᱲ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } ᱵᱟᱝ {
                // No match.
                bha_nga;
            }
        } ᱵᱟᱝ ᱡᱩᱫᱤ(node&kValueIsFinal) {
            // No further matching units.
            bha_nga;
        } ᱵᱟᱝ {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    const char16_t *pos=pos_;
    ᱡᱩᱫᱤ(pos==nullptr) {
        ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ᱡᱩᱫᱤ(length>=0) {
        // Remaining part of a linear-match node.
        ᱡᱩᱫᱤ(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ᱨᱩᱣᱟᱹᱲ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } ᱵᱟᱝ {
            stop();
            ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
        }
    }
    ᱨᱩᱣᱟᱹᱲ nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    const char16_t *s=ptr;
    ᱡᱩᱫᱤ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ᱨᱩᱣᱟᱹᱲ current();
    }
    const char16_t *pos=pos_;
    ᱡᱩᱫᱤ(pos==nullptr) {
        ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    for(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        ᱡᱩᱫᱤ(sLength<0) {
            for(;;) {
                ᱡᱩᱫᱤ((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᱨᱩᱣᱟᱹᱲ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ᱡᱩᱫᱤ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᱡᱩᱫᱤ(uchar!=*pos) {
                    stop();
                    ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ᱵᱟᱝ {
            for(;;) {
                ᱡᱩᱫᱤ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᱨᱩᱣᱟᱹᱲ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                ᱡᱩᱫᱤ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᱡᱩᱫᱤ(uchar!=*pos) {
                    stop();
                    ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        for(;;) {
            ᱡᱩᱫᱤ(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                ᱡᱩᱫᱤ(result==USTRINGTRIE_NO_MATCH) {
                    ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                ᱡᱩᱫᱤ(sLength<0) {
                    ᱡᱩᱫᱤ((uchar=*s++)==0) {
                        ᱨᱩᱣᱟᱹᱲ result;
                    }
                } ᱵᱟᱝ {
                    ᱡᱩᱫᱤ(sLength==0) {
                        ᱨᱩᱣᱟᱹᱲ result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                ᱡᱩᱫᱤ(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } ᱵᱟᱝ ᱡᱩᱫᱤ(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ᱡᱩᱫᱤ(uchar!=*pos) {
                    stop();
                    ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } ᱵᱟᱝ ᱡᱩᱫᱤ(node&kValueIsFinal) {
                // No further matching units.
                stop();
                ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
            } ᱵᱟᱝ {
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
    ᱡᱚᱠᱷᱚᱱ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        ᱡᱩᱫᱤ(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ᱨᱩᱣᱟᱹᱲ nullptr;
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
        ᱡᱩᱫᱤ(isFinal) {
            ᱡᱩᱫᱤ(haveUniqueValue) {
                ᱡᱩᱫᱤ(value!=uniqueValue) {
                    ᱨᱩᱣᱟᱹᱲ nullptr;
                }
            } ᱵᱟᱝ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } ᱵᱟᱝ {
            ᱡᱩᱫᱤ(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ᱨᱩᱣᱟᱹᱲ nullptr;
            }
            haveUniqueValue=true;
        }
    } ᱡᱚᱠᱷᱚᱱ(--length>1);
    ᱨᱩᱣᱟᱹᱲ pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(const char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    for(;;) {
        ᱡᱩᱫᱤ(node<kMinLinearMatch) {
            ᱡᱩᱫᱤ(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ᱡᱩᱫᱤ(pos==nullptr) {
                ᱨᱩᱣᱟᱹᱲ false;
            }
            haveUniqueValue=true;
            node=*pos++;
        } ᱵᱟᱝ ᱡᱩᱫᱤ(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } ᱵᱟᱝ {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            ᱡᱩᱫᱤ(isFinal) {
                value=readValue(pos, node&0x7fff);
            } ᱵᱟᱝ {
                value=readNodeValue(pos, node);
            }
            ᱡᱩᱫᱤ(haveUniqueValue) {
                ᱡᱩᱫᱤ(value!=uniqueValue) {
                    ᱨᱩᱣᱟᱹᱲ false;
                }
            } ᱵᱟᱝ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ᱡᱩᱫᱤ(isFinal) {
                ᱨᱩᱣᱟᱹᱲ true;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) const {
    const char16_t *pos=pos_;
    ᱡᱩᱫᱤ(pos==nullptr) {
        ᱨᱩᱣᱟᱹᱲ 0;
    }
    ᱡᱩᱫᱤ(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        ᱨᱩᱣᱟᱹᱲ 1;
    }
    int32_t node=*pos++;
    ᱡᱩᱫᱤ(node>=kMinValueLead) {
        ᱡᱩᱫᱤ(node&kValueIsFinal) {
            ᱨᱩᱣᱟᱹᱲ 0;
        } ᱵᱟᱝ {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    ᱡᱩᱫᱤ(node<kMinLinearMatch) {
        ᱡᱩᱫᱤ(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        ᱨᱩᱣᱟᱹᱲ node;
    } ᱵᱟᱝ {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        ᱨᱩᱣᱟᱹᱲ 1;
    }
}

void
UCharsTrie::getNextBranchUChars(const char16_t *pos, int32_t length, Appendable &out) {
    ᱡᱚᱠᱷᱚᱱ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        out.appendCodeUnit(*pos++);
        pos=skipValue(pos);
    } ᱡᱚᱠᱷᱚᱱ(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
