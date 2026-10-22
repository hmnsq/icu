// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#અન્તર્ભાવ <complex>
#અન્તર્ભાવ <utility>

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_BREAK_ITERATION

#અન્તર્ભાવ "brkeng.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "lstmbe.h"
#અન્તર્ભાવ "putilimp.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ubrkimpl.h"
#અન્તર્ભાવ "uresimp.h"
#અન્તર્ભાવ "uvectr32.h"
#અન્તર્ભાવ "uvector.h"

#અન્તર્ભાવ "unicode/brkiter.h"
#અન્તર્ભાવ "unicode/resbund.h"
#અન્તર્ભાવ "unicode/ubrk.h"
#અન્તર્ભાવ "unicode/uniset.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/utf.h"

U_NAMESPACE_BEGIN

// Uncomment the following #define to debug.
// #define LSTM_DEBUG 1
// #define LSTM_VECTORIZER_DEBUG 1

/**
 * Interface for reading 1D array.
 */
વર્ગ ReadArray1D {
જાહેર:
    આભાસી ~ReadArray1D();
    આભાસી int32_t d1() અચલ = 0;
    આભાસી દશાંશ get(int32_t i) અચલ = 0;

#ifdef LSTM_DEBUG
    શૂન્ય print() અચલ {
        printf("\n[");
        માટે (int32_t i = 0; i < d1(); i++) {
           printf("%0.8e ", get(i));
           જો (i % 4 == 3) printf("\n");
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
વર્ગ ReadArray2D {
જાહેર:
    આભાસી ~ReadArray2D();
    આભાસી int32_t d1() અચલ = 0;
    આભાસી int32_t d2() અચલ = 0;
    આભાસી દશાંશ get(int32_t i, int32_t j) અચલ = 0;
};

ReadArray2D::~ReadArray2D()
{
}

/**
 * A class to index a float array as a 1D Array without owning the pointer or
 * copy the data.
 */
વર્ગ ConstArray1D : જાહેર ReadArray1D {
જાહેર:
    ConstArray1D() : data_(nullptr), d1_(0) {}

    ConstArray1D(અચલ દશાંશ* data, int32_t d1) : data_(data), d1_(d1) {}

    આભાસી ~ConstArray1D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    શૂન્ય init(અચલ int32_t* data, int32_t d1) {
        U_ASSERT(IEEE_754 == 1);
        data_ = reinterpret_cast<અચલ દશાંશ*>(data);
        d1_ = d1;
    }

    // ReadArray1D methods.
    આભાસી int32_t d1() અચલ override { પાછા d1_; }
    આભાસી દશાંશ get(int32_t i) અચલ override {
        U_ASSERT(i < d1_);
        પાછા data_[i];
    }

ખાનગી:
    અચલ દશાંશ* data_;
    int32_t d1_;
};

ConstArray1D::~ConstArray1D()
{
}

/**
 * A class to index a float array as a 2D Array without owning the pointer or
 * copy the data.
 */
વર્ગ ConstArray2D : જાહેર ReadArray2D {
જાહેર:
    ConstArray2D() : data_(nullptr), d1_(0), d2_(0) {}

    ConstArray2D(અચલ દશાંશ* data, int32_t d1, int32_t d2)
        : data_(data), d1_(d1), d2_(d2) {}

    આભાસી ~ConstArray2D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    શૂન્ય init(અચલ int32_t* data, int32_t d1, int32_t d2) {
        U_ASSERT(IEEE_754 == 1);
        data_ = reinterpret_cast<અચલ દશાંશ*>(data);
        d1_ = d1;
        d2_ = d2;
    }

    // ReadArray2D methods.
    અંતઃરેખ int32_t d1() અચલ override { પાછા d1_; }
    અંતઃરેખ int32_t d2() અચલ override { પાછા d2_; }
    દશાંશ get(int32_t i, int32_t j) અચલ override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        પાછા data_[i * d2_ + j];
    }

    // Expose the ith row as a ConstArray1D
    અંતઃરેખ ConstArray1D row(int32_t i) અચલ {
        U_ASSERT(i < d1_);
        પાછા ConstArray1D(data_ + i * d2_, d2_);
    }

ખાનગી:
    અચલ દશાંશ* data_;
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
વર્ગ Array1D : જાહેર ReadArray1D {
જાહેર:
    Array1D() : memory_(nullptr), data_(nullptr), d1_(0) {}
    Array1D(int32_t d1, UErrorCode &status)
        : memory_(uprv_malloc(d1 * કદ(દશાંશ))),
          data_(static_cast<દશાંશ*>(memory_)), d1_(d1) {
        જો (U_SUCCESS(status)) {
            જો (memory_ == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                પાછા;
            }
            clear();
        }
    }

    આભાસી ~Array1D();

    // A special constructor which does not own the memory but writeable
    // as a slice of an array.
    Array1D(દશાંશ* data, int32_t d1)
        : memory_(nullptr), data_(data), d1_(d1) {}

    // ReadArray1D methods.
    આભાસી int32_t d1() અચલ override { પાછા d1_; }
    આભાસી દશાંશ get(int32_t i) અચલ override {
        U_ASSERT(i < d1_);
        પાછા data_[i];
    }

    // Return the index which point to the max data in the array.
    અંતઃરેખ int32_t maxIndex() અચલ {
        int32_t index = 0;
        દશાંશ max = data_[0];
        માટે (int32_t i = 1; i < d1_; i++) {
            જો (data_[i] > max) {
                max = data_[i];
                index = i;
            }
        }
        પાછા index;
    }

    // Slice part of the array to a new one.
    અંતઃરેખ Array1D slice(int32_t from, int32_t size) અચલ {
        U_ASSERT(from >= 0);
        U_ASSERT(from < d1_);
        U_ASSERT(from + size <= d1_);
        પાછા Array1D(data_ + from, size);
    }

    // Add dot product of a 1D array and a 2D array into this one.
    અંતઃરેખ Array1D& addDotProduct(અચલ ReadArray1D& a, અચલ ReadArray2D& b) {
        U_ASSERT(a.d1() == b.d1());
        U_ASSERT(b.d2() == d1());
        માટે (int32_t i = 0; i < d1(); i++) {
            માટે (int32_t j = 0; j < a.d1(); j++) {
                data_[i] += a.get(j) * b.get(j, i);
            }
        }
        પાછા *આ;
    }

    // Hadamard Product the values of another array of the same size into this one.
    અંતઃરેખ Array1D& hadamardProduct(અચલ ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        માટે (int32_t i = 0; i < d1(); i++) {
            data_[i] *= a.get(i);
        }
        પાછા *આ;
    }

    // Add the Hadamard Product of two arrays of the same size into this one.
    અંતઃરેખ Array1D& addHadamardProduct(અચલ ReadArray1D& a, અચલ ReadArray1D& b) {
        U_ASSERT(a.d1() == d1());
        U_ASSERT(b.d1() == d1());
        માટે (int32_t i = 0; i < d1(); i++) {
            data_[i] += a.get(i) * b.get(i);
        }
        પાછા *આ;
    }

    // Add the values of another array of the same size into this one.
    અંતઃરેખ Array1D& add(અચલ ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        માટે (int32_t i = 0; i < d1(); i++) {
            data_[i] += a.get(i);
        }
        પાછા *આ;
    }

    // Assign the values of another array of the same size into this one.
    અંતઃરેખ Array1D& assign(અચલ ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        માટે (int32_t i = 0; i < d1(); i++) {
            data_[i] = a.get(i);
        }
        પાછા *આ;
    }

    // Apply tanh to all the elements in the array.
    અંતઃરેખ Array1D& tanh() {
        પાછા tanh(*આ);
    }

    // Apply tanh of a and store into this array.
    અંતઃરેખ Array1D& tanh(અચલ Array1D& a) {
        U_ASSERT(a.d1() == d1());
        માટે (int32_t i = 0; i < d1_; i++) {
            data_[i] = std::tanh(a.get(i));
        }
        પાછા *આ;
    }

    // Apply sigmoid to all the elements in the array.
    અંતઃરેખ Array1D& sigmoid() {
        માટે (int32_t i = 0; i < d1_; i++) {
            data_[i] = 1.0f/(1.0f + expf(-data_[i]));
        }
        પાછા *આ;
    }

    અંતઃરેખ Array1D& clear() {
        uprv_memset(data_, 0, d1_ * કદ(દશાંશ));
        પાછા *આ;
    }

ખાનગી:
    શૂન્ય* memory_;
    દશાંશ* data_;
    int32_t d1_;
};

Array1D::~Array1D()
{
    uprv_free(memory_);
}

વર્ગ Array2D : જાહેર ReadArray2D {
જાહેર:
    Array2D() : memory_(nullptr), data_(nullptr), d1_(0), d2_(0) {}
    Array2D(int32_t d1, int32_t d2, UErrorCode &status)
        : memory_(uprv_malloc(d1 * d2 * કદ(દશાંશ))),
          data_(static_cast<દશાંશ*>(memory_)), d1_(d1), d2_(d2) {
        જો (U_SUCCESS(status)) {
            જો (memory_ == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                પાછા;
            }
            clear();
        }
    }
    આભાસી ~Array2D();

    // ReadArray2D methods.
    આભાસી int32_t d1() અચલ override { પાછા d1_; }
    આભાસી int32_t d2() અચલ override { પાછા d2_; }
    આભાસી દશાંશ get(int32_t i, int32_t j) અચલ override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        પાછા data_[i * d2_ + j];
    }

    અંતઃરેખ Array1D row(int32_t i) અચલ {
        U_ASSERT(i < d1_);
        પાછા Array1D(data_ + i * d2_, d2_);
    }

    અંતઃરેખ Array2D& clear() {
        uprv_memset(data_, 0, d1_ * d2_ * કદ(દશાંશ));
        પાછા *આ;
    }

ખાનગી:
    શૂન્ય* memory_;
    દશાંશ* data_;
    int32_t d1_;
    int32_t d2_;
};

Array2D::~Array2D()
{
    uprv_free(memory_);
}

પ્રકારવ્યાખ્યા ગણના {
    BEGIN,
    INSIDE,
    END,
    SINGLE
} LSTMClass;

પ્રકારવ્યાખ્યા ગણના {
    UNKNOWN,
    CODE_POINTS,
    GRAPHEME_CLUSTER,
} EmbeddingType;

સંરચના LSTMData : જાહેર UMemory {
    LSTMData(UResourceBundle* rb, UErrorCode &status);
    ~LSTMData();
    UHashtable* fDict;
    EmbeddingType fType;
    અચલ char16_t* fName;
    ConstArray2D fEmbedding;
    ConstArray2D fForwardW;
    ConstArray2D fForwardU;
    ConstArray1D fForwardB;
    ConstArray2D fBackwardW;
    ConstArray2D fBackwardU;
    ConstArray1D fBackwardB;
    ConstArray2D fOutputW;
    ConstArray1D fOutputB;

ખાનગી:
    UResourceBundle* fBundle;
};

LSTMData::LSTMData(UResourceBundle* rb, UErrorCode &status)
    : fDict(nullptr), fType(UNKNOWN), fName(nullptr),
      fBundle(rb)
{
    જો (U_FAILURE(status)) {
        પાછા;
    }
    જો (IEEE_754 != 1) {
        status = U_UNSUPPORTED_ERROR;
        પાછા;
    }
    LocalUResourceBundlePointer embeddings_res(
        ures_get_By_ચાવી(rb, "embeddings", nullptr, &status));
    int32_t embedding_size = ures_getInt(embeddings_res.getAlias(), &status);
    LocalUResourceBundlePointer hunits_res(
        ures_get_By_ચાવી(rb, "hunits", nullptr, &status));
    જો (U_FAILURE(status)) પાછા;
    int32_t hunits = ures_getInt(hunits_res.getAlias(), &status);
    અચલ char16_t* type = ures_get_String_By_ચાવી(rb, "type", nullptr, &status);
    જો (U_FAILURE(status)) પાછા;
    જો (u_strCompare(type, -1, u"codepoints", -1, ખોટું) == 0) {
        fType = CODE_POINTS;
    } નહીંતર જો (u_strCompare(type, -1, u"graphclust", -1, ખોટું) == 0) {
        fType = GRAPHEME_CLUSTER;
    }
    fName = ures_get_String_By_ચાવી(rb, "model", nullptr, &status);
    LocalUResourceBundlePointer dataRes(ures_get_By_ચાવી(rb, "data", nullptr, &status));
    જો (U_FAILURE(status)) પાછા;
    int32_t data_len = 0;
    અચલ int32_t* data = ures_getIntVector(dataRes.getAlias(), &data_len, &status);
    fDict = uhash_open(uhash_hashUChars, uhash_compareUChars, nullptr, &status);

    ઢગલો_UResource_Bundle ઢગલો_Temp_Bundle;
    Resource_Data_કિંમત value;
    ures_get_કિંમત_With_Fallback(rb, "dict", ઢગલો_Temp_Bundle.getAlias(), value, status);
    ResourceArray stringArray = value.getArray(status);
    int32_t અંક_index = stringArray.getSize();
    જો (U_FAILURE(status)) { પાછા; }

    // put dict into hash
    int32_t stringLength;
    માટે (int32_t idx = 0; idx < અંક_index; idx++) {
        stringArray.get_કિંમત(idx, value);
        અચલ char16_t* str = value.getString(stringLength, status);
        uhash_putiAllowZero(fDict, (શૂન્ય*)str, idx, &status);
        જો (U_FAILURE(status)) પાછા;
#ifdef LSTM_VECTORIZER_DEBUG
        printf("Assign [");
        જ્યારે (*str != 0x0000) {
            printf("U+%04x ", *str);
            str++;
        }
        printf("] map to %d\n", idx-1);
#endif
    }
    int32_t mat1_size = (અંક_index + 1) * embedding_size;
    int32_t mat2_size = embedding_size * 4 * hunits;
    int32_t mat3_size = hunits * 4 * hunits;
    int32_t mat4_size = 4 * hunits;
    int32_t mat5_size = mat2_size;
    int32_t mat6_size = mat3_size;
    int32_t mat7_size = mat4_size;
    int32_t mat8_size = 2 * hunits * 4;
#જો U_DEBUG
    int32_t mat9_size = 4;
    U_ASSERT(data_len == mat1_size + mat2_size + mat3_size + mat4_size + mat5_size +
        mat6_size + mat7_size + mat8_size + mat9_size);
#endif

    fEmbedding.init(data, (અંક_index + 1), embedding_size);
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

વર્ગ Vectorizer : જાહેર UMemory {
જાહેર:
    Vectorizer(UHashtable* dict) : fDict(dict) {}
    આભાસી ~Vectorizer();
    આભાસી શૂન્ય vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) અચલ = 0;
સુરક્ષિત:
    int32_t stringToIndex(અચલ char16_t* str) અચલ {
        UBool found = ખોટું;
        int32_t ret = uhash_getiAndFound(fDict, (અચલ શૂન્ય*)str, &found);
        જો (!found) {
            ret = fDict->count;
        }
#ifdef LSTM_VECTORIZER_DEBUG
        printf("[");
        જ્યારે (*str != 0x0000) {
            printf("U+%04x ", *str);
            str++;
        }
        printf("] map to %d\n", ret);
#endif
        પાછા ret;
    }

ખાનગી:
    UHashtable* fDict;
};

Vectorizer::~Vectorizer()
{
}

વર્ગ CodePointsVectorizer : જાહેર Vectorizer {
જાહેર:
    CodePointsVectorizer(UHashtable* dict) : Vectorizer(dict) {}
    આભાસી ~CodePointsVectorizer();
    આભાસી શૂન્ય vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) અચલ override;
};

CodePointsVectorizer::~CodePointsVectorizer()
{
}

શૂન્ય CodePointsVectorizer::vectorize(
    UText *text, int32_t startPos, int32_t endPos,
    UVector32 &offsets, UVector32 &indices, UErrorCode &status) અચલ
{
    જો (offsets.ensureCapacity(endPos - startPos, status) &&
            indices.ensureCapacity(endPos - startPos, status)) {
        જો (U_FAILURE(status)) પાછા;
        utext_setNativeIndex(text, startPos);
        int32_t current;
        char16_t str[2] = {0, 0};
        જ્યારે (U_SUCCESS(status) &&
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

વર્ગ GraphemeClusterVectorizer : જાહેર Vectorizer {
જાહેર:
    GraphemeClusterVectorizer(UHashtable* dict)
        : Vectorizer(dict)
    {
    }
    આભાસી ~GraphemeClusterVectorizer();
    આભાસી શૂન્ય vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) અચલ override;
};

GraphemeClusterVectorizer::~GraphemeClusterVectorizer()
{
}

constexpr int32_t MAX_GRAPHEME_CLSTER_LENGTH = 10;

શૂન્ય GraphemeClusterVectorizer::vectorize(
    UText *text, int32_t startPos, int32_t endPos,
    UVector32 &offsets, UVector32 &indices, UErrorCode &status) અચલ
{
    જો (U_FAILURE(status)) પાછા;
    જો (!offsets.ensureCapacity(endPos - startPos, status) ||
            !indices.ensureCapacity(endPos - startPos, status)) {
        પાછા;
    }
    જો (U_FAILURE(status)) પાછા;
    LocalPointer<BreakIterator> graphemeIter(BreakIterator::createCharacterInstance(Locale(), status));
    જો (U_FAILURE(status)) પાછા;
    graphemeIter->setText(text, status);
    જો (U_FAILURE(status)) પાછા;

    જો (startPos != 0) {
        graphemeIter->preceding(startPos);
    }
    int32_t last = startPos;
    int32_t current = startPos;
    char16_t str[MAX_GRAPHEME_CLSTER_LENGTH];
    જ્યારે ((current = graphemeIter->next()) != BreakIterator::DONE) {
        જો (current >= endPos) {
            તોડો;
        }
        જો (current > startPos) {
            utext_extract(text, last, current, str, MAX_GRAPHEME_CLSTER_LENGTH, &status);
            જો (U_FAILURE(status)) પાછા;
            offsets.addElement(last, status);
            indices.addElement(stringToIndex(str), status);
            જો (U_FAILURE(status)) પાછા;
        }
        last = current;
    }
    જો (U_FAILURE(status) || last >= endPos) {
        પાછા;
    }
    utext_extract(text, last, endPos, str, MAX_GRAPHEME_CLSTER_LENGTH, &status);
    જો (U_SUCCESS(status)) {
        offsets.addElement(last, status);
        indices.addElement(stringToIndex(str), status);
    }
}

// Computing LSTM as stated in
// https://en.wikipedia.org/wiki/Long_short-term_memory#LSTM_with_a_forget_gate
// ifco is temp array allocate outside which does not need to be
// input/output value but could avoid unnecessary memory alloc/free if passing
// in.
શૂન્ય compute(
    int32_t hunits,
    અચલ ReadArray2D& W, અચલ ReadArray2D& U, અચલ ReadArray1D& b,
    અચલ ReadArray1D& x, Array1D& h, Array1D& c,
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
સ્થિર અચલ int32_t MIN_WORD = 2;

// Minimum number of characters for two words
સ્થિર અચલ int32_t MIN_WORD_SPAN = MIN_WORD * 2;

int32_t
LSTMBreakEngine::divideUpDictionaryRange( UText *text,
                                                int32_t startPos,
                                                int32_t endPos,
                                                UVector32 &foundBreaks,
                                                UBool /* isPhraseBreaking */,
                                                UErrorCode& status) અચલ {
    જો (U_FAILURE(status)) પાછા 0;
    int32_t beginFoundBreakSize = foundBreaks.size();
    utext_setNativeIndex(text, startPos);
    utext_moveIndex32(text, MIN_WORD_SPAN);
    જો (utext_getNativeIndex(text) >= endPos) {
        પાછા 0;       // Not enough characters for two words
    }
    utext_setNativeIndex(text, startPos);

    UVector32 offsets(status);
    UVector32 indices(status);
    જો (U_FAILURE(status)) પાછા 0;
    fVectorizer->vectorize(text, startPos, endPos, offsets, indices, status);
    જો (U_FAILURE(status)) પાછા 0;
    int32_t* offsetsBuf = offsets.getBuffer();
    int32_t* indicesBuf = indices.getBuffer();

    int32_t આગત_seq_len = indices.size();
    int32_t hunits = fData->fForwardU.d1();

    // ----- Begin of all the Array memory allocation needed for this function
    // Allocate temp array used inside compute()
    Array1D ifco(4 * hunits, status);

    Array1D c(hunits, status);
    Array1D logp(4, status);

    // TODO: limit size of hBackward. If input_seq_len is too big, we could
    // run out of memory.
    // Backward LSTM
    Array2D hBackward(આગત_seq_len, hunits, status);

    // Allocate fbRow and slice the internal array in two.
    Array1D fbRow(2 * hunits, status);

    // ----- End of all the Array memory allocation needed for this function
    જો (U_FAILURE(status)) પાછા 0;

    // To save the needed memory usage, the following is different from the
    // Python or ICU4X implementation. We first perform the Backward LSTM
    // and then merge the iteration of the forward LSTM and the output layer
    // together because we only neetdto remember the h[t-1] for Forward LSTM.
    માટે (int32_t i = આગત_seq_len - 1; i >= 0; i--) {
        Array1D hRow = hBackward.row(i);
        જો (i != આગત_seq_len - 1) {
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
    માટે (int32_t i = 0; i < આગત_seq_len; i++) {
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
        જો (current == BEGIN || current == SINGLE) {
            જો (i != 0) {
                foundBreaks.addElement(offsetsBuf[i], status);
                જો (U_FAILURE(status)) પાછા 0;
            }
        }
    }
    પાછા foundBreaks.size() - beginFoundBreakSize;
}

Vectorizer* createVectorizer(અચલ LSTMData* data, UErrorCode &status) {
    જો (U_FAILURE(status)) {
        પાછા nullptr;
    }
    પસંદગી (data->fType) {
        સ્થિતિ CODE_POINTS:
            પાછા નવું CodePointsVectorizer(data->fDict);
            તોડો;
        સ્થિતિ GRAPHEME_CLUSTER:
            પાછા નવું GraphemeClusterVectorizer(data->fDict);
            તોડો;
        મૂળભૂત:
            તોડો;
    }
    UPRV_UNREACHABLE_EXIT;
}

LSTMBreakEngine::LSTMBreakEngine(અચલ LSTMData* data, અચલ UnicodeSet& set, UErrorCode &status)
    : DictionaryBreakEngine(), fData(data), fVectorizer(createVectorizer(fData, status))
{
    જો (U_FAILURE(status)) {
      fData = nullptr;  // If failure, we should not delete fData in destructor because the caller will do so.
      પાછા;
    }
    setCharacters(set);
}

LSTMBreakEngine::~LSTMBreakEngine() {
    delete fData;
    delete fVectorizer;
}

અચલ char16_t* LSTMBreakEngine::name() અચલ {
    પાછા fData->fName;
}

UnicodeString defaultLSTM(UScriptCode script, UErrorCode& status) {
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_get_By_ચાવી_With_Fallback(b, "lstm", b, &status);
    UnicodeString result = ures_get_Unicode_String_By_ચાવી(b, uscript_getShortName(script), &status);
    ures_close(b);
    પાછા result;
}

U_CAPI અચલ LSTMData* U_EXPORT2 CreateLSTMDataForScript(UScriptCode script, UErrorCode& status)
{
    જો (script != USCRIPT_KHMER && script != USCRIPT_LAO && script != USCRIPT_MYANMAR && script != USCRIPT_THAI) {
        પાછા nullptr;
    }
    UnicodeString name = defaultLSTM(script, status);
    જો (U_FAILURE(status)) પાછા nullptr;
    CharString namebuf;
    namebuf.appendInvariantChars(name, status).truncate(namebuf.lastIndexOf('.'));

    LocalUResourceBundlePointer rb(
        ures_openDirect(U_ICUDATA_BRKITR, namebuf.data(), &status));
    જો (U_FAILURE(status)) પાછા nullptr;

    પાછા CreateLSTMData(rb.orphan(), status);
}

U_CAPI અચલ LSTMData* U_EXPORT2 CreateLSTMData(UResourceBundle* rb, UErrorCode& status)
{
    જો (U_FAILURE(status)) {
        પાછા nullptr;
    }
    અચલ LSTMData* result = નવું LSTMData(rb, status);
    જો (U_FAILURE(status)) {
        delete result;
        પાછા nullptr;
    }
    પાછા result;
}

U_CAPI અચલ LanguageBreakEngine* U_EXPORT2
CreateLSTMBreakEngine(UScriptCode script, અચલ LSTMData* data, UErrorCode& status)
{
    UnicodeString unicodeSetString;
    પસંદગી(script) {
        સ્થિતિ USCRIPT_THAI:
            unicodeSetString = UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]");
            તોડો;
        સ્થિતિ USCRIPT_MYANMAR:
            unicodeSetString = UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]");
            તોડો;
        મૂળભૂત:
            delete data;
            પાછા nullptr;
    }
    UnicodeSet unicodeSet;
    unicodeSet.applyPattern(unicodeSetString, status);
    અચલ LanguageBreakEngine* engine = નવું LSTMBreakEngine(data, unicodeSet, status);
    જો (U_FAILURE(status) || engine == nullptr) {
        જો (engine != nullptr) {
            delete engine;
        } નહીંતર {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        પાછા nullptr;
    }
    પાછા engine;
}

U_CAPI શૂન્ય U_EXPORT2 DeleteLSTMData(અચલ LSTMData* data)
{
    delete data;
}

U_CAPI અચલ char16_t* U_EXPORT2 LSTMDataName(અચલ LSTMData* data)
{
    પાછા data->fName;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
