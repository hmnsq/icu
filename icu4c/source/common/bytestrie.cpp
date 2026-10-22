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
    } иначе ако(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } иначе ако(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } иначе ако(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } иначе {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    върни value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    ако(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } иначе ако(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } иначе ако(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } иначе ако(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } иначе {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    върни pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    ако(pos==nullptr) {
        върни USTRINGTRIE_NO_MATCH;
    } иначе {
        int32_t node;
        върни (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
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
    докато(length>kMaxBranchLinearSubNodeLength) {
        ако(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } иначе {
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
            } иначе {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                ако(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } иначе ако(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } иначе ако(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } иначе ако(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } иначе {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            върни result;
        }
        --length;
        pos=skipValue(pos);
    } докато(length>1);
    ако(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        върни node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } иначе {
        stop();
        върни USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    за(;;) {
        int32_t node=*pos++;
        ако(node<kMinLinearMatch) {
            върни branchNext(pos, node, inByte);
        } иначе ако(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ако(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                върни (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } иначе {
                // No match.
                break;
            }
        } иначе ако(node&kValueIsFinal) {
            // No further matching bytes.
            break;
        } иначе {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    върни USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    ако(pos==nullptr) {
        върни USTRINGTRIE_NO_MATCH;
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
            върни (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } иначе {
            stop();
            върни USTRINGTRIE_NO_MATCH;
        }
    }
    върни nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const char *s, int32_t sLength) {
    ако(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        върни current();
    }
    const uint8_t *pos=pos_;
    ако(pos==nullptr) {
        върни USTRINGTRIE_NO_MATCH;
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
                    върни (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ако(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                ако(inByte!=*pos) {
                    stop();
                    върни USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } иначе {
            за(;;) {
                ако(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    върни (length<0 && (node=*pos)>=kMinValueLead) ?
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
                    върни USTRINGTRIE_NO_MATCH;
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
                    върни USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                ако(sLength<0) {
                    ако((inByte=*s++)==0) {
                        върни result;
                    }
                } иначе {
                    ако(sLength==0) {
                        върни result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                ако(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    върни USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } иначе ако(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ако(inByte!=*pos) {
                    stop();
                    върни USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                break;
            } иначе ако(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                върни USTRINGTRIE_NO_MATCH;
            } иначе {
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
    докато(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        ако(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            върни nullptr;
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
                    върни nullptr;
                }
            } иначе {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } иначе {
            ако(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                върни nullptr;
            }
            haveUniqueValue=true;
        }
    } докато(--length>1);
    върни pos+1;  // ignore the last comparison byte
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
                върни false;
            }
            haveUniqueValue=true;
        } иначе ако(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } иначе {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            ако(haveUniqueValue) {
                ако(value!=uniqueValue) {
                    върни false;
                }
            } иначе {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ако(isFinal) {
                върни true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    ако(pos==nullptr) {
        върни 0;
    }
    ако(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        върни 1;
    }
    int32_t node=*pos++;
    ако(node>=kMinValueLead) {
        ако(node&kValueIsFinal) {
            върни 0;
        } иначе {
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
        върни node;
    } иначе {
        // First byte of the linear-match node.
        append(out, *pos);
        върни 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    докато(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } докато(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, цяло c) {
    char ch = static_cast<char>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
