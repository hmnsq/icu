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
    ᮚᮦᮔ᮪(pos==nullptr) {
        ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
    } ᮜᮤᮚ {
        int32_t node;
        ᮝᮜᮤ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    ᮝᮜᮤ cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    ᮝᮜᮤ cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(const char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    ᮚᮦᮔ᮪(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    ᮞᮨᮓᮨᮜ(length>kMaxBranchLinearSubNodeLength) {
        ᮚᮦᮔ᮪(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ᮜᮤᮚ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ᮚᮦᮔ᮪(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            ᮚᮦᮔ᮪(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } ᮜᮤᮚ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                ᮚᮦᮔ᮪(node<kMinTwoUnitValueLead) {
                    delta=node;
                } ᮜᮤᮚ ᮚᮦᮔ᮪(node<kThreeUnitValueLead) {
                    delta=((node-kMinTwoUnitValueLead)<<16)|*pos++;
                } ᮜᮤᮚ {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ᮝᮜᮤ result;
        }
        --length;
        pos=skipValue(pos);
    } ᮞᮨᮓᮨᮜ(length>1);
    ᮚᮦᮔ᮪(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ᮝᮜᮤ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } ᮜᮤᮚ {
        stop();
        ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(const char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    for(;;) {
        ᮚᮦᮔ᮪(node<kMinLinearMatch) {
            ᮝᮜᮤ branchNext(pos, node, uchar);
        } ᮜᮤᮚ ᮚᮦᮔ᮪(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ᮚᮦᮔ᮪(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ᮝᮜᮤ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } ᮜᮤᮚ {
                // No match.
                bha_nga;
            }
        } ᮜᮤᮚ ᮚᮦᮔ᮪(node&kValueIsFinal) {
            // No further matching units.
            bha_nga;
        } ᮜᮤᮚ {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    const char16_t *pos=pos_;
    ᮚᮦᮔ᮪(pos==nullptr) {
        ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ᮚᮦᮔ᮪(length>=0) {
        // Remaining part of a linear-match node.
        ᮚᮦᮔ᮪(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ᮝᮜᮤ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } ᮜᮤᮚ {
            stop();
            ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
        }
    }
    ᮝᮜᮤ nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    const char16_t *s=ptr;
    ᮚᮦᮔ᮪(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ᮝᮜᮤ current();
    }
    const char16_t *pos=pos_;
    ᮚᮦᮔ᮪(pos==nullptr) {
        ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    for(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        ᮚᮦᮔ᮪(sLength<0) {
            for(;;) {
                ᮚᮦᮔ᮪((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᮝᮜᮤ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ᮚᮦᮔ᮪(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᮚᮦᮔ᮪(uchar!=*pos) {
                    stop();
                    ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ᮜᮤᮚ {
            for(;;) {
                ᮚᮦᮔ᮪(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᮝᮜᮤ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                ᮚᮦᮔ᮪(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᮚᮦᮔ᮪(uchar!=*pos) {
                    stop();
                    ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        for(;;) {
            ᮚᮦᮔ᮪(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                ᮚᮦᮔ᮪(result==USTRINGTRIE_NO_MATCH) {
                    ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                ᮚᮦᮔ᮪(sLength<0) {
                    ᮚᮦᮔ᮪((uchar=*s++)==0) {
                        ᮝᮜᮤ result;
                    }
                } ᮜᮤᮚ {
                    ᮚᮦᮔ᮪(sLength==0) {
                        ᮝᮜᮤ result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                ᮚᮦᮔ᮪(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } ᮜᮤᮚ ᮚᮦᮔ᮪(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ᮚᮦᮔ᮪(uchar!=*pos) {
                    stop();
                    ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } ᮜᮤᮚ ᮚᮦᮔ᮪(node&kValueIsFinal) {
                // No further matching units.
                stop();
                ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
            } ᮜᮤᮚ {
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
    ᮞᮨᮓᮨᮜ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        ᮚᮦᮔ᮪(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ᮝᮜᮤ nullptr;
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
        ᮚᮦᮔ᮪(isFinal) {
            ᮚᮦᮔ᮪(haveUniqueValue) {
                ᮚᮦᮔ᮪(value!=uniqueValue) {
                    ᮝᮜᮤ nullptr;
                }
            } ᮜᮤᮚ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } ᮜᮤᮚ {
            ᮚᮦᮔ᮪(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ᮝᮜᮤ nullptr;
            }
            haveUniqueValue=true;
        }
    } ᮞᮨᮓᮨᮜ(--length>1);
    ᮝᮜᮤ pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(const char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    for(;;) {
        ᮚᮦᮔ᮪(node<kMinLinearMatch) {
            ᮚᮦᮔ᮪(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ᮚᮦᮔ᮪(pos==nullptr) {
                ᮝᮜᮤ false;
            }
            haveUniqueValue=true;
            node=*pos++;
        } ᮜᮤᮚ ᮚᮦᮔ᮪(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } ᮜᮤᮚ {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            ᮚᮦᮔ᮪(isFinal) {
                value=readValue(pos, node&0x7fff);
            } ᮜᮤᮚ {
                value=readNodeValue(pos, node);
            }
            ᮚᮦᮔ᮪(haveUniqueValue) {
                ᮚᮦᮔ᮪(value!=uniqueValue) {
                    ᮝᮜᮤ false;
                }
            } ᮜᮤᮚ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ᮚᮦᮔ᮪(isFinal) {
                ᮝᮜᮤ true;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) const {
    const char16_t *pos=pos_;
    ᮚᮦᮔ᮪(pos==nullptr) {
        ᮝᮜᮤ 0;
    }
    ᮚᮦᮔ᮪(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        ᮝᮜᮤ 1;
    }
    int32_t node=*pos++;
    ᮚᮦᮔ᮪(node>=kMinValueLead) {
        ᮚᮦᮔ᮪(node&kValueIsFinal) {
            ᮝᮜᮤ 0;
        } ᮜᮤᮚ {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    ᮚᮦᮔ᮪(node<kMinLinearMatch) {
        ᮚᮦᮔ᮪(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        ᮝᮜᮤ node;
    } ᮜᮤᮚ {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        ᮝᮜᮤ 1;
    }
}

void
UCharsTrie::getNextBranchUChars(const char16_t *pos, int32_t length, Appendable &out) {
    ᮞᮨᮓᮨᮜ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        out.appendCodeUnit(*pos++);
        pos=skipValue(pos);
    } ᮞᮨᮓᮨᮜ(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
