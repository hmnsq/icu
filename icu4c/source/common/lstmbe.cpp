// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ଅନ୍ତର୍ଭାଵ <complex>
#ଅନ୍ତର୍ଭାଵ <utility>

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_BREAK_ITERATION

#ଅନ୍ତର୍ଭାଵ "brkeng.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "lstmbe.h"
#ଅନ୍ତର୍ଭାଵ "putilimp.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ubrkimpl.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"
#ଅନ୍ତର୍ଭାଵ "uvectr32.h"
#ଅନ୍ତର୍ଭାଵ "uvector.h"

#ଅନ୍ତର୍ଭାଵ "unicode/brkiter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/resbund.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ubrk.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uniset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf.h"

U_NAMESPACE_BEGIN

// Uncomment the following #define to debug.
// #define LSTM_DEBUG 1
// #define LSTM_VECTORIZER_DEBUG 1

/**
 * Interface for reading 1D array.
 */
ଶ୍ରେଣୀ ReadArray1D {
ସର୍ବସାଧାରଣ:
    ଆଭାସୀ ~ReadArray1D();
    ଆଭାସୀ int32_t d1() ଅଚଳ = 0;
    ଆଭାସୀ ଦଶମିକ get(int32_t i) ଅଚଳ = 0;

#ifdef LSTM_DEBUG
    ଶୂନ୍ୟ print() ଅଚଳ {
        printf("\n[");
        ପାଇଁ (int32_t i = 0; i < d1(); i++) {
           printf("%0.8e ", get(i));
           ଯଦି (i % 4 == 3) printf("\n");
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
ଶ୍ରେଣୀ ReadArray2D {
ସର୍ବସାଧାରଣ:
    ଆଭାସୀ ~ReadArray2D();
    ଆଭାସୀ int32_t d1() ଅଚଳ = 0;
    ଆଭାସୀ int32_t d2() ଅଚଳ = 0;
    ଆଭାସୀ ଦଶମିକ get(int32_t i, int32_t j) ଅଚଳ = 0;
};

ReadArray2D::~ReadArray2D()
{
}

/**
 * A class to index a float array as a 1D Array without owning the pointer or
 * copy the data.
 */
ଶ୍ରେଣୀ ConstArray1D : ସର୍ବସାଧାରଣ ReadArray1D {
ସର୍ବସାଧାରଣ:
    ConstArray1D() : data_(nullptr), d1_(0) {}

    ConstArray1D(ଅଚଳ ଦଶମିକ* data, int32_t d1) : data_(data), d1_(d1) {}

    ଆଭାସୀ ~ConstArray1D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    ଶୂନ୍ୟ init(ଅଚଳ int32_t* data, int32_t d1) {
        U_ASSERT(IEEE_754 == 1);
        data_ = reinterpret_cast<ଅଚଳ ଦଶମିକ*>(data);
        d1_ = d1;
    }

    // ReadArray1D methods.
    ଆଭାସୀ int32_t d1() ଅଚଳ override { ଫେରନ୍ତୁ d1_; }
    ଆଭାସୀ ଦଶମିକ get(int32_t i) ଅଚଳ override {
        U_ASSERT(i < d1_);
        ଫେରନ୍ତୁ data_[i];
    }

ବ୍ୟକ୍ତିଗତ:
    ଅଚଳ ଦଶମିକ* data_;
    int32_t d1_;
};

ConstArray1D::~ConstArray1D()
{
}

/**
 * A class to index a float array as a 2D Array without owning the pointer or
 * copy the data.
 */
ଶ୍ରେଣୀ ConstArray2D : ସର୍ବସାଧାରଣ ReadArray2D {
ସର୍ବସାଧାରଣ:
    ConstArray2D() : data_(nullptr), d1_(0), d2_(0) {}

    ConstArray2D(ଅଚଳ ଦଶମିକ* data, int32_t d1, int32_t d2)
        : data_(data), d1_(d1), d2_(d2) {}

    ଆଭାସୀ ~ConstArray2D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    ଶୂନ୍ୟ init(ଅଚଳ int32_t* data, int32_t d1, int32_t d2) {
        U_ASSERT(IEEE_754 == 1);
        data_ = reinterpret_cast<ଅଚଳ ଦଶମିକ*>(data);
        d1_ = d1;
        d2_ = d2;
    }

    // ReadArray2D methods.
    ଧାଡ଼ିଭିତରେ int32_t d1() ଅଚଳ override { ଫେରନ୍ତୁ d1_; }
    ଧାଡ଼ିଭିତରେ int32_t d2() ଅଚଳ override { ଫେରନ୍ତୁ d2_; }
    ଦଶମିକ get(int32_t i, int32_t j) ଅଚଳ override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        ଫେରନ୍ତୁ data_[i * d2_ + j];
    }

    // Expose the ith row as a ConstArray1D
    ଧାଡ଼ିଭିତରେ ConstArray1D row(int32_t i) ଅଚଳ {
        U_ASSERT(i < d1_);
        ଫେରନ୍ତୁ ConstArray1D(data_ + i * d2_, d2_);
    }

ବ୍ୟକ୍ତିଗତ:
    ଅଚଳ ଦଶମିକ* data_;
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
ଶ୍ରେଣୀ Array1D : ସର୍ବସାଧାରଣ ReadArray1D {
ସର୍ବସାଧାରଣ:
    Array1D() : memory_(nullptr), data_(nullptr), d1_(0) {}
    Array1D(int32_t d1, UErrorCode &status)
        : memory_(uprv_malloc(d1 * ଆକାର(ଦଶମିକ))),
          data_(static_cast<ଦଶମିକ*>(memory_)), d1_(d1) {
        ଯଦି (U_SUCCESS(status)) {
            ଯଦି (memory_ == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ;
            }
            clear();
        }
    }

    ଆଭାସୀ ~Array1D();

    // A special constructor which does not own the memory but writeable
    // as a slice of an array.
    Array1D(ଦଶମିକ* data, int32_t d1)
        : memory_(nullptr), data_(data), d1_(d1) {}

    // ReadArray1D methods.
    ଆଭାସୀ int32_t d1() ଅଚଳ override { ଫେରନ୍ତୁ d1_; }
    ଆଭାସୀ ଦଶମିକ get(int32_t i) ଅଚଳ override {
        U_ASSERT(i < d1_);
        ଫେରନ୍ତୁ data_[i];
    }

    // Return the index which point to the max data in the array.
    ଧାଡ଼ିଭିତରେ int32_t maxIndex() ଅଚଳ {
        int32_t index = 0;
        ଦଶମିକ max = data_[0];
        ପାଇଁ (int32_t i = 1; i < d1_; i++) {
            ଯଦି (data_[i] > max) {
                max = data_[i];
                index = i;
            }
        }
        ଫେରନ୍ତୁ index;
    }

    // Slice part of the array to a new one.
    ଧାଡ଼ିଭିତରେ Array1D slice(int32_t from, int32_t size) ଅଚଳ {
        U_ASSERT(from >= 0);
        U_ASSERT(from < d1_);
        U_ASSERT(from + size <= d1_);
        ଫେରନ୍ତୁ Array1D(data_ + from, size);
    }

    // Add dot product of a 1D array and a 2D array into this one.
    ଧାଡ଼ିଭିତରେ Array1D& addDotProduct(ଅଚଳ ReadArray1D& a, ଅଚଳ ReadArray2D& b) {
        U_ASSERT(a.d1() == b.d1());
        U_ASSERT(b.d2() == d1());
        ପାଇଁ (int32_t i = 0; i < d1(); i++) {
            ପାଇଁ (int32_t j = 0; j < a.d1(); j++) {
                data_[i] += a.get(j) * b.get(j, i);
            }
        }
        ଫେରନ୍ତୁ *ଏହି;
    }

    // Hadamard Product the values of another array of the same size into this one.
    ଧାଡ଼ିଭିତରେ Array1D& hadamardProduct(ଅଚଳ ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        ପାଇଁ (int32_t i = 0; i < d1(); i++) {
            data_[i] *= a.get(i);
        }
        ଫେରନ୍ତୁ *ଏହି;
    }

    // Add the Hadamard Product of two arrays of the same size into this one.
    ଧାଡ଼ିଭିତରେ Array1D& addHadamardProduct(ଅଚଳ ReadArray1D& a, ଅଚଳ ReadArray1D& b) {
        U_ASSERT(a.d1() == d1());
        U_ASSERT(b.d1() == d1());
        ପାଇଁ (int32_t i = 0; i < d1(); i++) {
            data_[i] += a.get(i) * b.get(i);
        }
        ଫେରନ୍ତୁ *ଏହି;
    }

    // Add the values of another array of the same size into this one.
    ଧାଡ଼ିଭିତରେ Array1D& add(ଅଚଳ ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        ପାଇଁ (int32_t i = 0; i < d1(); i++) {
            data_[i] += a.get(i);
        }
        ଫେରନ୍ତୁ *ଏହି;
    }

    // Assign the values of another array of the same size into this one.
    ଧାଡ଼ିଭିତରେ Array1D& assign(ଅଚଳ ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        ପାଇଁ (int32_t i = 0; i < d1(); i++) {
            data_[i] = a.get(i);
        }
        ଫେରନ୍ତୁ *ଏହି;
    }

    // Apply tanh to all the elements in the array.
    ଧାଡ଼ିଭିତରେ Array1D& tanh() {
        ଫେରନ୍ତୁ tanh(*ଏହି);
    }

    // Apply tanh of a and store into this array.
    ଧାଡ଼ିଭିତରେ Array1D& tanh(ଅଚଳ Array1D& a) {
        U_ASSERT(a.d1() == d1());
        ପାଇଁ (int32_t i = 0; i < d1_; i++) {
            data_[i] = std::tanh(a.get(i));
        }
        ଫେରନ୍ତୁ *ଏହି;
    }

    // Apply sigmoid to all the elements in the array.
    ଧାଡ଼ିଭିତରେ Array1D& sigmoid() {
        ପାଇଁ (int32_t i = 0; i < d1_; i++) {
            data_[i] = 1.0f/(1.0f + expf(-data_[i]));
        }
        ଫେରନ୍ତୁ *ଏହି;
    }

    ଧାଡ଼ିଭିତରେ Array1D& clear() {
        uprv_memset(data_, 0, d1_ * ଆକାର(ଦଶମିକ));
        ଫେରନ୍ତୁ *ଏହି;
    }

ବ୍ୟକ୍ତିଗତ:
    ଶୂନ୍ୟ* memory_;
    ଦଶମିକ* data_;
    int32_t d1_;
};

Array1D::~Array1D()
{
    uprv_free(memory_);
}

ଶ୍ରେଣୀ Array2D : ସର୍ବସାଧାରଣ ReadArray2D {
ସର୍ବସାଧାରଣ:
    Array2D() : memory_(nullptr), data_(nullptr), d1_(0), d2_(0) {}
    Array2D(int32_t d1, int32_t d2, UErrorCode &status)
        : memory_(uprv_malloc(d1 * d2 * ଆକାର(ଦଶମିକ))),
          data_(static_cast<ଦଶମିକ*>(memory_)), d1_(d1), d2_(d2) {
        ଯଦି (U_SUCCESS(status)) {
            ଯଦି (memory_ == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ;
            }
            clear();
        }
    }
    ଆଭାସୀ ~Array2D();

    // ReadArray2D methods.
    ଆଭାସୀ int32_t d1() ଅଚଳ override { ଫେରନ୍ତୁ d1_; }
    ଆଭାସୀ int32_t d2() ଅଚଳ override { ଫେରନ୍ତୁ d2_; }
    ଆଭାସୀ ଦଶମିକ get(int32_t i, int32_t j) ଅଚଳ override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        ଫେରନ୍ତୁ data_[i * d2_ + j];
    }

    ଧାଡ଼ିଭିତରେ Array1D row(int32_t i) ଅଚଳ {
        U_ASSERT(i < d1_);
        ଫେରନ୍ତୁ Array1D(data_ + i * d2_, d2_);
    }

    ଧାଡ଼ିଭିତରେ Array2D& clear() {
        uprv_memset(data_, 0, d1_ * d2_ * ଆକାର(ଦଶମିକ));
        ଫେରନ୍ତୁ *ଏହି;
    }

ବ୍ୟକ୍ତିଗତ:
    ଶୂନ୍ୟ* memory_;
    ଦଶମିକ* data_;
    int32_t d1_;
    int32_t d2_;
};

Array2D::~Array2D()
{
    uprv_free(memory_);
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    BEGIN,
    INSIDE,
    END,
    SINGLE
} LSTMClass;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    UNKNOWN,
    CODE_POINTS,
    GRAPHEME_CLUSTER,
} EmbeddingType;

ସଂରଚନା LSTMData : ସର୍ବସାଧାରଣ UMemory {
    LSTMData(UResourceBundle* rb, UErrorCode &status);
    ~LSTMData();
    UHashtable* fDict;
    EmbeddingType fType;
    ଅଚଳ char16_t* fName;
    ConstArray2D fEmbedding;
    ConstArray2D fForwardW;
    ConstArray2D fForwardU;
    ConstArray1D fForwardB;
    ConstArray2D fBackwardW;
    ConstArray2D fBackwardU;
    ConstArray1D fBackwardB;
    ConstArray2D fOutputW;
    ConstArray1D fOutputB;

ବ୍ୟକ୍ତିଗତ:
    UResourceBundle* fBundle;
};

LSTMData::LSTMData(UResourceBundle* rb, UErrorCode &status)
    : fDict(nullptr), fType(UNKNOWN), fName(nullptr),
      fBundle(rb)
{
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (IEEE_754 != 1) {
        status = U_UNSUPPORTED_ERROR;
        ଫେରନ୍ତୁ;
    }
    LocalUResourceBundlePointer embeddings_res(
        ures_get_By_ଚାବି(rb, "embeddings", nullptr, &status));
    int32_t embedding_size = ures_getInt(embeddings_res.getAlias(), &status);
    LocalUResourceBundlePointer hunits_res(
        ures_get_By_ଚାବି(rb, "hunits", nullptr, &status));
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ;
    int32_t hunits = ures_getInt(hunits_res.getAlias(), &status);
    ଅଚଳ char16_t* type = ures_get_String_By_ଚାବି(rb, "type", nullptr, &status);
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ;
    ଯଦି (u_strCompare(type, -1, u"codepoints", -1, ମିଥ୍ୟା) == 0) {
        fType = CODE_POINTS;
    } ଅନ୍ୟଥା ଯଦି (u_strCompare(type, -1, u"graphclust", -1, ମିଥ୍ୟା) == 0) {
        fType = GRAPHEME_CLUSTER;
    }
    fName = ures_get_String_By_ଚାବି(rb, "model", nullptr, &status);
    LocalUResourceBundlePointer dataRes(ures_get_By_ଚାବି(rb, "data", nullptr, &status));
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ;
    int32_t data_len = 0;
    ଅଚଳ int32_t* data = ures_getIntVector(dataRes.getAlias(), &data_len, &status);
    fDict = uhash_open(uhash_hashUChars, uhash_compareUChars, nullptr, &status);

    ଗଦା_UResource_Bundle ଗଦା_Temp_Bundle;
    Resource_Data_ମୂଲ୍ୟ value;
    ures_get_ମୂଲ୍ୟ_With_Fallback(rb, "dict", ଗଦା_Temp_Bundle.getAlias(), value, status);
    ResourceArray stringArray = value.getArray(status);
    int32_t ଅଙ୍କ_index = stringArray.getSize();
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }

    // put dict into hash
    int32_t stringLength;
    ପାଇଁ (int32_t idx = 0; idx < ଅଙ୍କ_index; idx++) {
        stringArray.get_ମୂଲ୍ୟ(idx, value);
        ଅଚଳ char16_t* str = value.getString(stringLength, status);
        uhash_putiAllowZero(fDict, (ଶୂନ୍ୟ*)str, idx, &status);
        ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ;
#ifdef LSTM_VECTORIZER_DEBUG
        printf("Assign [");
        ଯେତେବେଳେ (*str != 0x0000) {
            printf("U+%04x ", *str);
            str++;
        }
        printf("] map to %d\n", idx-1);
#endif
    }
    int32_t mat1_size = (ଅଙ୍କ_index + 1) * embedding_size;
    int32_t mat2_size = embedding_size * 4 * hunits;
    int32_t mat3_size = hunits * 4 * hunits;
    int32_t mat4_size = 4 * hunits;
    int32_t mat5_size = mat2_size;
    int32_t mat6_size = mat3_size;
    int32_t mat7_size = mat4_size;
    int32_t mat8_size = 2 * hunits * 4;
#ଯଦି U_DEBUG
    int32_t mat9_size = 4;
    U_ASSERT(data_len == mat1_size + mat2_size + mat3_size + mat4_size + mat5_size +
        mat6_size + mat7_size + mat8_size + mat9_size);
#endif

    fEmbedding.init(data, (ଅଙ୍କ_index + 1), embedding_size);
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

ଶ୍ରେଣୀ Vectorizer : ସର୍ବସାଧାରଣ UMemory {
ସର୍ବସାଧାରଣ:
    Vectorizer(UHashtable* dict) : fDict(dict) {}
    ଆଭାସୀ ~Vectorizer();
    ଆଭାସୀ ଶୂନ୍ୟ vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) ଅଚଳ = 0;
ସୁରକ୍ଷିତ:
    int32_t stringToIndex(ଅଚଳ char16_t* str) ଅଚଳ {
        UBool found = ମିଥ୍ୟା;
        int32_t ret = uhash_getiAndFound(fDict, (ଅଚଳ ଶୂନ୍ୟ*)str, &found);
        ଯଦି (!found) {
            ret = fDict->count;
        }
#ifdef LSTM_VECTORIZER_DEBUG
        printf("[");
        ଯେତେବେଳେ (*str != 0x0000) {
            printf("U+%04x ", *str);
            str++;
        }
        printf("] map to %d\n", ret);
#endif
        ଫେରନ୍ତୁ ret;
    }

ବ୍ୟକ୍ତିଗତ:
    UHashtable* fDict;
};

