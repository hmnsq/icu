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
    གལ་སྲིད(pos==nullptr) {
        ལོག USTRINGTRIE_NO_MATCH;
    } དེ་མིན {
        int32_t node;
        ལོག (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    ལོག cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    ལོག cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(const char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    གལ་སྲིད(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    སྐབས(length>kMaxBranchLinearSubNodeLength) {
        གལ་སྲིད(uchar<*pos++) {
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
        གལ་སྲིད(uchar==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            གལ་སྲིད(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } དེ་མིན {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                གལ་སྲིད(node<kMinTwoUnitValueLead) {
                    delta=node;
                } དེ་མིན གལ་སྲིད(node<kThreeUnitValueLead) {
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
            ལོག result;
        }
        --length;
        pos=skipValue(pos);
    } སྐབས(length>1);
    གལ་སྲིད(uchar==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ལོག node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } དེ་མིན {
        stop();
        ལོག USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(const char16_t *pos, int32_t uchar) {
    int32_t node=*pos++;
    དོན་ལུ(;;) {
        གལ་སྲིད(node<kMinLinearMatch) {
            ལོག branchNext(pos, node, uchar);
        } དེ་མིན གལ་སྲིད(node<kMinValueLead) {
            // Match the first of length+1 units.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            གལ་སྲིད(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ལོག (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } དེ་མིན {
                // No match.
                bha_nga;
            }
        } དེ་མིན གལ་སྲིད(node&kValueIsFinal) {
            // No further matching units.
            bha_nga;
        } དེ་མིན {
            // Skip intermediate value.
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    stop();
    ལོག USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    const char16_t *pos=pos_;
    གལ་སྲིད(pos==nullptr) {
        ལོག USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    གལ་སྲིད(length>=0) {
        // Remaining part of a linear-match node.
        གལ་སྲིད(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ལོག (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } དེ་མིན {
            stop();
            ལོག USTRINGTRIE_NO_MATCH;
        }
    }
    ལོག nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    const char16_t *s=ptr;
    གལ་སྲིད(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ལོག current();
    }
    const char16_t *pos=pos_;
    གལ་སྲིད(pos==nullptr) {
        ལོག USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    དོན་ལུ(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        གལ་སྲིད(sLength<0) {
            དོན་ལུ(;;) {
                གལ་སྲིད((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ལོག (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                གལ་སྲིད(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                གལ་སྲིད(uchar!=*pos) {
                    stop();
                    ལོག USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } དེ་མིན {
            དོན་ལུ(;;) {
                གལ་སྲིད(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ལོག (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                uchar=*s++;
                --sLength;
                གལ་སྲིད(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                གལ་སྲིད(uchar!=*pos) {
                    stop();
                    ལོག USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t node=*pos++;
        དོན་ལུ(;;) {
            གལ་སྲིད(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, uchar);
                གལ་སྲིད(result==USTRINGTRIE_NO_MATCH) {
                    ལོག USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                གལ་སྲིད(sLength<0) {
                    གལ་སྲིད((uchar=*s++)==0) {
                        ལོག result;
                    }
                } དེ་མིན {
                    གལ་སྲིད(sLength==0) {
                        ལོག result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                གལ་སྲིད(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching units.
                    stop();
                    ལོག USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                node=*pos++;
            } དེ་མིན གལ་སྲིད(node<kMinValueLead) {
                // Match length+1 units.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                གལ་སྲིད(uchar!=*pos) {
                    stop();
                    ལོག USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } དེ་མིན གལ་སྲིད(node&kValueIsFinal) {
                // No further matching units.
                stop();
                ལོག USTRINGTRIE_NO_MATCH;
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
        གལ་སྲིད(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ལོག nullptr;
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
        གལ་སྲིད(isFinal) {
            གལ་སྲིད(haveUniqueValue) {
                གལ་སྲིད(value!=uniqueValue) {
                    ལོག nullptr;
                }
            } དེ་མིན {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } དེ་མིན {
            གལ་སྲིད(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ལོག nullptr;
            }
            haveUniqueValue=true;
        }
    } སྐབས(--length>1);
    ལོག pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::findUniqueValue(const char16_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    int32_t node=*pos++;
    དོན་ལུ(;;) {
        གལ་སྲིད(node<kMinLinearMatch) {
            གལ་སྲིད(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            གལ་སྲིད(pos==nullptr) {
                ལོག false;
            }
            haveUniqueValue=true;
            node=*pos++;
        } དེ་མིན གལ་སྲིད(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match units.
            node=*pos++;
        } དེ་མིན {
            UBool isFinal = static_cast<UBool>(node >> 15);
            int32_t value;
            གལ་སྲིད(isFinal) {
                value=readValue(pos, node&0x7fff);
            } དེ་མིན {
                value=readNodeValue(pos, node);
            }
            གལ་སྲིད(haveUniqueValue) {
                གལ་སྲིད(value!=uniqueValue) {
                    ལོག false;
                }
            } དེ་མིན {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            གལ་སྲིད(isFinal) {
                ལོག true;
            }
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) const {
    const char16_t *pos=pos_;
    གལ་སྲིད(pos==nullptr) {
        ལོག 0;
    }
    གལ་སྲིད(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        ལོག 1;
    }
    int32_t node=*pos++;
    གལ་སྲིད(node>=kMinValueLead) {
        གལ་སྲིད(node&kValueIsFinal) {
            ལོག 0;
        } དེ་མིན {
            pos=skipNodeValue(pos, node);
            node&=kNodeTypeMask;
        }
    }
    གལ་སྲིད(node<kMinLinearMatch) {
        གལ་སྲིད(node==0) {
            node=*pos++;
        }
        out.reserveAppendCapacity(++node);
        getNextBranchUChars(pos, node, out);
        ལོག node;
    } དེ་མིན {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        ལོག 1;
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
