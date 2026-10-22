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
    ако(pos==nullptr) {
        врати USTRINGTRIE_NO_MATCH;
    } инако {
        int32_t node;
        врати (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    врати cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    врати cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(const char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    ако(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    док(length>kMaxBranchLinearSubNodeLength) {
        ако(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } инако {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ако(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            ако(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } инако {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                ако(node<kMinTwoUnitValueLead) {
                    delta=node;
                } инако ако(node<kThreeUnitValueLead) {
                    delta=((node-kMinTwoUnitValueLead)<<16)|*pos++;
                } инако {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            врати result;
        }
        --length;
        pos=skipValue(pos);
    } док(length>1);
    ако(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        врати node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } инако {
        stop();
        врати USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(const char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    за(;;) {
        ако(node<kMinLinearMatch) {
            врати branchNext(pos, node, uchar);
        } инако ако(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ако(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                врати (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } инако {
                // No match.
                break;
            }
        } инако ако(node&kValueIsFinal) {
            // No further matching units.
            break;
        } инако {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    врати USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    const char16_t *pos=pos_;
    ако(pos==nullptr) {
        врати USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ако(length>=0) {
        // Remaining part of a linear-match node.
        ако(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            врати (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } инако {
            stop();
            врати USTRINGTRIE_NO_MATCH;
        }
    }
    врати nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    const char16_t *s=ptr;
    ако(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        врати current();
    }
    const char16_t *pos=pos_;
    ако(pos==nullptr) {
        врати USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    за(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        ако(sLength<0) {
            за(;;) {
                ако((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    врати (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ако(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                ако(uchar!=*pos) {
                    stop();
                    врати USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } инако {
            за(;;) {
                ако(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    врати (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                ако(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                ако(uchar!=*pos) {
                    stop();
                    врати USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        за(;;) {
            ако(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                ако(result==USTRINGTRIE_NO_MATCH) {
                    врати USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                ако(sLength<0) {
                    ако((uchar=*s++)==0) {
                        врати result;
                    }
                } инако {
                    ако(sLength==0) {
                        врати result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                ако(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    врати USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } инако ако(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ако(uchar!=*pos) {
                    stop();
                    врати USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                break;
            } инако ако(node&kValueIsFinal) {
                // No further matching units.
                stop();
                врати USTRINGTRIE_NO_MATCH;
            } инако {
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
    док(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        ако(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            врати nullptr;
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
        ако(isFinal) {
            ако(haveUniqueValue) {
                ако(value!=uniqueValue) {
                    врати nullptr;
                }
            } инако {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } инако {
            ако(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                врати nullptr;
            }
            haveUniqueValue=true;
        }
    } док(--length>1);
    врати pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(const char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    за(;;) {
        ако(node<kMinLinearMatch) {
            ако(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ако(pos==nullptr) {
                врати false;
            }
            haveUniqueValue=true;
            node=*pos++;
        } инако ако(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } инако {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            ако(isFinal) {
                value=readValue(pos, node&0x7fff);
            } инако {
                value=readNodeValue(pos, node);
            }
            ако(haveUniqueValue) {
                ако(value!=uniqueValue) {
                    врати false;
                }
            } инако {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ако(isFinal) {
                врати true;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) const {
    const char16_t *pos=pos_;
    ако(pos==nullptr) {
        врати 0;
    }
    ако(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        врати 1;
    }
    int32_t node=*pos++;
    ако(node>=kMinValueLead) {
        ако(node&kValueIsFinal) {
            врати 0;
        } инако {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    ако(node<kMinLinearMatch) {
        ако(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        врати node;
    } инако {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        врати 1;
    }
}

void
UCharsTrie::getNextBranchUChars(const char16_t *pos, int32_t length, Appendable &out) {
    док(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        out.appendCodeUnit(*pos++);
        pos=skipValue(pos);
    } док(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
