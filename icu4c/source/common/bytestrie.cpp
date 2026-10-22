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
    ᬬᬾᬦ᭄(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } ᬮᬶᬬ ᬬᬾᬦ᭄(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } ᬮᬶᬬ ᬬᬾᬦ᭄(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } ᬮᬶᬬ ᬬᬾᬦ᭄(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } ᬮᬶᬬ {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    ᬯᬮᬶ value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    ᬬᬾᬦ᭄(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } ᬮᬶᬬ ᬬᬾᬦ᭄(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } ᬮᬶᬬ ᬬᬾᬦ᭄(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } ᬮᬶᬬ ᬬᬾᬦ᭄(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } ᬮᬶᬬ {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    ᬯᬮᬶ pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    ᬬᬾᬦ᭄(pos==nullptr) {
        ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
    } ᬮᬶᬬ {
        int32_t node;
        ᬯᬮᬶ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(const uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    ᬬᬾᬦ᭄(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    ᬲᭂᬤᭂᬮ(length>kMaxBranchLinearSubNodeLength) {
        ᬬᬾᬦ᭄(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ᬮᬶᬬ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ᬬᬾᬦ᭄(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            ᬬᬾᬦ᭄(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } ᬮᬶᬬ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                ᬬᬾᬦ᭄(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } ᬮᬶᬬ ᬬᬾᬦ᭄(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } ᬮᬶᬬ ᬬᬾᬦ᭄(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } ᬮᬶᬬ ᬬᬾᬦ᭄(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } ᬮᬶᬬ {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ᬯᬮᬶ result;
        }
        --length;
        pos=skipValue(pos);
    } ᬲᭂᬤᭂᬮ(length>1);
    ᬬᬾᬦ᭄(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ᬯᬮᬶ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } ᬮᬶᬬ {
        stop();
        ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    for(;;) {
        int32_t node=*pos++;
        ᬬᬾᬦ᭄(node<kMinLinearMatch) {
            ᬯᬮᬶ branchNext(pos, node, inByte);
        } ᬮᬶᬬ ᬬᬾᬦ᭄(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ᬬᬾᬦ᭄(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ᬯᬮᬶ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } ᬮᬶᬬ {
                // No match.
                bha_nga;
            }
        } ᬮᬶᬬ ᬬᬾᬦ᭄(node&kValueIsFinal) {
            // No further matching bytes.
            bha_nga;
        } ᬮᬶᬬ {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    ᬬᬾᬦ᭄(pos==nullptr) {
        ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
    }
    ᬬᬾᬦ᭄(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ᬬᬾᬦ᭄(length>=0) {
        // Remaining part of a linear-match node.
        ᬬᬾᬦ᭄(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ᬯᬮᬶ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } ᬮᬶᬬ {
            stop();
            ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
        }
    }
    ᬯᬮᬶ nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const var_na *s, int32_t sLength) {
    ᬬᬾᬦ᭄(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ᬯᬮᬶ current();
    }
    const uint8_t *pos=pos_;
    ᬬᬾᬦ᭄(pos==nullptr) {
        ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    for(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        ᬬᬾᬦ᭄(sLength<0) {
            for(;;) {
                ᬬᬾᬦ᭄((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᬯᬮᬶ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ᬬᬾᬦ᭄(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᬬᬾᬦ᭄(inByte!=*pos) {
                    stop();
                    ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ᬮᬶᬬ {
            for(;;) {
                ᬬᬾᬦ᭄(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᬯᬮᬶ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                ᬬᬾᬦ᭄(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᬬᬾᬦ᭄(inByte!=*pos) {
                    stop();
                    ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        for(;;) {
            int32_t node=*pos++;
            ᬬᬾᬦ᭄(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                ᬬᬾᬦ᭄(result==USTRINGTRIE_NO_MATCH) {
                    ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                ᬬᬾᬦ᭄(sLength<0) {
                    ᬬᬾᬦ᭄((inByte=*s++)==0) {
                        ᬯᬮᬶ result;
                    }
                } ᬮᬶᬬ {
                    ᬬᬾᬦ᭄(sLength==0) {
                        ᬯᬮᬶ result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                ᬬᬾᬦ᭄(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } ᬮᬶᬬ ᬬᬾᬦ᭄(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ᬬᬾᬦ᭄(inByte!=*pos) {
                    stop();
                    ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } ᬮᬶᬬ ᬬᬾᬦ᭄(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                ᬯᬮᬶ USTRINGTRIE_NO_MATCH;
            } ᬮᬶᬬ {
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
    ᬲᭂᬤᭂᬮ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        ᬬᬾᬦ᭄(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ᬯᬮᬶ nullptr;
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
        ᬬᬾᬦ᭄(isFinal) {
            ᬬᬾᬦ᭄(haveUniqueValue) {
                ᬬᬾᬦ᭄(value!=uniqueValue) {
                    ᬯᬮᬶ nullptr;
                }
            } ᬮᬶᬬ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } ᬮᬶᬬ {
            ᬬᬾᬦ᭄(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ᬯᬮᬶ nullptr;
            }
            haveUniqueValue=true;
        }
    } ᬲᭂᬤᭂᬮ(--length>1);
    ᬯᬮᬶ pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(const uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    for(;;) {
        int32_t node=*pos++;
        ᬬᬾᬦ᭄(node<kMinLinearMatch) {
            ᬬᬾᬦ᭄(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ᬬᬾᬦ᭄(pos==nullptr) {
                ᬯᬮᬶ false;
            }
            haveUniqueValue=true;
        } ᬮᬶᬬ ᬬᬾᬦ᭄(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } ᬮᬶᬬ {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            ᬬᬾᬦ᭄(haveUniqueValue) {
                ᬬᬾᬦ᭄(value!=uniqueValue) {
                    ᬯᬮᬶ false;
                }
            } ᬮᬶᬬ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ᬬᬾᬦ᭄(isFinal) {
                ᬯᬮᬶ true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    ᬬᬾᬦ᭄(pos==nullptr) {
        ᬯᬮᬶ 0;
    }
    ᬬᬾᬦ᭄(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ᬯᬮᬶ 1;
    }
    int32_t node=*pos++;
    ᬬᬾᬦ᭄(node>=kMinValueLead) {
        ᬬᬾᬦ᭄(node&kValueIsFinal) {
            ᬯᬮᬶ 0;
        } ᬮᬶᬬ {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    ᬬᬾᬦ᭄(node<kMinLinearMatch) {
        ᬬᬾᬦ᭄(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        ᬯᬮᬶ node;
    } ᬮᬶᬬ {
        // First byte of the linear-match node.
        append(out, *pos);
        ᬯᬮᬶ 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    ᬲᭂᬤᭂᬮ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } ᬲᭂᬤᭂᬮ(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, puur_naa_nka c) {
    var_na ch = static_cast<var_na>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
