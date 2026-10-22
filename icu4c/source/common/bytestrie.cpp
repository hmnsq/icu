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
    ถ้า(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } มิฉะนั้น ถ้า(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } มิฉะนั้น ถ้า(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } มิฉะนั้น ถ้า(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } มิฉะนั้น {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    คืน value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    ถ้า(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } มิฉะนั้น ถ้า(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } มิฉะนั้น ถ้า(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } มิฉะนั้น ถ้า(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } มิฉะนั้น {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    คืน pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    ถ้า(pos==nullptr) {
        คืน USTRINGTRIE_NO_MATCH;
    } มิฉะนั้น {
        int32_t node;
        คืน (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(const uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    ถ้า(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    ขณะ(length>kMaxBranchLinearSubNodeLength) {
        ถ้า(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } มิฉะนั้น {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ถ้า(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            ถ้า(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } มิฉะนั้น {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                ถ้า(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } มิฉะนั้น ถ้า(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } มิฉะนั้น ถ้า(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } มิฉะนั้น ถ้า(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } มิฉะนั้น {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            คืน result;
        }
        --length;
        pos=skipValue(pos);
    } ขณะ(length>1);
    ถ้า(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        คืน node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } มิฉะนั้น {
        stop();
        คืน USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    สำหรับ(;;) {
        int32_t node=*pos++;
        ถ้า(node<kMinLinearMatch) {
            คืน branchNext(pos, node, inByte);
        } มิฉะนั้น ถ้า(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ถ้า(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                คืน (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } มิฉะนั้น {
                // No match.
                bha_nga;
            }
        } มิฉะนั้น ถ้า(node&kValueIsFinal) {
            // No further matching bytes.
            bha_nga;
        } มิฉะนั้น {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    คืน USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    ถ้า(pos==nullptr) {
        คืน USTRINGTRIE_NO_MATCH;
    }
    ถ้า(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ถ้า(length>=0) {
        // Remaining part of a linear-match node.
        ถ้า(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            คืน (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } มิฉะนั้น {
            stop();
            คืน USTRINGTRIE_NO_MATCH;
        }
    }
    คืน nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const var_na *s, int32_t sLength) {
    ถ้า(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        คืน current();
    }
    const uint8_t *pos=pos_;
    ถ้า(pos==nullptr) {
        คืน USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    สำหรับ(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        ถ้า(sLength<0) {
            สำหรับ(;;) {
                ถ้า((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    คืน (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ถ้า(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ถ้า(inByte!=*pos) {
                    stop();
                    คืน USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } มิฉะนั้น {
            สำหรับ(;;) {
                ถ้า(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    คืน (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                ถ้า(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ถ้า(inByte!=*pos) {
                    stop();
                    คืน USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        สำหรับ(;;) {
            int32_t node=*pos++;
            ถ้า(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                ถ้า(result==USTRINGTRIE_NO_MATCH) {
                    คืน USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                ถ้า(sLength<0) {
                    ถ้า((inByte=*s++)==0) {
                        คืน result;
                    }
                } มิฉะนั้น {
                    ถ้า(sLength==0) {
                        คืน result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                ถ้า(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    คืน USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } มิฉะนั้น ถ้า(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ถ้า(inByte!=*pos) {
                    stop();
                    คืน USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } มิฉะนั้น ถ้า(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                คืน USTRINGTRIE_NO_MATCH;
            } มิฉะนั้น {
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
    ขณะ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        ถ้า(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            คืน nullptr;
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
        ถ้า(isFinal) {
            ถ้า(haveUniqueValue) {
                ถ้า(value!=uniqueValue) {
                    คืน nullptr;
                }
            } มิฉะนั้น {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } มิฉะนั้น {
            ถ้า(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                คืน nullptr;
            }
            haveUniqueValue=true;
        }
    } ขณะ(--length>1);
    คืน pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(const uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    สำหรับ(;;) {
        int32_t node=*pos++;
        ถ้า(node<kMinLinearMatch) {
            ถ้า(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ถ้า(pos==nullptr) {
                คืน false;
            }
            haveUniqueValue=true;
        } มิฉะนั้น ถ้า(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } มิฉะนั้น {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            ถ้า(haveUniqueValue) {
                ถ้า(value!=uniqueValue) {
                    คืน false;
                }
            } มิฉะนั้น {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ถ้า(isFinal) {
                คืน true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    ถ้า(pos==nullptr) {
        คืน 0;
    }
    ถ้า(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        คืน 1;
    }
    int32_t node=*pos++;
    ถ้า(node>=kMinValueLead) {
        ถ้า(node&kValueIsFinal) {
            คืน 0;
        } มิฉะนั้น {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    ถ้า(node<kMinLinearMatch) {
        ถ้า(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        คืน node;
    } มิฉะนั้น {
        // First byte of the linear-match node.
        append(out, *pos);
        คืน 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    ขณะ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } ขณะ(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, จำนวนเต็ม c) {
    var_na ch = static_cast<var_na>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
