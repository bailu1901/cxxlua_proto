//
//  Singleton.h
//  ProjectM
//
//  Created by ruby on 13-4-17.
//
//

#ifndef ProjectM_Singleton_h
#define ProjectM_Singleton_h
#include "SingletonManager.h"

namespace Base {
    
    class SingletonDelegate {
    public:
		virtual ~SingletonDelegate() {
			
		}
        virtual void init() {}
        virtual void unInit() {}
		virtual void onCocos2dExit() {}
    };

    template <class T>
    class Singleton : public SingletonDelegate {
    public:
        virtual ~Singleton(){}
        
        static T* getInstance() {
            if (s_instance_ == NULL) {
                s_instance_ = new T();
                s_instance_->init();
                
                SingletonManager::sharedInstance()->addSingleton(s_instance_);
            }
            
            return s_instance_;
        }
        
//         void destroy() {
//             if (s_instance_ != NULL) {
//                 s_instance_->unInit();
//                 s_instance_->release();
//                 s_instance_ = NULL;
//             }
//         }
        
    private:
        static T* s_instance_;
    };
    
    template <class T>
    T* Singleton<T>::s_instance_ = NULL;
}

#endif
