// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ಅನ್ತರ್ಭಾವ <complex>
#ಅನ್ತರ್ಭಾವ <utility>

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "brkeng.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "lstmbe.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ubrkimpl.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "uvectr32.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"

#ಅನ್ತರ್ಭಾವ "unicode/brkiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/resbund.h"
#ಅನ್ತರ್ಭಾವ "unicode/ubrk.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf.h"

U_NAMESPACE_BEGIN

// Uncomment the following #define to debug.
// #define LSTM_DEBUG 1
// #define LSTM_VECTORIZER_DEBUG 1

/**
 * Interface for reading 1D array.
 */
ವರ್ಗ ReadArray1D {
ಸಾರ್ವಜನಿಕ:
    ಆಭಾಸ ~ReadArray1D();
    ಆಭಾಸ int32_t d1() ಧ್ರುವ = 0;
    ಆಭಾಸ ಭಿನ್ನ get(int32_t i) ಧ್ರುವ = 0;

#ifdef LSTM_DEBUG
    ಶೂನ್ಯ print() ಧ್ರುವ {
        printf("\n[");
        ಪ್ರತಿ (int32_t i = 0; i < d1(); i++) {
           printf("%0.8e ", get(i));
           ಯದಿ (i % 4 == 3) printf("\n");
        }
        printf("]\n");
    }
#endif
};

ReadArray1D::~ReadArray1D()
{
}

/**
 * Interface for reading 2D array.
 */
ವರ್ಗ ReadArray2D {
ಸಾರ್ವಜನಿಕ:
    ಆಭಾಸ ~ReadArray2D();
    ಆಭಾಸ int32_t d1() ಧ್ರುವ = 0;
    ಆಭಾಸ int32_t d2() ಧ್ರುವ = 0;
    ಆಭಾಸ ಭಿನ್ನ get(int32_t i, int32_t j) ಧ್ರುವ = 0;
};

ReadArray2D::~ReadArray2D()
{
}

/**
 * A class to index a float array as a 1D Array without owning the pointer or
 * copy the data.
 */
ವರ್ಗ ConstArray1D : ಸಾರ್ವಜನಿಕ ReadArray1D {
ಸಾರ್ವಜನಿಕ:
    ConstArray1D() : data_(nullptr), d1_(0) {}

    ConstArray1D(ಧ್ರುವ ಭಿನ್ನ* data, int32_t d1) : data_(data), d1_(d1) {}

    ಆಭಾಸ ~ConstArray1D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    ಶೂನ್ಯ init(ಧ್ರುವ int32_t* data, int32_t d1) {
        U_ASSERT(IEEE_754 == 1);
        data_ = reinterpret_cast<ಧ್ರುವ ಭಿನ್ನ*>(data);
        d1_ = d1;
    }

    // ReadArray1D methods.
    ಆಭಾಸ int32_t d1() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ d1_; }
    ಆಭಾಸ ಭಿನ್ನ get(int32_t i) ಧ್ರುವ override {
        U_ASSERT(i < d1_);
        ಪ್ರತ್ಯಾಗಮ data_[i];
    }

ಗುಪ್ತ:
    ಧ್ರುವ ಭಿನ್ನ* data_;
    int32_t d1_;
};

ConstArray1D::~ConstArray1D()
{
}

/**
 * A class to index a float array as a 2D Array without owning the pointer or
 * copy the data.
 */
ವರ್ಗ ConstArray2D : ಸಾರ್ವಜನಿಕ ReadArray2D {
ಸಾರ್ವಜನಿಕ:
    ConstArray2D() : data_(nullptr), d1_(0), d2_(0) {}

    ConstArray2D(ಧ್ರುವ ಭಿನ್ನ* data, int32_t d1, int32_t d2)
        : data_(data), d1_(d1), d2_(d2) {}

    ಆಭಾಸ ~ConstArray2D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    ಶೂನ್ಯ init(ಧ್ರುವ int32_t* data, int32_t d1, int32_t d2) {
        U_ASSERT(IEEE_754 == 1);
        data_ = reinterpret_cast<ಧ್ರುವ ಭಿನ್ನ*>(data);
        d1_ = d1;
        d2_ = d2;
    }

    // ReadArray2D methods.
    ಅನ್ತಃಸ್ಥ int32_t d1() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ d1_; }
    ಅನ್ತಃಸ್ಥ int32_t d2() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ d2_; }
    ಭಿನ್ನ get(int32_t i, int32_t j) ಧ್ರುವ override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        ಪ್ರತ್ಯಾಗಮ data_[i * d2_ + j];
    }

    // Expose the ith row as a ConstArray1D
    ಅನ್ತಃಸ್ಥ ConstArray1D row(int32_t i) ಧ್ರುವ {
        U_ASSERT(i < d1_);
        ಪ್ರತ್ಯಾಗಮ ConstArray1D(data_ + i * d2_, d2_);
    }

