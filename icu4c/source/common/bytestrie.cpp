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
    ប្រសិនបើ(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } បើមិនដូច្នេះ ប្រសិនបើ(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } បើមិនដូច្នេះ ប្រសិនបើ(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } បើមិនដូច្នេះ ប្រសិនបើ(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } បើមិនដូច្នេះ {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    ត្រឡប់ value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    ប្រសិនបើ(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } បើមិនដូច្នេះ ប្រសិនបើ(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } បើមិនដូច្នេះ ប្រសិនបើ(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } បើមិនដូច្នេះ ប្រសិនបើ(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } បើមិនដូច្នេះ {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    ត្រឡប់ pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    ប្រសិនបើ(pos==nullptr) {
        ត្រឡប់ USTRINGTRIE_NO_MATCH;
    } បើមិនដូច្នេះ {
        int32_t node;
        ត្រឡប់ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(const uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    ប្រសិនបើ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    ខណៈពេល(length>kMaxBranchLinearSubNodeLength) {
        ប្រសិនបើ(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } បើមិនដូច្នេះ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        ប្រសិនបើ(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            ប្រសិនបើ(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } បើមិនដូច្នេះ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                ប្រសិនបើ(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } បើមិនដូច្នេះ ប្រសិនបើ(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } បើមិនដូច្នេះ ប្រសិនបើ(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } បើមិនដូច្នេះ ប្រសិនបើ(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } បើមិនដូច្នេះ {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ត្រឡប់ result;
        }
        --length;
        pos=skipValue(pos);
    } ខណៈពេល(length>1);
    ប្រសិនបើ(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ត្រឡប់ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } បើមិនដូច្នេះ {
        stop();
        ត្រឡប់ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    សម្រាប់(;;) {
        int32_t node=*pos++;
        ប្រសិនបើ(node<kMinLinearMatch) {
            ត្រឡប់ branchNext(pos, node, inByte);
        } បើមិនដូច្នេះ ប្រសិនបើ(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ប្រសិនបើ(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ត្រឡប់ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } បើមិនដូច្នេះ {
                // No match.
                bha_nga;
            }
        } បើមិនដូច្នេះ ប្រសិនបើ(node&kValueIsFinal) {
            // No further matching bytes.
            bha_nga;
        } បើមិនដូច្នេះ {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    ត្រឡប់ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    ប្រសិនបើ(pos==nullptr) {
        ត្រឡប់ USTRINGTRIE_NO_MATCH;
    }
    ប្រសិនបើ(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ប្រសិនបើ(length>=0) {
        // Remaining part of a linear-match node.
        ប្រសិនបើ(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ត្រឡប់ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } បើមិនដូច្នេះ {
            stop();
            ត្រឡប់ USTRINGTRIE_NO_MATCH;
        }
    }
    ត្រឡប់ nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const var_na *s, int32_t sLength) {
    ប្រសិនបើ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ត្រឡប់ current();
    }
    const uint8_t *pos=pos_;
    ប្រសិនបើ(pos==nullptr) {
        ត្រឡប់ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    សម្រាប់(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        ប្រសិនបើ(sLength<0) {
            សម្រាប់(;;) {
                ប្រសិនបើ((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ត្រឡប់ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ប្រសិនបើ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ប្រសិនបើ(inByte!=*pos) {
                    stop();
                    ត្រឡប់ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } បើមិនដូច្នេះ {
            សម្រាប់(;;) {
                ប្រសិនបើ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ត្រឡប់ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                ប្រសិនបើ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                ប្រសិនបើ(inByte!=*pos) {
                    stop();
                    ត្រឡប់ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        សម្រាប់(;;) {
            int32_t node=*pos++;
            ប្រសិនបើ(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                ប្រសិនបើ(result==USTRINGTRIE_NO_MATCH) {
                    ត្រឡប់ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                ប្រសិនបើ(sLength<0) {
                    ប្រសិនបើ((inByte=*s++)==0) {
                        ត្រឡប់ result;
                    }
                } បើមិនដូច្នេះ {
                    ប្រសិនបើ(sLength==0) {
                        ត្រឡប់ result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                ប្រសិនបើ(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    ត្រឡប់ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } បើមិនដូច្នេះ ប្រសិនបើ(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ប្រសិនបើ(inByte!=*pos) {
                    stop();
                    ត្រឡប់ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } បើមិនដូច្នេះ ប្រសិនបើ(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                ត្រឡប់ USTRINGTRIE_NO_MATCH;
            } បើមិនដូច្នេះ {
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
    ខណៈពេល(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        ប្រសិនបើ(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ត្រឡប់ nullptr;
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
        ប្រសិនបើ(isFinal) {
            ប្រសិនបើ(haveUniqueValue) {
                ប្រសិនបើ(value!=uniqueValue) {
                    ត្រឡប់ nullptr;
                }
            } បើមិនដូច្នេះ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } បើមិនដូច្នេះ {
            ប្រសិនបើ(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ត្រឡប់ nullptr;
            }
            haveUniqueValue=true;
        }
    } ខណៈពេល(--length>1);
    ត្រឡប់ pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(const uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    សម្រាប់(;;) {
        int32_t node=*pos++;
        ប្រសិនបើ(node<kMinLinearMatch) {
            ប្រសិនបើ(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ប្រសិនបើ(pos==nullptr) {
                ត្រឡប់ false;
            }
            haveUniqueValue=true;
        } បើមិនដូច្នេះ ប្រសិនបើ(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } បើមិនដូច្នេះ {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            ប្រសិនបើ(haveUniqueValue) {
                ប្រសិនបើ(value!=uniqueValue) {
                    ត្រឡប់ false;
                }
            } បើមិនដូច្នេះ {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            ប្រសិនបើ(isFinal) {
                ត្រឡប់ true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    ប្រសិនបើ(pos==nullptr) {
        ត្រឡប់ 0;
    }
    ប្រសិនបើ(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ត្រឡប់ 1;
    }
    int32_t node=*pos++;
    ប្រសិនបើ(node>=kMinValueLead) {
        ប្រសិនបើ(node&kValueIsFinal) {
            ត្រឡប់ 0;
        } បើមិនដូច្នេះ {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    ប្រសិនបើ(node<kMinLinearMatch) {
        ប្រសិនបើ(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        ត្រឡប់ node;
    } បើមិនដូច្នេះ {
        // First byte of the linear-match node.
        append(out, *pos);
        ត្រឡប់ 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    ខណៈពេល(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } ខណៈពេល(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, ចំនួនគត់ c) {
    var_na ch = static_cast<var_na>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
