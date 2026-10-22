// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#شامل <optional>
#شامل <string_view>
#شامل <utility>

#شامل "bytesinkutil.h"  // StringByteSink<CharString>
#شامل "charstr.h"
#شامل "cstring.h"
#شامل "fixedstring.h"
#شامل "ulocimp.h"
#شامل "unicode/localebuilder.h"
#شامل "unicode/locid.h"

فضاینام {

درونخطی منطقی UPRV_ISDIGIT(نویسه c) { بازگشت c >= '0' && c <= '9'; }
درونخطی منطقی UPRV_ISALPHANUM(نویسه c) { بازگشت uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr ثابت نویسه* k_Attribute_کلید = "attribute";

منطقی _isExtensionSubtags(نویسه کلید, ثابت نویسه* s, int32_t len) {
    انتخاب (uprv_tolower(کلید)) {
        حالت 'u':
            بازگشت ultag_isUnicodeExtensionSubtags(s, len);
        حالت 't':
            بازگشت ultag_isTransformedExtensionSubtags(s, len);
        حالت 'x':
            بازگشت ultag_is_Privateuse_مقدار_Subtags(s, len);
        پیشفرض:
            بازگشت ultag_isExtensionSubtags(s, len);
    }
}

}  // namespace

U_NAMESPACE_BEGIN

LocaleBuilder::LocaleBuilder() : UObject(), status_(U_ZERO_ERROR), language_(),
    script_(), region_(), variant_(nullptr), extensions_(nullptr)
{
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
}

LocaleBuilder::~LocaleBuilder()
{
    delete variant_;
    delete extensions_;
}

LocaleBuilder& LocaleBuilder::setLocale(ثابت Locale& locale) U_LIFETIME_BOUND
{
    clear();
    setLanguage(locale.getLanguage());
    setScript(locale.getScript());
    setRegion(locale.getCountry());
    setVariant(locale.getVariant());
    extensions_ = locale.clone();
    اگر (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    بازگشت *این;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    اگر (U_FAILURE(status_)) { بازگشت *این; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    بازگشت *این;
}

فضاینام {

تهی setField(StringPiece input, نویسه* dest, UErrorCode& errorCode,
              منطقی (*test)(ثابت نویسه*, int32_t)) {
    اگر (U_FAILURE(errorCode)) { بازگشت; }
    اگر (input.empty()) {
        dest[0] = '\0';
    } وگرنه اگر (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } وگرنه {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    بازگشت *این;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    بازگشت *این;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    بازگشت *این;
}

فضاینام {

تهی transform(نویسه* data, int32_t len) {
    برای (int32_t i = 0; i < len; i++, data++) {
        اگر (*data == '_') {
            *data = '-';
        } وگرنه {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    اگر (U_FAILURE(status_)) { بازگشت *این; }
    اگر (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        بازگشت *این;
    }
    FixedString* new_variant = جدید FixedString(variant);
    اگر (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        بازگشت *این;
    }
    transform(new_variant->getAlias(), variant.length());
    اگر (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت *این;
    }
    delete variant_;
    variant_ = new_variant;
    بازگشت *این;
}

فضاینام {

منطقی
is_Keyword_مقدار(ثابت نویسه* کلید, ثابت نویسه* value, int32_t مقدار_len)
{
    اگر (کلید[1] == '\0') {
        // one char key
        بازگشت (UPRV_ISALPHANUM(uprv_tolower(کلید[0])) &&
                _isExtensionSubtags(کلید[0], value, مقدار_len));
    } وگرنه اگر (uprv_strcmp(کلید, k_Attribute_کلید) == 0) {
        // unicode attributes
        بازگشت ultag_isUnicodeLocaleAttributes(value, مقدار_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_کلید = ulocimp_to_Bcp_کلید_With_Fallback(کلید);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(کلید, value);

    بازگشت unicode_locale_کلید.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_کلید(unicode_locale_کلید->data(),
                                    static_cast<int32_t>(unicode_locale_کلید->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->data(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

تهی
_copyExtensions(ثابت Locale& from, icu::StringEnumeration *keywords,
                Locale& to, منطقی validate, UErrorCode& errorCode)
{
    اگر (U_FAILURE(errorCode)) { بازگشت; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    اگر (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        اگر (U_FAILURE(errorCode) || ownedKeywords.isNull()) { بازگشت; }
        keywords = ownedKeywords.getAlias();
    }
    ثابت نویسه* کلید;
    وقتی ((کلید = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.get_Keyword_مقدار<CharString>(کلید, errorCode);
        اگر (U_FAILURE(errorCode)) { بازگشت; }
        اگر (uprv_strcmp(کلید, k_Attribute_کلید) == 0) {
            transform(value.data(), value.length());
        }
        اگر (validate &&
            !is_Keyword_مقدار(کلید, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت;
        }
        to.set_Keyword_مقدار(کلید, value.data(), errorCode);
        اگر (U_FAILURE(errorCode)) { بازگشت; }
    }
}

تهی
clear_UAttributes_And_کلید_Type(Locale& locale, UErrorCode& errorCode)
{
    اگر (U_FAILURE(errorCode)) { بازگشت; }
    // Clear Unicode attributes
    locale.set_Keyword_مقدار(k_Attribute_کلید, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    اگر (U_FAILURE(errorCode) || iter.isNull()) { بازگشت; }
    ثابت نویسه* کلید;
    وقتی ((کلید = iter->next(nullptr, errorCode)) != nullptr) {
        locale.set_Unicode_Keyword_مقدار(کلید, nullptr, errorCode);
    }
}

تهی
_setUnicodeExtensions(Locale& locale, ثابت CharString& value, UErrorCode& errorCode)
{
    اگر (U_FAILURE(errorCode)) { بازگشت; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", errorCode);
    locale_str.append(value, errorCode);
    _copyExtensions(
        Locale::forLanguageTag(locale_str.data(), errorCode), nullptr,
        locale, نادرست, errorCode);
}

}  // namespace

LocaleBuilder& LocaleBuilder::setExtension(نویسه کلید, StringPiece value) U_LIFETIME_BOUND
{
    اگر (U_FAILURE(status_)) { بازگشت *این; }
    اگر (!UPRV_ISALPHANUM(کلید)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت *این;
    }
    CharString مقدار_str(value, status_);
    اگر (U_FAILURE(status_)) { بازگشت *این; }
    transform(مقدار_str.data(), مقدار_str.length());
    اگر (!مقدار_str.isEmpty() &&
            !_isExtensionSubtags(کلید, مقدار_str.data(), مقدار_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت *این;
    }
    اگر (extensions_ == nullptr) {
        extensions_ = Locale::get_ریشه().clone();
        اگر (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            بازگشت *این;
        }
    }
    اگر (uprv_tolower(کلید) != 'u') {
        // for t, x and others extension.
        extensions_->set_Keyword_مقدار(StringPiece(&کلید, 1), مقدار_str.data(),
                                     status_);
        بازگشت *این;
    }
    clear_UAttributes_And_کلید_Type(*extensions_, status_);
    اگر (U_FAILURE(status_)) { بازگشت *این; }
    اگر (!value.empty()) {
        _setUnicodeExtensions(*extensions_, مقدار_str, status_);
    }
    بازگشت *این;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece کلید,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    اگر (U_FAILURE(status_)) { بازگشت *این; }
    اگر (!ultag_is_Unicode_Locale_کلید(کلید.data(), کلید.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      بازگشت *این;
    }
    اگر (extensions_ == nullptr) {
        extensions_ = Locale::get_ریشه().clone();
        اگر (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            بازگشت *این;
        }
    }
    extensions_->set_Unicode_Keyword_مقدار(کلید, type, status_);
    بازگشت *این;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString مقدار_str(value, status_);
    اگر (U_FAILURE(status_)) { بازگشت *این; }
    transform(مقدار_str.data(), مقدار_str.length());
    اگر (!ultag_isUnicodeLocaleAttribute(مقدار_str.data(), مقدار_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت *این;
    }
    اگر (extensions_ == nullptr) {
        extensions_ = Locale::get_ریشه().clone();
        اگر (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            بازگشت *این;
        }
        extensions_->set_Keyword_مقدار(k_Attribute_کلید, مقدار_str.data(), status_);
        بازگشت *این;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_مقدار<CharString>(k_Attribute_کلید, localErrorCode);
    اگر (U_FAILURE(localErrorCode)) {
        CharString new_attributes(مقدار_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->set_Keyword_مقدار(k_Attribute_کلید, new_attributes.data(), status_);
        بازگشت *این;
    }

    transform(attributes.data(),attributes.length());
    ثابت نویسه* start = attributes.data();
    ثابت نویسه* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    منطقی inserted = نادرست;
    وقتی (start < limit) {
        اگر (!inserted) {
            صحیح cmp = uprv_strcmp(start, مقدار_str.data());
            اگر (cmp == 0) { بازگشت *این; }  // Found it in attributes: Just return
            اگر (cmp > 0) {
                اگر (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(مقدار_str.data(), status_);
                inserted = درست;
            }
        }
        اگر (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    اگر (!inserted) {
        اگر (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(مقدار_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->set_Keyword_مقدار(k_Attribute_کلید, new_attributes.data(), status_);
    بازگشت *این;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString مقدار_str(value, status_);
    اگر (U_FAILURE(status_)) { بازگشت *این; }
    transform(مقدار_str.data(), مقدار_str.length());
    اگر (!ultag_isUnicodeLocaleAttribute(مقدار_str.data(), مقدار_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت *این;
    }
    اگر (extensions_ == nullptr) { بازگشت *این; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_مقدار<CharString>(k_Attribute_کلید, localErrorCode);
    // get failure, just return
    اگر (U_FAILURE(localErrorCode)) { بازگشت *این; }
    // Do not have any attributes, just return.
    اگر (attributes.isEmpty()) { بازگشت *این; }

    نویسه* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    برای (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    ثابت نویسه* start = attributes.data();
    ثابت نویسه* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    منطقی found = نادرست;
    وقتی (start < limit) {
        اگر (uprv_strcmp(start, مقدار_str.data()) == 0) {
            found = درست;
        } وگرنه {
            اگر (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    اگر (found) {
        extensions_->set_Keyword_مقدار(k_Attribute_کلید, new_attributes.data(), status_);
    }
    بازگشت *این;
}

LocaleBuilder& LocaleBuilder::clear() U_LIFETIME_BOUND
{
    status_ = U_ZERO_ERROR;
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
    delete variant_;
    variant_ = nullptr;
    clearExtensions();
    بازگشت *این;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    بازگشت *این;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  بازگشت bogus;
}

تهی LocaleBuilder::copyExtensionsFrom(ثابت Locale& src, UErrorCode& errorCode)
{
    اگر (U_FAILURE(errorCode)) { بازگشت; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    اگر (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        بازگشت;
    }
    اگر (extensions_ == nullptr) {
        extensions_ = Locale::get_ریشه().clone();
        اگر (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            بازگشت;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, نادرست, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    اگر (U_FAILURE(errorCode)) {
        بازگشت makeBogusLocale();
    }
    اگر (U_FAILURE(status_)) {
        errorCode = status_;
        بازگشت makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    اگر (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    اگر (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    اگر (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    اگر (U_FAILURE(errorCode)) {
        بازگشت makeBogusLocale();
    }
    Locale product(locale_str.data());
    اگر (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, درست, errorCode);
    }
    اگر (U_FAILURE(errorCode)) {
        بازگشت makeBogusLocale();
    }
    بازگشت product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) ثابت {
    اگر (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        بازگشت درست;
    }
    outErrorCode = status_;
    بازگشت U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
