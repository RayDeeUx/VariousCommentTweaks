#include <Geode/modify/MenuLayer.hpp>
#include "Settings.hpp"
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
	(void) Mod::get()->registerCustomSettingType("configdir", &MyButtonSettingV3::parse);
	(void) Mod::get()->registerCustomSettingType("updatelists", &MyButtonSettingV3::parse);
	// (void) Mod::get()->registerCustomSettingType("likebaitexterminator", &MyButtonSettingV3::parse);
	(void) Utils::updateLists();
	Mod::get()->setLoggingEnabled(Utils::getBool("logging"));
	listenForSettingChanges("sourceLanguage", [](std::string sourceLanguage) {
		if (sourceLanguage == "Detect language") sourceLanguage = "Auto";
		std::string otherLanguage = Utils::getString("targetLanguage");
		if (otherLanguage == "Detect language") otherLanguage = "Auto";
		const auto title = typeinfo_cast<CCLabelBMFont*>(CCScene::get()->querySelector("TranslateMenu > main-layer > title"));
		if (!title) return;
		title->setString(fmt::format("From: {} • To: {}", sourceLanguage, otherLanguage).c_str());
		title->limitLabelWidth(390.f, .9f, 0.001f);
	});
	listenForSettingChanges("targetLanguage", [](std::string targetLanguage) {
		if (targetLanguage == "Detect language") targetLanguage = "Auto";
		std::string otherLanguage = Utils::getString("sourceLanguage");
		if (otherLanguage == "Detect language") otherLanguage = "Auto";
		const auto title = typeinfo_cast<CCLabelBMFont*>(CCScene::get()->querySelector("TranslateMenu > main-layer > title"));
		if (!title) return;
		title->setString(fmt::format("From: {} • To: {}", otherLanguage, targetLanguage).c_str());
		title->limitLabelWidth(390.f, .9f, 0.001f);
	});
	listenForSettingChanges("rescaleLargeComments", [](bool rescaleLargeComments) {
		if (!rescaleLargeComments) return;
		FLAlertLayer::create("Heads up!", "By changing this setting, you are aware that <c_>comment text may not fit inside its comment box</c>.", "I Understand")->show();
	});
	listenForSettingChanges("largeRescaleFactor", [](double largeRescaleFactor) {
		if (largeRescaleFactor <= 1.0) return;
		FLAlertLayer::create("Heads up!", "By changing this setting, you are aware that <c_>comment text may not fit inside its comment box</c>.", "I Understand")->show();
	});
	listenForSettingChanges("logging", [](bool logging){
		Mod::get()->setLoggingEnabled(logging);
	});
}

class $modify(MyMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;

		if (Manager::getSharedInstance()->calledAlready) return true;
		const auto manager = Manager::getSharedInstance();
		manager->calledAlready = true;

		manager->ownUsername = utils::string::toLower(GameManager::get()->m_playerName);

		return true;
	}
};