ಗುಪ್ತ:
    ಧ್ರುವ ಭಿನ್ನ* data_;
    int32_t d1_;
    int32_t d2_;
};

ConstArray2D::~ConstArray2D()
{
}

/**
 * A class to allocate data as a writable 1D array.
 * This is the main class implement matrix operation.
 */
ವರ್ಗ Array1D : ಸಾರ್ವಜನಿಕ ReadArray1D {
ಸಾರ್ವಜನಿಕ:
    Array1D() : memory_(nullptr), data_(nullptr), d1_(0) {}
    Array1D(int32_t d1, UErrorCode &status)
        : memory_(uprv_malloc(d1 * ಪರಿಮಾಣ(ಭಿನ್ನ))),
          data_(static_cast<ಭಿನ್ನ*>(memory_)), d1_(d1) {
        ಯದಿ (U_SUCCESS(status)) {
            ಯದಿ (memory_ == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ;
            }
            clear();
        }
    }

    ಆಭಾಸ ~Array1D();

    // A special constructor which does not own the memory but writeable
    // as a slice of an array.
    Array1D(ಭಿನ್ನ* data, int32_t d1)
        : memory_(nullptr), data_(data), d1_(d1) {}

    // ReadArray1D methods.
    ಆಭಾಸ int32_t d1() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ d1_; }
    ಆಭಾಸ ಭಿನ್ನ get(int32_t i) ಧ್ರುವ override {
        U_ASSERT(i < d1_);
        ಪ್ರತ್ಯಾಗಮ data_[i];
    }

    // Return the index which point to the max data in the array.
    ಅನ್ತಃಸ್ಥ int32_t maxIndex() ಧ್ರುವ {
        int32_t index = 0;
        ಭಿನ್ನ max = data_[0];
        ಪ್ರತಿ (int32_t i = 1; i < d1_; i++) {
            ಯದಿ (data_[i] > max) {
                max = data_[i];
                index = i;
            }
        }
        ಪ್ರತ್ಯಾಗಮ index;
    }

    // Slice part of the array to a new one.
    ಅನ್ತಃಸ್ಥ Array1D slice(int32_t from, int32_t size) ಧ್ರುವ {
        U_ASSERT(from >= 0);
        U_ASSERT(from < d1_);
        U_ASSERT(from + size <= d1_);
        ಪ್ರತ್ಯಾಗಮ Array1D(data_ + from, size);
    }

    // Add dot product of a 1D array and a 2D array into this one.
    ಅನ್ತಃಸ್ಥ Array1D& addDotProduct(ಧ್ರುವ ReadArray1D& a, ಧ್ರುವ ReadArray2D& b) {
        U_ASSERT(a.d1() == b.d1());
        U_ASSERT(b.d2() == d1());
        ಪ್ರತಿ (int32_t i = 0; i < d1(); i++) {
            ಪ್ರತಿ (int32_t j = 0; j < a.d1(); j++) {
                data_[i] += a.get(j) * b.get(j, i);
            }
        }
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }

    // Hadamard Product the values of another array of the same size into this one.
    ಅನ್ತಃಸ್ಥ Array1D& hadamardProduct(ಧ್ರುವ ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        ಪ್ರತಿ (int32_t i = 0; i < d1(); i++) {
            data_[i] *= a.get(i);
        }
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }

    // Add the Hadamard Product of two arrays of the same size into this one.
    ಅನ್ತಃಸ್ಥ Array1D& addHadamardProduct(ಧ್ರುವ ReadArray1D& a, ಧ್ರುವ ReadArray1D& b) {
        U_ASSERT(a.d1() == d1());
        U_ASSERT(b.d1() == d1());
        ಪ್ರತಿ (int32_t i = 0; i < d1(); i++) {
            data_[i] += a.get(i) * b.get(i);
        }
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }

    // Add the values of another array of the same size into this one.
    ಅನ್ತಃಸ್ಥ Array1D& add(ಧ್ರುವ ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        ಪ್ರತಿ (int32_t i = 0; i < d1(); i++) {
            data_[i] += a.get(i);
        }
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }

    // Assign the values of another array of the same size into this one.
    ಅನ್ತಃಸ್ಥ Array1D& assign(ಧ್ರುವ ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        ಪ್ರತಿ (int32_t i = 0; i < d1(); i++) {
            data_[i] = a.get(i);
        }
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }

    // Apply tanh to all the elements in the array.
    ಅನ್ತಃಸ್ಥ Array1D& tanh() {
        ಪ್ರತ್ಯಾಗಮ tanh(*ಏತತ್);
    }

    // Apply tanh of a and store into this array.
    ಅನ್ತಃಸ್ಥ Array1D& tanh(ಧ್ರುವ Array1D& a) {
        U_ASSERT(a.d1() == d1());
        ಪ್ರತಿ (int32_t i = 0; i < d1_; i++) {
            data_[i] = std::tanh(a.get(i));
        }
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }

    // Apply sigmoid to all the elements in the array.
    ಅನ್ತಃಸ್ಥ Array1D& sigmoid() {
        ಪ್ರತಿ (int32_t i = 0; i < d1_; i++) {
            data_[i] = 1.0f/(1.0f + expf(-data_[i]));
        }
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }

    ಅನ್ತಃಸ್ಥ Array1D& clear() {
        uprv_memset(data_, 0, d1_ * ಪರಿಮಾಣ(ಭಿನ್ನ));
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }

