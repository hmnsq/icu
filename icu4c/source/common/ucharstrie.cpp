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
    အကယ်၍(pos==nullptr) {
        ပြန်ပေး USTRINGTRIE_NO_MATCH;
    } မဟုတ်ပါက {
        int32_t node;
        ပြန်ပေး (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    ပြန်ပေး cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    ပြန်ပေး cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(const char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    အကယ်၍(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    နေစဉ်(length>kMaxBranchLinearSubNodeLength) {
        အကယ်၍(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } မဟုတ်ပါက {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        အကယ်၍(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            အကယ်၍(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } မဟုတ်ပါက {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                အကယ်၍(node<kMinTwoUnitValueLead) {
                    delta=node;
                } မဟုတ်ပါက အကယ်၍(node<kThreeUnitValueLead) {
                    delta=((node-kMinTwoUnitValueLead)<<16)|*pos++;
                } မဟုတ်ပါက {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ပြန်ပေး result;
        }
        --length;
        pos=skipValue(pos);
    } နေစဉ်(length>1);
    အကယ်၍(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ပြန်ပေး node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } မဟုတ်ပါက {
        stop();
        ပြန်ပေး USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(const char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    အတွက်(;;) {
        အကယ်၍(node<kMinLinearMatch) {
            ပြန်ပေး branchNext(pos, node, uchar);
        } မဟုတ်ပါက အကယ်၍(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            အကယ်၍(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ပြန်ပေး (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } မဟုတ်ပါက {
                // No match.
                bha_nga;
            }
        } မဟုတ်ပါက အကယ်၍(node&kValueIsFinal) {
            // No further matching units.
            bha_nga;
        } မဟုတ်ပါက {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    ပြန်ပေး USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    const char16_t *pos=pos_;
    အကယ်၍(pos==nullptr) {
        ပြန်ပေး USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    အကယ်၍(length>=0) {
        // Remaining part of a linear-match node.
        အကယ်၍(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ပြန်ပေး (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } မဟုတ်ပါက {
            stop();
            ပြန်ပေး USTRINGTRIE_NO_MATCH;
        }
    }
    ပြန်ပေး nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    const char16_t *s=ptr;
    အကယ်၍(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ပြန်ပေး current();
    }
    const char16_t *pos=pos_;
    အကယ်၍(pos==nullptr) {
        ပြန်ပေး USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    အတွက်(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        အကယ်၍(sLength<0) {
            အတွက်(;;) {
                အကယ်၍((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ပြန်ပေး (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                အကယ်၍(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                အကယ်၍(uchar!=*pos) {
                    stop();
                    ပြန်ပေး USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } မဟုတ်ပါက {
            အတွက်(;;) {
                အကယ်၍(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ပြန်ပေး (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                အကယ်၍(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                အကယ်၍(uchar!=*pos) {
                    stop();
                    ပြန်ပေး USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        အတွက်(;;) {
            အကယ်၍(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                အကယ်၍(result==USTRINGTRIE_NO_MATCH) {
                    ပြန်ပေး USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                အကယ်၍(sLength<0) {
                    အကယ်၍((uchar=*s++)==0) {
                        ပြန်ပေး result;
                    }
                } မဟုတ်ပါက {
                    အကယ်၍(sLength==0) {
                        ပြန်ပေး result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                အကယ်၍(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    ပြန်ပေး USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } မဟုတ်ပါက အကယ်၍(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                အကယ်၍(uchar!=*pos) {
                    stop();
                    ပြန်ပေး USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } မဟုတ်ပါက အကယ်၍(node&kValueIsFinal) {
                // No further matching units.
                stop();
                ပြန်ပေး USTRINGTRIE_NO_MATCH;
            } မဟုတ်ပါက {
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
    နေစဉ်(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        အကယ်၍(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ပြန်ပေး nullptr;
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
        အကယ်၍(isFinal) {
            အကယ်၍(haveUniqueValue) {
                အကယ်၍(value!=uniqueValue) {
                    ပြန်ပေး nullptr;
                }
            } မဟုတ်ပါက {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } မဟုတ်ပါက {
            အကယ်၍(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ပြန်ပေး nullptr;
            }
            haveUniqueValue=true;
        }
    } နေစဉ်(--length>1);
    ပြန်ပေး pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(const char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    အတွက်(;;) {
        အကယ်၍(node<kMinLinearMatch) {
            အကယ်၍(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            အကယ်၍(pos==nullptr) {
                ပြန်ပေး false;
            }
            haveUniqueValue=true;
            node=*pos++;
        } မဟုတ်ပါက အကယ်၍(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } မဟုတ်ပါက {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            အကယ်၍(isFinal) {
                value=readValue(pos, node&0x7fff);
            } မဟုတ်ပါက {
                value=readNodeValue(pos, node);
            }
            အကယ်၍(haveUniqueValue) {
                အကယ်၍(value!=uniqueValue) {
                    ပြန်ပေး false;
                }
            } မဟုတ်ပါက {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            အကယ်၍(isFinal) {
                ပြန်ပေး true;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) const {
    const char16_t *pos=pos_;
    အကယ်၍(pos==nullptr) {
        ပြန်ပေး 0;
    }
    အကယ်၍(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        ပြန်ပေး 1;
    }
    int32_t node=*pos++;
    အကယ်၍(node>=kMinValueLead) {
        အကယ်၍(node&kValueIsFinal) {
            ပြန်ပေး 0;
        } မဟုတ်ပါက {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    အကယ်၍(node<kMinLinearMatch) {
        အကယ်၍(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        ပြန်ပေး node;
    } မဟုတ်ပါက {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        ပြန်ပေး 1;
    }
}

void
UCharsTrie::getNextBranchUChars(const char16_t *pos, int32_t length, Appendable &out) {
    နေစဉ်(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        out.appendCodeUnit(*pos++);
        pos=skipValue(pos);
    } နေစဉ်(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
