// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#অন্তর্ভাব <optional>
#অন্তর্ভাব <string_view>
#অন্তর্ভাব <utility>

#অন্তর্ভাব "bytesinkutil.h"  // StringByteSink<CharString>
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "fixedstring.h"
#অন্তর্ভাব "ulocimp.h"
#অন্তর্ভাব "unicode/localebuilder.h"
#অন্তর্ভাব "unicode/locid.h"

নামাকাশ {

অন্তঃরেখ যৌক্তিক UPRV_ISDIGIT(অক্ষর c) { ফেরত c >= '0' && c <= '9'; }
অন্তঃরেখ যৌক্তিক UPRV_ISALPHANUM(অক্ষর c) { ফেরত uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr ধ্রুবক অক্ষর* k_Attribute_চাবি = "attribute";

যৌক্তিক _isExtensionSubtags(অক্ষর চাবি, ধ্রুবক অক্ষর* s, int32_t len) {
    নির্বাচন (uprv_tolower(চাবি)) {
        ক্ষেত্র 'u':
            ফেরত ultag_isUnicodeExtensionSubtags(s, len);
        ক্ষেত্র 't':
            ফেরত ultag_isTransformedExtensionSubtags(s, len);
        ক্ষেত্র 'x':
            ফেরত ultag_is_Privateuse_মান_Subtags(s, len);
        পূর্বনির্ধারিত:
            ফেরত ultag_isExtensionSubtags(s, len);
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

LocaleBuilder& LocaleBuilder::setLocale(ধ্রুবক Locale& locale) U_LIFETIME_BOUND
{
    clear();
    setLanguage(locale.getLanguage());
    setScript(locale.getScript());
    setRegion(locale.getCountry());
    setVariant(locale.getVariant());
    extensions_ = locale.clone();
    যদি (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    ফেরত *এই;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    যদি (U_FAILURE(status_)) { ফেরত *এই; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    ফেরত *এই;
}

নামাকাশ {

শূন্য setField(StringPiece input, অক্ষর* dest, UErrorCode& errorCode,
              যৌক্তিক (*test)(ধ্রুবক অক্ষর*, int32_t)) {
    যদি (U_FAILURE(errorCode)) { ফেরত; }
    যদি (input.empty()) {
        dest[0] = '\0';
    } অন্যথা যদি (test(input.তথ্য(), input.length())) {
        uprv_memcpy(dest, input.তথ্য(), input.length());
        dest[input.length()] = '\0';
    } অন্যথা {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    ফেরত *এই;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    ফেরত *এই;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    ফেরত *এই;
}

নামাকাশ {

শূন্য transform(অক্ষর* তথ্য, int32_t len) {
    জন্য (int32_t i = 0; i < len; i++, তথ্য++) {
        যদি (*তথ্য == '_') {
            *তথ্য = '-';
        } অন্যথা {
            *তথ্য = uprv_tolower(*তথ্য);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    যদি (U_FAILURE(status_)) { ফেরত *এই; }
    যদি (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        ফেরত *এই;
    }
    FixedString* new_variant = নতুন FixedString(variant);
    যদি (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        ফেরত *এই;
    }
    transform(new_variant->getAlias(), variant.length());
    যদি (!ultag_isVariantSubtags(new_variant->তথ্য(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত *এই;
    }
    delete variant_;
    variant_ = new_variant;
    ফেরত *এই;
}

নামাকাশ {

যৌক্তিক
is_Keyword_মান(ধ্রুবক অক্ষর* চাবি, ধ্রুবক অক্ষর* value, int32_t মান_len)
{
    যদি (চাবি[1] == '\0') {
        // one char key
        ফেরত (UPRV_ISALPHANUM(uprv_tolower(চাবি[0])) &&
                _isExtensionSubtags(চাবি[0], value, মান_len));
    } অন্যথা যদি (uprv_strcmp(চাবি, k_Attribute_চাবি) == 0) {
        // unicode attributes
        ফেরত ultag_isUnicodeLocaleAttributes(value, মান_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_চাবি = ulocimp_to_Bcp_চাবি_With_Fallback(চাবি);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(চাবি, value);

    ফেরত unicode_locale_চাবি.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_চাবি(unicode_locale_চাবি->তথ্য(),
                                    static_cast<int32_t>(unicode_locale_চাবি->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->তথ্য(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

শূন্য
_copyExtensions(ধ্রুবক Locale& from, icu::StringEnumeration *keywords,
                Locale& to, যৌক্তিক validate, UErrorCode& errorCode)
{
    যদি (U_FAILURE(errorCode)) { ফেরত; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    যদি (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        যদি (U_FAILURE(errorCode) || ownedKeywords.isNull()) { ফেরত; }
        keywords = ownedKeywords.getAlias();
    }
    ধ্রুবক অক্ষর* চাবি;
    যখন ((চাবি = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.get_Keyword_মান<CharString>(চাবি, errorCode);
        যদি (U_FAILURE(errorCode)) { ফেরত; }
        যদি (uprv_strcmp(চাবি, k_Attribute_চাবি) == 0) {
            transform(value.তথ্য(), value.length());
        }
        যদি (validate &&
            !is_Keyword_মান(চাবি, value.তথ্য(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত;
        }
        to.set_Keyword_মান(চাবি, value.তথ্য(), errorCode);
        যদি (U_FAILURE(errorCode)) { ফেরত; }
    }
}

শূন্য
clear_UAttributes_And_চাবি_Type(Locale& locale, UErrorCode& errorCode)
{
    যদি (U_FAILURE(errorCode)) { ফেরত; }
    // Clear Unicode attributes
    locale.set_Keyword_মান(k_Attribute_চাবি, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    যদি (U_FAILURE(errorCode) || iter.isNull()) { ফেরত; }
    ধ্রুবক অক্ষর* চাবি;
    যখন ((চাবি = iter->next(nullptr, errorCode)) != nullptr) {
        locale.set_Unicode_Keyword_মান(চাবি, nullptr, errorCode);
    }
}

শূন্য
_setUnicodeExtensions(Locale& locale, ধ্রুবক CharString& value, UErrorCode& errorCode)
{
    যদি (U_FAILURE(errorCode)) { ফেরত; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", errorCode);
    locale_str.append(value, errorCode);
    _copyExtensions(
        Locale::forLanguageTag(locale_str.তথ্য(), errorCode), nullptr,
        locale, মিথ্যা, errorCode);
}

}  // namespace

LocaleBuilder& LocaleBuilder::setExtension(অক্ষর চাবি, StringPiece value) U_LIFETIME_BOUND
{
    যদি (U_FAILURE(status_)) { ফেরত *এই; }
    যদি (!UPRV_ISALPHANUM(চাবি)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত *এই;
    }
    CharString মান_str(value, status_);
    যদি (U_FAILURE(status_)) { ফেরত *এই; }
    transform(মান_str.তথ্য(), মান_str.length());
    যদি (!মান_str.isEmpty() &&
            !_isExtensionSubtags(চাবি, মান_str.তথ্য(), মান_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত *এই;
    }
    যদি (extensions_ == nullptr) {
        extensions_ = Locale::get_মূল().clone();
        যদি (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ফেরত *এই;
        }
    }
    যদি (uprv_tolower(চাবি) != 'u') {
        // for t, x and others extension.
        extensions_->set_Keyword_মান(StringPiece(&চাবি, 1), মান_str.তথ্য(),
                                     status_);
        ফেরত *এই;
    }
    clear_UAttributes_And_চাবি_Type(*extensions_, status_);
    যদি (U_FAILURE(status_)) { ফেরত *এই; }
    যদি (!value.empty()) {
        _setUnicodeExtensions(*extensions_, মান_str, status_);
    }
    ফেরত *এই;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece চাবি,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    যদি (U_FAILURE(status_)) { ফেরত *এই; }
    যদি (!ultag_is_Unicode_Locale_চাবি(চাবি.তথ্য(), চাবি.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.তথ্য(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      ফেরত *এই;
    }
    যদি (extensions_ == nullptr) {
        extensions_ = Locale::get_মূল().clone();
        যদি (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ফেরত *এই;
        }
    }
    extensions_->set_Unicode_Keyword_মান(চাবি, type, status_);
    ফেরত *এই;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString মান_str(value, status_);
    যদি (U_FAILURE(status_)) { ফেরত *এই; }
    transform(মান_str.তথ্য(), মান_str.length());
    যদি (!ultag_isUnicodeLocaleAttribute(মান_str.তথ্য(), মান_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত *এই;
    }
    যদি (extensions_ == nullptr) {
        extensions_ = Locale::get_মূল().clone();
        যদি (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ফেরত *এই;
        }
        extensions_->set_Keyword_মান(k_Attribute_চাবি, মান_str.তথ্য(), status_);
        ফেরত *এই;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_মান<CharString>(k_Attribute_চাবি, localErrorCode);
    যদি (U_FAILURE(localErrorCode)) {
        CharString new_attributes(মান_str.তথ্য(), status_);
        // No attributes, set the attribute.
        extensions_->set_Keyword_মান(k_Attribute_চাবি, new_attributes.তথ্য(), status_);
        ফেরত *এই;
    }

    transform(attributes.তথ্য(),attributes.length());
    ধ্রুবক অক্ষর* start = attributes.তথ্য();
    ধ্রুবক অক্ষর* limit = attributes.তথ্য() + attributes.length();
    CharString new_attributes;
    যৌক্তিক inserted = মিথ্যা;
    যখন (start < limit) {
        যদি (!inserted) {
            পূর্ণসংখ্যা cmp = uprv_strcmp(start, মান_str.তথ্য());
            যদি (cmp == 0) { ফেরত *এই; }  // Found it in attributes: Just return
            যদি (cmp > 0) {
                যদি (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(মান_str.তথ্য(), status_);
                inserted = সত্য;
            }
        }
        যদি (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    যদি (!inserted) {
        যদি (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(মান_str.তথ্য(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->set_Keyword_মান(k_Attribute_চাবি, new_attributes.তথ্য(), status_);
    ফেরত *এই;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString মান_str(value, status_);
    যদি (U_FAILURE(status_)) { ফেরত *এই; }
    transform(মান_str.তথ্য(), মান_str.length());
    যদি (!ultag_isUnicodeLocaleAttribute(মান_str.তথ্য(), মান_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত *এই;
    }
    যদি (extensions_ == nullptr) { ফেরত *এই; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_মান<CharString>(k_Attribute_চাবি, localErrorCode);
    // get failure, just return
    যদি (U_FAILURE(localErrorCode)) { ফেরত *এই; }
    // Do not have any attributes, just return.
    যদি (attributes.isEmpty()) { ফেরত *এই; }

    অক্ষর* p = attributes.তথ্য();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    জন্য (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    ধ্রুবক অক্ষর* start = attributes.তথ্য();
    ধ্রুবক অক্ষর* limit = attributes.তথ্য() + attributes.length();
    CharString new_attributes;
    যৌক্তিক found = মিথ্যা;
    যখন (start < limit) {
        যদি (uprv_strcmp(start, মান_str.তথ্য()) == 0) {
            found = সত্য;
        } অন্যথা {
            যদি (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    যদি (found) {
        extensions_->set_Keyword_মান(k_Attribute_চাবি, new_attributes.তথ্য(), status_);
    }
    ফেরত *এই;
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
    ফেরত *এই;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    ফেরত *এই;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  ফেরত bogus;
}

শূন্য LocaleBuilder::copyExtensionsFrom(ধ্রুবক Locale& src, UErrorCode& errorCode)
{
    যদি (U_FAILURE(errorCode)) { ফেরত; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    যদি (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        ফেরত;
    }
    যদি (extensions_ == nullptr) {
        extensions_ = Locale::get_মূল().clone();
        যদি (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ফেরত;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, মিথ্যা, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    যদি (U_FAILURE(errorCode)) {
        ফেরত makeBogusLocale();
    }
    যদি (U_FAILURE(status_)) {
        errorCode = status_;
        ফেরত makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    যদি (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    যদি (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    যদি (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->তথ্য()), errorCode);
    }
    যদি (U_FAILURE(errorCode)) {
        ফেরত makeBogusLocale();
    }
    Locale product(locale_str.তথ্য());
    যদি (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, সত্য, errorCode);
    }
    যদি (U_FAILURE(errorCode)) {
        ফেরত makeBogusLocale();
    }
    ফেরত product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) ধ্রুবক {
    যদি (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        ফেরত সত্য;
    }
    outErrorCode = status_;
    ফেরত U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
