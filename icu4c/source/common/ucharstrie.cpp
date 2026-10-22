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
    ᬬᬾᬦ᭄(pos==nullptr) {
        ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
    } ᬮᬶᬬ {
        int32_t node;
        ᬯᬮᬶ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    ᬯᬮᬶ cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    ᬯᬮᬶ cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(const char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    ᬬᬾᬦ᭄(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    ᬲᭂᬤᭂᬮ(length>kMaxBranchLinearSubNodeLength) {
        ᬬᬾᬦ᭄(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ᬮᬶᬬ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ᬬᬾᬦ᭄(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            ᬬᬾᬦ᭄(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } ᬮᬶᬬ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                ᬬᬾᬦ᭄(node<kMinTwoUnitValueLead) {
                    delta=node;
                } ᬮᬶᬬ ᬬᬾᬦ᭄(node<kThreeUnitValueLead) {
                    delta=((node-kMinTwoUnitValueLead)<<16)|*pos++;
                } ᬮᬶᬬ {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ᬯᬮᬶ result;
        }
        --length;
        pos=skipValue(pos);
    } ᬲᭂᬤᭂᬮ(length>1);
    ᬬᬾᬦ᭄(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ᬯᬮᬶ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } ᬮᬶᬬ {
        stop();
        ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(const char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    for(;;) {
        ᬬᬾᬦ᭄(node<kMinLinearMatch) {
            ᬯᬮᬶ branchNext(pos, node, uchar);
        } ᬮᬶᬬ ᬬᬾᬦ᭄(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ᬬᬾᬦ᭄(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ᬯᬮᬶ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } ᬮᬶᬬ {
                // No match.
                bha_nga;
            }
        } ᬮᬶᬬ ᬬᬾᬦ᭄(node&kValueIsFinal) {
            // No further matching units.
            bha_nga;
        } ᬮᬶᬬ {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    const char16_t *pos=pos_;
    ᬬᬾᬦ᭄(pos==nullptr) {
        ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ᬬᬾᬦ᭄(length>=0) {
        // Remaining part of a linear-match node.
        ᬬᬾᬦ᭄(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ᬯᬮᬶ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } ᬮᬶᬬ {
            stop();
            ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
        }
    }
    ᬯᬮᬶ nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    const char16_t *s=ptr;
    ᬬᬾᬦ᭄(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ᬯᬮᬶ current();
    }
    const char16_t *pos=pos_;
    ᬬᬾᬦ᭄(pos==nullptr) {
        ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    for(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        ᬬᬾᬦ᭄(sLength<0) {
            for(;;) {
                ᬬᬾᬦ᭄((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᬯᬮᬶ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ᬬᬾᬦ᭄(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᬬᬾᬦ᭄(uchar!=*pos) {
                    stop();
                    ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ᬮᬶᬬ {
            for(;;) {
                ᬬᬾᬦ᭄(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᬯᬮᬶ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                ᬬᬾᬦ᭄(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᬬᬾᬦ᭄(uchar!=*pos) {
                    stop();
                    ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        for(;;) {
            ᬬᬾᬦ᭄(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                ᬬᬾᬦ᭄(result==USTRINGTRIE_NO_MATCH) {
                    ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                ᬬᬾᬦ᭄(sLength<0) {
                    ᬬᬾᬦ᭄((uchar=*s++)==0) {
                        ᬯᬮᬶ result;
                    }
                } ᬮᬶᬬ {
                    ᬬᬾᬦ᭄(sLength==0) {
                        ᬯᬮᬶ result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                ᬬᬾᬦ᭄(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } ᬮᬶᬬ ᬬᬾᬦ᭄(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ᬬᬾᬦ᭄(uchar!=*pos) {
                    stop();
                    ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } ᬮᬶᬬ ᬬᬾᬦ᭄(node&kValueIsFinal) {
                // No further matching units.
                stop();
                ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
            } ᬮᬶᬬ {
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
    ᬲᭂᬤᭂᬮ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        ᬬᬾᬦ᭄(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ᬯᬮᬶ nullptr;
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
        ᬬᬾᬦ᭄(isFinal) {
            ᬬᬾᬦ᭄(haveUniqueValue) {
                ᬬᬾᬦ᭄(value!=uniqueValue) {
                    ᬯᬮᬶ nullptr;
                }
            } ᬮᬶᬬ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } ᬮᬶᬬ {
            ᬬᬾᬦ᭄(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ᬯᬮᬶ nullptr;
            }
            haveUniqueValue=true;
        }
    } ᬲᭂᬤᭂᬮ(--length>1);
    ᬯᬮᬶ pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(const char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    for(;;) {
        ᬬᬾᬦ᭄(node<kMinLinearMatch) {
            ᬬᬾᬦ᭄(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ᬬᬾᬦ᭄(pos==nullptr) {
                ᬯᬮᬶ false;
            }
            haveUniqueValue=true;
            node=*pos++;
        } ᬮᬶᬬ ᬬᬾᬦ᭄(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } ᬮᬶᬬ {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            ᬬᬾᬦ᭄(isFinal) {
                value=readValue(pos, node&0x7fff);
            } ᬮᬶᬬ {
                value=readNodeValue(pos, node);
            }
            ᬬᬾᬦ᭄(haveUniqueValue) {
                ᬬᬾᬦ᭄(value!=uniqueValue) {
                    ᬯᬮᬶ false;
                }
            } ᬮᬶᬬ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ᬬᬾᬦ᭄(isFinal) {
                ᬯᬮᬶ true;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) const {
    const char16_t *pos=pos_;
    ᬬᬾᬦ᭄(pos==nullptr) {
        ᬯᬮᬶ 0;
    }
    ᬬᬾᬦ᭄(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        ᬯᬮᬶ 1;
    }
    int32_t node=*pos++;
    ᬬᬾᬦ᭄(node>=kMinValueLead) {
        ᬬᬾᬦ᭄(node&kValueIsFinal) {
            ᬯᬮᬶ 0;
        } ᬮᬶᬬ {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    ᬬᬾᬦ᭄(node<kMinLinearMatch) {
        ᬬᬾᬦ᭄(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        ᬯᬮᬶ node;
    } ᬮᬶᬬ {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        ᬯᬮᬶ 1;
    }
}

void
UCharsTrie::getNextBranchUChars(const char16_t *pos, int32_t length, Appendable &out) {
    ᬲᭂᬤᭂᬮ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        out.appendCodeUnit(*pos++);
        pos=skipValue(pos);
    } ᬲᭂᬤᭂᬮ(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
