#pragma once

class TranslateMenu final : public geode::Popup {
protected:
	void onOpenModSettings(cocos2d::CCObject*);
public:
	static TranslateMenu* create(const std::string&);
	bool init(const std::string& text) override;
	static std::pair<std::string, std::string> findLanguageCodes(const std::unordered_map<std::string, std::string>& languagesMap, const bool isDeepL = false);
	static void encodeToURL(const std::string&);
	void onBoogleTranslate(cocos2d::CCObject*);
	void onLibreTranslate(cocos2d::CCObject*);
	void onDeepLTranslate(cocos2d::CCObject*);
};