ಗುಪ್ತ:
    ಶೂನ್ಯ* memory_;
    ಭಿನ್ನ* data_;
    int32_t d1_;
};

Array1D::~Array1D()
{
    uprv_free(memory_);
}

ವರ್ಗ Array2D : ಸಾರ್ವಜನಿಕ ReadArray2D {
ಸಾರ್ವಜನಿಕ:
    Array2D() : memory_(nullptr), data_(nullptr), d1_(0), d2_(0) {}
    Array2D(int32_t d1, int32_t d2, UErrorCode &status)
        : memory_(uprv_malloc(d1 * d2 * ಪರಿಮಾಣ(ಭಿನ್ನ))),
          data_(static_cast<ಭಿನ್ನ*>(memory_)), d1_(d1), d2_(d2) {
        ಯದಿ (U_SUCCESS(status)) {
            ಯದಿ (memory_ == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ;
            }
            clear();
        }
    }
    ಆಭಾಸ ~Array2D();

    // ReadArray2D methods.
    ಆಭಾಸ int32_t d1() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ d1_; }
    ಆಭಾಸ int32_t d2() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ d2_; }
    ಆಭಾಸ ಭಿನ್ನ get(int32_t i, int32_t j) ಧ್ರುವ override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        ಪ್ರತ್ಯಾಗಮ data_[i * d2_ + j];
    }

    ಅನ್ತಃಸ್ಥ Array1D row(int32_t i) ಧ್ರುವ {
        U_ASSERT(i < d1_);
        ಪ್ರತ್ಯಾಗಮ Array1D(data_ + i * d2_, d2_);
    }

    ಅನ್ತಃಸ್ಥ Array2D& clear() {
        uprv_memset(data_, 0, d1_ * d2_ * ಪರಿಮಾಣ(ಭಿನ್ನ));
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }

ಗುಪ್ತ:
    ಶೂನ್ಯ* memory_;
    ಭಿನ್ನ* data_;
    int32_t d1_;
    int32_t d2_;
};

