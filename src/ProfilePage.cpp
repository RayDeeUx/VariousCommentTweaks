#include <Geode/modify/ProfilePage.hpp>
#include <Geode/modify/InfoLayer.hpp>
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

class $modify(MyProfilePage, ProfilePage) {
	void loadPageFromUserInfo(GJUserScore* userInfo) {
		Manager* manager = Manager::getSharedInstance();
		manager->doNotHighlight = m_ownProfile || Utils::contains<int>(manager->friends, userInfo->m_accountID);
		ProfilePage::loadPageFromUserInfo(userInfo);
	}
};

class $modify(MyInfoLayer, InfoLayer) {
	void loadPage(int p0, bool p1) {
		Manager* manager = Manager::getSharedInstance();
		if (m_score) manager->doNotHighlight = Utils::contains<int>(manager->friends, m_score->m_accountID);
		else manager->doNotHighlight = false;
		InfoLayer::loadPage(p0, p1);
	}
};