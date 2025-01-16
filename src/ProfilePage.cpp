#include <Geode/modify/ProfilePage.hpp>
#include <Geode/modify/InfoLayer.hpp>
#include "Manager.hpp"

using namespace geode::prelude;

class $modify(MyProfilePage, ProfilePage) {
	void loadPageFromUserInfo(GJUserScore* p0) {
		Manager* manager = Manager::getSharedInstance();
		manager->doNotHighlight = true;
		ProfilePage::loadPageFromUserInfo(p0);
	}
};

class $modify(MyInfoLayer, InfoLayer) {
	bool init(GJGameLevel *p0, GJUserScore *p1, GJLevelList *p2) {
		Manager* manager = Manager::getSharedInstance();
		if (p1) manager->doNotHighlight = true;
		else manager->doNotHighlight = false;
		InfoLayer::init(p0, p1, p2);
	}
};