Array2D::~Array2D()
{
    uprv_free(memory_);
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    BEGIN,
    INSIDE,
    END,
    SINGLE
} LSTMClass;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    UNKNOWN,
    CODE_POINTS,
    GRAPHEME_CLUSTER,
} EmbeddingType;

ಸಂರಚನಾ LSTMData : ಸಾರ್ವಜನಿಕ UMemory {
    LSTMData(UResourceBundle* rb, UErrorCode &status);
    ~LSTMData();
    UHashtable* fDict;
    EmbeddingType fType;
    ಧ್ರುವ char16_t* fName;
    ConstArray2D fEmbedding;
    ConstArray2D fForwardW;
    ConstArray2D fForwardU;
    ConstArray1D fForwardB;
    ConstArray2D fBackwardW;
    ConstArray2D fBackwardU;
    ConstArray1D fBackwardB;
    ConstArray2D fOutputW;
    ConstArray1D fOutputB;

ಗುಪ್ತ:
    UResourceBundle* fBundle;
};

LSTMData::LSTMData(UResourceBundle* rb, UErrorCode &status)
    : fDict(nullptr), fType(UNKNOWN), fName(nullptr),
      fBundle(rb)
{
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (IEEE_754 != 1) {
        status = U_UNSUPPORTED_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    LocalUResourceBundlePointer embeddings_res(
        ures_get_By_ಕೀಲಿ(rb, "embeddings", nullptr, &status));
    int32_t embedding_size = ures_getInt(embeddings_res.getAlias(), &status);
    LocalUResourceBundlePointer hunits_res(
        ures_get_By_ಕೀಲಿ(rb, "hunits", nullptr, &status));
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ;
    int32_t hunits = ures_getInt(hunits_res.getAlias(), &status);
    ಧ್ರುವ char16_t* type = ures_get_String_By_ಕೀಲಿ(rb, "type", nullptr, &status);
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (u_strCompare(type, -1, u"codepoints", -1, ಅಸತ್ಯ) == 0) {
        fType = CODE_POINTS;
    } ಅನ್ಯಥಾ ಯದಿ (u_strCompare(type, -1, u"graphclust", -1, ಅಸತ್ಯ) == 0) {
        fType = GRAPHEME_CLUSTER;
    }
    fName = ures_get_String_By_ಕೀಲಿ(rb, "model", nullptr, &status);
    LocalUResourceBundlePointer dataRes(ures_get_By_ಕೀಲಿ(rb, "data", nullptr, &status));
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ;
    int32_t data_len = 0;
    ಧ್ರುವ int32_t* data = ures_getIntVector(dataRes.getAlias(), &data_len, &status);
    fDict = uhash_open(uhash_hashUChars, uhash_compareUChars, nullptr, &status);

    ರಾಶಿ_UResource_Bundle ರಾಶಿ_Temp_Bundle;
    Resource_Data_ಮೌಲ್ಯ value;
    ures_get_ಮೌಲ್ಯ_With_Fallback(rb, "dict", ರಾಶಿ_Temp_Bundle.getAlias(), value, status);
    ResourceArray stringArray = value.getArray(status);
    int32_t ಅಂಕೆ_index = stringArray.getSize();
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }

    // put dict into hash
    int32_t stringLength;
    ಪ್ರತಿ (int32_t idx = 0; idx < ಅಂಕೆ_index; idx++) {
        stringArray.get_ಮೌಲ್ಯ(idx, value);
        ಧ್ರುವ char16_t* str = value.getString(stringLength, status);
        uhash_putiAllowZero(fDict, (ಶೂನ್ಯ*)str, idx, &status);
        ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ;
#ifdef LSTM_VECTORIZER_DEBUG
        printf("Assign [");
        ಯಾವತ್ (*str != 0x0000) {
            printf("U+%04x ", *str);
            str++;
        }
        printf("] map to %d\n", idx-1);
#endif
    }
    int32_t mat1_size = (ಅಂಕೆ_index + 1) * embedding_size;
    int32_t mat2_size = embedding_size * 4 * hunits;
    int32_t mat3_size = hunits * 4 * hunits;
    int32_t mat4_size = 4 * hunits;
    int32_t mat5_size = mat2_size;
    int32_t mat6_size = mat3_size;
    int32_t mat7_size = mat4_size;
    int32_t mat8_size = 2 * hunits * 4;
#ಯದಿ U_DEBUG
    int32_t mat9_size = 4;
    U_ASSERT(data_len == mat1_size + mat2_size + mat3_size + mat4_size + mat5_size +
        mat6_size + mat7_size + mat8_size + mat9_size);
#endif

    fEmbedding.init(data, (ಅಂಕೆ_index + 1), embedding_size);
    data += mat1_size;
    fForwardW.init(data, embedding_size, 4 * hunits);
    data += mat2_size;
    fForwardU.init(data, hunits, 4 * hunits);
    data += mat3_size;
    fForwardB.init(data, 4 * hunits);
    data += mat4_size;
    fBackwardW.init(data, embedding_size, 4 * hunits);
    data += mat5_size;
    fBackwardU.init(data, hunits, 4 * hunits);
    data += mat6_size;
    fBackwardB.init(data, 4 * hunits);
    data += mat7_size;
    fOutputW.init(data, 2 * hunits, 4);
    data += mat8_size;
    fOutputB.init(data, 4);
}

