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
    ᯀᯮᯝ(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } ᯀᯮᯝᯂᯪ {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    ᯀᯮᯞ value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    ᯀᯮᯝ(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } ᯀᯮᯝᯂᯪ {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    ᯀᯮᯞ pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    ᯀᯮᯝ(pos==nullptr) {
        ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
    } ᯀᯮᯝᯂᯪ {
        int32_t node;
        ᯀᯮᯞ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(const uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    ᯀᯮᯝ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    ᯀᯬᯝ(length>kMaxBranchLinearSubNodeLength) {
        ᯀᯮᯝ(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ᯀᯮᯝᯂᯪ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ᯀᯮᯝ(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            ᯀᯮᯝ(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } ᯀᯮᯝᯂᯪ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                ᯀᯮᯝ(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } ᯀᯮᯝᯂᯪ {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ᯀᯮᯞ result;
        }
        --length;
        pos=skipValue(pos);
    } ᯀᯬᯝ(length>1);
    ᯀᯮᯝ(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ᯀᯮᯞ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } ᯀᯮᯝᯂᯪ {
        stop();
        ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    for(;;) {
        int32_t node=*pos++;
        ᯀᯮᯝ(node<kMinLinearMatch) {
            ᯀᯮᯞ branchNext(pos, node, inByte);
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ᯀᯮᯝ(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ᯀᯮᯞ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } ᯀᯮᯝᯂᯪ {
                // No match.
                bha_nga;
            }
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(node&kValueIsFinal) {
            // No further matching bytes.
            bha_nga;
        } ᯀᯮᯝᯂᯪ {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    ᯀᯮᯝ(pos==nullptr) {
        ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
    }
    ᯀᯮᯝ(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ᯀᯮᯝ(length>=0) {
        // Remaining part of a linear-match node.
        ᯀᯮᯝ(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ᯀᯮᯞ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } ᯀᯮᯝᯂᯪ {
            stop();
            ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
        }
    }
    ᯀᯮᯞ nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const var_na *s, int32_t sLength) {
    ᯀᯮᯝ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ᯀᯮᯞ current();
    }
    const uint8_t *pos=pos_;
    ᯀᯮᯝ(pos==nullptr) {
        ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    for(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        ᯀᯮᯝ(sLength<0) {
            for(;;) {
                ᯀᯮᯝ((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᯀᯮᯞ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ᯀᯮᯝ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᯀᯮᯝ(inByte!=*pos) {
                    stop();
                    ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ᯀᯮᯝᯂᯪ {
            for(;;) {
                ᯀᯮᯝ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᯀᯮᯞ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                ᯀᯮᯝ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᯀᯮᯝ(inByte!=*pos) {
                    stop();
                    ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        for(;;) {
            int32_t node=*pos++;
            ᯀᯮᯝ(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                ᯀᯮᯝ(result==USTRINGTRIE_NO_MATCH) {
                    ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                ᯀᯮᯝ(sLength<0) {
                    ᯀᯮᯝ((inByte=*s++)==0) {
                        ᯀᯮᯞ result;
                    }
                } ᯀᯮᯝᯂᯪ {
                    ᯀᯮᯝ(sLength==0) {
                        ᯀᯮᯞ result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                ᯀᯮᯝ(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ᯀᯮᯝ(inByte!=*pos) {
                    stop();
                    ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                ᯀᯮᯞ USTRINGTRIE_NO_MATCH;
            } ᯀᯮᯝᯂᯪ {
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
    ᯀᯬᯝ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        ᯀᯮᯝ(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ᯀᯮᯞ nullptr;
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
        ᯀᯮᯝ(isFinal) {
            ᯀᯮᯝ(haveUniqueValue) {
                ᯀᯮᯝ(value!=uniqueValue) {
                    ᯀᯮᯞ nullptr;
                }
            } ᯀᯮᯝᯂᯪ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } ᯀᯮᯝᯂᯪ {
            ᯀᯮᯝ(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ᯀᯮᯞ nullptr;
            }
            haveUniqueValue=true;
        }
    } ᯀᯬᯝ(--length>1);
    ᯀᯮᯞ pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(const uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    for(;;) {
        int32_t node=*pos++;
        ᯀᯮᯝ(node<kMinLinearMatch) {
            ᯀᯮᯝ(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ᯀᯮᯝ(pos==nullptr) {
                ᯀᯮᯞ false;
            }
            haveUniqueValue=true;
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } ᯀᯮᯝᯂᯪ {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            ᯀᯮᯝ(haveUniqueValue) {
                ᯀᯮᯝ(value!=uniqueValue) {
                    ᯀᯮᯞ false;
                }
            } ᯀᯮᯝᯂᯪ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ᯀᯮᯝ(isFinal) {
                ᯀᯮᯞ true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    ᯀᯮᯝ(pos==nullptr) {
        ᯀᯮᯞ 0;
    }
    ᯀᯮᯝ(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ᯀᯮᯞ 1;
    }
    int32_t node=*pos++;
    ᯀᯮᯝ(node>=kMinValueLead) {
        ᯀᯮᯝ(node&kValueIsFinal) {
            ᯀᯮᯞ 0;
        } ᯀᯮᯝᯂᯪ {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    ᯀᯮᯝ(node<kMinLinearMatch) {
        ᯀᯮᯝ(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        ᯀᯮᯞ node;
    } ᯀᯮᯝᯂᯪ {
        // First byte of the linear-match node.
        append(out, *pos);
        ᯀᯮᯞ 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    ᯀᯬᯝ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } ᯀᯬᯝ(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, puur_naa_nka c) {
    var_na ch = static_cast<var_na>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
