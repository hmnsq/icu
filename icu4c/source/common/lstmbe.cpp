// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#அந்தர்பாவ <complex>
#அந்தர்பாவ <utility>

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_BREAK_ITERATION

#அந்தர்பாவ "brkeng.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "lstmbe.h"
#அந்தர்பாவ "putilimp.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ubrkimpl.h"
#அந்தர்பாவ "uresimp.h"
#அந்தர்பாவ "uvectr32.h"
#அந்தர்பாவ "uvector.h"

#அந்தர்பாவ "unicode/brkiter.h"
#அந்தர்பாவ "unicode/resbund.h"
#அந்தர்பாவ "unicode/ubrk.h"
#அந்தர்பாவ "unicode/uniset.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/utf.h"

U_NAMESPACE_BEGIN

// Uncomment the following #define to debug.
// #define LSTM_DEBUG 1
// #define LSTM_VECTORIZER_DEBUG 1

/**
 * Interface for reading 1D array.
 */
வகுப்பு ReadArray1D {
பொது:
    மெய்நிகர் ~ReadArray1D();
    மெய்நிகர் int32_t d1() மாறிலி = 0;
    மெய்நிகர் தசம get(int32_t i) மாறிலி = 0;

#ifdef LSTM_DEBUG
    வெறுமை print() மாறிலி {
        printf("\n[");
        க்கு (int32_t i = 0; i < d1(); i++) {
           printf("%0.8e ", get(i));
           எனில் (i % 4 == 3) printf("\n");
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
வகுப்பு ReadArray2D {
பொது:
    மெய்நிகர் ~ReadArray2D();
    மெய்நிகர் int32_t d1() மாறிலி = 0;
    மெய்நிகர் int32_t d2() மாறிலி = 0;
    மெய்நிகர் தசம get(int32_t i, int32_t j) மாறிலி = 0;
};

ReadArray2D::~ReadArray2D()
{
}

/**
 * A class to index a float array as a 1D Array without owning the pointer or
 * copy the data.
 */
வகுப்பு ConstArray1D : பொது ReadArray1D {
பொது:
    ConstArray1D() : data_(nullptr), d1_(0) {}

    ConstArray1D(மாறிலி தசம* data, int32_t d1) : data_(data), d1_(d1) {}

    மெய்நிகர் ~ConstArray1D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    வெறுமை init(மாறிலி int32_t* data, int32_t d1) {
        U_ASSERT(IEEE_754 == 1);
        data_ = reinterpret_cast<மாறிலி தசம*>(data);
        d1_ = d1;
    }

    // ReadArray1D methods.
    மெய்நிகர் int32_t d1() மாறிலி override { திரும்பு d1_; }
    மெய்நிகர் தசம get(int32_t i) மாறிலி override {
        U_ASSERT(i < d1_);
        திரும்பு data_[i];
    }

தனிப்பட்ட:
    மாறிலி தசம* data_;
    int32_t d1_;
};

ConstArray1D::~ConstArray1D()
{
}

/**
 * A class to index a float array as a 2D Array without owning the pointer or
 * copy the data.
 */
வகுப்பு ConstArray2D : பொது ReadArray2D {
பொது:
    ConstArray2D() : data_(nullptr), d1_(0), d2_(0) {}

    ConstArray2D(மாறிலி தசம* data, int32_t d1, int32_t d2)
        : data_(data), d1_(d1), d2_(d2) {}

    மெய்நிகர் ~ConstArray2D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    வெறுமை init(மாறிலி int32_t* data, int32_t d1, int32_t d2) {
        U_ASSERT(IEEE_754 == 1);
        data_ = reinterpret_cast<மாறிலி தசம*>(data);
        d1_ = d1;
        d2_ = d2;
    }

    // ReadArray2D methods.
    உள்வரி int32_t d1() மாறிலி override { திரும்பு d1_; }
    உள்வரி int32_t d2() மாறிலி override { திரும்பு d2_; }
    தசம get(int32_t i, int32_t j) மாறிலி override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        திரும்பு data_[i * d2_ + j];
    }

    // Expose the ith row as a ConstArray1D
    உள்வரி ConstArray1D row(int32_t i) மாறிலி {
        U_ASSERT(i < d1_);
        திரும்பு ConstArray1D(data_ + i * d2_, d2_);
    }

தனிப்பட்ட:
    மாறிலி தசம* data_;
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
வகுப்பு Array1D : பொது ReadArray1D {
பொது:
    Array1D() : memory_(nullptr), data_(nullptr), d1_(0) {}
    Array1D(int32_t d1, UErrorCode &status)
        : memory_(uprv_malloc(d1 * அளவு(தசம))),
          data_(static_cast<தசம*>(memory_)), d1_(d1) {
        எனில் (U_SUCCESS(status)) {
            எனில் (memory_ == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                திரும்பு;
            }
            clear();
        }
    }

    மெய்நிகர் ~Array1D();

    // A special constructor which does not own the memory but writeable
    // as a slice of an array.
    Array1D(தசம* data, int32_t d1)
        : memory_(nullptr), data_(data), d1_(d1) {}

    // ReadArray1D methods.
    மெய்நிகர் int32_t d1() மாறிலி override { திரும்பு d1_; }
    மெய்நிகர் தசம get(int32_t i) மாறிலி override {
        U_ASSERT(i < d1_);
        திரும்பு data_[i];
    }

    // Return the index which point to the max data in the array.
    உள்வரி int32_t maxIndex() மாறிலி {
        int32_t index = 0;
        தசம max = data_[0];
        க்கு (int32_t i = 1; i < d1_; i++) {
            எனில் (data_[i] > max) {
                max = data_[i];
                index = i;
            }
        }
        திரும்பு index;
    }

    // Slice part of the array to a new one.
    உள்வரி Array1D slice(int32_t from, int32_t size) மாறிலி {
        U_ASSERT(from >= 0);
        U_ASSERT(from < d1_);
        U_ASSERT(from + size <= d1_);
        திரும்பு Array1D(data_ + from, size);
    }

    // Add dot product of a 1D array and a 2D array into this one.
    உள்வரி Array1D& addDotProduct(மாறிலி ReadArray1D& a, மாறிலி ReadArray2D& b) {
        U_ASSERT(a.d1() == b.d1());
        U_ASSERT(b.d2() == d1());
        க்கு (int32_t i = 0; i < d1(); i++) {
            க்கு (int32_t j = 0; j < a.d1(); j++) {
                data_[i] += a.get(j) * b.get(j, i);
            }
        }
        திரும்பு *இது;
    }

    // Hadamard Product the values of another array of the same size into this one.
    உள்வரி Array1D& hadamardProduct(மாறிலி ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        க்கு (int32_t i = 0; i < d1(); i++) {
            data_[i] *= a.get(i);
        }
        திரும்பு *இது;
    }

    // Add the Hadamard Product of two arrays of the same size into this one.
    உள்வரி Array1D& addHadamardProduct(மாறிலி ReadArray1D& a, மாறிலி ReadArray1D& b) {
        U_ASSERT(a.d1() == d1());
        U_ASSERT(b.d1() == d1());
        க்கு (int32_t i = 0; i < d1(); i++) {
            data_[i] += a.get(i) * b.get(i);
        }
        திரும்பு *இது;
    }

    // Add the values of another array of the same size into this one.
    உள்வரி Array1D& add(மாறிலி ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        க்கு (int32_t i = 0; i < d1(); i++) {
            data_[i] += a.get(i);
        }
        திரும்பு *இது;
    }

    // Assign the values of another array of the same size into this one.
    உள்வரி Array1D& assign(மாறிலி ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        க்கு (int32_t i = 0; i < d1(); i++) {
            data_[i] = a.get(i);
        }
        திரும்பு *இது;
    }

    // Apply tanh to all the elements in the array.
    உள்வரி Array1D& tanh() {
        திரும்பு tanh(*இது);
    }

    // Apply tanh of a and store into this array.
    உள்வரி Array1D& tanh(மாறிலி Array1D& a) {
        U_ASSERT(a.d1() == d1());
        க்கு (int32_t i = 0; i < d1_; i++) {
            data_[i] = std::tanh(a.get(i));
        }
        திரும்பு *இது;
    }

    // Apply sigmoid to all the elements in the array.
    உள்வரி Array1D& sigmoid() {
        க்கு (int32_t i = 0; i < d1_; i++) {
            data_[i] = 1.0f/(1.0f + expf(-data_[i]));
        }
        திரும்பு *இது;
    }

    உள்வரி Array1D& clear() {
        uprv_memset(data_, 0, d1_ * அளவு(தசம));
        திரும்பு *இது;
    }

தனிப்பட்ட:
    வெறுமை* memory_;
    தசம* data_;
    int32_t d1_;
};

Array1D::~Array1D()
{
    uprv_free(memory_);
}

வகுப்பு Array2D : பொது ReadArray2D {
பொது:
    Array2D() : memory_(nullptr), data_(nullptr), d1_(0), d2_(0) {}
    Array2D(int32_t d1, int32_t d2, UErrorCode &status)
        : memory_(uprv_malloc(d1 * d2 * அளவு(தசம))),
          data_(static_cast<தசம*>(memory_)), d1_(d1), d2_(d2) {
        எனில் (U_SUCCESS(status)) {
            எனில் (memory_ == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                திரும்பு;
            }
            clear();
        }
    }
    மெய்நிகர் ~Array2D();

    // ReadArray2D methods.
    மெய்நிகர் int32_t d1() மாறிலி override { திரும்பு d1_; }
    மெய்நிகர் int32_t d2() மாறிலி override { திரும்பு d2_; }
    மெய்நிகர் தசம get(int32_t i, int32_t j) மாறிலி override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        திரும்பு data_[i * d2_ + j];
    }

    உள்வரி Array1D row(int32_t i) மாறிலி {
        U_ASSERT(i < d1_);
        திரும்பு Array1D(data_ + i * d2_, d2_);
    }

    உள்வரி Array2D& clear() {
        uprv_memset(data_, 0, d1_ * d2_ * அளவு(தசம));
        திரும்பு *இது;
    }

தனிப்பட்ட:
    வெறுமை* memory_;
    தசம* data_;
    int32_t d1_;
    int32_t d2_;
};

Array2D::~Array2D()
{
    uprv_free(memory_);
}

வகைவரை எண்ணிக்கை {
    BEGIN,
    INSIDE,
    END,
    SINGLE
} LSTMClass;

வகைவரை எண்ணிக்கை {
    UNKNOWN,
    CODE_POINTS,
    GRAPHEME_CLUSTER,
} EmbeddingType;

கட்டமைப்பு LSTMData : பொது UMemory {
    LSTMData(UResourceBundle* rb, UErrorCode &status);
    ~LSTMData();
    UHashtable* fDict;
    EmbeddingType fType;
    மாறிலி char16_t* fName;
    ConstArray2D fEmbedding;
    ConstArray2D fForwardW;
    ConstArray2D fForwardU;
    ConstArray1D fForwardB;
    ConstArray2D fBackwardW;
    ConstArray2D fBackwardU;
    ConstArray1D fBackwardB;
    ConstArray2D fOutputW;
    ConstArray1D fOutputB;

தனிப்பட்ட:
    UResourceBundle* fBundle;
};

LSTMData::LSTMData(UResourceBundle* rb, UErrorCode &status)
    : fDict(nullptr), fType(UNKNOWN), fName(nullptr),
      fBundle(rb)
{
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    எனில் (IEEE_754 != 1) {
        status = U_UNSUPPORTED_ERROR;
        திரும்பு;
    }
    LocalUResourceBundlePointer embeddings_res(
        ures_get_By_சாவி(rb, "embeddings", nullptr, &status));
    int32_t embedding_size = ures_getInt(embeddings_res.getAlias(), &status);
    LocalUResourceBundlePointer hunits_res(
        ures_get_By_சாவி(rb, "hunits", nullptr, &status));
    எனில் (U_FAILURE(status)) திரும்பு;
    int32_t hunits = ures_getInt(hunits_res.getAlias(), &status);
    மாறிலி char16_t* type = ures_get_String_By_சாவி(rb, "type", nullptr, &status);
    எனில் (U_FAILURE(status)) திரும்பு;
    எனில் (u_strCompare(type, -1, u"codepoints", -1, பொய்) == 0) {
        fType = CODE_POINTS;
    } இல்லையெனில் எனில் (u_strCompare(type, -1, u"graphclust", -1, பொய்) == 0) {
        fType = GRAPHEME_CLUSTER;
    }
    fName = ures_get_String_By_சாவி(rb, "model", nullptr, &status);
    LocalUResourceBundlePointer dataRes(ures_get_By_சாவி(rb, "data", nullptr, &status));
    எனில் (U_FAILURE(status)) திரும்பு;
    int32_t data_len = 0;
    மாறிலி int32_t* data = ures_getIntVector(dataRes.getAlias(), &data_len, &status);
    fDict = uhash_open(uhash_hashUChars, uhash_compareUChars, nullptr, &status);

    அடுக்கு_UResource_Bundle அடுக்கு_Temp_Bundle;
    Resource_Data_மதிப்பு value;
    ures_get_மதிப்பு_With_Fallback(rb, "dict", அடுக்கு_Temp_Bundle.getAlias(), value, status);
    ResourceArray stringArray = value.getArray(status);
    int32_t இலக்கம்_index = stringArray.getSize();
    எனில் (U_FAILURE(status)) { திரும்பு; }

    // put dict into hash
    int32_t stringLength;
    க்கு (int32_t idx = 0; idx < இலக்கம்_index; idx++) {
        stringArray.get_மதிப்பு(idx, value);
        மாறிலி char16_t* str = value.getString(stringLength, status);
        uhash_putiAllowZero(fDict, (வெறுமை*)str, idx, &status);
        எனில் (U_FAILURE(status)) திரும்பு;
#ifdef LSTM_VECTORIZER_DEBUG
        printf("Assign [");
        போது (*str != 0x0000) {
            printf("U+%04x ", *str);
            str++;
        }
        printf("] map to %d\n", idx-1);
#endif
    }
    int32_t mat1_size = (இலக்கம்_index + 1) * embedding_size;
    int32_t mat2_size = embedding_size * 4 * hunits;
    int32_t mat3_size = hunits * 4 * hunits;
    int32_t mat4_size = 4 * hunits;
    int32_t mat5_size = mat2_size;
    int32_t mat6_size = mat3_size;
    int32_t mat7_size = mat4_size;
    int32_t mat8_size = 2 * hunits * 4;
#எனில் U_DEBUG
    int32_t mat9_size = 4;
    U_ASSERT(data_len == mat1_size + mat2_size + mat3_size + mat4_size + mat5_size +
        mat6_size + mat7_size + mat8_size + mat9_size);
#endif

    fEmbedding.init(data, (இலக்கம்_index + 1), embedding_size);
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

வகுப்பு Vectorizer : பொது UMemory {
பொது:
    Vectorizer(UHashtable* dict) : fDict(dict) {}
    மெய்நிகர் ~Vectorizer();
    மெய்நிகர் வெறுமை vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) மாறிலி = 0;
பாதுகாக்கப்பட்ட:
    int32_t stringToIndex(மாறிலி char16_t* str) மாறிலி {
        UBool found = பொய்;
        int32_t ret = uhash_getiAndFound(fDict, (மாறிலி வெறுமை*)str, &found);
        எனில் (!found) {
            ret = fDict->count;
        }
#ifdef LSTM_VECTORIZER_DEBUG
        printf("[");
        போது (*str != 0x0000) {
            printf("U+%04x ", *str);
            str++;
        }
        printf("] map to %d\n", ret);
#endif
        திரும்பு ret;
    }

தனிப்பட்ட:
    UHashtable* fDict;
};

Vectorizer::~Vectorizer()
{
}

வகுப்பு CodePointsVectorizer : பொது Vectorizer {
பொது:
    CodePointsVectorizer(UHashtable* dict) : Vectorizer(dict) {}
    மெய்நிகர் ~CodePointsVectorizer();
    மெய்நிகர் வெறுமை vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) மாறிலி override;
};

CodePointsVectorizer::~CodePointsVectorizer()
{
}

வெறுமை CodePointsVectorizer::vectorize(
    UText *text, int32_t startPos, int32_t endPos,
    UVector32 &offsets, UVector32 &indices, UErrorCode &status) மாறிலி
{
    எனில் (offsets.ensureCapacity(endPos - startPos, status) &&
            indices.ensureCapacity(endPos - startPos, status)) {
        எனில் (U_FAILURE(status)) திரும்பு;
        utext_setNativeIndex(text, startPos);
        int32_t current;
        char16_t str[2] = {0, 0};
        போது (U_SUCCESS(status) &&
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

வகுப்பு GraphemeClusterVectorizer : பொது Vectorizer {
பொது:
    GraphemeClusterVectorizer(UHashtable* dict)
        : Vectorizer(dict)
    {
    }
    மெய்நிகர் ~GraphemeClusterVectorizer();
    மெய்நிகர் வெறுமை vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) மாறிலி override;
};

GraphemeClusterVectorizer::~GraphemeClusterVectorizer()
{
}

constexpr int32_t MAX_GRAPHEME_CLSTER_LENGTH = 10;

வெறுமை GraphemeClusterVectorizer::vectorize(
    UText *text, int32_t startPos, int32_t endPos,
    UVector32 &offsets, UVector32 &indices, UErrorCode &status) மாறிலி
{
    எனில் (U_FAILURE(status)) திரும்பு;
    எனில் (!offsets.ensureCapacity(endPos - startPos, status) ||
            !indices.ensureCapacity(endPos - startPos, status)) {
        திரும்பு;
    }
    எனில் (U_FAILURE(status)) திரும்பு;
    LocalPointer<BreakIterator> graphemeIter(BreakIterator::createCharacterInstance(Locale(), status));
    எனில் (U_FAILURE(status)) திரும்பு;
    graphemeIter->setText(text, status);
    எனில் (U_FAILURE(status)) திரும்பு;

    எனில் (startPos != 0) {
        graphemeIter->preceding(startPos);
    }
    int32_t last = startPos;
    int32_t current = startPos;
    char16_t str[MAX_GRAPHEME_CLSTER_LENGTH];
    போது ((current = graphemeIter->next()) != BreakIterator::DONE) {
        எனில் (current >= endPos) {
            நிறுத்து;
        }
        எனில் (current > startPos) {
            utext_extract(text, last, current, str, MAX_GRAPHEME_CLSTER_LENGTH, &status);
            எனில் (U_FAILURE(status)) திரும்பு;
            offsets.addElement(last, status);
            indices.addElement(stringToIndex(str), status);
            எனில் (U_FAILURE(status)) திரும்பு;
        }
        last = current;
    }
    எனில் (U_FAILURE(status) || last >= endPos) {
        திரும்பு;
    }
    utext_extract(text, last, endPos, str, MAX_GRAPHEME_CLSTER_LENGTH, &status);
    எனில் (U_SUCCESS(status)) {
        offsets.addElement(last, status);
        indices.addElement(stringToIndex(str), status);
    }
}

// Computing LSTM as stated in
// https://en.wikipedia.org/wiki/Long_short-term_memory#LSTM_with_a_forget_gate
// ifco is temp array allocate outside which does not need to be
// input/output value but could avoid unnecessary memory alloc/free if passing
// in.
வெறுமை compute(
    int32_t hunits,
    மாறிலி ReadArray2D& W, மாறிலி ReadArray2D& U, மாறிலி ReadArray1D& b,
    மாறிலி ReadArray1D& x, Array1D& h, Array1D& c,
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
நிலையான மாறிலி int32_t MIN_WORD = 2;

// Minimum number of characters for two words
நிலையான மாறிலி int32_t MIN_WORD_SPAN = MIN_WORD * 2;

int32_t
LSTMBreakEngine::divideUpDictionaryRange( UText *text,
                                                int32_t startPos,
                                                int32_t endPos,
                                                UVector32 &foundBreaks,
                                                UBool /* isPhraseBreaking */,
                                                UErrorCode& status) மாறிலி {
    எனில் (U_FAILURE(status)) திரும்பு 0;
    int32_t beginFoundBreakSize = foundBreaks.size();
    utext_setNativeIndex(text, startPos);
    utext_moveIndex32(text, MIN_WORD_SPAN);
    எனில் (utext_getNativeIndex(text) >= endPos) {
        திரும்பு 0;       // Not enough characters for two words
    }
    utext_setNativeIndex(text, startPos);

    UVector32 offsets(status);
    UVector32 indices(status);
    எனில் (U_FAILURE(status)) திரும்பு 0;
    fVectorizer->vectorize(text, startPos, endPos, offsets, indices, status);
    எனில் (U_FAILURE(status)) திரும்பு 0;
    int32_t* offsetsBuf = offsets.getBuffer();
    int32_t* indicesBuf = indices.getBuffer();

    int32_t உள்ளீடு_seq_len = indices.size();
    int32_t hunits = fData->fForwardU.d1();

    // ----- Begin of all the Array memory allocation needed for this function
    // Allocate temp array used inside compute()
    Array1D ifco(4 * hunits, status);

    Array1D c(hunits, status);
    Array1D logp(4, status);

    // TODO: limit size of hBackward. If input_seq_len is too big, we could
    // run out of memory.
    // Backward LSTM
    Array2D hBackward(உள்ளீடு_seq_len, hunits, status);

    // Allocate fbRow and slice the internal array in two.
    Array1D fbRow(2 * hunits, status);

    // ----- End of all the Array memory allocation needed for this function
    எனில் (U_FAILURE(status)) திரும்பு 0;

    // To save the needed memory usage, the following is different from the
    // Python or ICU4X implementation. We first perform the Backward LSTM
    // and then merge the iteration of the forward LSTM and the output layer
    // together because we only neetdto remember the h[t-1] for Forward LSTM.
    க்கு (int32_t i = உள்ளீடு_seq_len - 1; i >= 0; i--) {
        Array1D hRow = hBackward.row(i);
        எனில் (i != உள்ளீடு_seq_len - 1) {
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
    க்கு (int32_t i = 0; i < உள்ளீடு_seq_len; i++) {
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
        எனில் (current == BEGIN || current == SINGLE) {
            எனில் (i != 0) {
                foundBreaks.addElement(offsetsBuf[i], status);
                எனில் (U_FAILURE(status)) திரும்பு 0;
            }
        }
    }
    திரும்பு foundBreaks.size() - beginFoundBreakSize;
}

Vectorizer* createVectorizer(மாறிலி LSTMData* data, UErrorCode &status) {
    எனில் (U_FAILURE(status)) {
        திரும்பு nullptr;
    }
    தேர்வு (data->fType) {
        நிலை CODE_POINTS:
            திரும்பு புதிய CodePointsVectorizer(data->fDict);
            நிறுத்து;
        நிலை GRAPHEME_CLUSTER:
            திரும்பு புதிய GraphemeClusterVectorizer(data->fDict);
            நிறுத்து;
        இயல்பு:
            நிறுத்து;
    }
    UPRV_UNREACHABLE_EXIT;
}

LSTMBreakEngine::LSTMBreakEngine(மாறிலி LSTMData* data, மாறிலி UnicodeSet& set, UErrorCode &status)
    : DictionaryBreakEngine(), fData(data), fVectorizer(createVectorizer(fData, status))
{
    எனில் (U_FAILURE(status)) {
      fData = nullptr;  // If failure, we should not delete fData in destructor because the caller will do so.
      திரும்பு;
    }
    setCharacters(set);
}

LSTMBreakEngine::~LSTMBreakEngine() {
    delete fData;
    delete fVectorizer;
}

மாறிலி char16_t* LSTMBreakEngine::name() மாறிலி {
    திரும்பு fData->fName;
}

UnicodeString defaultLSTM(UScriptCode script, UErrorCode& status) {
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_get_By_சாவி_With_Fallback(b, "lstm", b, &status);
    UnicodeString result = ures_get_Unicode_String_By_சாவி(b, uscript_getShortName(script), &status);
    ures_close(b);
    திரும்பு result;
}

U_CAPI மாறிலி LSTMData* U_EXPORT2 CreateLSTMDataForScript(UScriptCode script, UErrorCode& status)
{
    எனில் (script != USCRIPT_KHMER && script != USCRIPT_LAO && script != USCRIPT_MYANMAR && script != USCRIPT_THAI) {
        திரும்பு nullptr;
    }
    UnicodeString name = defaultLSTM(script, status);
    எனில் (U_FAILURE(status)) திரும்பு nullptr;
    CharString namebuf;
    namebuf.appendInvariantChars(name, status).truncate(namebuf.lastIndexOf('.'));

    LocalUResourceBundlePointer rb(
        ures_openDirect(U_ICUDATA_BRKITR, namebuf.data(), &status));
    எனில் (U_FAILURE(status)) திரும்பு nullptr;

    திரும்பு CreateLSTMData(rb.orphan(), status);
}

U_CAPI மாறிலி LSTMData* U_EXPORT2 CreateLSTMData(UResourceBundle* rb, UErrorCode& status)
{
    எனில் (U_FAILURE(status)) {
        திரும்பு nullptr;
    }
    மாறிலி LSTMData* result = புதிய LSTMData(rb, status);
    எனில் (U_FAILURE(status)) {
        delete result;
        திரும்பு nullptr;
    }
    திரும்பு result;
}

U_CAPI மாறிலி LanguageBreakEngine* U_EXPORT2
CreateLSTMBreakEngine(UScriptCode script, மாறிலி LSTMData* data, UErrorCode& status)
{
    UnicodeString unicodeSetString;
    தேர்வு(script) {
        நிலை USCRIPT_THAI:
            unicodeSetString = UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]");
            நிறுத்து;
        நிலை USCRIPT_MYANMAR:
            unicodeSetString = UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]");
            நிறுத்து;
        இயல்பு:
            delete data;
            திரும்பு nullptr;
    }
    UnicodeSet unicodeSet;
    unicodeSet.applyPattern(unicodeSetString, status);
    மாறிலி LanguageBreakEngine* engine = புதிய LSTMBreakEngine(data, unicodeSet, status);
    எனில் (U_FAILURE(status) || engine == nullptr) {
        எனில் (engine != nullptr) {
            delete engine;
        } இல்லையெனில் {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        திரும்பு nullptr;
    }
    திரும்பு engine;
}

U_CAPI வெறுமை U_EXPORT2 DeleteLSTMData(மாறிலி LSTMData* data)
{
    delete data;
}

U_CAPI மாறிலி char16_t* U_EXPORT2 LSTMDataName(மாறிலி LSTMData* data)
{
    திரும்பு data->fName;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
