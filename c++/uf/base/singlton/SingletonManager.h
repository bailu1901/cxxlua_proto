//
//  SingletonManager.h
//  ProjectM
//
//  Created by ruby on 13-4-17.
//
//

#ifndef ProjectM_SingletonManager_h
#define ProjectM_SingletonManager_h
#include <list>


namespace Base {
    
    class SingletonDelegate;
    typedef std::list<Base::SingletonDelegate*> SingletonList;

    class SingletonManager {
		
	protected:

		

     public:
        SingletonManager();
        virtual ~SingletonManager();
        static SingletonManager* sharedInstance();
		//static void pugreInstance();
        
        void addSingleton(SingletonDelegate* s);

	protected:
		friend class ExitObserver;
        void destroySingleton();
		void doCocos2dExit();
        
		void onCocos2dExit();

    private:
        SingletonList   singletonList_;
        static SingletonManager* singletonInstance_;
    };
}

#endif
