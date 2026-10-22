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
    якщо(pos==nullptr) {
        повернути USTRINGTRIE_NO_MATCH;
    } інакше {
        int32_t node;
        повернути (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    повернути cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    повернути cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(const char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    якщо(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    поки(length>kMaxBranchLinearSubNodeLength) {
        якщо(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } інакше {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        якщо(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            якщо(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } інакше {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                якщо(node<kMinTwoUnitValueLead) {
                    delta=node;
                } інакше якщо(node<kThreeUnitValueLead) {
                    delta=((node-kMinTwoUnitValueLead)<<16)|*pos++;
                } інакше {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            повернути result;
        }
        --length;
        pos=skipValue(pos);
    } поки(length>1);
    якщо(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        повернути node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } інакше {
        stop();
        повернути USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(const char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    для(;;) {
        якщо(node<kMinLinearMatch) {
            повернути branchNext(pos, node, uchar);
        } інакше якщо(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            якщо(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                повернути (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } інакше {
                // No match.
                break;
            }
        } інакше якщо(node&kValueIsFinal) {
            // No further matching units.
            break;
        } інакше {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    повернути USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    const char16_t *pos=pos_;
    якщо(pos==nullptr) {
        повернути USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    якщо(length>=0) {
        // Remaining part of a linear-match node.
        якщо(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            повернути (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } інакше {
            stop();
            повернути USTRINGTRIE_NO_MATCH;
        }
    }
    повернути nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    const char16_t *s=ptr;
    якщо(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        повернути current();
    }
    const char16_t *pos=pos_;
    якщо(pos==nullptr) {
        повернути USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    для(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        якщо(sLength<0) {
            для(;;) {
                якщо((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    повернути (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                якщо(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                якщо(uchar!=*pos) {
                    stop();
                    повернути USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } інакше {
            для(;;) {
                якщо(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    повернути (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                якщо(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                якщо(uchar!=*pos) {
                    stop();
                    повернути USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        для(;;) {
            якщо(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                якщо(result==USTRINGTRIE_NO_MATCH) {
                    повернути USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                якщо(sLength<0) {
                    якщо((uchar=*s++)==0) {
                        повернути result;
                    }
                } інакше {
                    якщо(sLength==0) {
                        повернути result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                якщо(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    повернути USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } інакше якщо(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                якщо(uchar!=*pos) {
                    stop();
                    повернути USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                break;
            } інакше якщо(node&kValueIsFinal) {
                // No further matching units.
                stop();
                повернути USTRINGTRIE_NO_MATCH;
            } інакше {
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
    поки(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        якщо(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            повернути nullptr;
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
        якщо(isFinal) {
            якщо(haveUniqueValue) {
                якщо(value!=uniqueValue) {
                    повернути nullptr;
                }
            } інакше {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } інакше {
            якщо(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                повернути nullptr;
            }
            haveUniqueValue=true;
        }
    } поки(--length>1);
    повернути pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(const char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    для(;;) {
        якщо(node<kMinLinearMatch) {
            якщо(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            якщо(pos==nullptr) {
                повернути false;
            }
            haveUniqueValue=true;
            node=*pos++;
        } інакше якщо(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } інакше {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            якщо(isFinal) {
                value=readValue(pos, node&0x7fff);
            } інакше {
                value=readNodeValue(pos, node);
            }
            якщо(haveUniqueValue) {
                якщо(value!=uniqueValue) {
                    повернути false;
                }
            } інакше {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            якщо(isFinal) {
                повернути true;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) const {
    const char16_t *pos=pos_;
    якщо(pos==nullptr) {
        повернути 0;
    }
    якщо(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        повернути 1;
    }
    int32_t node=*pos++;
    якщо(node>=kMinValueLead) {
        якщо(node&kValueIsFinal) {
            повернути 0;
        } інакше {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    якщо(node<kMinLinearMatch) {
        якщо(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        повернути node;
    } інакше {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        повернути 1;
    }
}

void
UCharsTrie::getNextBranchUChars(const char16_t *pos, int32_t length, Appendable &out) {
    поки(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        out.appendCodeUnit(*pos++);
        pos=skipValue(pos);
    } поки(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
