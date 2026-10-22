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
    නම්(pos==nullptr) {
        ආපසු USTRINGTRIE_NO_MATCH;
    } නැතහොත් {
        int32_t node;
        ආපසු (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    ආපසු cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    ආපසු cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(const char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    නම්(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    අතරතුර(length>kMaxBranchLinearSubNodeLength) {
        නම්(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } නැතහොත් {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        නම්(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            නම්(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } නැතහොත් {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                නම්(node<kMinTwoUnitValueLead) {
                    delta=node;
                } නැතහොත් නම්(node<kThreeUnitValueLead) {
                    delta=((node-kMinTwoUnitValueLead)<<16)|*pos++;
                } නැතහොත් {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ආපසු result;
        }
        --length;
        pos=skipValue(pos);
    } අතරතුර(length>1);
    නම්(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ආපසු node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } නැතහොත් {
        stop();
        ආපසු USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(const char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    සඳහා(;;) {
        නම්(node<kMinLinearMatch) {
            ආපසු branchNext(pos, node, uchar);
        } නැතහොත් නම්(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            නම්(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ආපසු (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } නැතහොත් {
                // No match.
                bha_nga;
            }
        } නැතහොත් නම්(node&kValueIsFinal) {
            // No further matching units.
            bha_nga;
        } නැතහොත් {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    ආපසු USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    const char16_t *pos=pos_;
    නම්(pos==nullptr) {
        ආපසු USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    නම්(length>=0) {
        // Remaining part of a linear-match node.
        නම්(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ආපසු (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } නැතහොත් {
            stop();
            ආපසු USTRINGTRIE_NO_MATCH;
        }
    }
    ආපසු nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    const char16_t *s=ptr;
    නම්(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ආපසු current();
    }
    const char16_t *pos=pos_;
    නම්(pos==nullptr) {
        ආපසු USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    සඳහා(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        නම්(sLength<0) {
            සඳහා(;;) {
                නම්((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ආපසු (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                නම්(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                නම්(uchar!=*pos) {
                    stop();
                    ආපසු USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } නැතහොත් {
            සඳහා(;;) {
                නම්(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ආපසු (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                නම්(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                නම්(uchar!=*pos) {
                    stop();
                    ආපසු USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        සඳහා(;;) {
            නම්(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                නම්(result==USTRINGTRIE_NO_MATCH) {
                    ආපසු USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                නම්(sLength<0) {
                    නම්((uchar=*s++)==0) {
                        ආපසු result;
                    }
                } නැතහොත් {
                    නම්(sLength==0) {
                        ආපසු result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                නම්(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    ආපසු USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } නැතහොත් නම්(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                නම්(uchar!=*pos) {
                    stop();
                    ආපසු USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } නැතහොත් නම්(node&kValueIsFinal) {
                // No further matching units.
                stop();
                ආපසු USTRINGTRIE_NO_MATCH;
            } නැතහොත් {
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
    අතරතුර(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        නම්(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ආපසු nullptr;
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
        නම්(isFinal) {
            නම්(haveUniqueValue) {
                නම්(value!=uniqueValue) {
                    ආපසු nullptr;
                }
            } නැතහොත් {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } නැතහොත් {
            නම්(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ආපසු nullptr;
            }
            haveUniqueValue=true;
        }
    } අතරතුර(--length>1);
    ආපසු pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(const char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    සඳහා(;;) {
        නම්(node<kMinLinearMatch) {
            නම්(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            නම්(pos==nullptr) {
                ආපසු false;
            }
            haveUniqueValue=true;
            node=*pos++;
        } නැතහොත් නම්(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } නැතහොත් {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            නම්(isFinal) {
                value=readValue(pos, node&0x7fff);
            } නැතහොත් {
                value=readNodeValue(pos, node);
            }
            නම්(haveUniqueValue) {
                නම්(value!=uniqueValue) {
                    ආපසු false;
                }
            } නැතහොත් {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            නම්(isFinal) {
                ආපසු true;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) const {
    const char16_t *pos=pos_;
    නම්(pos==nullptr) {
        ආපසු 0;
    }
    නම්(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        ආපසු 1;
    }
    int32_t node=*pos++;
    නම්(node>=kMinValueLead) {
        නම්(node&kValueIsFinal) {
            ආපසු 0;
        } නැතහොත් {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    නම්(node<kMinLinearMatch) {
        නම්(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        ආපසු node;
    } නැතහොත් {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        ආපසු 1;
    }
}

void
UCharsTrie::getNextBranchUChars(const char16_t *pos, int32_t length, Appendable &out) {
    අතරතුර(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        out.appendCodeUnit(*pos++);
        pos=skipValue(pos);
    } අතරතුර(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
