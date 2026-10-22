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
    nếu(pos==nullptr) {
        trả_về USTRINGTRIE_NO_MATCH;
    } không_thì {
        int32_t node;
        trả_về (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    trả_về cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    trả_về cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(const char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    nếu(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    trong_khi(length>kMaxBranchLinearSubNodeLength) {
        nếu(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } không_thì {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        nếu(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            nếu(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } không_thì {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                nếu(node<kMinTwoUnitValueLead) {
                    delta=node;
                } không_thì nếu(node<kThreeUnitValueLead) {
                    delta=((node-kMinTwoUnitValueLead)<<16)|*pos++;
                } không_thì {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            trả_về result;
        }
        --length;
        pos=skipValue(pos);
    } trong_khi(length>1);
    nếu(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        trả_về node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } không_thì {
        stop();
        trả_về USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(const char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    cho(;;) {
        nếu(node<kMinLinearMatch) {
            trả_về branchNext(pos, node, uchar);
        } không_thì nếu(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            nếu(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                trả_về (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } không_thì {
                // No match.
                break;
            }
        } không_thì nếu(node&kValueIsFinal) {
            // No further matching units.
            break;
        } không_thì {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    trả_về USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    const char16_t *pos=pos_;
    nếu(pos==nullptr) {
        trả_về USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    nếu(length>=0) {
        // Remaining part of a linear-match node.
        nếu(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            trả_về (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } không_thì {
            stop();
            trả_về USTRINGTRIE_NO_MATCH;
        }
    }
    trả_về nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    const char16_t *s=ptr;
    nếu(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        trả_về current();
    }
    const char16_t *pos=pos_;
    nếu(pos==nullptr) {
        trả_về USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    cho(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        nếu(sLength<0) {
            cho(;;) {
                nếu((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    trả_về (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                nếu(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                nếu(uchar!=*pos) {
                    stop();
                    trả_về USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } không_thì {
            cho(;;) {
                nếu(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    trả_về (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                nếu(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                nếu(uchar!=*pos) {
                    stop();
                    trả_về USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        cho(;;) {
            nếu(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                nếu(result==USTRINGTRIE_NO_MATCH) {
                    trả_về USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                nếu(sLength<0) {
                    nếu((uchar=*s++)==0) {
                        trả_về result;
                    }
                } không_thì {
                    nếu(sLength==0) {
                        trả_về result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                nếu(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    trả_về USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } không_thì nếu(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                nếu(uchar!=*pos) {
                    stop();
                    trả_về USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                break;
            } không_thì nếu(node&kValueIsFinal) {
                // No further matching units.
                stop();
                trả_về USTRINGTRIE_NO_MATCH;
            } không_thì {
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
    trong_khi(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        nếu(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            trả_về nullptr;
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
        nếu(isFinal) {
            nếu(haveUniqueValue) {
                nếu(value!=uniqueValue) {
                    trả_về nullptr;
                }
            } không_thì {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } không_thì {
            nếu(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                trả_về nullptr;
            }
            haveUniqueValue=true;
        }
    } trong_khi(--length>1);
    trả_về pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(const char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    cho(;;) {
        nếu(node<kMinLinearMatch) {
            nếu(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            nếu(pos==nullptr) {
                trả_về false;
            }
            haveUniqueValue=true;
            node=*pos++;
        } không_thì nếu(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } không_thì {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            nếu(isFinal) {
                value=readValue(pos, node&0x7fff);
            } không_thì {
                value=readNodeValue(pos, node);
            }
            nếu(haveUniqueValue) {
                nếu(value!=uniqueValue) {
                    trả_về false;
                }
            } không_thì {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            nếu(isFinal) {
                trả_về true;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) const {
    const char16_t *pos=pos_;
    nếu(pos==nullptr) {
        trả_về 0;
    }
    nếu(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        trả_về 1;
    }
    int32_t node=*pos++;
    nếu(node>=kMinValueLead) {
        nếu(node&kValueIsFinal) {
            trả_về 0;
        } không_thì {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    nếu(node<kMinLinearMatch) {
        nếu(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        trả_về node;
    } không_thì {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        trả_về 1;
    }
}

void
UCharsTrie::getNextBranchUChars(const char16_t *pos, int32_t length, Appendable &out) {
    trong_khi(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        out.appendCodeUnit(*pos++);
        pos=skipValue(pos);
    } trong_khi(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
