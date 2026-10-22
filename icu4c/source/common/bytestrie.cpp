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
    གལ་སྲིད(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } དེ་མིན གལ་སྲིད(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } དེ་མིན གལ་སྲིད(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } དེ་མིན གལ་སྲིད(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } དེ་མིན {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    ལོག value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    གལ་སྲིད(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } དེ་མིན གལ་སྲིད(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } དེ་མིན གལ་སྲིད(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } དེ་མིན གལ་སྲིད(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } དེ་མིན {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    ལོག pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    གལ་སྲིད(pos==nullptr) {
        ལོག USTRINGTRIE_NO_MATCH;
    } དེ་མིན {
        int32_t node;
        ལོག (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(const uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    གལ་སྲིད(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    སྐབས(length>kMaxBranchLinearSubNodeLength) {
        གལ་སྲིད(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } དེ་མིན {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        གལ་སྲིད(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            གལ་སྲིད(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } དེ་མིན {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                གལ་སྲིད(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } དེ་མིན གལ་སྲིད(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } དེ་མིན གལ་སྲིད(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } དེ་མིན གལ་སྲིད(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } དེ་མིན {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ལོག result;
        }
        --length;
        pos=skipValue(pos);
    } སྐབས(length>1);
    གལ་སྲིད(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ལོག node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } དེ་མིན {
        stop();
        ལོག USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    དོན་ལུ(;;) {
        int32_t node=*pos++;
        གལ་སྲིད(node<kMinLinearMatch) {
            ལོག branchNext(pos, node, inByte);
        } དེ་མིན གལ་སྲིད(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            གལ་སྲིད(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ལོག (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } དེ་མིན {
                // No match.
                bha_nga;
            }
        } དེ་མིན གལ་སྲིད(node&kValueIsFinal) {
            // No further matching bytes.
            bha_nga;
        } དེ་མིན {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    ལོག USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    གལ་སྲིད(pos==nullptr) {
        ལོག USTRINGTRIE_NO_MATCH;
    }
    གལ་སྲིད(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    གལ་སྲིད(length>=0) {
        // Remaining part of a linear-match node.
        གལ་སྲིད(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ལོག (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } དེ་མིན {
            stop();
            ལོག USTRINGTRIE_NO_MATCH;
        }
    }
    ལོག nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const var_na *s, int32_t sLength) {
    གལ་སྲིད(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ལོག current();
    }
    const uint8_t *pos=pos_;
    གལ་སྲིད(pos==nullptr) {
        ལོག USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    དོན་ལུ(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        གལ་སྲིད(sLength<0) {
            དོན་ལུ(;;) {
                གལ་སྲིད((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ལོག (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                གལ་སྲིད(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                གལ་སྲིད(inByte!=*pos) {
                    stop();
                    ལོག USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } དེ་མིན {
            དོན་ལུ(;;) {
                གལ་སྲིད(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ལོག (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                གལ་སྲིད(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                གལ་སྲིད(inByte!=*pos) {
                    stop();
                    ལོག USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        དོན་ལུ(;;) {
            int32_t node=*pos++;
            གལ་སྲིད(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                གལ་སྲིད(result==USTRINGTRIE_NO_MATCH) {
                    ལོག USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                གལ་སྲིད(sLength<0) {
                    གལ་སྲིད((inByte=*s++)==0) {
                        ལོག result;
                    }
                } དེ་མིན {
                    གལ་སྲིད(sLength==0) {
                        ལོག result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                གལ་སྲིད(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    ལོག USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } དེ་མིན གལ་སྲིད(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                གལ་སྲིད(inByte!=*pos) {
                    stop();
                    ལོག USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } དེ་མིན གལ་སྲིད(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                ལོག USTRINGTRIE_NO_MATCH;
            } དེ་མིན {
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
    སྐབས(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        གལ་སྲིད(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ལོག nullptr;
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
        གལ་སྲིད(isFinal) {
            གལ་སྲིད(haveUniqueValue) {
                གལ་སྲིད(value!=uniqueValue) {
                    ལོག nullptr;
                }
            } དེ་མིན {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } དེ་མིན {
            གལ་སྲིད(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ལོག nullptr;
            }
            haveUniqueValue=true;
        }
    } སྐབས(--length>1);
    ལོག pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(const uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    དོན་ལུ(;;) {
        int32_t node=*pos++;
        གལ་སྲིད(node<kMinLinearMatch) {
            གལ་སྲིད(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            གལ་སྲིད(pos==nullptr) {
                ལོག false;
            }
            haveUniqueValue=true;
        } དེ་མིན གལ་སྲིད(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } དེ་མིན {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            གལ་སྲིད(haveUniqueValue) {
                གལ་སྲིད(value!=uniqueValue) {
                    ལོག false;
                }
            } དེ་མིན {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            གལ་སྲིད(isFinal) {
                ལོག true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    གལ་སྲིད(pos==nullptr) {
        ལོག 0;
    }
    གལ་སྲིད(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ལོག 1;
    }
    int32_t node=*pos++;
    གལ་སྲིད(node>=kMinValueLead) {
        གལ་སྲིད(node&kValueIsFinal) {
            ལོག 0;
        } དེ་མིན {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    གལ་སྲིད(node<kMinLinearMatch) {
        གལ་སྲིད(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        ལོག node;
    } དེ་མིན {
        // First byte of the linear-match node.
        append(out, *pos);
        ལོག 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    སྐབས(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } སྐབས(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, ཧྲིལ c) {
    var_na ch = static_cast<var_na>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
