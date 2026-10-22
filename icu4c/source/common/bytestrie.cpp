// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  bytestrie.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010sep25
*   created by: Markus W. Scherer
*/

#include "unicode/utypes.h"
#include "unicode/bytestream.h"
#include "unicode/bytestrie.h"
#include "unicode/uobject.h"
#include "cmemory.h"
#include "uassert.h"

U_NAMESPACE_BEGIN

BytesTrie::~BytesTrie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
BytesTrie::readValue(const uint8_t *pos, int32_t leadByte) {
    int32_t value;
    ᰀᰧ(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } ᰀᰧᰀ ᰀᰧ(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } ᰀᰧᰀ ᰀᰧ(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } ᰀᰧᰀ ᰀᰧ(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } ᰀᰧᰀ {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    ᰀᰧᰌ value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    ᰀᰧ(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } ᰀᰧᰀ ᰀᰧ(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } ᰀᰧᰀ ᰀᰧ(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } ᰀᰧᰀ ᰀᰧ(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } ᰀᰧᰀ {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    ᰀᰧᰌ pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    ᰀᰧ(pos==nullptr) {
        ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
    } ᰀᰧᰀ {
        int32_t node;
        ᰀᰧᰌ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(const uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    ᰀᰧ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    ᰀᰬ(length>kMaxBranchLinearSubNodeLength) {
        ᰀᰧ(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ᰀᰧᰀ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ᰀᰧ(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            ᰀᰧ(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } ᰀᰧᰀ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                ᰀᰧ(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } ᰀᰧᰀ ᰀᰧ(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } ᰀᰧᰀ ᰀᰧ(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } ᰀᰧᰀ ᰀᰧ(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } ᰀᰧᰀ {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ᰀᰧᰌ result;
        }
        --length;
        pos=skipValue(pos);
    } ᰀᰬ(length>1);
    ᰀᰧ(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ᰀᰧᰌ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } ᰀᰧᰀ {
        stop();
        ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    for(;;) {
        int32_t node=*pos++;
        ᰀᰧ(node<kMinLinearMatch) {
            ᰀᰧᰌ branchNext(pos, node, inByte);
        } ᰀᰧᰀ ᰀᰧ(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ᰀᰧ(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ᰀᰧᰌ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } ᰀᰧᰀ {
                // No match.
                bha_nga;
            }
        } ᰀᰧᰀ ᰀᰧ(node&kValueIsFinal) {
            // No further matching bytes.
            bha_nga;
        } ᰀᰧᰀ {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    ᰀᰧ(pos==nullptr) {
        ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
    }
    ᰀᰧ(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ᰀᰧ(length>=0) {
        // Remaining part of a linear-match node.
        ᰀᰧ(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ᰀᰧᰌ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } ᰀᰧᰀ {
            stop();
            ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
        }
    }
    ᰀᰧᰌ nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const var_na *s, int32_t sLength) {
    ᰀᰧ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ᰀᰧᰌ current();
    }
    const uint8_t *pos=pos_;
    ᰀᰧ(pos==nullptr) {
        ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    for(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        ᰀᰧ(sLength<0) {
            for(;;) {
                ᰀᰧ((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᰀᰧᰌ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ᰀᰧ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᰀᰧ(inByte!=*pos) {
                    stop();
                    ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ᰀᰧᰀ {
            for(;;) {
                ᰀᰧ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᰀᰧᰌ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                ᰀᰧ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᰀᰧ(inByte!=*pos) {
                    stop();
                    ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        for(;;) {
            int32_t node=*pos++;
            ᰀᰧ(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                ᰀᰧ(result==USTRINGTRIE_NO_MATCH) {
                    ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                ᰀᰧ(sLength<0) {
                    ᰀᰧ((inByte=*s++)==0) {
                        ᰀᰧᰌ result;
                    }
                } ᰀᰧᰀ {
                    ᰀᰧ(sLength==0) {
                        ᰀᰧᰌ result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                ᰀᰧ(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } ᰀᰧᰀ ᰀᰧ(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ᰀᰧ(inByte!=*pos) {
                    stop();
                    ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } ᰀᰧᰀ ᰀᰧ(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                ᰀᰧᰌ USTRINGTRIE_NO_MATCH;
            } ᰀᰧᰀ {
                // Skip intermediate value.
                pos=skipValue(pos, node);
                // The next node must not also be a value node.
                U_ASSERT(*pos<kMinValueLead);
            }
        }
    }
}

const uint8_t*
BytesTrie::findUniqueValueFromBranch(const uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool haveUniqueValue,
                                     int32_t& uniqueValue) {
    ᰀᰬ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        ᰀᰧ(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ᰀᰧᰌ nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t node=*pos++;
        UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
        int32_t value=readValue(pos, node>>1);
        pos=skipValue(pos, node);
        ᰀᰧ(isFinal) {
            ᰀᰧ(haveUniqueValue) {
                ᰀᰧ(value!=uniqueValue) {
                    ᰀᰧᰌ nullptr;
                }
            } ᰀᰧᰀ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } ᰀᰧᰀ {
            ᰀᰧ(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ᰀᰧᰌ nullptr;
            }
            haveUniqueValue=true;
        }
    } ᰀᰬ(--length>1);
    ᰀᰧᰌ pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(const uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    for(;;) {
        int32_t node=*pos++;
        ᰀᰧ(node<kMinLinearMatch) {
            ᰀᰧ(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ᰀᰧ(pos==nullptr) {
                ᰀᰧᰌ false;
            }
            haveUniqueValue=true;
        } ᰀᰧᰀ ᰀᰧ(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } ᰀᰧᰀ {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            ᰀᰧ(haveUniqueValue) {
                ᰀᰧ(value!=uniqueValue) {
                    ᰀᰧᰌ false;
                }
            } ᰀᰧᰀ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ᰀᰧ(isFinal) {
                ᰀᰧᰌ true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    ᰀᰧ(pos==nullptr) {
        ᰀᰧᰌ 0;
    }
    ᰀᰧ(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ᰀᰧᰌ 1;
    }
    int32_t node=*pos++;
    ᰀᰧ(node>=kMinValueLead) {
        ᰀᰧ(node&kValueIsFinal) {
            ᰀᰧᰌ 0;
        } ᰀᰧᰀ {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    ᰀᰧ(node<kMinLinearMatch) {
        ᰀᰧ(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        ᰀᰧᰌ node;
    } ᰀᰧᰀ {
        // First byte of the linear-match node.
        append(out, *pos);
        ᰀᰧᰌ 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    ᰀᰬ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } ᰀᰬ(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, puur_naa_nka c) {
    var_na ch = static_cast<var_na>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
