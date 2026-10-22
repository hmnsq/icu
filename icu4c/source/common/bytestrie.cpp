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
    ꦪꦼꦤ꧀(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } ꦭꦶꦪ ꦪꦼꦤ꧀(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } ꦭꦶꦪ ꦪꦼꦤ꧀(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } ꦭꦶꦪ ꦪꦼꦤ꧀(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } ꦭꦶꦪ {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    ꦮꦭꦶ value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    ꦪꦼꦤ꧀(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } ꦭꦶꦪ ꦪꦼꦤ꧀(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } ꦭꦶꦪ ꦪꦼꦤ꧀(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } ꦭꦶꦪ ꦪꦼꦤ꧀(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } ꦭꦶꦪ {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    ꦮꦭꦶ pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    ꦪꦼꦤ꧀(pos==nullptr) {
        ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
    } ꦭꦶꦪ {
        int32_t node;
        ꦮꦭꦶ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(const uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    ꦪꦼꦤ꧀(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    ꦱꦼꦢꦼꦭ(length>kMaxBranchLinearSubNodeLength) {
        ꦪꦼꦤ꧀(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ꦭꦶꦪ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ꦪꦼꦤ꧀(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            ꦪꦼꦤ꧀(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } ꦭꦶꦪ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                ꦪꦼꦤ꧀(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } ꦭꦶꦪ ꦪꦼꦤ꧀(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } ꦭꦶꦪ ꦪꦼꦤ꧀(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } ꦭꦶꦪ ꦪꦼꦤ꧀(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } ꦭꦶꦪ {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ꦮꦭꦶ result;
        }
        --length;
        pos=skipValue(pos);
    } ꦱꦼꦢꦼꦭ(length>1);
    ꦪꦼꦤ꧀(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ꦮꦭꦶ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } ꦭꦶꦪ {
        stop();
        ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    ꦏꦁꦒꦺꦴ(;;) {
        int32_t node=*pos++;
        ꦪꦼꦤ꧀(node<kMinLinearMatch) {
            ꦮꦭꦶ branchNext(pos, node, inByte);
        } ꦭꦶꦪ ꦪꦼꦤ꧀(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ꦪꦼꦤ꧀(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ꦮꦭꦶ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } ꦭꦶꦪ {
                // No match.
                bha_nga;
            }
        } ꦭꦶꦪ ꦪꦼꦤ꧀(node&kValueIsFinal) {
            // No further matching bytes.
            bha_nga;
        } ꦭꦶꦪ {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    ꦪꦼꦤ꧀(pos==nullptr) {
        ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
    }
    ꦪꦼꦤ꧀(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ꦪꦼꦤ꧀(length>=0) {
        // Remaining part of a linear-match node.
        ꦪꦼꦤ꧀(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ꦮꦭꦶ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } ꦭꦶꦪ {
            stop();
            ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
        }
    }
    ꦮꦭꦶ nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const var_na *s, int32_t sLength) {
    ꦪꦼꦤ꧀(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ꦮꦭꦶ current();
    }
    const uint8_t *pos=pos_;
    ꦪꦼꦤ꧀(pos==nullptr) {
        ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ꦏꦁꦒꦺꦴ(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        ꦪꦼꦤ꧀(sLength<0) {
            ꦏꦁꦒꦺꦴ(;;) {
                ꦪꦼꦤ꧀((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ꦮꦭꦶ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ꦪꦼꦤ꧀(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ꦪꦼꦤ꧀(inByte!=*pos) {
                    stop();
                    ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ꦭꦶꦪ {
            ꦏꦁꦒꦺꦴ(;;) {
                ꦪꦼꦤ꧀(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ꦮꦭꦶ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                ꦪꦼꦤ꧀(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ꦪꦼꦤ꧀(inByte!=*pos) {
                    stop();
                    ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        ꦏꦁꦒꦺꦴ(;;) {
            int32_t node=*pos++;
            ꦪꦼꦤ꧀(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                ꦪꦼꦤ꧀(result==USTRINGTRIE_NO_MATCH) {
                    ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                ꦪꦼꦤ꧀(sLength<0) {
                    ꦪꦼꦤ꧀((inByte=*s++)==0) {
                        ꦮꦭꦶ result;
                    }
                } ꦭꦶꦪ {
                    ꦪꦼꦤ꧀(sLength==0) {
                        ꦮꦭꦶ result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                ꦪꦼꦤ꧀(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } ꦭꦶꦪ ꦪꦼꦤ꧀(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ꦪꦼꦤ꧀(inByte!=*pos) {
                    stop();
                    ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } ꦭꦶꦪ ꦪꦼꦤ꧀(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                ꦮꦭꦶ USTRINGTRIE_NO_MATCH;
            } ꦭꦶꦪ {
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
    ꦱꦼꦢꦼꦭ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        ꦪꦼꦤ꧀(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ꦮꦭꦶ nullptr;
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
        ꦪꦼꦤ꧀(isFinal) {
            ꦪꦼꦤ꧀(haveUniqueValue) {
                ꦪꦼꦤ꧀(value!=uniqueValue) {
                    ꦮꦭꦶ nullptr;
                }
            } ꦭꦶꦪ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } ꦭꦶꦪ {
            ꦪꦼꦤ꧀(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ꦮꦭꦶ nullptr;
            }
            haveUniqueValue=true;
        }
    } ꦱꦼꦢꦼꦭ(--length>1);
    ꦮꦭꦶ pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(const uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    ꦏꦁꦒꦺꦴ(;;) {
        int32_t node=*pos++;
        ꦪꦼꦤ꧀(node<kMinLinearMatch) {
            ꦪꦼꦤ꧀(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ꦪꦼꦤ꧀(pos==nullptr) {
                ꦮꦭꦶ false;
            }
            haveUniqueValue=true;
        } ꦭꦶꦪ ꦪꦼꦤ꧀(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } ꦭꦶꦪ {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            ꦪꦼꦤ꧀(haveUniqueValue) {
                ꦪꦼꦤ꧀(value!=uniqueValue) {
                    ꦮꦭꦶ false;
                }
            } ꦭꦶꦪ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ꦪꦼꦤ꧀(isFinal) {
                ꦮꦭꦶ true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    ꦪꦼꦤ꧀(pos==nullptr) {
        ꦮꦭꦶ 0;
    }
    ꦪꦼꦤ꧀(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ꦮꦭꦶ 1;
    }
    int32_t node=*pos++;
    ꦪꦼꦤ꧀(node>=kMinValueLead) {
        ꦪꦼꦤ꧀(node&kValueIsFinal) {
            ꦮꦭꦶ 0;
        } ꦭꦶꦪ {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    ꦪꦼꦤ꧀(node<kMinLinearMatch) {
        ꦪꦼꦤ꧀(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        ꦮꦭꦶ node;
    } ꦭꦶꦪ {
        // First byte of the linear-match node.
        append(out, *pos);
        ꦮꦭꦶ 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    ꦱꦼꦢꦼꦭ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } ꦱꦼꦢꦼꦭ(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, puur_naa_nka c) {
    var_na ch = static_cast<var_na>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
