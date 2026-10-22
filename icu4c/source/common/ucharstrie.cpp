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
    གལ་ཏེ(pos==nullptr) {
        ཕྱིར USTRINGTRIE_NO_MATCH;
    } དེ་མིན {
        int32_t node;
        ཕྱིར (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    ཕྱིར cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    ཕྱིར cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(const char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    གལ་ཏེ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    སྐབས(length>kMaxBranchLinearSubNodeLength) {
        གལ་ཏེ(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } དེ་མིན {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        གལ་ཏེ(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            གལ་ཏེ(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } དེ་མིན {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                གལ་ཏེ(node<kMinTwoUnitValueLead) {
                    delta=node;
                } དེ་མིན གལ་ཏེ(node<kThreeUnitValueLead) {
                    delta=((node-kMinTwoUnitValueLead)<<16)|*pos++;
                } དེ་མིན {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ཕྱིར result;
        }
        --length;
        pos=skipValue(pos);
    } སྐབས(length>1);
    གལ་ཏེ(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ཕྱིར node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } དེ་མིན {
        stop();
        ཕྱིར USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(const char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    ཆེད་དུ(;;) {
        གལ་ཏེ(node<kMinLinearMatch) {
            ཕྱིར branchNext(pos, node, uchar);
        } དེ་མིན གལ་ཏེ(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            གལ་ཏེ(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ཕྱིར (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } དེ་མིན {
                // No match.
                bha_nga;
            }
        } དེ་མིན གལ་ཏེ(node&kValueIsFinal) {
            // No further matching units.
            bha_nga;
        } དེ་མིན {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    ཕྱིར USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    const char16_t *pos=pos_;
    གལ་ཏེ(pos==nullptr) {
        ཕྱིར USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    གལ་ཏེ(length>=0) {
        // Remaining part of a linear-match node.
        གལ་ཏེ(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ཕྱིར (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } དེ་མིན {
            stop();
            ཕྱིར USTRINGTRIE_NO_MATCH;
        }
    }
    ཕྱིར nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    const char16_t *s=ptr;
    གལ་ཏེ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ཕྱིར current();
    }
    const char16_t *pos=pos_;
    གལ་ཏེ(pos==nullptr) {
        ཕྱིར USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ཆེད་དུ(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        གལ་ཏེ(sLength<0) {
            ཆེད་དུ(;;) {
                གལ་ཏེ((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ཕྱིར (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                གལ་ཏེ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                གལ་ཏེ(uchar!=*pos) {
                    stop();
                    ཕྱིར USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } དེ་མིན {
            ཆེད་དུ(;;) {
                གལ་ཏེ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ཕྱིར (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                གལ་ཏེ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                གལ་ཏེ(uchar!=*pos) {
                    stop();
                    ཕྱིར USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        ཆེད་དུ(;;) {
            གལ་ཏེ(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                གལ་ཏེ(result==USTRINGTRIE_NO_MATCH) {
                    ཕྱིར USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                གལ་ཏེ(sLength<0) {
                    གལ་ཏེ((uchar=*s++)==0) {
                        ཕྱིར result;
                    }
                } དེ་མིན {
                    གལ་ཏེ(sLength==0) {
                        ཕྱིར result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                གལ་ཏེ(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    ཕྱིར USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } དེ་མིན གལ་ཏེ(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                གལ་ཏེ(uchar!=*pos) {
                    stop();
                    ཕྱིར USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } དེ་མིན གལ་ཏེ(node&kValueIsFinal) {
                // No further matching units.
                stop();
                ཕྱིར USTRINGTRIE_NO_MATCH;
            } དེ་མིན {
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
    སྐབས(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        གལ་ཏེ(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ཕྱིར nullptr;
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
        གལ་ཏེ(isFinal) {
            གལ་ཏེ(haveUniqueValue) {
                གལ་ཏེ(value!=uniqueValue) {
                    ཕྱིར nullptr;
                }
            } དེ་མིན {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } དེ་མིན {
            གལ་ཏེ(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ཕྱིར nullptr;
            }
            haveUniqueValue=true;
        }
    } སྐབས(--length>1);
    ཕྱིར pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(const char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    ཆེད་དུ(;;) {
        གལ་ཏེ(node<kMinLinearMatch) {
            གལ་ཏེ(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            གལ་ཏེ(pos==nullptr) {
                ཕྱིར false;
            }
            haveUniqueValue=true;
            node=*pos++;
        } དེ་མིན གལ་ཏེ(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } དེ་མིན {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            གལ་ཏེ(isFinal) {
                value=readValue(pos, node&0x7fff);
            } དེ་མིན {
                value=readNodeValue(pos, node);
            }
            གལ་ཏེ(haveUniqueValue) {
                གལ་ཏེ(value!=uniqueValue) {
                    ཕྱིར false;
                }
            } དེ་མིན {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            གལ་ཏེ(isFinal) {
                ཕྱིར true;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) const {
    const char16_t *pos=pos_;
    གལ་ཏེ(pos==nullptr) {
        ཕྱིར 0;
    }
    གལ་ཏེ(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        ཕྱིར 1;
    }
    int32_t node=*pos++;
    གལ་ཏེ(node>=kMinValueLead) {
        གལ་ཏེ(node&kValueIsFinal) {
            ཕྱིར 0;
        } དེ་མིན {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    གལ་ཏེ(node<kMinLinearMatch) {
        གལ་ཏེ(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        ཕྱིར node;
    } དེ་མིན {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        ཕྱིར 1;
    }
}

void
UCharsTrie::getNextBranchUChars(const char16_t *pos, int32_t length, Appendable &out) {
    སྐབས(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        out.appendCodeUnit(*pos++);
        pos=skipValue(pos);
    } སྐབས(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
