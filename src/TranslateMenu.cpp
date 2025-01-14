#include <Geode/ui/GeodeUI.hpp>
#include <Geode/utils/web.hpp>
#include "TranslateMenu.hpp"
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

TranslateMenu* TranslateMenu::create(const std::string& text) {
	auto *ret = new TranslateMenu();
	if (ret && ret->init(text)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool TranslateMenu::init(const std::string& text) {
	std::string sourceLanguage = Utils::getString("sourceLanguage");
	if (string::toLower(sourceLanguage) == "detect language") sourceLanguage = "Auto";
	if (!FLAlertLayer::init(nullptr, fmt::format("From: {} • To: {}", sourceLanguage, Utils::getString("targetLanguage")).c_str(),
		text, "foo", nullptr, 420.f, false, 300.f, .69f)) return false;
	this->setID("TranslateMenu");
	this->m_button1->getParent()->removeMeAndCleanup(); // the parent is a CCMenuItemSpriteExtra, calm down
	this->m_buttonMenu->setContentWidth(390.f);
	static_cast<CCLabelBMFont*>(this->m_mainLayer->getChildByID("title"))->limitLabelWidth(390.f, .9f, 0.001f);
	this->urlEncoded = TranslateMenu::encodeToURL(text);

	ButtonSprite* libreTranslate = ButtonSprite::create("    LibreTranslate", "goldFont.fnt", "GJ_button_01.png", 0.8f);
	CCMenuItemSpriteExtra* libreTranslateButton = CCMenuItemSpriteExtra::create(libreTranslate, this, menu_selector(TranslateMenu::onLibreTranslate));
	CCSprite* libreTranslateLogo = CCSprite::createWithSpriteFrameName("libreTranslate.png"_spr);
	libreTranslateLogo->setScale(1.8f);
	libreTranslateLogo->setID("libre-translate-logo"_spr);
	libreTranslate->addChild(libreTranslateLogo);
	libreTranslateLogo->setPosition({24, 15});
	libreTranslateButton->setID("libre-translate-button"_spr);

	ButtonSprite* deeplTranslate = ButtonSprite::create("    DeepL", "goldFont.fnt", "GJ_button_01.png", 0.8f);
	CCMenuItemSpriteExtra* deeplTranslateButton = CCMenuItemSpriteExtra::create(deeplTranslate, this, menu_selector(TranslateMenu::onDeepLTranslate));
	CCSprite* deeplTranslateLogo = CCSprite::createWithSpriteFrameName("deeplTranslate.png"_spr);
	deeplTranslateLogo->setScale(1.3f);
	deeplTranslateLogo->setID("deepl-translate-logo"_spr);
	deeplTranslate->addChild(deeplTranslateLogo);
	deeplTranslateLogo->setPosition({24, 15});
	deeplTranslateButton->setID("deepl-translate-button"_spr);

	ButtonSprite* boogleTranslate = ButtonSprite::create("    Google", "goldFont.fnt", "GJ_button_01.png", 0.8f);
	CCMenuItemSpriteExtra* boogleTranslateButton = CCMenuItemSpriteExtra::create(boogleTranslate, this, menu_selector(TranslateMenu::onBoogleTranslate));
	CCSprite* boogleTranslateLogo = CCSprite::createWithSpriteFrameName("boogleTranslate.png"_spr);
	boogleTranslateLogo->setScale(1.3f);
	boogleTranslateLogo->setID("boogle-translate-logo"_spr);
	boogleTranslate->addChild(boogleTranslateLogo);
	boogleTranslateLogo->setPosition({24, 15});
	boogleTranslateButton->setID("boogle-translate-button"_spr);

	CCSprite* closeButtonSprite = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
	CCMenuItemSpriteExtra* closeButton = CCMenuItemSpriteExtra::create(closeButtonSprite, this, menu_selector(TranslateMenu::onCloseTranslateMenu));
	closeButton->setID("close-button"_spr);

	CCSprite* modSettingsButtonSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
	CCMenuItemSpriteExtra* modSettingsButton = CCMenuItemSpriteExtra::create(modSettingsButtonSprite, this, menu_selector(TranslateMenu::onOpenModSettings));
	modSettingsButton->setID("mod-setting-button"_spr);

	this->m_buttonMenu->addChild(closeButton);
	this->m_buttonMenu->addChild(libreTranslateButton);
	this->m_buttonMenu->addChild(deeplTranslateButton);
	this->m_buttonMenu->addChild(boogleTranslateButton);
	this->m_buttonMenu->addChild(modSettingsButton);
	this->m_buttonMenu->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Center)->setAutoScale(true));
	// yes, jeffery, i need all five buttons in the menu leave me alone :(

	return true;
}

