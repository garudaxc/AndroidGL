#include "CommandSystem.h"
#include "GlobalVar.h"
#include "MyLog.h"
#include <string.h>


namespace FancyTech
{

#define LogConsole GLog.LogInfo

	int AsciiStricmp( const char * dst, const char * src)
	{
		int f, l;

		do
		{
			if (((f = (unsigned char)(*(dst++))) >= 'A') && (f <= 'Z'))
				f -= 'A' - 'a';
			if (((l = (unsigned char)(*(src++))) >= 'A') && (l <= 'Z'))
				l -= 'A' - 'a';
		} while (f && (f == l));

		return(f - l);
	}



	CommandInitializer* CommandInitializer::Link = NULL;
	CommandInitializer::CommandInitializer(const string& name, CommandFunc func, const string& desc):
		Name(name),
		Func(func),
		Desc(desc)
	{
		pNext = Link;
		Link = this;
	}

	//////////////////////////////////////////////////////////////////////////

	Arguments::Arguments(const vector<string>& args):m_args(args)
	{	
	}

	Arguments::~Arguments()
	{
	}

	const string& Arguments::GetArg(int index) const
	{
		return m_args[index];
	}

	//////////////////////////////////////////////////////////////////////////

	struct CmdFunInfo
	{
		string name;
		CommandFunc func;
		string desc;
	};

	class CommandSystemImpl : public CommandSystem
	{
	public:
		virtual void Init();
		virtual void Destroy();

		virtual void AddCommand(const string& name, CommandFunc func, const string& desc);

		virtual void ExecuteCommandText(const char* text);

		virtual bool ExecuteFile(const string& filename);

		void ListCommand() const;

	private:
		CmdFunInfo* FindCommand(const string& cmdName);

		vector<CmdFunInfo>	m_cmds;
	};

	CommandSystemImpl cmdSys;
	CommandSystem* GCommandSystem = &cmdSys;


	static void cmdListCommand(const Arguments& args)
	{
		cmdSys.ListCommand();
	}

	void CommandSystemImpl::Init()
	{
		CommandInitializer* pInit = CommandInitializer::Link;
		while (pInit != NULL)
		{
			AddCommand(pInit->Name, pInit->Func, pInit->Desc);
			pInit = pInit->pNext;
		}
		CommandInitializer::Link = NULL;


		// init static command
		AddCommand("ListCommand", cmdListCommand, "列出当前所有Command");
	}

	void CommandSystemImpl::Destroy()
	{
	}

	void CommandSystemImpl::AddCommand(const string& name, CommandFunc func, const string& desc)
	{
		CmdFunInfo info;
		info.name = name;
		info.func = func;
		info.desc = desc;
		
		m_cmds.push_back(info);
	}

	CmdFunInfo* CommandSystemImpl::FindCommand(const string& cmdName)
	{
		for (vector<CmdFunInfo>::iterator it = m_cmds.begin(); it != m_cmds.end(); ++it)
		{
			if (AsciiStricmp(it->name.c_str(), cmdName.c_str()) == 0)
			{
				return &(*it);
			}
		}

		return NULL;
	}

	void CommandSystemImpl::ExecuteCommandText(const char* text)
	{
		string strText = text;

		vector<string> commandLines;
		//FilePathStringHelper::split_string(commandLines, strText, "\n");

		if (commandLines.empty() || commandLines[0].empty())
		{
			return;
		}

		for (size_t i = 0; i < commandLines.size(); i++)
		{			
			if (commandLines[i].size() >= 2 && commandLines[i][0] == '/' && commandLines[i][1] == '/')
			{
				// 跳过注释
				continue;
			}

			vector<string> tokens;
			//FilePathStringHelper::split_string(tokens, commandLines[i], " ");

			if (tokens.empty())
			{
				continue;
			}

			CmdFunInfo* pCmdInfo = FindCommand(tokens[0]);
			if (pCmdInfo != NULL)
			{
				Arguments args(tokens);
				pCmdInfo->func(args);
				continue;
			}

			// if is a globlaVar
			if (!GGlobalVarManager->OnCommand(tokens[0]))
			{
				LogConsole("can not find command %s\n", tokens[0].c_str());
			}
		}
	}

	void CommandSystemImpl::ListCommand() const
	{
		for (vector<CmdFunInfo>::const_iterator it = m_cmds.begin(); it != m_cmds.end(); ++it)
		{
			char buff[255];
			sprintf(buff, "%s\t%s\n", it->name.c_str(), it->desc.c_str());
			LogConsole(buff);
		}
	}

	bool CommandSystemImpl::ExecuteFile(const string& filename)
	{
		FILE* pf = fopen(filename.c_str(), "r");
		if (pf == NULL)
		{
			LogConsole("CommandSystem can not find command file %s\n", filename.c_str());
			return false;
		}

		fseek(pf, 0, SEEK_END);
		size_t size = ftell(pf);
		fseek(pf, 0, SEEK_SET);

		char* buff = new char[size + 1];
		size_t readSize = fread(buff, 1, size, pf);
		buff[readSize] = '\0';

		ExecuteCommandText(buff);

		delete []buff;
		fclose(pf);

		return true;
	}
	

}
