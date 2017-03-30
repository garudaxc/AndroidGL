#ifndef _COMMAND_SYSTEM_
#define _COMMAND_SYSTEM_

#include <vector>
#include <string>

using namespace std;

namespace FancyTech
{

	int AsciiStricmp(const char * dst, const char * src);

	// ��װcommand�Ĳ���
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



	// ���ڸ�ϵͳע�Ṧ�ܺ���
	// ��������Сд������
	class CommandSystem
	{
	public:
		virtual void Init() = 0;
		virtual void Destroy() = 0;

		virtual void AddCommand(const string& name, CommandFunc func, const string& desc) = 0;

		// ����command�������ж��command,��'\n'�ָ�
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