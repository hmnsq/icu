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
    nếu(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } không_thì nếu(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } không_thì nếu(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } không_thì nếu(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } không_thì {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    trả_về value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    nếu(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } không_thì nếu(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } không_thì nếu(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } không_thì nếu(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } không_thì {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    trả_về pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    nếu(pos==nullptr) {
        trả_về USTRINGTRIE_NO_MATCH;
    } không_thì {
        int32_t node;
        trả_về (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(const uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    nếu(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    trong_khi(length>kMaxBranchLinearSubNodeLength) {
        nếu(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } không_thì {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        nếu(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            nếu(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } không_thì {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                nếu(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } không_thì nếu(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } không_thì nếu(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } không_thì nếu(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } không_thì {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            trả_về result;
        }
        --length;
        pos=skipValue(pos);
    } trong_khi(length>1);
    nếu(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        trả_về node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } không_thì {
        stop();
        trả_về USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    cho(;;) {
        int32_t node=*pos++;
        nếu(node<kMinLinearMatch) {
            trả_về branchNext(pos, node, inByte);
        } không_thì nếu(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            nếu(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                trả_về (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } không_thì {
                // No match.
                break;
            }
        } không_thì nếu(node&kValueIsFinal) {
            // No further matching bytes.
            break;
        } không_thì {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    trả_về USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    nếu(pos==nullptr) {
        trả_về USTRINGTRIE_NO_MATCH;
    }
    nếu(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    nếu(length>=0) {
        // Remaining part of a linear-match node.
        nếu(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            trả_về (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } không_thì {
            stop();
            trả_về USTRINGTRIE_NO_MATCH;
        }
    }
    trả_về nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const char *s, int32_t sLength) {
    nếu(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        trả_về current();
    }
    const uint8_t *pos=pos_;
    nếu(pos==nullptr) {
        trả_về USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    cho(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        nếu(sLength<0) {
            cho(;;) {
                nếu((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    trả_về (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                nếu(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                nếu(inByte!=*pos) {
                    stop();
                    trả_về USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } không_thì {
            cho(;;) {
                nếu(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    trả_về (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                nếu(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                nếu(inByte!=*pos) {
                    stop();
                    trả_về USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        cho(;;) {
            int32_t node=*pos++;
            nếu(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                nếu(result==USTRINGTRIE_NO_MATCH) {
                    trả_về USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                nếu(sLength<0) {
                    nếu((inByte=*s++)==0) {
                        trả_về result;
                    }
                } không_thì {
                    nếu(sLength==0) {
                        trả_về result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                nếu(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    trả_về USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } không_thì nếu(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                nếu(inByte!=*pos) {
                    stop();
                    trả_về USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                break;
            } không_thì nếu(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                trả_về USTRINGTRIE_NO_MATCH;
            } không_thì {
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
    trong_khi(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        nếu(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            trả_về nullptr;
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
        nếu(isFinal) {
            nếu(haveUniqueValue) {
                nếu(value!=uniqueValue) {
                    trả_về nullptr;
                }
            } không_thì {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } không_thì {
            nếu(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                trả_về nullptr;
            }
            haveUniqueValue=true;
        }
    } trong_khi(--length>1);
    trả_về pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(const uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    cho(;;) {
        int32_t node=*pos++;
        nếu(node<kMinLinearMatch) {
            nếu(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            nếu(pos==nullptr) {
                trả_về false;
            }
            haveUniqueValue=true;
        } không_thì nếu(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } không_thì {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            nếu(haveUniqueValue) {
                nếu(value!=uniqueValue) {
                    trả_về false;
                }
            } không_thì {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            nếu(isFinal) {
                trả_về true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    nếu(pos==nullptr) {
        trả_về 0;
    }
    nếu(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        trả_về 1;
    }
    int32_t node=*pos++;
    nếu(node>=kMinValueLead) {
        nếu(node&kValueIsFinal) {
            trả_về 0;
        } không_thì {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    nếu(node<kMinLinearMatch) {
        nếu(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        trả_về node;
    } không_thì {
        // First byte of the linear-match node.
        append(out, *pos);
        trả_về 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    trong_khi(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } trong_khi(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, số_nguyên c) {
    char ch = static_cast<char>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
