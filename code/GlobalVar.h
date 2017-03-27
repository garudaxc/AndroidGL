#ifndef _GLOBALVAR_
#define _GLOBALVAR_

#include <vector>
#include <string>

using namespace std;

namespace Aurora
{
    

	enum
	{
		GVFLAG_BOOL		= 0x1,		// 布尔类型
		GVFLAG_INT		= 0x2,		// 整型
		GVFLAG_FLOAT	= 0x4,		// 浮点型
		GVFLAG_STRING	= 0x8,		// 字符串
		GVFLAG_PROFILE	= 0x10,		// 用于profile的全局变量
		GVFLAG_DEBUG	= 0x20,		// 用于debug的全局变量
		GVFLAG_READONLY	= 0x40		// 该常量不应该直接被修改，而是应该用一个功能函数修改
									// 因为有些功能的开启或关闭需要额外的操作，例如，EnablePhysX
	};

	typedef void (*GlobalVarCallback)();

	class GlobalVar
	{
	public:
		GlobalVar(const string& name, const string& value, int flag, const string& desc);

		inline const string& GetName() const	{	return m_pInternal->m_Name;	}
		inline int		GetFlag() const		{	return m_pInternal->m_flag;	}
		inline const string& GetDefaultValue() const {	return m_pInternal->m_DefaultValue;	}
		inline const string& GetDesc() const	{	return m_pInternal->m_Desc;	}

		inline bool		GetBool() const		{	return (m_pInternal->nValue != 0);	}
		inline int		GetInt() const		{	return	m_pInternal->nValue;	}
		inline float	GetFloat() const	{	return m_pInternal->fValue;	}
		//const char* GetString() const	{	return m_pInternal->m_Value.c_str();	}

		virtual void SetBool(bool value)	{	m_pInternal->SetBool(value);	}
		virtual void SetInt(int value)		{	m_pInternal->SetInt(value);		}
		virtual void SetFloat(float value)	{	m_pInternal->SetFloat(value);	}
		virtual void SetValue(const string& value) {	m_pInternal->SetValue(value);	}

		virtual void SetCallBack(GlobalVarCallback pCallback)	{	m_pInternal->SetCallBack(pCallback);	}
		
		void	SetInternal(GlobalVar* p)	{	m_pInternal = p;	}
				
		// reset to default
		virtual void	ResetValue()		{	m_pInternal->ResetValue();	}

		static GlobalVar* StaticInit;
		//for static init
		GlobalVar* next;
	protected:
		GlobalVar()	{}

		string m_Name;
		string m_DefaultValue;
		int m_flag;
		string m_Desc;
		union
		{
			int nValue;
			float fValue;
		};				

		GlobalVar* m_pInternal;
		GlobalVarCallback	m_pCallback;
	};


	class GlobalVarManager
	{
	public:
		virtual void Init() = 0;
		virtual void Destroy() = 0;

		virtual void Register(GlobalVar* gvar) = 0;

		virtual void SetGlobalVar(const string& name, const string& value) = 0;
		//add 获取某个GV的value zhangshuai
		virtual string GetGloabalVar(const string& name)=0;

		// 输入一个GlobalVar的名字，在console中打印对应的值
		virtual bool OnCommand(const string& cmd) = 0;
		//virtual void SetGlobalVar(const char* name, bool value) = 0;
		//virtual void SetGlobalVar(const char* name, int value) = 0;
		//virtual void SetGlobalVar(const char* name, float value) = 0;
		
		//virtual void Command()
	};

	extern GlobalVarManager* GGlobalVarManager;


}

#endif