LSTMData::~LSTMData() {
    uhash_close(fDict);
    ures_close(fBundle);
}

ವರ್ಗ Vectorizer : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    Vectorizer(UHashtable* dict) : fDict(dict) {}
    ಆಭಾಸ ~Vectorizer();
    ಆಭಾಸ ಶೂನ್ಯ vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) ಧ್ರುವ = 0;
ರಕ್ಷಿತ:
    int32_t stringToIndex(ಧ್ರುವ char16_t* str) ಧ್ರುವ {
        UBool found = ಅಸತ್ಯ;
        int32_t ret = uhash_getiAndFound(fDict, (ಧ್ರುವ ಶೂನ್ಯ*)str, &found);
        ಯದಿ (!found) {
            ret = fDict->count;
        }
#ifdef LSTM_VECTORIZER_DEBUG
        printf("[");
        ಯಾವತ್ (*str != 0x0000) {
            printf("U+%04x ", *str);
            str++;
        }
        printf("] map to %d\n", ret);
#endif
        ಪ್ರತ್ಯಾಗಮ ret;
    }

ಗುಪ್ತ:
    UHashtable* fDict;
};

Vectorizer::~Vectorizer()
{
}

ವರ್ಗ CodePointsVectorizer : ಸಾರ್ವಜನಿಕ Vectorizer {
ಸಾರ್ವಜನಿಕ:
    CodePointsVectorizer(UHashtable* dict) : Vectorizer(dict) {}
    ಆಭಾಸ ~CodePointsVectorizer();
    ಆಭಾಸ ಶೂನ್ಯ vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) ಧ್ರುವ override;
};

CodePointsVectorizer::~CodePointsVectorizer()
{
}

ಶೂನ್ಯ CodePointsVectorizer::vectorize(
    UText *text, int32_t startPos, int32_t endPos,
    UVector32 &offsets, UVector32 &indices, UErrorCode &status) ಧ್ರುವ
{
    ಯದಿ (offsets.ensureCapacity(endPos - startPos, status) &&
            indices.ensureCapacity(endPos - startPos, status)) {
        ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ;
        utext_setNativeIndex(text, startPos);
        int32_t current;
        char16_t str[2] = {0, 0};
        ಯಾವತ್ (U_SUCCESS(status) &&
               (current = static_cast<int32_t>(utext_getNativeIndex(text))) < endPos) {
            // Since the LSTMBreakEngine is currently only accept chars in BMP,
            // we can ignore the possibility of hitting supplementary code
            // point.
            str[0] = static_cast<char16_t>(utext_next32(text));
            U_ASSERT(!U_IS_SURROGATE(str[0]));
            offsets.addElement(current, status);
            indices.addElement(stringToIndex(str), status);
        }
    }
}

ವರ್ಗ GraphemeClusterVectorizer : ಸಾರ್ವಜನಿಕ Vectorizer {
ಸಾರ್ವಜನಿಕ:
    GraphemeClusterVectorizer(UHashtable* dict)
        : Vectorizer(dict)
    {
    }
    ಆಭಾಸ ~GraphemeClusterVectorizer();
    ಆಭಾಸ ಶೂನ್ಯ vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) ಧ್ರುವ override;
};

