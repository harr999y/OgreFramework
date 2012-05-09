#ifndef _COMMON_CALSSES_INL_H_
#define _COMMON_CALSSES_INL_H_

/** Define DISALLOW_COPY_AND_ASSIGN macro for copy-constructor
and operator =. */
#define DISALLOW_COPY_AND_ASSIGN(Type)\
	Type(Type const &);\
	void operator=(Type const &);

namespace CommonClasses
{
	/** The common singleton class. */
	template <class T> class Singleton
	{
	public:
		/// return singletonPtr.
	    static T * getSingletonPtr()
		{
			static T instance;
			return &instance;
		}
		/// return singleton.
		static T & getSingleton()
		{
			return *getSingletonPtr();
		}

	protected:
		/// Constructor
		Singleton() {}

	private:		
		DISALLOW_COPY_AND_ASSIGN(Singleton);
	};

}

#endif
