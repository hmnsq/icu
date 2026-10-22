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
    ប្រសិនបើ(pos==nullptr) {
        ត្រឡប់ USTRINGTRIE_NO_MATCH;
    } បើមិនដូច្នេះ {
        int32_t node;
        ត្រឡប់ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    ត្រឡប់ cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    ត្រឡប់ cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(const char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    ប្រសិនបើ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    ខណៈពេល(length>kMaxBranchLinearSubNodeLength) {
        ប្រសិនបើ(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } បើមិនដូច្នេះ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ប្រសិនបើ(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            ប្រសិនបើ(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } បើមិនដូច្នេះ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                ប្រសិនបើ(node<kMinTwoUnitValueLead) {
                    delta=node;
                } បើមិនដូច្នេះ ប្រសិនបើ(node<kThreeUnitValueLead) {
                    delta=((node-kMinTwoUnitValueLead)<<16)|*pos++;
                } បើមិនដូច្នេះ {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ត្រឡប់ result;
        }
        --length;
        pos=skipValue(pos);
    } ខណៈពេល(length>1);
    ប្រសិនបើ(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ត្រឡប់ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } បើមិនដូច្នេះ {
        stop();
        ត្រឡប់ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(const char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    សម្រាប់(;;) {
        ប្រសិនបើ(node<kMinLinearMatch) {
            ត្រឡប់ branchNext(pos, node, uchar);
        } បើមិនដូច្នេះ ប្រសិនបើ(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ប្រសិនបើ(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ត្រឡប់ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } បើមិនដូច្នេះ {
                // No match.
                bha_nga;
            }
        } បើមិនដូច្នេះ ប្រសិនបើ(node&kValueIsFinal) {
            // No further matching units.
            bha_nga;
        } បើមិនដូច្នេះ {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    ត្រឡប់ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    const char16_t *pos=pos_;
    ប្រសិនបើ(pos==nullptr) {
        ត្រឡប់ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ប្រសិនបើ(length>=0) {
        // Remaining part of a linear-match node.
        ប្រសិនបើ(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ត្រឡប់ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } បើមិនដូច្នេះ {
            stop();
            ត្រឡប់ USTRINGTRIE_NO_MATCH;
        }
    }
    ត្រឡប់ nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    const char16_t *s=ptr;
    ប្រសិនបើ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ត្រឡប់ current();
    }
    const char16_t *pos=pos_;
    ប្រសិនបើ(pos==nullptr) {
        ត្រឡប់ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    សម្រាប់(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        ប្រសិនបើ(sLength<0) {
            សម្រាប់(;;) {
                ប្រសិនបើ((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ត្រឡប់ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ប្រសិនបើ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ប្រសិនបើ(uchar!=*pos) {
                    stop();
                    ត្រឡប់ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } បើមិនដូច្នេះ {
            សម្រាប់(;;) {
                ប្រសិនបើ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ត្រឡប់ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                ប្រសិនបើ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ប្រសិនបើ(uchar!=*pos) {
                    stop();
                    ត្រឡប់ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        សម្រាប់(;;) {
            ប្រសិនបើ(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                ប្រសិនបើ(result==USTRINGTRIE_NO_MATCH) {
                    ត្រឡប់ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                ប្រសិនបើ(sLength<0) {
                    ប្រសិនបើ((uchar=*s++)==0) {
                        ត្រឡប់ result;
                    }
                } បើមិនដូច្នេះ {
                    ប្រសិនបើ(sLength==0) {
                        ត្រឡប់ result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                ប្រសិនបើ(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    ត្រឡប់ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } បើមិនដូច្នេះ ប្រសិនបើ(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ប្រសិនបើ(uchar!=*pos) {
                    stop();
                    ត្រឡប់ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } បើមិនដូច្នេះ ប្រសិនបើ(node&kValueIsFinal) {
                // No further matching units.
                stop();
                ត្រឡប់ USTRINGTRIE_NO_MATCH;
            } បើមិនដូច្នេះ {
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
    ខណៈពេល(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        ប្រសិនបើ(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ត្រឡប់ nullptr;
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
        ប្រសិនបើ(isFinal) {
            ប្រសិនបើ(haveUniqueValue) {
                ប្រសិនបើ(value!=uniqueValue) {
                    ត្រឡប់ nullptr;
                }
            } បើមិនដូច្នេះ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } បើមិនដូច្នេះ {
            ប្រសិនបើ(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ត្រឡប់ nullptr;
            }
            haveUniqueValue=true;
        }
    } ខណៈពេល(--length>1);
    ត្រឡប់ pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(const char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    សម្រាប់(;;) {
        ប្រសិនបើ(node<kMinLinearMatch) {
            ប្រសិនបើ(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ប្រសិនបើ(pos==nullptr) {
                ត្រឡប់ false;
            }
            haveUniqueValue=true;
            node=*pos++;
        } បើមិនដូច្នេះ ប្រសិនបើ(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } បើមិនដូច្នេះ {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            ប្រសិនបើ(isFinal) {
                value=readValue(pos, node&0x7fff);
            } បើមិនដូច្នេះ {
                value=readNodeValue(pos, node);
            }
            ប្រសិនបើ(haveUniqueValue) {
                ប្រសិនបើ(value!=uniqueValue) {
                    ត្រឡប់ false;
                }
            } បើមិនដូច្នេះ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ប្រសិនបើ(isFinal) {
                ត្រឡប់ true;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) const {
    const char16_t *pos=pos_;
    ប្រសិនបើ(pos==nullptr) {
        ត្រឡប់ 0;
    }
    ប្រសិនបើ(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        ត្រឡប់ 1;
    }
    int32_t node=*pos++;
    ប្រសិនបើ(node>=kMinValueLead) {
        ប្រសិនបើ(node&kValueIsFinal) {
            ត្រឡប់ 0;
        } បើមិនដូច្នេះ {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    ប្រសិនបើ(node<kMinLinearMatch) {
        ប្រសិនបើ(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        ត្រឡប់ node;
    } បើមិនដូច្នេះ {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        ត្រឡប់ 1;
    }
}

void
UCharsTrie::getNextBranchUChars(const char16_t *pos, int32_t length, Appendable &out) {
    ខណៈពេល(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        out.appendCodeUnit(*pos++);
        pos=skipValue(pos);
    } ខណៈពេល(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
