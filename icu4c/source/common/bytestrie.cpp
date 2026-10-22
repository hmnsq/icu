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
    ᨀᨘ(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } ᨀᨘᨁ ᨀᨘ(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } ᨀᨘᨁ ᨀᨘ(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } ᨀᨘᨁ ᨀᨘ(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } ᨀᨘᨁ {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    ᨀᨘᨒ value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    ᨀᨘ(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } ᨀᨘᨁ ᨀᨘ(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } ᨀᨘᨁ ᨀᨘ(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } ᨀᨘᨁ ᨀᨘ(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } ᨀᨘᨁ {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    ᨀᨘᨒ pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    ᨀᨘ(pos==nullptr) {
        ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
    } ᨀᨘᨁ {
        int32_t node;
        ᨀᨘᨒ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(const uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    ᨀᨘ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    ᨀᨛ(length>kMaxBranchLinearSubNodeLength) {
        ᨀᨘ(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ᨀᨘᨁ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ᨀᨘ(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            ᨀᨘ(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } ᨀᨘᨁ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                ᨀᨘ(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } ᨀᨘᨁ ᨀᨘ(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } ᨀᨘᨁ ᨀᨘ(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } ᨀᨘᨁ ᨀᨘ(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } ᨀᨘᨁ {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ᨀᨘᨒ result;
        }
        --length;
        pos=skipValue(pos);
    } ᨀᨛ(length>1);
    ᨀᨘ(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ᨀᨘᨒ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } ᨀᨘᨁ {
        stop();
        ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    for(;;) {
        int32_t node=*pos++;
        ᨀᨘ(node<kMinLinearMatch) {
            ᨀᨘᨒ branchNext(pos, node, inByte);
        } ᨀᨘᨁ ᨀᨘ(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ᨀᨘ(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ᨀᨘᨒ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } ᨀᨘᨁ {
                // No match.
                bha_nga;
            }
        } ᨀᨘᨁ ᨀᨘ(node&kValueIsFinal) {
            // No further matching bytes.
            bha_nga;
        } ᨀᨘᨁ {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    ᨀᨘ(pos==nullptr) {
        ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
    }
    ᨀᨘ(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ᨀᨘ(length>=0) {
        // Remaining part of a linear-match node.
        ᨀᨘ(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ᨀᨘᨒ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } ᨀᨘᨁ {
            stop();
            ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
        }
    }
    ᨀᨘᨒ nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const var_na *s, int32_t sLength) {
    ᨀᨘ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ᨀᨘᨒ current();
    }
    const uint8_t *pos=pos_;
    ᨀᨘ(pos==nullptr) {
        ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    for(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        ᨀᨘ(sLength<0) {
            for(;;) {
                ᨀᨘ((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᨀᨘᨒ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ᨀᨘ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᨀᨘ(inByte!=*pos) {
                    stop();
                    ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ᨀᨘᨁ {
            for(;;) {
                ᨀᨘ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᨀᨘᨒ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                ᨀᨘ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᨀᨘ(inByte!=*pos) {
                    stop();
                    ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        for(;;) {
            int32_t node=*pos++;
            ᨀᨘ(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                ᨀᨘ(result==USTRINGTRIE_NO_MATCH) {
                    ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                ᨀᨘ(sLength<0) {
                    ᨀᨘ((inByte=*s++)==0) {
                        ᨀᨘᨒ result;
                    }
                } ᨀᨘᨁ {
                    ᨀᨘ(sLength==0) {
                        ᨀᨘᨒ result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                ᨀᨘ(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } ᨀᨘᨁ ᨀᨘ(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ᨀᨘ(inByte!=*pos) {
                    stop();
                    ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } ᨀᨘᨁ ᨀᨘ(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                ᨀᨘᨒ USTRINGTRIE_NO_MATCH;
            } ᨀᨘᨁ {
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
    ᨀᨛ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        ᨀᨘ(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ᨀᨘᨒ nullptr;
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
        ᨀᨘ(isFinal) {
            ᨀᨘ(haveUniqueValue) {
                ᨀᨘ(value!=uniqueValue) {
                    ᨀᨘᨒ nullptr;
                }
            } ᨀᨘᨁ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } ᨀᨘᨁ {
            ᨀᨘ(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ᨀᨘᨒ nullptr;
            }
            haveUniqueValue=true;
        }
    } ᨀᨛ(--length>1);
    ᨀᨘᨒ pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(const uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    for(;;) {
        int32_t node=*pos++;
        ᨀᨘ(node<kMinLinearMatch) {
            ᨀᨘ(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ᨀᨘ(pos==nullptr) {
                ᨀᨘᨒ false;
            }
            haveUniqueValue=true;
        } ᨀᨘᨁ ᨀᨘ(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } ᨀᨘᨁ {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            ᨀᨘ(haveUniqueValue) {
                ᨀᨘ(value!=uniqueValue) {
                    ᨀᨘᨒ false;
                }
            } ᨀᨘᨁ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ᨀᨘ(isFinal) {
                ᨀᨘᨒ true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    ᨀᨘ(pos==nullptr) {
        ᨀᨘᨒ 0;
    }
    ᨀᨘ(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ᨀᨘᨒ 1;
    }
    int32_t node=*pos++;
    ᨀᨘ(node>=kMinValueLead) {
        ᨀᨘ(node&kValueIsFinal) {
            ᨀᨘᨒ 0;
        } ᨀᨘᨁ {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    ᨀᨘ(node<kMinLinearMatch) {
        ᨀᨘ(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        ᨀᨘᨒ node;
    } ᨀᨘᨁ {
        // First byte of the linear-match node.
        append(out, *pos);
        ᨀᨘᨒ 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    ᨀᨛ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } ᨀᨛ(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, puur_naa_nka c) {
    var_na ch = static_cast<var_na>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
