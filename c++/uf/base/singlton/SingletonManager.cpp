//
//  SingletonManager.cpp
//  ProjectM
//
//  Created by ruby on 13-4-17.
//
//

#include "SingletonManager.h"
#include "Singleton.h"


namespace Base {
    
    SingletonManager* SingletonManager::singletonInstance_ = NULL;
    SingletonManager::SingletonManager() {
        singletonInstance_ = this;

    }
    
    SingletonManager::~SingletonManager() {

		destroySingleton();
    }
    
    SingletonManager* SingletonManager::sharedInstance() {
		if (!singletonInstance_){
			singletonInstance_ = new SingletonManager();
		}
        return singletonInstance_;
    }
   
// 	void SingletonManager::pugreInstance() {
// 		if (singletonInstance_ != NULL) {
// 			singletonInstance_->destroySingleton();
// 			delete singletonInstance_;
// 			singletonInstance_ = NULL;
// 		}
// 	}

    void SingletonManager::addSingleton(Base::SingletonDelegate *s) {
        if (s == NULL)
            return ;
        
        //s->retain();
        singletonList_.push_back(s);
    }
    
    void SingletonManager::destroySingleton() {
        for (SingletonList::iterator iter = singletonList_.begin();
             iter != singletonList_.end(); ++iter) {
            if ((*iter) != NULL) {
				(*iter)->unInit();
                delete (*iter);
            }
        }
        
        singletonList_.clear();
    }

	void SingletonManager::doCocos2dExit() {
		for (SingletonList::iterator iter = singletonList_.begin();
			iter != singletonList_.end(); ++iter) {
				if ((*iter) != NULL) {
					(*iter)->onCocos2dExit();
				}
		}
	}

	void SingletonManager::onCocos2dExit() {
		doCocos2dExit();
		
	}
}