GraphemeClusterVectorizer::~GraphemeClusterVectorizer()
{
}

constexpr int32_t MAX_GRAPHEME_CLSTER_LENGTH = 10;

ಶೂನ್ಯ GraphemeClusterVectorizer::vectorize(
    UText *text, int32_t startPos, int32_t endPos,
    UVector32 &offsets, UVector32 &indices, UErrorCode &status) ಧ್ರುವ
{
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (!offsets.ensureCapacity(endPos - startPos, status) ||
            !indices.ensureCapacity(endPos - startPos, status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ;
    LocalPointer<BreakIterator> graphemeIter(BreakIterator::createCharacterInstance(Locale(), status));
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ;
    graphemeIter->setText(text, status);
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (startPos != 0) {
        graphemeIter->preceding(startPos);
    }
    int32_t last = startPos;
    int32_t current = startPos;
    char16_t str[MAX_GRAPHEME_CLSTER_LENGTH];
    ಯಾವತ್ ((current = graphemeIter->next()) != BreakIterator::DONE) {
        ಯದಿ (current >= endPos) {
            ಭಙ್ಗ;
        }
        ಯದಿ (current > startPos) {
            utext_extract(text, last, current, str, MAX_GRAPHEME_CLSTER_LENGTH, &status);
            ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ;
            offsets.addElement(last, status);
            indices.addElement(stringToIndex(str), status);
            ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ;
        }
        last = current;
    }
    ಯದಿ (U_FAILURE(status) || last >= endPos) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    utext_extract(text, last, endPos, str, MAX_GRAPHEME_CLSTER_LENGTH, &status);
    ಯದಿ (U_SUCCESS(status)) {
        offsets.addElement(last, status);
        indices.addElement(stringToIndex(str), status);
    }
}

// Computing LSTM as stated in
// https://en.wikipedia.org/wiki/Long_short-term_memory#LSTM_with_a_forget_gate
// ifco is temp array allocate outside which does not need to be
// input/output value but could avoid unnecessary memory alloc/free if passing
// in.
ಶೂನ್ಯ compute(
    int32_t hunits,
    ಧ್ರುವ ReadArray2D& W, ಧ್ರುವ ReadArray2D& U, ಧ್ರುವ ReadArray1D& b,
    ಧ್ರುವ ReadArray1D& x, Array1D& h, Array1D& c,
    Array1D& ifco)
{
    // ifco = x * W + h * U + b
    ifco.assign(b)
        .addDotProduct(x, W)
        .addDotProduct(h, U);

    ifco.slice(0*hunits, hunits).sigmoid();  // i: sigmod
    ifco.slice(1*hunits, hunits).sigmoid(); // f: sigmoid
    ifco.slice(2*hunits, hunits).tanh(); // c_: tanh
    ifco.slice(3*hunits, hunits).sigmoid(); // o: sigmod

    c.hadamardProduct(ifco.slice(hunits, hunits))
        .addHadamardProduct(ifco.slice(0, hunits), ifco.slice(2*hunits, hunits));

    h.tanh(c)
        .hadamardProduct(ifco.slice(3*hunits, hunits));
}

// Minimum word size
ಸ್ಥಿರ ಧ್ರುವ int32_t MIN_WORD = 2;

// Minimum number of characters for two words
ಸ್ಥಿರ ಧ್ರುವ int32_t MIN_WORD_SPAN = MIN_WORD * 2;

int32_t
LSTMBreakEngine::divideUpDictionaryRange( UText *text,
                                                int32_t startPos,
                                                int32_t endPos,
                                                UVector32 &foundBreaks,
                                                UBool /* isPhraseBreaking */,
                                                UErrorCode& status) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ 0;
    int32_t beginFoundBreakSize = foundBreaks.size();
    utext_setNativeIndex(text, startPos);
    utext_moveIndex32(text, MIN_WORD_SPAN);
    ಯದಿ (utext_getNativeIndex(text) >= endPos) {
        ಪ್ರತ್ಯಾಗಮ 0;       // Not enough characters for two words
    }
    utext_setNativeIndex(text, startPos);

    UVector32 offsets(status);
    UVector32 indices(status);
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ 0;
    fVectorizer->vectorize(text, startPos, endPos, offsets, indices, status);
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ 0;
    int32_t* offsetsBuf = offsets.getBuffer();
    int32_t* indicesBuf = indices.getBuffer();

    int32_t ಒಳಹರಿವು_seq_len = indices.size();
    int32_t hunits = fData->fForwardU.d1();

    // ----- Begin of all the Array memory allocation needed for this function
    // Allocate temp array used inside compute()
    Array1D ifco(4 * hunits, status);

    Array1D c(hunits, status);
    Array1D logp(4, status);

    // TODO: limit size of hBackward. If input_seq_len is too big, we could
    // run out of memory.
    // Backward LSTM
    Array2D hBackward(ಒಳಹರಿವು_seq_len, hunits, status);

    // Allocate fbRow and slice the internal array in two.
    Array1D fbRow(2 * hunits, status);

    // ----- End of all the Array memory allocation needed for this function
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ 0;

    // To save the needed memory usage, the following is different from the
    // Python or ICU4X implementation. We first perform the Backward LSTM
    // and then merge the iteration of the forward LSTM and the output layer
    // together because we only neetdto remember the h[t-1] for Forward LSTM.
    ಪ್ರತಿ (int32_t i = ಒಳಹರಿವು_seq_len - 1; i >= 0; i--) {
        Array1D hRow = hBackward.row(i);
        ಯದಿ (i != ಒಳಹರಿವು_seq_len - 1) {
            hRow.assign(hBackward.row(i+1));
        }
#ifdef LSTM_DEBUG
        printf("hRow %d\n", i);
        hRow.print();
        printf("indicesBuf[%d] = %d\n", i, indicesBuf[i]);
        printf("fData->fEmbedding.row(indicesBuf[%d]):\n", i);
        fData->fEmbedding.row(indicesBuf[i]).print();
#endif  // LSTM_DEBUG
        compute(hunits,
                fData->fBackwardW, fData->fBackwardU, fData->fBackwardB,
                fData->fEmbedding.row(indicesBuf[i]),
                hRow, c, ifco);
    }


    Array1D forwardRow = fbRow.slice(0, hunits);  // point to first half of data in fbRow.
    Array1D backwardRow = fbRow.slice(hunits, hunits);  // point to second half of data n fbRow.

    // The following iteration merge the forward LSTM and the output layer
    // together.
    c.clear();  // reuse c since it is the same size.
    ಪ್ರತಿ (int32_t i = 0; i < ಒಳಹರಿವು_seq_len; i++) {
#ifdef LSTM_DEBUG
        printf("forwardRow %d\n", i);
        forwardRow.print();
#endif  // LSTM_DEBUG
        // Forward LSTM
        // Calculate the result into forwardRow, which point to the data in the first half
        // of fbRow.
        compute(hunits,
                fData->fForwardW, fData->fForwardU, fData->fForwardB,
                fData->fEmbedding.row(indicesBuf[i]),
                forwardRow, c, ifco);

        // assign the data from hBackward.row(i) to second half of fbRowa.
        backwardRow.assign(hBackward.row(i));

        logp.assign(fData->fOutputB).addDotProduct(fbRow, fData->fOutputW);
#ifdef LSTM_DEBUG
        printf("backwardRow %d\n", i);
        backwardRow.print();
        printf("logp %d\n", i);
        logp.print();
#endif  // LSTM_DEBUG

        // current = argmax(logp)
        LSTMClass current = static_cast<LSTMClass>(logp.maxIndex());
        // BIES logic.
        ಯದಿ (current == BEGIN || current == SINGLE) {
            ಯದಿ (i != 0) {
                foundBreaks.addElement(offsetsBuf[i], status);
                ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ 0;
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ foundBreaks.size() - beginFoundBreakSize;
}

Vectorizer* createVectorizer(ಧ್ರುವ LSTMData* data, UErrorCode &status) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ವಿಕಲ್ಪ (data->fType) {
        ಪಕ್ಷ CODE_POINTS:
            ಪ್ರತ್ಯಾಗಮ ನೂತನ CodePointsVectorizer(data->fDict);
            ಭಙ್ಗ;
        ಪಕ್ಷ GRAPHEME_CLUSTER:
            ಪ್ರತ್ಯಾಗಮ ನೂತನ GraphemeClusterVectorizer(data->fDict);
            ಭಙ್ಗ;
        ಸ್ವಭಾವ:
            ಭಙ್ಗ;
    }
    UPRV_UNREACHABLE_EXIT;
}

LSTMBreakEngine::LSTMBreakEngine(ಧ್ರುವ LSTMData* data, ಧ್ರುವ UnicodeSet& set, UErrorCode &status)
    : DictionaryBreakEngine(), fData(data), fVectorizer(createVectorizer(fData, status))
{
    ಯದಿ (U_FAILURE(status)) {
      fData = nullptr;  // If failure, we should not delete fData in destructor because the caller will do so.
      ಪ್ರತ್ಯಾಗಮ;
    }
    setCharacters(set);
}

LSTMBreakEngine::~LSTMBreakEngine() {
    delete fData;
    delete fVectorizer;
}

ಧ್ರುವ char16_t* LSTMBreakEngine::name() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ fData->fName;
}

UnicodeString defaultLSTM(UScriptCode script, UErrorCode& status) {
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_get_By_ಕೀಲಿ_With_Fallback(b, "lstm", b, &status);
    UnicodeString result = ures_get_Unicode_String_By_ಕೀಲಿ(b, uscript_getShortName(script), &status);
    ures_close(b);
    ಪ್ರತ್ಯಾಗಮ result;
}

U_CAPI ಧ್ರುವ LSTMData* U_EXPORT2 CreateLSTMDataForScript(UScriptCode script, UErrorCode& status)
{
    ಯದಿ (script != USCRIPT_KHMER && script != USCRIPT_LAO && script != USCRIPT_MYANMAR && script != USCRIPT_THAI) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    UnicodeString name = defaultLSTM(script, status);
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ nullptr;
    CharString namebuf;
    namebuf.appendInvariantChars(name, status).truncate(namebuf.lastIndexOf('.'));

    LocalUResourceBundlePointer rb(
        ures_openDirect(U_ICUDATA_BRKITR, namebuf.data(), &status));
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ nullptr;

    ಪ್ರತ್ಯಾಗಮ CreateLSTMData(rb.orphan(), status);
}

U_CAPI ಧ್ರುವ LSTMData* U_EXPORT2 CreateLSTMData(UResourceBundle* rb, UErrorCode& status)
{
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಧ್ರುವ LSTMData* result = ನೂತನ LSTMData(rb, status);
    ಯದಿ (U_FAILURE(status)) {
        delete result;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

U_CAPI ಧ್ರುವ LanguageBreakEngine* U_EXPORT2
CreateLSTMBreakEngine(UScriptCode script, ಧ್ರುವ LSTMData* data, UErrorCode& status)
{
    UnicodeString unicodeSetString;
    ವಿಕಲ್ಪ(script) {
        ಪಕ್ಷ USCRIPT_THAI:
            unicodeSetString = UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]");
            ಭಙ್ಗ;
        ಪಕ್ಷ USCRIPT_MYANMAR:
            unicodeSetString = UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]");
            ಭಙ್ಗ;
        ಸ್ವಭಾವ:
            delete data;
            ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    UnicodeSet unicodeSet;
    unicodeSet.applyPattern(unicodeSetString, status);
    ಧ್ರುವ LanguageBreakEngine* engine = ನೂತನ LSTMBreakEngine(data, unicodeSet, status);
    ಯದಿ (U_FAILURE(status) || engine == nullptr) {
        ಯದಿ (engine != nullptr) {
            delete engine;
        } ಅನ್ಯಥಾ {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ engine;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2 DeleteLSTMData(ಧ್ರುವ LSTMData* data)
{
    delete data;
}

U_CAPI ಧ್ರುವ char16_t* U_EXPORT2 LSTMDataName(ಧ್ರುವ LSTMData* data)
{
    ಪ್ರತ್ಯಾಗಮ data->fName;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
