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
    ຖ້າ(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } ບໍ່ດັ່ງນັ້ນ {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    ສົ່ງຄືນ value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    ຖ້າ(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } ບໍ່ດັ່ງນັ້ນ {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    ສົ່ງຄືນ pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    ຖ້າ(pos==nullptr) {
        ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
    } ບໍ່ດັ່ງນັ້ນ {
        int32_t node;
        ສົ່ງຄືນ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(const uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    ຖ້າ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    ໃນຂະນະທີ່(length>kMaxBranchLinearSubNodeLength) {
        ຖ້າ(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ບໍ່ດັ່ງນັ້ນ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ຖ້າ(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            ຖ້າ(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } ບໍ່ດັ່ງນັ້ນ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                ຖ້າ(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } ບໍ່ດັ່ງນັ້ນ ຖ້າ(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } ບໍ່ດັ່ງນັ້ນ ຖ້າ(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } ບໍ່ດັ່ງນັ້ນ ຖ້າ(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } ບໍ່ດັ່ງນັ້ນ {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ສົ່ງຄືນ result;
        }
        --length;
        pos=skipValue(pos);
    } ໃນຂະນະທີ່(length>1);
    ຖ້າ(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ສົ່ງຄືນ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } ບໍ່ດັ່ງນັ້ນ {
        stop();
        ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    ສຳລັບ(;;) {
        int32_t node=*pos++;
        ຖ້າ(node<kMinLinearMatch) {
            ສົ່ງຄືນ branchNext(pos, node, inByte);
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ຖ້າ(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ສົ່ງຄືນ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } ບໍ່ດັ່ງນັ້ນ {
                // No match.
                bha_nga;
            }
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ(node&kValueIsFinal) {
            // No further matching bytes.
            bha_nga;
        } ບໍ່ດັ່ງນັ້ນ {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    ຖ້າ(pos==nullptr) {
        ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
    }
    ຖ້າ(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ຖ້າ(length>=0) {
        // Remaining part of a linear-match node.
        ຖ້າ(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ສົ່ງຄືນ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } ບໍ່ດັ່ງນັ້ນ {
            stop();
            ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
        }
    }
    ສົ່ງຄືນ nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const var_na *s, int32_t sLength) {
    ຖ້າ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ສົ່ງຄືນ current();
    }
    const uint8_t *pos=pos_;
    ຖ້າ(pos==nullptr) {
        ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ສຳລັບ(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        ຖ້າ(sLength<0) {
            ສຳລັບ(;;) {
                ຖ້າ((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ສົ່ງຄືນ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ຖ້າ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ຖ້າ(inByte!=*pos) {
                    stop();
                    ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ບໍ່ດັ່ງນັ້ນ {
            ສຳລັບ(;;) {
                ຖ້າ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ສົ່ງຄືນ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                ຖ້າ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ຖ້າ(inByte!=*pos) {
                    stop();
                    ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        ສຳລັບ(;;) {
            int32_t node=*pos++;
            ຖ້າ(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                ຖ້າ(result==USTRINGTRIE_NO_MATCH) {
                    ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                ຖ້າ(sLength<0) {
                    ຖ້າ((inByte=*s++)==0) {
                        ສົ່ງຄືນ result;
                    }
                } ບໍ່ດັ່ງນັ້ນ {
                    ຖ້າ(sLength==0) {
                        ສົ່ງຄືນ result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                ຖ້າ(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } ບໍ່ດັ່ງນັ້ນ ຖ້າ(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ຖ້າ(inByte!=*pos) {
                    stop();
                    ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } ບໍ່ດັ່ງນັ້ນ ຖ້າ(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                ສົ່ງຄືນ USTRINGTRIE_NO_MATCH;
            } ບໍ່ດັ່ງນັ້ນ {
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
    ໃນຂະນະທີ່(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        ຖ້າ(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ສົ່ງຄືນ nullptr;
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
        ຖ້າ(isFinal) {
            ຖ້າ(haveUniqueValue) {
                ຖ້າ(value!=uniqueValue) {
                    ສົ່ງຄືນ nullptr;
                }
            } ບໍ່ດັ່ງນັ້ນ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } ບໍ່ດັ່ງນັ້ນ {
            ຖ້າ(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ສົ່ງຄືນ nullptr;
            }
            haveUniqueValue=true;
        }
    } ໃນຂະນະທີ່(--length>1);
    ສົ່ງຄືນ pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(const uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    ສຳລັບ(;;) {
        int32_t node=*pos++;
        ຖ້າ(node<kMinLinearMatch) {
            ຖ້າ(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ຖ້າ(pos==nullptr) {
                ສົ່ງຄືນ false;
            }
            haveUniqueValue=true;
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } ບໍ່ດັ່ງນັ້ນ {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            ຖ້າ(haveUniqueValue) {
                ຖ້າ(value!=uniqueValue) {
                    ສົ່ງຄືນ false;
                }
            } ບໍ່ດັ່ງນັ້ນ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ຖ້າ(isFinal) {
                ສົ່ງຄືນ true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    ຖ້າ(pos==nullptr) {
        ສົ່ງຄືນ 0;
    }
    ຖ້າ(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ສົ່ງຄືນ 1;
    }
    int32_t node=*pos++;
    ຖ້າ(node>=kMinValueLead) {
        ຖ້າ(node&kValueIsFinal) {
            ສົ່ງຄືນ 0;
        } ບໍ່ດັ່ງນັ້ນ {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    ຖ້າ(node<kMinLinearMatch) {
        ຖ້າ(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        ສົ່ງຄືນ node;
    } ບໍ່ດັ່ງນັ້ນ {
        // First byte of the linear-match node.
        append(out, *pos);
        ສົ່ງຄືນ 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    ໃນຂະນະທີ່(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } ໃນຂະນະທີ່(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, ຈຳນວນເຕັມ c) {
    var_na ch = static_cast<var_na>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
