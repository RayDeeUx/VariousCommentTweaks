#include <Geode/modify/InfoLayer.hpp>
#include "Manager.hpp"

using namespace geode::prelude;

class $modify(MyInfoLayer, InfoLayer) {
	void setupCommentsBrowser(cocos2d::CCArray* p0) {
		Manager* manager = Manager::getSharedInstance();
		if (m_score) manager->doNotHighlight = true;
		else manager->doNotHighlight = false;
		InfoLayer::setupCommentsBrowser(p0);
	}
};