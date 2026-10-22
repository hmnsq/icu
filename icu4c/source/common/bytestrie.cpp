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
    만약(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } 아니면 만약(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } 아니면 만약(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } 아니면 만약(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } 아니면 {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    반환 value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    만약(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } 아니면 만약(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } 아니면 만약(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } 아니면 만약(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } 아니면 {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    반환 pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    만약(pos==nullptr) {
        반환 USTRINGTRIE_NO_MATCH;
    } 아니면 {
        int32_t node;
        반환 (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(const uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    만약(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    동안(length>kMaxBranchLinearSubNodeLength) {
        만약(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } 아니면 {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        만약(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            만약(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } 아니면 {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                만약(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } 아니면 만약(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } 아니면 만약(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } 아니면 만약(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } 아니면 {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            반환 result;
        }
        --length;
        pos=skipValue(pos);
    } 동안(length>1);
    만약(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        반환 node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } 아니면 {
        stop();
        반환 USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    위해(;;) {
        int32_t node=*pos++;
        만약(node<kMinLinearMatch) {
            반환 branchNext(pos, node, inByte);
        } 아니면 만약(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            만약(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                반환 (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } 아니면 {
                // No match.
                break;
            }
        } 아니면 만약(node&kValueIsFinal) {
            // No further matching bytes.
            break;
        } 아니면 {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    반환 USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    만약(pos==nullptr) {
        반환 USTRINGTRIE_NO_MATCH;
    }
    만약(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    만약(length>=0) {
        // Remaining part of a linear-match node.
        만약(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            반환 (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } 아니면 {
            stop();
            반환 USTRINGTRIE_NO_MATCH;
        }
    }
    반환 nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const char *s, int32_t sLength) {
    만약(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        반환 current();
    }
    const uint8_t *pos=pos_;
    만약(pos==nullptr) {
        반환 USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    위해(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        만약(sLength<0) {
            위해(;;) {
                만약((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    반환 (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                만약(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                만약(inByte!=*pos) {
                    stop();
                    반환 USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } 아니면 {
            위해(;;) {
                만약(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    반환 (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                만약(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                만약(inByte!=*pos) {
                    stop();
                    반환 USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        위해(;;) {
            int32_t node=*pos++;
            만약(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                만약(result==USTRINGTRIE_NO_MATCH) {
                    반환 USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                만약(sLength<0) {
                    만약((inByte=*s++)==0) {
                        반환 result;
                    }
                } 아니면 {
                    만약(sLength==0) {
                        반환 result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                만약(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    반환 USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } 아니면 만약(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                만약(inByte!=*pos) {
                    stop();
                    반환 USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                break;
            } 아니면 만약(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                반환 USTRINGTRIE_NO_MATCH;
            } 아니면 {
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
    동안(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        만약(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            반환 nullptr;
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
        만약(isFinal) {
            만약(haveUniqueValue) {
                만약(value!=uniqueValue) {
                    반환 nullptr;
                }
            } 아니면 {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } 아니면 {
            만약(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                반환 nullptr;
            }
            haveUniqueValue=true;
        }
    } 동안(--length>1);
    반환 pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(const uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    위해(;;) {
        int32_t node=*pos++;
        만약(node<kMinLinearMatch) {
            만약(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            만약(pos==nullptr) {
                반환 false;
            }
            haveUniqueValue=true;
        } 아니면 만약(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } 아니면 {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            만약(haveUniqueValue) {
                만약(value!=uniqueValue) {
                    반환 false;
                }
            } 아니면 {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            만약(isFinal) {
                반환 true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    만약(pos==nullptr) {
        반환 0;
    }
    만약(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        반환 1;
    }
    int32_t node=*pos++;
    만약(node>=kMinValueLead) {
        만약(node&kValueIsFinal) {
            반환 0;
        } 아니면 {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    만약(node<kMinLinearMatch) {
        만약(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        반환 node;
    } 아니면 {
        // First byte of the linear-match node.
        append(out, *pos);
        반환 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    동안(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } 동안(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, 정수 c) {
    char ch = static_cast<char>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
