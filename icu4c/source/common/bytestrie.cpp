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
    ако(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } инако ако(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } инако ако(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } инако ако(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } инако {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    врати value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    ако(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } инако ако(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } инако ако(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } инако ако(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } инако {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    врати pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    ако(pos==nullptr) {
        врати USTRINGTRIE_NO_MATCH;
    } инако {
        int32_t node;
        врати (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(const uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    ако(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    док(length>kMaxBranchLinearSubNodeLength) {
        ако(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } инако {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ако(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            ако(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } инако {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                ако(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } инако ако(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } инако ако(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } инако ако(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } инако {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            врати result;
        }
        --length;
        pos=skipValue(pos);
    } док(length>1);
    ако(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        врати node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } инако {
        stop();
        врати USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    за(;;) {
        int32_t node=*pos++;
        ако(node<kMinLinearMatch) {
            врати branchNext(pos, node, inByte);
        } инако ако(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ако(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                врати (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } инако {
                // No match.
                break;
            }
        } инако ако(node&kValueIsFinal) {
            // No further matching bytes.
            break;
        } инако {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    врати USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    ако(pos==nullptr) {
        врати USTRINGTRIE_NO_MATCH;
    }
    ако(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ако(length>=0) {
        // Remaining part of a linear-match node.
        ако(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            врати (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } инако {
            stop();
            врати USTRINGTRIE_NO_MATCH;
        }
    }
    врати nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const char *s, int32_t sLength) {
    ако(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        врати current();
    }
    const uint8_t *pos=pos_;
    ако(pos==nullptr) {
        врати USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    за(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        ако(sLength<0) {
            за(;;) {
                ако((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    врати (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ако(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                ако(inByte!=*pos) {
                    stop();
                    врати USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } инако {
            за(;;) {
                ако(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    врати (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                ако(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                ако(inByte!=*pos) {
                    stop();
                    врати USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        за(;;) {
            int32_t node=*pos++;
            ако(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                ако(result==USTRINGTRIE_NO_MATCH) {
                    врати USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                ако(sLength<0) {
                    ако((inByte=*s++)==0) {
                        врати result;
                    }
                } инако {
                    ако(sLength==0) {
                        врати result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                ако(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    врати USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } инако ако(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ако(inByte!=*pos) {
                    stop();
                    врати USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                break;
            } инако ако(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                врати USTRINGTRIE_NO_MATCH;
            } инако {
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
    док(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        ако(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            врати nullptr;
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
        ако(isFinal) {
            ако(haveUniqueValue) {
                ако(value!=uniqueValue) {
                    врати nullptr;
                }
            } инако {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } инако {
            ако(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                врати nullptr;
            }
            haveUniqueValue=true;
        }
    } док(--length>1);
    врати pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(const uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    за(;;) {
        int32_t node=*pos++;
        ако(node<kMinLinearMatch) {
            ако(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ако(pos==nullptr) {
                врати false;
            }
            haveUniqueValue=true;
        } инако ако(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } инако {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            ако(haveUniqueValue) {
                ако(value!=uniqueValue) {
                    врати false;
                }
            } инако {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ако(isFinal) {
                врати true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    ако(pos==nullptr) {
        врати 0;
    }
    ако(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        врати 1;
    }
    int32_t node=*pos++;
    ако(node>=kMinValueLead) {
        ако(node&kValueIsFinal) {
            врати 0;
        } инако {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    ако(node<kMinLinearMatch) {
        ако(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        врати node;
    } инако {
        // First byte of the linear-match node.
        append(out, *pos);
        врати 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    док(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } док(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, цео c) {
    char ch = static_cast<char>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
