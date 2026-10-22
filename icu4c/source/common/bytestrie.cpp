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
    如果(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } 否则 如果(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } 否则 如果(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } 否则 如果(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } 否则 {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    返回 value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    如果(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } 否则 如果(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } 否则 如果(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } 否则 如果(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } 否则 {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    返回 pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    如果(pos==nullptr) {
        返回 USTRINGTRIE_NO_MATCH;
    } 否则 {
        int32_t node;
        返回 (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(const uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    如果(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    当(length>kMaxBranchLinearSubNodeLength) {
        如果(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } 否则 {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        如果(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            如果(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } 否则 {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                如果(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } 否则 如果(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } 否则 如果(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } 否则 如果(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } 否则 {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            返回 result;
        }
        --length;
        pos=skipValue(pos);
    } 当(length>1);
    如果(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        返回 node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } 否则 {
        stop();
        返回 USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    为(;;) {
        int32_t node=*pos++;
        如果(node<kMinLinearMatch) {
            返回 branchNext(pos, node, inByte);
        } 否则 如果(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            如果(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                返回 (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } 否则 {
                // No match.
                break;
            }
        } 否则 如果(node&kValueIsFinal) {
            // No further matching bytes.
            break;
        } 否则 {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    返回 USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    如果(pos==nullptr) {
        返回 USTRINGTRIE_NO_MATCH;
    }
    如果(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    如果(length>=0) {
        // Remaining part of a linear-match node.
        如果(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            返回 (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } 否则 {
            stop();
            返回 USTRINGTRIE_NO_MATCH;
        }
    }
    返回 nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const char *s, int32_t sLength) {
    如果(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        返回 current();
    }
    const uint8_t *pos=pos_;
    如果(pos==nullptr) {
        返回 USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    为(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        如果(sLength<0) {
            为(;;) {
                如果((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    返回 (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                如果(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                如果(inByte!=*pos) {
                    stop();
                    返回 USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } 否则 {
            为(;;) {
                如果(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    返回 (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                如果(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                如果(inByte!=*pos) {
                    stop();
                    返回 USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        为(;;) {
            int32_t node=*pos++;
            如果(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                如果(result==USTRINGTRIE_NO_MATCH) {
                    返回 USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                如果(sLength<0) {
                    如果((inByte=*s++)==0) {
                        返回 result;
                    }
                } 否则 {
                    如果(sLength==0) {
                        返回 result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                如果(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    返回 USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } 否则 如果(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                如果(inByte!=*pos) {
                    stop();
                    返回 USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                break;
            } 否则 如果(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                返回 USTRINGTRIE_NO_MATCH;
            } 否则 {
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
    当(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        如果(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            返回 nullptr;
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
        如果(isFinal) {
            如果(haveUniqueValue) {
                如果(value!=uniqueValue) {
                    返回 nullptr;
                }
            } 否则 {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } 否则 {
            如果(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                返回 nullptr;
            }
            haveUniqueValue=true;
        }
    } 当(--length>1);
    返回 pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(const uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    为(;;) {
        int32_t node=*pos++;
        如果(node<kMinLinearMatch) {
            如果(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            如果(pos==nullptr) {
                返回 false;
            }
            haveUniqueValue=true;
        } 否则 如果(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } 否则 {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            如果(haveUniqueValue) {
                如果(value!=uniqueValue) {
                    返回 false;
                }
            } 否则 {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            如果(isFinal) {
                返回 true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    如果(pos==nullptr) {
        返回 0;
    }
    如果(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        返回 1;
    }
    int32_t node=*pos++;
    如果(node>=kMinValueLead) {
        如果(node&kValueIsFinal) {
            返回 0;
        } 否则 {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    如果(node<kMinLinearMatch) {
        如果(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        返回 node;
    } 否则 {
        // First byte of the linear-match node.
        append(out, *pos);
        返回 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    当(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } 当(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, 整数 c) {
    char ch = static_cast<char>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
