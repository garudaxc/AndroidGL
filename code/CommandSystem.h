#ifndef _COMMAND_SYSTEM_
#define _COMMAND_SYSTEM_

#include <vector>
#include <string>

using namespace std;

namespace FancyTech
{

	int AsciiStricmp(const char * dst, const char * src);

	// 封装command的参数
	class Arguments
	{
	public:
		Arguments(const vector<string>& args);
		~Arguments();

		int GetNumArgs() const	{	return m_args.size();	}
		const string& GetArg(int index) const;

	private:
		vector<string> m_args;		
	};

	typedef void (*CommandFunc)(const Arguments& args);



	// 用于各系统注册功能函数
	// 命令名大小写不敏感
	class CommandSystem
	{
	public:
		virtual void Init() = 0;
		virtual void Destroy() = 0;

		virtual void AddCommand(const string& name, CommandFunc func, const string& desc) = 0;

		// 调用command，可以有多个command,用'\n'分隔
		virtual void ExecuteCommandText(const char* text) = 0;

		virtual bool ExecuteFile(const string& filename) = 0;
	};

	extern CommandSystem* GCommandSystem;


	class CommandInitializer
	{
	public:
		CommandInitializer(const string& name, CommandFunc func, const string& desc);

		string		Name;
		CommandFunc		Func;
		string		Desc;

		CommandInitializer* pNext;

		static CommandInitializer* Link;
	};

}

#endif