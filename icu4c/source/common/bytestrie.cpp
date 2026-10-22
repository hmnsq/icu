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
    ᤀᤡ᤺(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } ᤀᤡ᤺ᤆᤡ {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    ᤀᤡᤴ value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    ᤀᤡ᤺(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } ᤀᤡ᤺ᤆᤡ {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    ᤀᤡᤴ pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    ᤀᤡ᤺(pos==nullptr) {
        ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
    } ᤀᤡ᤺ᤆᤡ {
        int32_t node;
        ᤀᤡᤴ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(const uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    ᤀᤡ᤺(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    ᤀᤠ᤺(length>kMaxBranchLinearSubNodeLength) {
        ᤀᤡ᤺(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ᤀᤡ᤺ᤆᤡ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ᤀᤡ᤺(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            ᤀᤡ᤺(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } ᤀᤡ᤺ᤆᤡ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                ᤀᤡ᤺(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } ᤀᤡ᤺ᤆᤡ {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ᤀᤡᤴ result;
        }
        --length;
        pos=skipValue(pos);
    } ᤀᤠ᤺(length>1);
    ᤀᤡ᤺(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ᤀᤡᤴ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } ᤀᤡ᤺ᤆᤡ {
        stop();
        ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    for(;;) {
        int32_t node=*pos++;
        ᤀᤡ᤺(node<kMinLinearMatch) {
            ᤀᤡᤴ branchNext(pos, node, inByte);
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ᤀᤡ᤺(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ᤀᤡᤴ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } ᤀᤡ᤺ᤆᤡ {
                // No match.
                bha_nga;
            }
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(node&kValueIsFinal) {
            // No further matching bytes.
            bha_nga;
        } ᤀᤡ᤺ᤆᤡ {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    ᤀᤡ᤺(pos==nullptr) {
        ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
    }
    ᤀᤡ᤺(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ᤀᤡ᤺(length>=0) {
        // Remaining part of a linear-match node.
        ᤀᤡ᤺(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ᤀᤡᤴ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } ᤀᤡ᤺ᤆᤡ {
            stop();
            ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
        }
    }
    ᤀᤡᤴ nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const var_na *s, int32_t sLength) {
    ᤀᤡ᤺(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ᤀᤡᤴ current();
    }
    const uint8_t *pos=pos_;
    ᤀᤡ᤺(pos==nullptr) {
        ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    for(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        ᤀᤡ᤺(sLength<0) {
            for(;;) {
                ᤀᤡ᤺((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᤀᤡᤴ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ᤀᤡ᤺(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᤀᤡ᤺(inByte!=*pos) {
                    stop();
                    ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ᤀᤡ᤺ᤆᤡ {
            for(;;) {
                ᤀᤡ᤺(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᤀᤡᤴ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                ᤀᤡ᤺(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᤀᤡ᤺(inByte!=*pos) {
                    stop();
                    ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        for(;;) {
            int32_t node=*pos++;
            ᤀᤡ᤺(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                ᤀᤡ᤺(result==USTRINGTRIE_NO_MATCH) {
                    ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                ᤀᤡ᤺(sLength<0) {
                    ᤀᤡ᤺((inByte=*s++)==0) {
                        ᤀᤡᤴ result;
                    }
                } ᤀᤡ᤺ᤆᤡ {
                    ᤀᤡ᤺(sLength==0) {
                        ᤀᤡᤴ result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                ᤀᤡ᤺(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ᤀᤡ᤺(inByte!=*pos) {
                    stop();
                    ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                ᤀᤡᤴ USTRINGTRIE_NO_MATCH;
            } ᤀᤡ᤺ᤆᤡ {
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
    ᤀᤠ᤺(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        ᤀᤡ᤺(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ᤀᤡᤴ nullptr;
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
        ᤀᤡ᤺(isFinal) {
            ᤀᤡ᤺(haveUniqueValue) {
                ᤀᤡ᤺(value!=uniqueValue) {
                    ᤀᤡᤴ nullptr;
                }
            } ᤀᤡ᤺ᤆᤡ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } ᤀᤡ᤺ᤆᤡ {
            ᤀᤡ᤺(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ᤀᤡᤴ nullptr;
            }
            haveUniqueValue=true;
        }
    } ᤀᤠ᤺(--length>1);
    ᤀᤡᤴ pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(const uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    for(;;) {
        int32_t node=*pos++;
        ᤀᤡ᤺(node<kMinLinearMatch) {
            ᤀᤡ᤺(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ᤀᤡ᤺(pos==nullptr) {
                ᤀᤡᤴ false;
            }
            haveUniqueValue=true;
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } ᤀᤡ᤺ᤆᤡ {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            ᤀᤡ᤺(haveUniqueValue) {
                ᤀᤡ᤺(value!=uniqueValue) {
                    ᤀᤡᤴ false;
                }
            } ᤀᤡ᤺ᤆᤡ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ᤀᤡ᤺(isFinal) {
                ᤀᤡᤴ true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    ᤀᤡ᤺(pos==nullptr) {
        ᤀᤡᤴ 0;
    }
    ᤀᤡ᤺(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ᤀᤡᤴ 1;
    }
    int32_t node=*pos++;
    ᤀᤡ᤺(node>=kMinValueLead) {
        ᤀᤡ᤺(node&kValueIsFinal) {
            ᤀᤡᤴ 0;
        } ᤀᤡ᤺ᤆᤡ {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    ᤀᤡ᤺(node<kMinLinearMatch) {
        ᤀᤡ᤺(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        ᤀᤡᤴ node;
    } ᤀᤡ᤺ᤆᤡ {
        // First byte of the linear-match node.
        append(out, *pos);
        ᤀᤡᤴ 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    ᤀᤠ᤺(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } ᤀᤠ᤺(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, puur_naa_nka c) {
    var_na ch = static_cast<var_na>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
