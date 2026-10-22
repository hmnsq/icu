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
    ꦪꦼꦤ꧀(pos==nullptr) {
        ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
    } ꦭꦶꦪ {
        int32_t node;
        ꦮꦭꦶ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    ꦮꦭꦶ cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    ꦮꦭꦶ cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(const char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    ꦪꦼꦤ꧀(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    ꦱꦼꦢꦼꦭ(length>kMaxBranchLinearSubNodeLength) {
        ꦪꦼꦤ꧀(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ꦭꦶꦪ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ꦪꦼꦤ꧀(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            ꦪꦼꦤ꧀(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } ꦭꦶꦪ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                ꦪꦼꦤ꧀(node<kMinTwoUnitValueLead) {
                    delta=node;
                } ꦭꦶꦪ ꦪꦼꦤ꧀(node<kThreeUnitValueLead) {
                    delta=((node-kMinTwoUnitValueLead)<<16)|*pos++;
                } ꦭꦶꦪ {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ꦮꦭꦶ result;
        }
        --length;
        pos=skipValue(pos);
    } ꦱꦼꦢꦼꦭ(length>1);
    ꦪꦼꦤ꧀(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ꦮꦭꦶ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } ꦭꦶꦪ {
        stop();
        ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(const char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    ꦏꦁꦒꦺꦴ(;;) {
        ꦪꦼꦤ꧀(node<kMinLinearMatch) {
            ꦮꦭꦶ branchNext(pos, node, uchar);
        } ꦭꦶꦪ ꦪꦼꦤ꧀(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ꦪꦼꦤ꧀(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ꦮꦭꦶ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } ꦭꦶꦪ {
                // No match.
                bha_nga;
            }
        } ꦭꦶꦪ ꦪꦼꦤ꧀(node&kValueIsFinal) {
            // No further matching units.
            bha_nga;
        } ꦭꦶꦪ {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    const char16_t *pos=pos_;
    ꦪꦼꦤ꧀(pos==nullptr) {
        ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ꦪꦼꦤ꧀(length>=0) {
        // Remaining part of a linear-match node.
        ꦪꦼꦤ꧀(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ꦮꦭꦶ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } ꦭꦶꦪ {
            stop();
            ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
        }
    }
    ꦮꦭꦶ nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    const char16_t *s=ptr;
    ꦪꦼꦤ꧀(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ꦮꦭꦶ current();
    }
    const char16_t *pos=pos_;
    ꦪꦼꦤ꧀(pos==nullptr) {
        ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ꦏꦁꦒꦺꦴ(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        ꦪꦼꦤ꧀(sLength<0) {
            ꦏꦁꦒꦺꦴ(;;) {
                ꦪꦼꦤ꧀((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ꦮꦭꦶ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ꦪꦼꦤ꧀(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ꦪꦼꦤ꧀(uchar!=*pos) {
                    stop();
                    ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ꦭꦶꦪ {
            ꦏꦁꦒꦺꦴ(;;) {
                ꦪꦼꦤ꧀(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ꦮꦭꦶ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                ꦪꦼꦤ꧀(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ꦪꦼꦤ꧀(uchar!=*pos) {
                    stop();
                    ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        ꦏꦁꦒꦺꦴ(;;) {
            ꦪꦼꦤ꧀(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                ꦪꦼꦤ꧀(result==USTRINGTRIE_NO_MATCH) {
                    ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                ꦪꦼꦤ꧀(sLength<0) {
                    ꦪꦼꦤ꧀((uchar=*s++)==0) {
                        ꦮꦭꦶ result;
                    }
                } ꦭꦶꦪ {
                    ꦪꦼꦤ꧀(sLength==0) {
                        ꦮꦭꦶ result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                ꦪꦼꦤ꧀(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } ꦭꦶꦪ ꦪꦼꦤ꧀(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ꦪꦼꦤ꧀(uchar!=*pos) {
                    stop();
                    ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } ꦭꦶꦪ ꦪꦼꦤ꧀(node&kValueIsFinal) {
                // No further matching units.
                stop();
                ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
            } ꦭꦶꦪ {
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
    ꦱꦼꦢꦼꦭ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        ꦪꦼꦤ꧀(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ꦮꦭꦶ nullptr;
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
        ꦪꦼꦤ꧀(isFinal) {
            ꦪꦼꦤ꧀(haveUniqueValue) {
                ꦪꦼꦤ꧀(value!=uniqueValue) {
                    ꦮꦭꦶ nullptr;
                }
            } ꦭꦶꦪ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } ꦭꦶꦪ {
            ꦪꦼꦤ꧀(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ꦮꦭꦶ nullptr;
            }
            haveUniqueValue=true;
        }
    } ꦱꦼꦢꦼꦭ(--length>1);
    ꦮꦭꦶ pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(const char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    ꦏꦁꦒꦺꦴ(;;) {
        ꦪꦼꦤ꧀(node<kMinLinearMatch) {
            ꦪꦼꦤ꧀(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ꦪꦼꦤ꧀(pos==nullptr) {
                ꦮꦭꦶ false;
            }
            haveUniqueValue=true;
            node=*pos++;
        } ꦭꦶꦪ ꦪꦼꦤ꧀(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } ꦭꦶꦪ {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            ꦪꦼꦤ꧀(isFinal) {
                value=readValue(pos, node&0x7fff);
            } ꦭꦶꦪ {
                value=readNodeValue(pos, node);
            }
            ꦪꦼꦤ꧀(haveUniqueValue) {
                ꦪꦼꦤ꧀(value!=uniqueValue) {
                    ꦮꦭꦶ false;
                }
            } ꦭꦶꦪ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ꦪꦼꦤ꧀(isFinal) {
                ꦮꦭꦶ true;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) const {
    const char16_t *pos=pos_;
    ꦪꦼꦤ꧀(pos==nullptr) {
        ꦮꦭꦶ 0;
    }
    ꦪꦼꦤ꧀(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        ꦮꦭꦶ 1;
    }
    int32_t node=*pos++;
    ꦪꦼꦤ꧀(node>=kMinValueLead) {
        ꦪꦼꦤ꧀(node&kValueIsFinal) {
            ꦮꦭꦶ 0;
        } ꦭꦶꦪ {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    ꦪꦼꦤ꧀(node<kMinLinearMatch) {
        ꦪꦼꦤ꧀(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        ꦮꦭꦶ node;
    } ꦭꦶꦪ {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        ꦮꦭꦶ 1;
    }
}

void
UCharsTrie::getNextBranchUChars(const char16_t *pos, int32_t length, Appendable &out) {
    ꦱꦼꦢꦼꦭ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        out.appendCodeUnit(*pos++);
        pos=skipValue(pos);
    } ꦱꦼꦢꦼꦭ(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
