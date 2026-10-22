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
    ပေႃး(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } ပေႃးဢမ်ႇ ပေႃး(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } ပေႃးဢမ်ႇ ပေႃး(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } ပေႃးဢမ်ႇ ပေႃး(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } ပေႃးဢမ်ႇ {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    ပွၵ်ႈ value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    ပေႃး(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } ပေႃးဢမ်ႇ ပေႃး(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } ပေႃးဢမ်ႇ ပေႃး(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } ပေႃးဢမ်ႇ ပေႃး(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } ပေႃးဢမ်ႇ {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    ပွၵ်ႈ pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    ပေႃး(pos==nullptr) {
        ပွၵ်ႈ USTRINGTRIE_NO_MATCH;
    } ပေႃးဢမ်ႇ {
        int32_t node;
        ပွၵ်ႈ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(const uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    ပေႃး(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    ၶၢဝ်း(length>kMaxBranchLinearSubNodeLength) {
        ပေႃး(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ပေႃးဢမ်ႇ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ပေႃး(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            ပေႃး(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } ပေႃးဢမ်ႇ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                ပေႃး(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } ပေႃးဢမ်ႇ ပေႃး(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } ပေႃးဢမ်ႇ ပေႃး(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } ပေႃးဢမ်ႇ ပေႃး(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } ပေႃးဢမ်ႇ {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ပွၵ်ႈ result;
        }
        --length;
        pos=skipValue(pos);
    } ၶၢဝ်း(length>1);
    ပေႃး(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ပွၵ်ႈ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } ပေႃးဢမ်ႇ {
        stop();
        ပွၵ်ႈ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    for(;;) {
        int32_t node=*pos++;
        ပေႃး(node<kMinLinearMatch) {
            ပွၵ်ႈ branchNext(pos, node, inByte);
        } ပေႃးဢမ်ႇ ပေႃး(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ပေႃး(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ပွၵ်ႈ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } ပေႃးဢမ်ႇ {
                // No match.
                bha_nga;
            }
        } ပေႃးဢမ်ႇ ပေႃး(node&kValueIsFinal) {
            // No further matching bytes.
            bha_nga;
        } ပေႃးဢမ်ႇ {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    ပွၵ်ႈ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    ပေႃး(pos==nullptr) {
        ပွၵ်ႈ USTRINGTRIE_NO_MATCH;
    }
    ပေႃး(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ပေႃး(length>=0) {
        // Remaining part of a linear-match node.
        ပေႃး(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ပွၵ်ႈ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } ပေႃးဢမ်ႇ {
            stop();
            ပွၵ်ႈ USTRINGTRIE_NO_MATCH;
        }
    }
    ပွၵ်ႈ nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const var_na *s, int32_t sLength) {
    ပေႃး(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ပွၵ်ႈ current();
    }
    const uint8_t *pos=pos_;
    ပေႃး(pos==nullptr) {
        ပွၵ်ႈ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    for(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        ပေႃး(sLength<0) {
            for(;;) {
                ပေႃး((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ပွၵ်ႈ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ပေႃး(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ပေႃး(inByte!=*pos) {
                    stop();
                    ပွၵ်ႈ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ပေႃးဢမ်ႇ {
            for(;;) {
                ပေႃး(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ပွၵ်ႈ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                ပေႃး(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ပေႃး(inByte!=*pos) {
                    stop();
                    ပွၵ်ႈ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        for(;;) {
            int32_t node=*pos++;
            ပေႃး(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                ပေႃး(result==USTRINGTRIE_NO_MATCH) {
                    ပွၵ်ႈ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                ပေႃး(sLength<0) {
                    ပေႃး((inByte=*s++)==0) {
                        ပွၵ်ႈ result;
                    }
                } ပေႃးဢမ်ႇ {
                    ပေႃး(sLength==0) {
                        ပွၵ်ႈ result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                ပေႃး(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    ပွၵ်ႈ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } ပေႃးဢမ်ႇ ပေႃး(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ပေႃး(inByte!=*pos) {
                    stop();
                    ပွၵ်ႈ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } ပေႃးဢမ်ႇ ပေႃး(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                ပွၵ်ႈ USTRINGTRIE_NO_MATCH;
            } ပေႃးဢမ်ႇ {
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
    ၶၢဝ်း(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        ပေႃး(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ပွၵ်ႈ nullptr;
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
        ပေႃး(isFinal) {
            ပေႃး(haveUniqueValue) {
                ပေႃး(value!=uniqueValue) {
                    ပွၵ်ႈ nullptr;
                }
            } ပေႃးဢမ်ႇ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } ပေႃးဢမ်ႇ {
            ပေႃး(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ပွၵ်ႈ nullptr;
            }
            haveUniqueValue=true;
        }
    } ၶၢဝ်း(--length>1);
    ပွၵ်ႈ pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(const uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    for(;;) {
        int32_t node=*pos++;
        ပေႃး(node<kMinLinearMatch) {
            ပေႃး(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ပေႃး(pos==nullptr) {
                ပွၵ်ႈ false;
            }
            haveUniqueValue=true;
        } ပေႃးဢမ်ႇ ပေႃး(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } ပေႃးဢမ်ႇ {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            ပေႃး(haveUniqueValue) {
                ပေႃး(value!=uniqueValue) {
                    ပွၵ်ႈ false;
                }
            } ပေႃးဢမ်ႇ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ပေႃး(isFinal) {
                ပွၵ်ႈ true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    ပေႃး(pos==nullptr) {
        ပွၵ်ႈ 0;
    }
    ပေႃး(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ပွၵ်ႈ 1;
    }
    int32_t node=*pos++;
    ပေႃး(node>=kMinValueLead) {
        ပေႃး(node&kValueIsFinal) {
            ပွၵ်ႈ 0;
        } ပေႃးဢမ်ႇ {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    ပေႃး(node<kMinLinearMatch) {
        ပေႃး(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        ပွၵ်ႈ node;
    } ပေႃးဢမ်ႇ {
        // First byte of the linear-match node.
        append(out, *pos);
        ပွၵ်ႈ 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    ၶၢဝ်း(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } ၶၢဝ်း(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, puur_naa_nka c) {
    var_na ch = static_cast<var_na>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
