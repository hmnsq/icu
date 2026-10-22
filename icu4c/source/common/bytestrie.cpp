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
    якщо(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } інакше якщо(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } інакше якщо(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } інакше якщо(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } інакше {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    повернути value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    якщо(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } інакше якщо(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } інакше якщо(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } інакше якщо(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } інакше {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    повернути pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    якщо(pos==nullptr) {
        повернути USTRINGTRIE_NO_MATCH;
    } інакше {
        int32_t node;
        повернути (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(const uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    якщо(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    поки(length>kMaxBranchLinearSubNodeLength) {
        якщо(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } інакше {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        якщо(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            якщо(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } інакше {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                якщо(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } інакше якщо(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } інакше якщо(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } інакше якщо(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } інакше {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            повернути result;
        }
        --length;
        pos=skipValue(pos);
    } поки(length>1);
    якщо(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        повернути node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } інакше {
        stop();
        повернути USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    для(;;) {
        int32_t node=*pos++;
        якщо(node<kMinLinearMatch) {
            повернути branchNext(pos, node, inByte);
        } інакше якщо(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            якщо(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                повернути (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } інакше {
                // No match.
                break;
            }
        } інакше якщо(node&kValueIsFinal) {
            // No further matching bytes.
            break;
        } інакше {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    повернути USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    якщо(pos==nullptr) {
        повернути USTRINGTRIE_NO_MATCH;
    }
    якщо(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    якщо(length>=0) {
        // Remaining part of a linear-match node.
        якщо(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            повернути (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } інакше {
            stop();
            повернути USTRINGTRIE_NO_MATCH;
        }
    }
    повернути nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const char *s, int32_t sLength) {
    якщо(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        повернути current();
    }
    const uint8_t *pos=pos_;
    якщо(pos==nullptr) {
        повернути USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    для(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        якщо(sLength<0) {
            для(;;) {
                якщо((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    повернути (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                якщо(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                якщо(inByte!=*pos) {
                    stop();
                    повернути USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } інакше {
            для(;;) {
                якщо(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    повернути (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                якщо(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                якщо(inByte!=*pos) {
                    stop();
                    повернути USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        для(;;) {
            int32_t node=*pos++;
            якщо(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                якщо(result==USTRINGTRIE_NO_MATCH) {
                    повернути USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                якщо(sLength<0) {
                    якщо((inByte=*s++)==0) {
                        повернути result;
                    }
                } інакше {
                    якщо(sLength==0) {
                        повернути result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                якщо(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    повернути USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } інакше якщо(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                якщо(inByte!=*pos) {
                    stop();
                    повернути USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                break;
            } інакше якщо(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                повернути USTRINGTRIE_NO_MATCH;
            } інакше {
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
    поки(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        якщо(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            повернути nullptr;
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
        якщо(isFinal) {
            якщо(haveUniqueValue) {
                якщо(value!=uniqueValue) {
                    повернути nullptr;
                }
            } інакше {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } інакше {
            якщо(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                повернути nullptr;
            }
            haveUniqueValue=true;
        }
    } поки(--length>1);
    повернути pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(const uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    для(;;) {
        int32_t node=*pos++;
        якщо(node<kMinLinearMatch) {
            якщо(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            якщо(pos==nullptr) {
                повернути false;
            }
            haveUniqueValue=true;
        } інакше якщо(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } інакше {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            якщо(haveUniqueValue) {
                якщо(value!=uniqueValue) {
                    повернути false;
                }
            } інакше {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            якщо(isFinal) {
                повернути true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    якщо(pos==nullptr) {
        повернути 0;
    }
    якщо(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        повернути 1;
    }
    int32_t node=*pos++;
    якщо(node>=kMinValueLead) {
        якщо(node&kValueIsFinal) {
            повернути 0;
        } інакше {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    якщо(node<kMinLinearMatch) {
        якщо(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        повернути node;
    } інакше {
        // First byte of the linear-match node.
        append(out, *pos);
        повернути 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    поки(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } поки(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, ціле c) {
    char ch = static_cast<char>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