void TranslateMenu::onOpenModSettings(CCObject*) {
	openSettingsPopup(Mod::get());
}

void TranslateMenu::onCloseTranslateMenu(CCObject*) {
	this->keyBackClicked();
}

void TranslateMenu::onBoogleTranslate(CCObject*) {
	if (!Utils::modEnabled() || !Utils::getBool("translateComments")) return;
	this->keyBackClicked();
	const auto languagesMap = Manager::getSharedInstance()->languages;
	const std::string& sourceLangSetting = Utils::getString("sourceLanguage");
	const std::string& targetLangSetting = Utils::getString("targetLanguage");
	const std::string& sourceLang = languagesMap.contains(sourceLangSetting)
	? languagesMap.find(sourceLangSetting)->second : "auto";
	const std::string& targetLang = languagesMap.contains(targetLangSetting)
		? languagesMap.find(targetLangSetting)->second : "en";
	const std::string& url = fmt::format("https://translate.google.com/?sl={}&tl={}&text={}", sourceLang, targetLang, urlEncoded);
	const auto dispatcher = CCKeyboardDispatcher::get();
	if (dispatcher->getShiftKeyPressed() && dispatcher->getAltKeyPressed()) geode::utils::clipboard::write(url);
	else geode::utils::web::openLinkInBrowser(url);
}

void TranslateMenu::onDeepLTranslate(CCObject*) {
	if (!Utils::modEnabled() || !Utils::getBool("translateComments")) return;
	this->keyBackClicked();
	const auto languagesMap = Manager::getSharedInstance()->deeplLanguages;
	const std::string& sourceLangSetting = Utils::getString("sourceLanguage");
	const std::string& targetLangSetting = Utils::getString("targetLanguage");
	std::string sourceLang = languagesMap.contains(sourceLangSetting)
		? languagesMap.find(sourceLangSetting)->second : "en";
	if (string::startsWith(sourceLang, "zh-")) sourceLang = "zh"; // 中文 compat, for some reason all chinese is treated as "zh" when used as a source language
	else if (sourceLangSetting == "English") sourceLang = "es"; // trick deepl into opening properly
	const std::string& targetLang = languagesMap.contains(targetLangSetting)
		? languagesMap.find(targetLangSetting)->second : "en";
	geode::utils::web::openLinkInBrowser(fmt::format("https://www.deepl.com/en/translator#{}/{}/{}", targetLang, sourceLang, urlEncoded));
}

void TranslateMenu::onLibreTranslate(CCObject*) {
	if (!Utils::modEnabled() || !Utils::getBool("translateComments")) return;
	this->keyBackClicked();
	const auto languagesMap = Manager::getSharedInstance()->languages;
	const std::string& sourceLangSetting = Utils::getString("sourceLanguage");
	const std::string& targetLangSetting = Utils::getString("targetLanguage");
	const std::string& sourceLang = languagesMap.contains(sourceLangSetting)
	? languagesMap.find(sourceLangSetting)->second : "auto";
	const std::string& targetLang = languagesMap.contains(targetLangSetting)
		? languagesMap.find(targetLangSetting)->second : "en";
	geode::utils::web::openLinkInBrowser(fmt::format("https://libretranslate.com/?source={}&target={}&q={}", sourceLang, targetLang, urlEncoded));
}

std::string TranslateMenu::encodeToURL(const std::string& text) {
	// adapted from tastyforreal
	// then edited for consistency
	// should work in most cases w/o "%20" or '+' collisions
	std::stringstream stream;
	for (const char c : utils::string::split(text)) {
		if (c == ' ') stream << '%' << '2' << '0';
		else if (c == '+' || c == '!' || c == '*' || c == '(' || c == ')' || c == '-' || c == '.' || c == '_' || isalnum(static_cast<unsigned char>(c))) stream << c;
		else stream << '%' << std::hex << (static_cast<unsigned int>(c) & 0xFF);
	}
	return stream.str();
}