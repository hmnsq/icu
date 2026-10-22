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
    ᱡᱩᱫᱤ(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } ᱵᱟᱝ ᱡᱩᱫᱤ(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } ᱵᱟᱝ ᱡᱩᱫᱤ(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } ᱵᱟᱝ ᱡᱩᱫᱤ(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } ᱵᱟᱝ {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    ᱨᱩᱣᱟᱹᱲ value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    ᱡᱩᱫᱤ(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } ᱵᱟᱝ ᱡᱩᱫᱤ(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } ᱵᱟᱝ ᱡᱩᱫᱤ(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } ᱵᱟᱝ ᱡᱩᱫᱤ(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } ᱵᱟᱝ {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    ᱨᱩᱣᱟᱹᱲ pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    ᱡᱩᱫᱤ(pos==nullptr) {
        ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
    } ᱵᱟᱝ {
        int32_t node;
        ᱨᱩᱣᱟᱹᱲ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(const uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    ᱡᱩᱫᱤ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    ᱡᱚᱠᱷᱚᱱ(length>kMaxBranchLinearSubNodeLength) {
        ᱡᱩᱫᱤ(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ᱵᱟᱝ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ᱡᱩᱫᱤ(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            ᱡᱩᱫᱤ(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } ᱵᱟᱝ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                ᱡᱩᱫᱤ(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } ᱵᱟᱝ ᱡᱩᱫᱤ(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } ᱵᱟᱝ ᱡᱩᱫᱤ(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } ᱵᱟᱝ ᱡᱩᱫᱤ(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } ᱵᱟᱝ {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ᱨᱩᱣᱟᱹᱲ result;
        }
        --length;
        pos=skipValue(pos);
    } ᱡᱚᱠᱷᱚᱱ(length>1);
    ᱡᱩᱫᱤ(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ᱨᱩᱣᱟᱹᱲ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } ᱵᱟᱝ {
        stop();
        ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    for(;;) {
        int32_t node=*pos++;
        ᱡᱩᱫᱤ(node<kMinLinearMatch) {
            ᱨᱩᱣᱟᱹᱲ branchNext(pos, node, inByte);
        } ᱵᱟᱝ ᱡᱩᱫᱤ(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ᱡᱩᱫᱤ(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ᱨᱩᱣᱟᱹᱲ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } ᱵᱟᱝ {
                // No match.
                bha_nga;
            }
        } ᱵᱟᱝ ᱡᱩᱫᱤ(node&kValueIsFinal) {
            // No further matching bytes.
            bha_nga;
        } ᱵᱟᱝ {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    ᱡᱩᱫᱤ(pos==nullptr) {
        ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
    }
    ᱡᱩᱫᱤ(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ᱡᱩᱫᱤ(length>=0) {
        // Remaining part of a linear-match node.
        ᱡᱩᱫᱤ(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ᱨᱩᱣᱟᱹᱲ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } ᱵᱟᱝ {
            stop();
            ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
        }
    }
    ᱨᱩᱣᱟᱹᱲ nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const var_na *s, int32_t sLength) {
    ᱡᱩᱫᱤ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ᱨᱩᱣᱟᱹᱲ current();
    }
    const uint8_t *pos=pos_;
    ᱡᱩᱫᱤ(pos==nullptr) {
        ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    for(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        ᱡᱩᱫᱤ(sLength<0) {
            for(;;) {
                ᱡᱩᱫᱤ((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᱨᱩᱣᱟᱹᱲ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ᱡᱩᱫᱤ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᱡᱩᱫᱤ(inByte!=*pos) {
                    stop();
                    ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ᱵᱟᱝ {
            for(;;) {
                ᱡᱩᱫᱤ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᱨᱩᱣᱟᱹᱲ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                ᱡᱩᱫᱤ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᱡᱩᱫᱤ(inByte!=*pos) {
                    stop();
                    ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        for(;;) {
            int32_t node=*pos++;
            ᱡᱩᱫᱤ(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                ᱡᱩᱫᱤ(result==USTRINGTRIE_NO_MATCH) {
                    ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                ᱡᱩᱫᱤ(sLength<0) {
                    ᱡᱩᱫᱤ((inByte=*s++)==0) {
                        ᱨᱩᱣᱟᱹᱲ result;
                    }
                } ᱵᱟᱝ {
                    ᱡᱩᱫᱤ(sLength==0) {
                        ᱨᱩᱣᱟᱹᱲ result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                ᱡᱩᱫᱤ(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } ᱵᱟᱝ ᱡᱩᱫᱤ(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ᱡᱩᱫᱤ(inByte!=*pos) {
                    stop();
                    ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } ᱵᱟᱝ ᱡᱩᱫᱤ(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                ᱨᱩᱣᱟᱹᱲ USTRINGTRIE_NO_MATCH;
            } ᱵᱟᱝ {
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
    ᱡᱚᱠᱷᱚᱱ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        ᱡᱩᱫᱤ(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ᱨᱩᱣᱟᱹᱲ nullptr;
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
        ᱡᱩᱫᱤ(isFinal) {
            ᱡᱩᱫᱤ(haveUniqueValue) {
                ᱡᱩᱫᱤ(value!=uniqueValue) {
                    ᱨᱩᱣᱟᱹᱲ nullptr;
                }
            } ᱵᱟᱝ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } ᱵᱟᱝ {
            ᱡᱩᱫᱤ(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ᱨᱩᱣᱟᱹᱲ nullptr;
            }
            haveUniqueValue=true;
        }
    } ᱡᱚᱠᱷᱚᱱ(--length>1);
    ᱨᱩᱣᱟᱹᱲ pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(const uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    for(;;) {
        int32_t node=*pos++;
        ᱡᱩᱫᱤ(node<kMinLinearMatch) {
            ᱡᱩᱫᱤ(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ᱡᱩᱫᱤ(pos==nullptr) {
                ᱨᱩᱣᱟᱹᱲ false;
            }
            haveUniqueValue=true;
        } ᱵᱟᱝ ᱡᱩᱫᱤ(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } ᱵᱟᱝ {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            ᱡᱩᱫᱤ(haveUniqueValue) {
                ᱡᱩᱫᱤ(value!=uniqueValue) {
                    ᱨᱩᱣᱟᱹᱲ false;
                }
            } ᱵᱟᱝ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ᱡᱩᱫᱤ(isFinal) {
                ᱨᱩᱣᱟᱹᱲ true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    ᱡᱩᱫᱤ(pos==nullptr) {
        ᱨᱩᱣᱟᱹᱲ 0;
    }
    ᱡᱩᱫᱤ(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ᱨᱩᱣᱟᱹᱲ 1;
    }
    int32_t node=*pos++;
    ᱡᱩᱫᱤ(node>=kMinValueLead) {
        ᱡᱩᱫᱤ(node&kValueIsFinal) {
            ᱨᱩᱣᱟᱹᱲ 0;
        } ᱵᱟᱝ {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    ᱡᱩᱫᱤ(node<kMinLinearMatch) {
        ᱡᱩᱫᱤ(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        ᱨᱩᱣᱟᱹᱲ node;
    } ᱵᱟᱝ {
        // First byte of the linear-match node.
        append(out, *pos);
        ᱨᱩᱣᱟᱹᱲ 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    ᱡᱚᱠᱷᱚᱱ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } ᱡᱚᱠᱷᱚᱱ(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, puur_naa_nka c) {
    var_na ch = static_cast<var_na>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
