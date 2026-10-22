// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ضمن <optional>
#ضمن <string_view>
#ضمن <utility>

#ضمن "bytesinkutil.h"  // StringByteSink<CharString>
#ضمن "charstr.h"
#ضمن "cstring.h"
#ضمن "fixedstring.h"
#ضمن "ulocimp.h"
#ضمن "unicode/localebuilder.h"
#ضمن "unicode/locid.h"

نطاق {

مضمن منطقي UPRV_ISDIGIT(حرف c) { أرجع c >= '0' && c <= '9'; }
مضمن منطقي UPRV_ISALPHANUM(حرف c) { أرجع uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr ثابت حرف* k_Attribute_کلی = "attribute";

منطقي _isExtensionSubtags(حرف کلی, ثابت حرف* s, int32_t len) {
    بدل (uprv_tolower(کلی)) {
        حالة 'u':
            أرجع ultag_isUnicodeExtensionSubtags(s, len);
        حالة 't':
            أرجع ultag_isTransformedExtensionSubtags(s, len);
        حالة 'x':
            أرجع ultag_is_Privateuse_ارزښت_Subtags(s, len);
        افتراضي:
            أرجع ultag_isExtensionSubtags(s, len);
    }
}

}  // namespace

U_NAMESPACE_BEGIN

LocaleBuilder::LocaleBuilder() : UObject(), حالت_2(U_ZERO_ERROR), language_(),
    script_(), region_(), variant_(تش_اشاره), extensions_(تش_اشاره)
{
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
}

LocaleBuilder::~LocaleBuilder()
{
    ړنګول variant_;
    ړنګول extensions_;
}

LocaleBuilder& LocaleBuilder::سېټ_Locale(ثابت Locale& locale) U_LIFETIME_BOUND
{
    clear();
    سېټ_Language(locale.getLanguage());
    سېټ_Script(locale.getScript());
    سېټ_Region(locale.getCountry());
    سېټ_Variant(locale.getVariant());
    extensions_ = locale.clone();
    إذا (extensions_ == تش_اشاره) {
        حالت_2 = U_حافظه_ویش_ERROR;
    }
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::سېټ_Language_نښان(StringPiece نښان) U_LIFETIME_BOUND
{
    Locale l = Locale::for_Language_نښان(نښان, حالت_2);
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    سېټ_Locale(l);
    أرجع *هذا;
}

نطاق {

فراغ سېټ_ډګر(StringPiece input, حرف* dest, UError_کوډ& error_کوډ,
              منطقي (*test)(ثابت حرف*, int32_t)) {
    إذا (U_FAILURE(error_کوډ)) { أرجع; }
    إذا (input.empty()) {
        dest[0] = '\0';
    } وإلا إذا (test(input.معلومات(), input.length())) {
        uprv_memcpy(dest, input.معلومات(), input.length());
        dest[input.length()] = '\0';
    } وإلا {
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::سېټ_Language(StringPiece language) U_LIFETIME_BOUND
{
    سېټ_ډګر(language, language_, حالت_2, &ultag_isLanguageSubtag);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::سېټ_Script(StringPiece script) U_LIFETIME_BOUND
{
    سېټ_ډګر(script, script_, حالت_2, &ultag_isScriptSubtag);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::سېټ_Region(StringPiece region) U_LIFETIME_BOUND
{
    سېټ_ډګر(region, region_, حالت_2, &ultag_isRegionSubtag);
    أرجع *هذا;
}

نطاق {

فراغ transform(حرف* معلومات, int32_t len) {
    لكل (int32_t i = 0; i < len; i++, معلومات++) {
        إذا (*معلومات == '_') {
            *معلومات = '-';
        } وإلا {
            *معلومات = uprv_tolower(*معلومات);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::سېټ_Variant(StringPiece variant) U_LIFETIME_BOUND
{
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    إذا (variant.empty()) {
        ړنګول variant_;
        variant_ = تش_اشاره;
        أرجع *هذا;
    }
    FixedString* new_variant = جديد FixedString(variant);
    إذا (new_variant == تش_اشاره || new_variant->is_تشه()) {
        حالت_2 = U_حافظه_ویش_ERROR;
        أرجع *هذا;
    }
    transform(new_variant->getAlias(), variant.length());
    إذا (!ultag_isVariantSubtags(new_variant->معلومات(), variant.length())) {
        ړنګول new_variant;
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        أرجع *هذا;
    }
    ړنګول variant_;
    variant_ = new_variant;
    أرجع *هذا;
}

نطاق {

منطقي
is_Keyword_ارزښت(ثابت حرف* کلی, ثابت حرف* value, int32_t ارزښت_len)
{
    إذا (کلی[1] == '\0') {
        // one char key
        أرجع (UPRV_ISALPHANUM(uprv_tolower(کلی[0])) &&
                _isExtensionSubtags(کلی[0], value, ارزښت_len));
    } وإلا إذا (uprv_strcmp(کلی, k_Attribute_کلی) == 0) {
        // unicode attributes
        أرجع ultag_is_Unicode_Locale_ځانګړتیاوې(value, ارزښت_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_کلی = ulocimp_منزل_Bcp_کلی_With_Fallback(کلی);
    std::optional<std::string_view> unicode_locale_type = ulocimp_منزل_Bcp_Type_With_Fallback(کلی, value);

    أرجع unicode_locale_کلی.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_کلی(unicode_locale_کلی->معلومات(),
                                    static_cast<int32_t>(unicode_locale_کلی->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->معلومات(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

فراغ
_copyExtensions(ثابت Locale& from, icu::StringEnumeration *keywords,
                Locale& منزل, منطقي validate, UError_کوډ& error_کوډ)
{
    إذا (U_FAILURE(error_کوډ)) { أرجع; }
    Local_اشاره_ګر<icu::StringEnumeration> ownedKeywords;
    إذا (keywords == تش_اشاره) {
        ownedKeywords.adoptInstead(from.createKeywords(error_کوډ));
        إذا (U_FAILURE(error_کوډ) || ownedKeywords.isNull()) { أرجع; }
        keywords = ownedKeywords.getAlias();
    }
    ثابت حرف* کلی;
    طالما ((کلی = keywords->next(تش_اشاره, error_کوډ)) != تش_اشاره) {
        خپلکاری value = from.get_Keyword_ارزښت<CharString>(کلی, error_کوډ);
        إذا (U_FAILURE(error_کوډ)) { أرجع; }
        إذا (uprv_strcmp(کلی, k_Attribute_کلی) == 0) {
            transform(value.معلومات(), value.length());
        }
        إذا (validate &&
            !is_Keyword_ارزښت(کلی, value.معلومات(), value.length())) {
            error_کوډ = U_ILLEGAL_دلیل_ERROR;
            أرجع;
        }
        منزل.سېټ_Keyword_ارزښت(کلی, value.معلومات(), error_کوډ);
        إذا (U_FAILURE(error_کوډ)) { أرجع; }
    }
}

فراغ
clear_UAttributes_And_کلی_Type(Locale& locale, UError_کوډ& error_کوډ)
{
    إذا (U_FAILURE(error_کوډ)) { أرجع; }
    // Clear Unicode attributes
    locale.سېټ_Keyword_ارزښت(k_Attribute_کلی, "", error_کوډ);

    // Clear all Unicode keyword values
    Local_اشاره_ګر<icu::StringEnumeration> iter(locale.createUnicodeKeywords(error_کوډ));
    إذا (U_FAILURE(error_کوډ) || iter.isNull()) { أرجع; }
    ثابت حرف* کلی;
    طالما ((کلی = iter->next(تش_اشاره, error_کوډ)) != تش_اشاره) {
        locale.سېټ_Unicode_Keyword_ارزښت(کلی, تش_اشاره, error_کوډ);
    }
}

فراغ
سېټ_Unicode_Extensions(Locale& locale, ثابت CharString& value, UError_کوډ& error_کوډ)
{
    إذا (U_FAILURE(error_کوډ)) { أرجع; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", error_کوډ);
    locale_str.append(value, error_کوډ);
    _copyExtensions(
        Locale::for_Language_نښان(locale_str.معلومات(), error_کوډ), تش_اشاره,
        locale, خطأ, error_کوډ);
}

}  // namespace

LocaleBuilder& LocaleBuilder::سېټ_Extension(حرف کلی, StringPiece value) U_LIFETIME_BOUND
{
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    إذا (!UPRV_ISALPHANUM(کلی)) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        أرجع *هذا;
    }
    CharString ارزښت_str(value, حالت_2);
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    transform(ارزښت_str.معلومات(), ارزښت_str.length());
    إذا (!ارزښت_str.is_تشه() &&
            !_isExtensionSubtags(کلی, ارزښت_str.معلومات(), ارزښت_str.length())) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        أرجع *هذا;
    }
    إذا (extensions_ == تش_اشاره) {
        extensions_ = Locale::get_ریښه().clone();
        إذا (extensions_ == تش_اشاره) {
            حالت_2 = U_حافظه_ویش_ERROR;
            أرجع *هذا;
        }
    }
    إذا (uprv_tolower(کلی) != 'u') {
        // for t, x and others extension.
        extensions_->سېټ_Keyword_ارزښت(StringPiece(&کلی, 1), ارزښت_str.معلومات(),
                                     حالت_2);
        أرجع *هذا;
    }
    clear_UAttributes_And_کلی_Type(*extensions_, حالت_2);
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    إذا (!value.empty()) {
        سېټ_Unicode_Extensions(*extensions_, ارزښت_str, حالت_2);
    }
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::سېټ_Unicode_Locale_Keyword(StringPiece کلی,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    إذا (!ultag_is_Unicode_Locale_کلی(کلی.معلومات(), کلی.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.معلومات(), type.length()))) {
      حالت_2 = U_ILLEGAL_دلیل_ERROR;
      أرجع *هذا;
    }
    إذا (extensions_ == تش_اشاره) {
        extensions_ = Locale::get_ریښه().clone();
        إذا (extensions_ == تش_اشاره) {
            حالت_2 = U_حافظه_ویش_ERROR;
            أرجع *هذا;
        }
    }
    extensions_->سېټ_Unicode_Keyword_ارزښت(کلی, type, حالت_2);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::جمع_کړه_Unicode_Locale_Attribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString ارزښت_str(value, حالت_2);
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    transform(ارزښت_str.معلومات(), ارزښت_str.length());
    إذا (!ultag_isUnicodeLocaleAttribute(ارزښت_str.معلومات(), ارزښت_str.length())) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        أرجع *هذا;
    }
    إذا (extensions_ == تش_اشاره) {
        extensions_ = Locale::get_ریښه().clone();
        إذا (extensions_ == تش_اشاره) {
            حالت_2 = U_حافظه_ویش_ERROR;
            أرجع *هذا;
        }
        extensions_->سېټ_Keyword_ارزښت(k_Attribute_کلی, ارزښت_str.معلومات(), حالت_2);
        أرجع *هذا;
    }

    UError_کوډ local_Error_کوډ = U_ZERO_ERROR;
    خپلکاری ځانګړتیاوې = extensions_->get_Keyword_ارزښت<CharString>(k_Attribute_کلی, local_Error_کوډ);
    إذا (U_FAILURE(local_Error_کوډ)) {
        CharString new_ځانګړتیاوې(ارزښت_str.معلومات(), حالت_2);
        // No attributes, set the attribute.
        extensions_->سېټ_Keyword_ارزښت(k_Attribute_کلی, new_ځانګړتیاوې.معلومات(), حالت_2);
        أرجع *هذا;
    }

    transform(ځانګړتیاوې.معلومات(),ځانګړتیاوې.length());
    ثابت حرف* پیل = ځانګړتیاوې.معلومات();
    ثابت حرف* حد = ځانګړتیاوې.معلومات() + ځانګړتیاوې.length();
    CharString new_ځانګړتیاوې;
    منطقي inserted = خطأ;
    طالما (پیل < حد) {
        إذا (!inserted) {
            صحيح cmp = uprv_strcmp(پیل, ارزښت_str.معلومات());
            إذا (cmp == 0) { أرجع *هذا; }  // Found it in attributes: Just return
            إذا (cmp > 0) {
                إذا (!new_ځانګړتیاوې.is_تشه()) new_ځانګړتیاوې.append('_', حالت_2);
                new_ځانګړتیاوې.append(ارزښت_str.معلومات(), حالت_2);
                inserted = ریښتیا;
            }
        }
        إذا (!new_ځانګړتیاوې.is_تشه()) {
            new_ځانګړتیاوې.append('_', حالت_2);
        }
        new_ځانګړتیاوې.append(پیل, حالت_2);
        پیل += uprv_strlen(پیل) + 1;
    }
    إذا (!inserted) {
        إذا (!new_ځانګړتیاوې.is_تشه()) {
            new_ځانګړتیاوې.append('_', حالت_2);
        }
        new_ځانګړتیاوې.append(ارزښت_str.معلومات(), حالت_2);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->سېټ_Keyword_ارزښت(k_Attribute_کلی, new_ځانګړتیاوې.معلومات(), حالت_2);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString ارزښت_str(value, حالت_2);
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    transform(ارزښت_str.معلومات(), ارزښت_str.length());
    إذا (!ultag_isUnicodeLocaleAttribute(ارزښت_str.معلومات(), ارزښت_str.length())) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        أرجع *هذا;
    }
    إذا (extensions_ == تش_اشاره) { أرجع *هذا; }
    UError_کوډ local_Error_کوډ = U_ZERO_ERROR;
    خپلکاری ځانګړتیاوې = extensions_->get_Keyword_ارزښت<CharString>(k_Attribute_کلی, local_Error_کوډ);
    // get failure, just return
    إذا (U_FAILURE(local_Error_کوډ)) { أرجع *هذا; }
    // Do not have any attributes, just return.
    إذا (ځانګړتیاوې.is_تشه()) { أرجع *هذا; }

    حرف* p = ځانګړتیاوې.معلومات();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    لكل (int32_t i = 0; i < ځانګړتیاوې.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    ثابت حرف* پیل = ځانګړتیاوې.معلومات();
    ثابت حرف* حد = ځانګړتیاوې.معلومات() + ځانګړتیاوې.length();
    CharString new_ځانګړتیاوې;
    منطقي وموندل = خطأ;
    طالما (پیل < حد) {
        إذا (uprv_strcmp(پیل, ارزښت_str.معلومات()) == 0) {
            وموندل = ریښتیا;
        } وإلا {
            إذا (!new_ځانګړتیاوې.is_تشه()) {
                new_ځانګړتیاوې.append('_', حالت_2);
            }
            new_ځانګړتیاوې.append(پیل, حالت_2);
        }
        پیل += uprv_strlen(پیل) + 1;
    }
    // Found the value in attributes, set the attribute.
    إذا (وموندل) {
        extensions_->سېټ_Keyword_ارزښت(k_Attribute_کلی, new_ځانګړتیاوې.معلومات(), حالت_2);
    }
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::clear() U_LIFETIME_BOUND
{
    حالت_2 = U_ZERO_ERROR;
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
    ړنګول variant_;
    variant_ = تش_اشاره;
    clearExtensions();
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    ړنګول extensions_;
    extensions_ = تش_اشاره;
    أرجع *هذا;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.سېټ_منزل_Bogus();
  أرجع bogus;
}

فراغ LocaleBuilder::copyExtensionsFrom(ثابت Locale& src, UError_کوډ& error_کوډ)
{
    إذا (U_FAILURE(error_کوډ)) { أرجع; }
    Local_اشاره_ګر<icu::StringEnumeration> keywords(src.createKeywords(error_کوډ));
    إذا (U_FAILURE(error_کوډ) || keywords.isNull() || keywords->count(error_کوډ) == 0) {
        // Error, or no extensions to copy.
        أرجع;
    }
    إذا (extensions_ == تش_اشاره) {
        extensions_ = Locale::get_ریښه().clone();
        إذا (extensions_ == تش_اشاره) {
            حالت_2 = U_حافظه_ویش_ERROR;
            أرجع;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, خطأ, error_کوډ);
}

Locale LocaleBuilder::build(UError_کوډ& error_کوډ)
{
    إذا (U_FAILURE(error_کوډ)) {
        أرجع makeBogusLocale();
    }
    إذا (U_FAILURE(حالت_2)) {
        error_کوډ = حالت_2;
        أرجع makeBogusLocale();
    }
    CharString locale_str(language_, error_کوډ);
    إذا (uprv_strlen(script_) > 0) {
        locale_str.append('-', error_کوډ).append(StringPiece(script_), error_کوډ);
    }
    إذا (uprv_strlen(region_) > 0) {
        locale_str.append('-', error_کوډ).append(StringPiece(region_), error_کوډ);
    }
    إذا (variant_ != تش_اشاره) {
        locale_str.append('-', error_کوډ).append(StringPiece(variant_->معلومات()), error_کوډ);
    }
    إذا (U_FAILURE(error_کوډ)) {
        أرجع makeBogusLocale();
    }
    Locale حاصل_ضرب(locale_str.معلومات());
    إذا (extensions_ != تش_اشاره) {
        _copyExtensions(*extensions_, تش_اشاره, حاصل_ضرب, ریښتیا, error_کوډ);
    }
    إذا (U_FAILURE(error_کوډ)) {
        أرجع makeBogusLocale();
    }
    أرجع حاصل_ضرب;
}

UBool LocaleBuilder::copy_Error_منزل(UError_کوډ &بهر_Error_کوډ) ثابت {
    إذا (U_FAILURE(بهر_Error_کوډ)) {
        // Do not overwrite the older error code
        أرجع ریښتیا;
    }
    بهر_Error_کوډ = حالت_2;
    أرجع U_FAILURE(بهر_Error_کوډ);
}

U_NAMESPACE_پای_ټکی