Vectorizer::~Vectorizer()
{
}

ଶ୍ରେଣୀ CodePointsVectorizer : ସର୍ବସାଧାରଣ Vectorizer {
ସର୍ବସାଧାରଣ:
    CodePointsVectorizer(UHashtable* dict) : Vectorizer(dict) {}
    ଆଭାସୀ ~CodePointsVectorizer();
    ଆଭାସୀ ଶୂନ୍ୟ vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) ଅଚଳ override;
};

CodePointsVectorizer::~CodePointsVectorizer()
{
}

ଶୂନ୍ୟ CodePointsVectorizer::vectorize(
    UText *text, int32_t startPos, int32_t endPos,
    UVector32 &offsets, UVector32 &indices, UErrorCode &status) ଅଚଳ
{
    ଯଦି (offsets.ensureCapacity(endPos - startPos, status) &&
            indices.ensureCapacity(endPos - startPos, status)) {
        ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ;
        utext_setNativeIndex(text, startPos);
        int32_t current;
        char16_t str[2] = {0, 0};
        ଯେତେବେଳେ (U_SUCCESS(status) &&
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

ଶ୍ରେଣୀ GraphemeClusterVectorizer : ସର୍ବସାଧାରଣ Vectorizer {
ସର୍ବସାଧାରଣ:
    GraphemeClusterVectorizer(UHashtable* dict)
        : Vectorizer(dict)
    {
    }
    ଆଭାସୀ ~GraphemeClusterVectorizer();
    ଆଭାସୀ ଶୂନ୍ୟ vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) ଅଚଳ override;
};

GraphemeClusterVectorizer::~GraphemeClusterVectorizer()
{
}

constexpr int32_t MAX_GRAPHEME_CLSTER_LENGTH = 10;

ଶୂନ୍ୟ GraphemeClusterVectorizer::vectorize(
    UText *text, int32_t startPos, int32_t endPos,
    UVector32 &offsets, UVector32 &indices, UErrorCode &status) ଅଚଳ
{
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ;
    ଯଦି (!offsets.ensureCapacity(endPos - startPos, status) ||
            !indices.ensureCapacity(endPos - startPos, status)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ;
    LocalPointer<BreakIterator> graphemeIter(BreakIterator::createCharacterInstance(Locale(), status));
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ;
    graphemeIter->setText(text, status);
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ;

    ଯଦି (startPos != 0) {
        graphemeIter->preceding(startPos);
    }
    int32_t last = startPos;
    int32_t current = startPos;
    char16_t str[MAX_GRAPHEME_CLSTER_LENGTH];
    ଯେତେବେଳେ ((current = graphemeIter->next()) != BreakIterator::DONE) {
        ଯଦି (current >= endPos) {
            ଭାଙ୍ଗ;
        }
        ଯଦି (current > startPos) {
            utext_extract(text, last, current, str, MAX_GRAPHEME_CLSTER_LENGTH, &status);
            ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ;
            offsets.addElement(last, status);
            indices.addElement(stringToIndex(str), status);
            ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ;
        }
        last = current;
    }
    ଯଦି (U_FAILURE(status) || last >= endPos) {
        ଫେରନ୍ତୁ;
    }
    utext_extract(text, last, endPos, str, MAX_GRAPHEME_CLSTER_LENGTH, &status);
    ଯଦି (U_SUCCESS(status)) {
        offsets.addElement(last, status);
        indices.addElement(stringToIndex(str), status);
    }
}

// Computing LSTM as stated in
// https://en.wikipedia.org/wiki/Long_short-term_memory#LSTM_with_a_forget_gate
// ifco is temp array allocate outside which does not need to be
// input/output value but could avoid unnecessary memory alloc/free if passing
// in.
ଶୂନ୍ୟ compute(
    int32_t hunits,
    ଅଚଳ ReadArray2D& W, ଅଚଳ ReadArray2D& U, ଅଚଳ ReadArray1D& b,
    ଅଚଳ ReadArray1D& x, Array1D& h, Array1D& c,
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
ସ୍ଥିର ଅଚଳ int32_t MIN_WORD = 2;

// Minimum number of characters for two words
ସ୍ଥିର ଅଚଳ int32_t MIN_WORD_SPAN = MIN_WORD * 2;

int32_t
LSTMBreakEngine::divideUpDictionaryRange( UText *text,
                                                int32_t startPos,
                                                int32_t endPos,
                                                UVector32 &foundBreaks,
                                                UBool /* isPhraseBreaking */,
                                                UErrorCode& status) ଅଚଳ {
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ 0;
    int32_t beginFoundBreakSize = foundBreaks.size();
    utext_setNativeIndex(text, startPos);
    utext_moveIndex32(text, MIN_WORD_SPAN);
    ଯଦି (utext_getNativeIndex(text) >= endPos) {
        ଫେରନ୍ତୁ 0;       // Not enough characters for two words
    }
    utext_setNativeIndex(text, startPos);

    UVector32 offsets(status);
    UVector32 indices(status);
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ 0;
    fVectorizer->vectorize(text, startPos, endPos, offsets, indices, status);
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ 0;
    int32_t* offsetsBuf = offsets.getBuffer();
    int32_t* indicesBuf = indices.getBuffer();

    int32_t ନିବେଶ_seq_len = indices.size();
    int32_t hunits = fData->fForwardU.d1();

    // ----- Begin of all the Array memory allocation needed for this function
    // Allocate temp array used inside compute()
    Array1D ifco(4 * hunits, status);

    Array1D c(hunits, status);
    Array1D logp(4, status);

    // TODO: limit size of hBackward. If input_seq_len is too big, we could
    // run out of memory.
    // Backward LSTM
    Array2D hBackward(ନିବେଶ_seq_len, hunits, status);

    // Allocate fbRow and slice the internal array in two.
    Array1D fbRow(2 * hunits, status);

    // ----- End of all the Array memory allocation needed for this function
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ 0;

    // To save the needed memory usage, the following is different from the
    // Python or ICU4X implementation. We first perform the Backward LSTM
    // and then merge the iteration of the forward LSTM and the output layer
    // together because we only neetdto remember the h[t-1] for Forward LSTM.
    ପାଇଁ (int32_t i = ନିବେଶ_seq_len - 1; i >= 0; i--) {
        Array1D hRow = hBackward.row(i);
        ଯଦି (i != ନିବେଶ_seq_len - 1) {
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
    ପାଇଁ (int32_t i = 0; i < ନିବେଶ_seq_len; i++) {
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
        ଯଦି (current == BEGIN || current == SINGLE) {
            ଯଦି (i != 0) {
                foundBreaks.addElement(offsetsBuf[i], status);
                ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ 0;
            }
        }
    }
    ଫେରନ୍ତୁ foundBreaks.size() - beginFoundBreakSize;
}

Vectorizer* createVectorizer(ଅଚଳ LSTMData* data, UErrorCode &status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ବାଛ (data->fType) {
        ସ୍ଥିତି CODE_POINTS:
            ଫେରନ୍ତୁ ନୂତନ CodePointsVectorizer(data->fDict);
            ଭାଙ୍ଗ;
        ସ୍ଥିତି GRAPHEME_CLUSTER:
            ଫେରନ୍ତୁ ନୂତନ GraphemeClusterVectorizer(data->fDict);
            ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            ଭାଙ୍ଗ;
    }
    UPRV_UNREACHABLE_EXIT;
}

LSTMBreakEngine::LSTMBreakEngine(ଅଚଳ LSTMData* data, ଅଚଳ UnicodeSet& set, UErrorCode &status)
    : DictionaryBreakEngine(), fData(data), fVectorizer(createVectorizer(fData, status))
{
    ଯଦି (U_FAILURE(status)) {
      fData = nullptr;  // If failure, we should not delete fData in destructor because the caller will do so.
      ଫେରନ୍ତୁ;
    }
    setCharacters(set);
}

LSTMBreakEngine::~LSTMBreakEngine() {
    delete fData;
    delete fVectorizer;
}

ଅଚଳ char16_t* LSTMBreakEngine::name() ଅଚଳ {
    ଫେରନ୍ତୁ fData->fName;
}

UnicodeString defaultLSTM(UScriptCode script, UErrorCode& status) {
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_get_By_ଚାବି_With_Fallback(b, "lstm", b, &status);
    UnicodeString result = ures_get_Unicode_String_By_ଚାବି(b, uscript_getShortName(script), &status);
    ures_close(b);
    ଫେରନ୍ତୁ result;
}

U_CAPI ଅଚଳ LSTMData* U_EXPORT2 CreateLSTMDataForScript(UScriptCode script, UErrorCode& status)
{
    ଯଦି (script != USCRIPT_KHMER && script != USCRIPT_LAO && script != USCRIPT_MYANMAR && script != USCRIPT_THAI) {
        ଫେରନ୍ତୁ nullptr;
    }
    UnicodeString name = defaultLSTM(script, status);
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ nullptr;
    CharString namebuf;
    namebuf.appendInvariantChars(name, status).truncate(namebuf.lastIndexOf('.'));

    LocalUResourceBundlePointer rb(
        ures_openDirect(U_ICUDATA_BRKITR, namebuf.data(), &status));
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ nullptr;

    ଫେରନ୍ତୁ CreateLSTMData(rb.orphan(), status);
}

U_CAPI ଅଚଳ LSTMData* U_EXPORT2 CreateLSTMData(UResourceBundle* rb, UErrorCode& status)
{
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଅଚଳ LSTMData* result = ନୂତନ LSTMData(rb, status);
    ଯଦି (U_FAILURE(status)) {
        delete result;
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ result;
}

U_CAPI ଅଚଳ LanguageBreakEngine* U_EXPORT2
CreateLSTMBreakEngine(UScriptCode script, ଅଚଳ LSTMData* data, UErrorCode& status)
{
    UnicodeString unicodeSetString;
    ବାଛ(script) {
        ସ୍ଥିତି USCRIPT_THAI:
            unicodeSetString = UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]");
            ଭାଙ୍ଗ;
        ସ୍ଥିତି USCRIPT_MYANMAR:
            unicodeSetString = UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]");
            ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            delete data;
            ଫେରନ୍ତୁ nullptr;
    }
    UnicodeSet unicodeSet;
    unicodeSet.applyPattern(unicodeSetString, status);
    ଅଚଳ LanguageBreakEngine* engine = ନୂତନ LSTMBreakEngine(data, unicodeSet, status);
    ଯଦି (U_FAILURE(status) || engine == nullptr) {
        ଯଦି (engine != nullptr) {
            delete engine;
        } ଅନ୍ୟଥା {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ engine;
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2 DeleteLSTMData(ଅଚଳ LSTMData* data)
{
    delete data;
}

U_CAPI ଅଚଳ char16_t* U_EXPORT2 LSTMDataName(ଅଚଳ LSTMData* data)
{
    ଫେରନ୍ତୁ data->fName;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
