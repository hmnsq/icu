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
    ຖ້າ(pos==nullptr) {
        ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
    } ບໍ່ດັ່ງນັ້ນ {
        int32_t node;
        ສົ່ງຄືນ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    ສົ່ງຄືນ cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    ສົ່ງຄືນ cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(const char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    ຖ້າ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    ໃນຂະນະທີ່(length>kMaxBranchLinearSubNodeLength) {
        ຖ້າ(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ບໍ່ດັ່ງນັ້ນ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ຖ້າ(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            ຖ້າ(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } ບໍ່ດັ່ງນັ້ນ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                ຖ້າ(node<kMinTwoUnitValueLead) {
                    delta=node;
                } ບໍ່ດັ່ງນັ້ນ ຖ້າ(node<kThreeUnitValueLead) {
                    delta=((node-kMinTwoUnitValueLead)<<16)|*pos++;
                } ບໍ່ດັ່ງນັ້ນ {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ສົ່ງຄືນ result;
        }
        --length;
        pos=skipValue(pos);
    } ໃນຂະນະທີ່(length>1);
    ຖ້າ(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ສົ່ງຄືນ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } ບໍ່ດັ່ງນັ້ນ {
        stop();
        ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(const char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    ສຳລັບ(;;) {
        ຖ້າ(node<kMinLinearMatch) {
            ສົ່ງຄືນ branchNext(pos, node, uchar);
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ຖ້າ(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ສົ່ງຄືນ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } ບໍ່ດັ່ງນັ້ນ {
                // No match.
                bha_nga;
            }
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ(node&kValueIsFinal) {
            // No further matching units.
            bha_nga;
        } ບໍ່ດັ່ງນັ້ນ {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    const char16_t *pos=pos_;
    ຖ້າ(pos==nullptr) {
        ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ຖ້າ(length>=0) {
        // Remaining part of a linear-match node.
        ຖ້າ(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ສົ່ງຄືນ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } ບໍ່ດັ່ງນັ້ນ {
            stop();
            ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
        }
    }
    ສົ່ງຄືນ nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    const char16_t *s=ptr;
    ຖ້າ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ສົ່ງຄືນ current();
    }
    const char16_t *pos=pos_;
    ຖ້າ(pos==nullptr) {
        ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ສຳລັບ(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        ຖ້າ(sLength<0) {
            ສຳລັບ(;;) {
                ຖ້າ((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ສົ່ງຄືນ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ຖ້າ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ຖ້າ(uchar!=*pos) {
                    stop();
                    ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ບໍ່ດັ່ງນັ້ນ {
            ສຳລັບ(;;) {
                ຖ້າ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ສົ່ງຄືນ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                ຖ້າ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ຖ້າ(uchar!=*pos) {
                    stop();
                    ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        ສຳລັບ(;;) {
            ຖ້າ(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                ຖ້າ(result==USTRINGTRIE_NO_MATCH) {
                    ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                ຖ້າ(sLength<0) {
                    ຖ້າ((uchar=*s++)==0) {
                        ສົ່ງຄືນ result;
                    }
                } ບໍ່ດັ່ງນັ້ນ {
                    ຖ້າ(sLength==0) {
                        ສົ່ງຄືນ result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                ຖ້າ(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } ບໍ່ດັ່ງນັ້ນ ຖ້າ(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ຖ້າ(uchar!=*pos) {
                    stop();
                    ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } ບໍ່ດັ່ງນັ້ນ ຖ້າ(node&kValueIsFinal) {
                // No further matching units.
                stop();
                ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
            } ບໍ່ດັ່ງນັ້ນ {
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
    ໃນຂະນະທີ່(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        ຖ້າ(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ສົ່ງຄືນ nullptr;
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
        ຖ້າ(isFinal) {
            ຖ້າ(haveUniqueValue) {
                ຖ້າ(value!=uniqueValue) {
                    ສົ່ງຄືນ nullptr;
                }
            } ບໍ່ດັ່ງນັ້ນ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } ບໍ່ດັ່ງນັ້ນ {
            ຖ້າ(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ສົ່ງຄືນ nullptr;
            }
            haveUniqueValue=true;
        }
    } ໃນຂະນະທີ່(--length>1);
    ສົ່ງຄືນ pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(const char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    ສຳລັບ(;;) {
        ຖ້າ(node<kMinLinearMatch) {
            ຖ້າ(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ຖ້າ(pos==nullptr) {
                ສົ່ງຄືນ false;
            }
            haveUniqueValue=true;
            node=*pos++;
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } ບໍ່ດັ່ງນັ້ນ {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            ຖ້າ(isFinal) {
                value=readValue(pos, node&0x7fff);
            } ບໍ່ດັ່ງນັ້ນ {
                value=readNodeValue(pos, node);
            }
            ຖ້າ(haveUniqueValue) {
                ຖ້າ(value!=uniqueValue) {
                    ສົ່ງຄືນ false;
                }
            } ບໍ່ດັ່ງນັ້ນ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ຖ້າ(isFinal) {
                ສົ່ງຄືນ true;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) const {
    const char16_t *pos=pos_;
    ຖ້າ(pos==nullptr) {
        ສົ່ງຄືນ 0;
    }
    ຖ້າ(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        ສົ່ງຄືນ 1;
    }
    int32_t node=*pos++;
    ຖ້າ(node>=kMinValueLead) {
        ຖ້າ(node&kValueIsFinal) {
            ສົ່ງຄືນ 0;
        } ບໍ່ດັ່ງນັ້ນ {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    ຖ້າ(node<kMinLinearMatch) {
        ຖ້າ(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        ສົ່ງຄືນ node;
    } ບໍ່ດັ່ງນັ້ນ {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        ສົ່ງຄືນ 1;
    }
}

void
UCharsTrie::getNextBranchUChars(const char16_t *pos, int32_t length, Appendable &out) {
    ໃນຂະນະທີ່(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        out.appendCodeUnit(*pos++);
        pos=skipValue(pos);
    } ໃນຂະນະທີ່(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
