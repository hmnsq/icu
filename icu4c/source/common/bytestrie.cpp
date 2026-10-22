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
    ᮚᮦᮔ᮪(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } ᮜᮤᮚ ᮚᮦᮔ᮪(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } ᮜᮤᮚ ᮚᮦᮔ᮪(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } ᮜᮤᮚ ᮚᮦᮔ᮪(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } ᮜᮤᮚ {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    ᮝᮜᮤ value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    ᮚᮦᮔ᮪(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } ᮜᮤᮚ ᮚᮦᮔ᮪(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } ᮜᮤᮚ ᮚᮦᮔ᮪(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } ᮜᮤᮚ ᮚᮦᮔ᮪(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } ᮜᮤᮚ {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    ᮝᮜᮤ pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    ᮚᮦᮔ᮪(pos==nullptr) {
        ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
    } ᮜᮤᮚ {
        int32_t node;
        ᮝᮜᮤ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(const uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    ᮚᮦᮔ᮪(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    ᮞᮨᮓᮨᮜ(length>kMaxBranchLinearSubNodeLength) {
        ᮚᮦᮔ᮪(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ᮜᮤᮚ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ᮚᮦᮔ᮪(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            ᮚᮦᮔ᮪(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } ᮜᮤᮚ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                ᮚᮦᮔ᮪(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } ᮜᮤᮚ ᮚᮦᮔ᮪(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } ᮜᮤᮚ ᮚᮦᮔ᮪(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } ᮜᮤᮚ ᮚᮦᮔ᮪(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } ᮜᮤᮚ {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ᮝᮜᮤ result;
        }
        --length;
        pos=skipValue(pos);
    } ᮞᮨᮓᮨᮜ(length>1);
    ᮚᮦᮔ᮪(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ᮝᮜᮤ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } ᮜᮤᮚ {
        stop();
        ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    for(;;) {
        int32_t node=*pos++;
        ᮚᮦᮔ᮪(node<kMinLinearMatch) {
            ᮝᮜᮤ branchNext(pos, node, inByte);
        } ᮜᮤᮚ ᮚᮦᮔ᮪(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ᮚᮦᮔ᮪(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ᮝᮜᮤ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } ᮜᮤᮚ {
                // No match.
                bha_nga;
            }
        } ᮜᮤᮚ ᮚᮦᮔ᮪(node&kValueIsFinal) {
            // No further matching bytes.
            bha_nga;
        } ᮜᮤᮚ {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    ᮚᮦᮔ᮪(pos==nullptr) {
        ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
    }
    ᮚᮦᮔ᮪(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ᮚᮦᮔ᮪(length>=0) {
        // Remaining part of a linear-match node.
        ᮚᮦᮔ᮪(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ᮝᮜᮤ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } ᮜᮤᮚ {
            stop();
            ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
        }
    }
    ᮝᮜᮤ nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const var_na *s, int32_t sLength) {
    ᮚᮦᮔ᮪(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ᮝᮜᮤ current();
    }
    const uint8_t *pos=pos_;
    ᮚᮦᮔ᮪(pos==nullptr) {
        ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    for(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        ᮚᮦᮔ᮪(sLength<0) {
            for(;;) {
                ᮚᮦᮔ᮪((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᮝᮜᮤ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ᮚᮦᮔ᮪(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᮚᮦᮔ᮪(inByte!=*pos) {
                    stop();
                    ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ᮜᮤᮚ {
            for(;;) {
                ᮚᮦᮔ᮪(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ᮝᮜᮤ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                ᮚᮦᮔ᮪(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ᮚᮦᮔ᮪(inByte!=*pos) {
                    stop();
                    ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        for(;;) {
            int32_t node=*pos++;
            ᮚᮦᮔ᮪(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                ᮚᮦᮔ᮪(result==USTRINGTRIE_NO_MATCH) {
                    ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                ᮚᮦᮔ᮪(sLength<0) {
                    ᮚᮦᮔ᮪((inByte=*s++)==0) {
                        ᮝᮜᮤ result;
                    }
                } ᮜᮤᮚ {
                    ᮚᮦᮔ᮪(sLength==0) {
                        ᮝᮜᮤ result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                ᮚᮦᮔ᮪(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } ᮜᮤᮚ ᮚᮦᮔ᮪(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ᮚᮦᮔ᮪(inByte!=*pos) {
                    stop();
                    ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } ᮜᮤᮚ ᮚᮦᮔ᮪(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                ᮝᮜᮤ USTRINGTRIE_NO_MATCH;
            } ᮜᮤᮚ {
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
    ᮞᮨᮓᮨᮜ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        ᮚᮦᮔ᮪(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ᮝᮜᮤ nullptr;
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
        ᮚᮦᮔ᮪(isFinal) {
            ᮚᮦᮔ᮪(haveUniqueValue) {
                ᮚᮦᮔ᮪(value!=uniqueValue) {
                    ᮝᮜᮤ nullptr;
                }
            } ᮜᮤᮚ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } ᮜᮤᮚ {
            ᮚᮦᮔ᮪(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ᮝᮜᮤ nullptr;
            }
            haveUniqueValue=true;
        }
    } ᮞᮨᮓᮨᮜ(--length>1);
    ᮝᮜᮤ pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(const uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    for(;;) {
        int32_t node=*pos++;
        ᮚᮦᮔ᮪(node<kMinLinearMatch) {
            ᮚᮦᮔ᮪(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ᮚᮦᮔ᮪(pos==nullptr) {
                ᮝᮜᮤ false;
            }
            haveUniqueValue=true;
        } ᮜᮤᮚ ᮚᮦᮔ᮪(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } ᮜᮤᮚ {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            ᮚᮦᮔ᮪(haveUniqueValue) {
                ᮚᮦᮔ᮪(value!=uniqueValue) {
                    ᮝᮜᮤ false;
                }
            } ᮜᮤᮚ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ᮚᮦᮔ᮪(isFinal) {
                ᮝᮜᮤ true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    ᮚᮦᮔ᮪(pos==nullptr) {
        ᮝᮜᮤ 0;
    }
    ᮚᮦᮔ᮪(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ᮝᮜᮤ 1;
    }
    int32_t node=*pos++;
    ᮚᮦᮔ᮪(node>=kMinValueLead) {
        ᮚᮦᮔ᮪(node&kValueIsFinal) {
            ᮝᮜᮤ 0;
        } ᮜᮤᮚ {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    ᮚᮦᮔ᮪(node<kMinLinearMatch) {
        ᮚᮦᮔ᮪(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        ᮝᮜᮤ node;
    } ᮜᮤᮚ {
        // First byte of the linear-match node.
        append(out, *pos);
        ᮝᮜᮤ 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    ᮞᮨᮓᮨᮜ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } ᮞᮨᮓᮨᮜ(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, puur_naa_nka c) {
    var_na ch = static_cast<var_na>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
