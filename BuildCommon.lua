
ABRoot = [[/炫舞2多线程版本发布/]]

function StringGetPath(s)
	local i = string.find(string.reverse(s), "[/\\]")
	local path = string.sub(s, 1, -i)
	return path
end

function OpenFile(fileName, mode)
	local f
	local err
	f, err = io.open(fileName, mode)
	if f == nil then
		print("open file failed! "..err)
		os.exit(false)
	end
	return f
end

-- critical shell
function CShell(cmd)
	local result = os.execute(cmd)
	if result == nil then
		print("execute "..cmd.." failed!")
		os.exit(false)
	end
end

function Shell(cmd)
	local result = os.execute(cmd)
	return result
end


function ChangeVersionString(fileName, pattern, s)
	local f = OpenFile(fileName, "r")
	local t = f:read("*a")
	f:close()
	
	local n = 0
	t, n = string.gsub(t, pattern, s)
	if n < 1 then		
		print(string.format("修改版本号失败! %s 找不到匹配%s", fileName, pattern))
		return false
	end	
	
	f = OpenFile(fileName, "w+")
	f:write(t)
	f:close()
	
	return true
end

function ChangeRCFileVersion(fileName, version)
	--"version: 2.1.0.0"
	--FILEVERSION 2,2,0,1136 (FILEVERSION%s+)%d+,%d+,%d+,%d+
	local result = true
	local pattern = "(FILEVERSION%s+)%d+,%d+,%d+,%d+"
	local ver = string.format("%%1%s,%s,%s,%s", version[1], version[2], version[3], version[4])
	local result = result and ChangeVersionString(fileName, pattern, ver)
		
	local pattern = "(PRODUCTVERSION%s+)%d+,%d+,%d+,%d+"
	local ver = string.format("%%1%s,%s,%s,%s", version[1], version[2], version[3], version[4])
	local result = result and ChangeVersionString(fileName, pattern, ver)
		
	--"FileVersion", "2.2.0.1136"
	local pattern = [[("FileVersion",%s+")%d+%.%d+%.%d+%.%d+]]
	local ver = string.format("%%1%s.%s.%s.%s", version[1], version[2], version[3], version[4])
	local result = result and ChangeVersionString(fileName, pattern, ver)
	
	local pattern = [[("ProductVersion",%s+")%d+%.%d+%.%d+%.%d+]]
	local ver = string.format("%%1%s.%s.%s.%s", version[1], version[2], version[3], version[4])
	local result = result and ChangeVersionString(fileName, pattern, ver)
		
	if not result then
		os.exit(false)
	end
end

function ChangeDotNetVersion(fileName, version)
	local result = true
	--AssemblyVersion("2.3.0.1136")
	local pattern = [[(AssemblyVersion%(")%d+%.%d+%.%d+%.%d+"%)]]
	local ver = string.format("%%1%s.%s.%s.%s\")", version[1], version[2], version[3], version[4])
	local result = result and ChangeVersionString(fileName, pattern, ver)
	
	local pattern = [[(AssemblyFileVersion%(")%d+%.%d+%.%d+%.%d+"%)]]
	local ver = string.format("%%1%s.%s.%s.%s\")", version[1], version[2], version[3], version[4])
	local result = result and ChangeVersionString(fileName, pattern, ver)
	
	if not result then
		print("failed")
		os.exit(false)
	end
end

function ClearReadOnlyAttrib(path)
	CShell(string.format("attrib -r /s /d %s*", path))
end

--------------------------------------------------------------

function LogonAB_Test()
	CShell([[ab logon -s H3d-131 -d engine-x52 -u "hudson" -p "engine"]])
	CShell([[ab logon -s H3d-131 -d engine-x52 -u "hudson" -p "engine"]])
	CShell([[ab setactivebranch "Root Branch"]])
end

function LogonAB_Release()
	CShell([[ab logon -s H3d-ab01 -d QQX5-2 -u "hudson" -p "engine"]])
	CShell([[ab logon -s H3d-ab01 -d QQX5-2 -u "hudson" -p "engine"]])
	CShell([[ab setactivebranch "Root Branch"]])
end

tmpFile = [[d:\tmpfile.tmp]]

function ListLocalFiles(path)
	local localFiles = {}
	
	-- 读取所有文件的列表
	CShell(string.format([[dir %s /b /s /og /a-h >%s]], path, tmpFile))
	local f = OpenFile(tmpFile)
	for l in f:lines() do
		local s = string.sub(l, #path + 1)
		s = string.gsub(s, "\\", "/")
		localFiles[s] = true
	end
	f:close()

	return localFiles
end

function CompareABtoLocal(abPath, localFiles)
	local both = {}
	local onlyAB = {}
	local onlyLocal = {}
	local dirList = {""}
	-- 对比ab上的文件
	while #dirList > 0 do
		local d = table.remove(dirList, 1)
		CShell(string.format([[ab enumobjects %s/%s -disk >%s]], abPath, d, tmpFile))
		local f = OpenFile(tmpFile)
		for line in f:lines() do
			-- 跳过一些信息行
			local result = string.find(line, [[Directory list of project node: "]])
			result = result or string.find(line, [[Total of %d+ folders, and %d+ files%.]])
			if result ~= nil then
				goto continue
			end
			local isDir = false
			if string.sub(line, -1) == '\\' then
				line = string.sub(line, 1, -2)
				isDir = true
			end		
		
			-- 本地比ab上多的
			if string.sub(line, 1, 1) == '*' then
				table.insert(onlyLocal, d..string.sub(line, 2))
			else
				local s = d..line
				-- ab比本地多的
				if not localFiles[s] then
					table.insert(onlyAB, s)
				else
					if isDir then
						table.insert(dirList, s..'/')
					else
						table.insert(both, s)
					end
				end
			end
			::continue::
		end
		
		f:close()
	end

	return both, onlyAB, onlyLocal
end

	
function CommitToAB(abPath, binPath, comment)
	-- working path后面不能跟斜杠
	local s = binPath
	if string.sub(s, -1) == '\\' or string.sub(s, -1) == '/' then
		s = string.sub(s, 1, -2)
	end
	CShell(string.format([[ab setworkingpath "%s" -dbpath "%s"]], s, abPath))
	
	s = binPath:gsub("/", "\\")
	local localFiles = ListLocalFiles(s)
	local both, onlyAB, onlyLocal = CompareABtoLocal(abPath, localFiles)

	local updated = 0
	print("both side", #both)
	for i, v in ipairs(both) do	
		 local isNewer = Shell(string.format([[ab isnewer "%s%s" >%s]], binPath, v, tmpFile))
		 if isNewer then
			CShell(string.format([[ab checkout "%s/%s" -dontgetlocal >%s]], abPath, v, tmpFile))
			CShell(string.format([[ab checkin "%s/%s" -comment %q]], abPath, v, comment))
			updated = updated + 1
			-- ab客户端里会显示本地没有更新到最新版本，可能是客户端自已维护了一份本地文件的版本信息
			-- CShell(string.format([[ab getlatest "%s/%s" -nosmartget]], abPath, v))
		 end
	end
	print(string.format("%d files updated", updated))

	print("onlyLocal", #onlyLocal)
	for i, v in ipairs(onlyLocal) do	
		local suffix = string.lower(string.sub(v, -4))
		-- 忽略svn文件和pdb文件
		if v == ".svn" or suffix == ".pdb" then
			print(string.format("ignore %s%s", binPath, v))
		else
			local i = string.find(string.reverse(v), "/") or 0
			local path = string.sub(v, 1, -i)
			CShell(string.format([[ab import "%s%s" -parent "%s/%s" -dontgetlocal -comment %q]], binPath, v, abPath, path, comment))
		end
	end
	print("")

	print("onlyAB", #onlyAB)
	for i, v in ipairs(onlyAB) do
		print(i, v)	
		-- delete命令需要手动输入确认信息，使用管道又会报错，未能实现自动从ab上删除文件
		--CShell(string.format([[@echo 0 | ab delete "%s/%s"]], abPath, v))
	end
	print("")

end

-- 
function EnsureFolderOnAB(path)
	local s = path:gsub("\\", "/")
	if s:sub(-1) == '/' then
		s = s:sub(1, -2)
	end
	
	local d = ""
	for i in s:gmatch("(/[^/]*)") do
		d = d..i
		local r = Shell(string.format([[ab existsindb %s]], d))
		if not r then
			print("创建ab目录", d)
			CShell(string.format([[ab newfolder %s]], d))
		end
	end	
end


ProjectType = {
	CPP = {
		ChangeVersion = ChangeRCFileVersion,
	},
	CSharp = {
		ChangeVersion = ChangeDotNetVersion,
	}
}

function Build(config)
	for _, file in pairs(config.versionFile) do
		if file:sub(-3) == ".rc" then
			ChangeRCFileVersion(file, config.version)
		elseif file:sub(-3) == ".cs" then
			ChangeDotNetVersion(file, config.version)
		else
			print("修改版本号失败，文件类型错误", file)
			os.exit(false)
		end
		--config.projectType.ChangeVersion(file, config.version)	
	end
	
	CShell(string.format([[buildconsole.exe %s /rebuild /cfg="%s" /prj="%s"]], solution.file, config.buildConfig, config.projectName))
	
	for _, file in pairs(config.versionFile) do
		CShell(string.format("svn revert %s", file))
	end
	
	if config.postBuild then
		config.postBuild()
	end
	
	local path = config.binPath:gsub("/", "\\")
	CShell(string.format("del /f /s /q %s*.lib", path))
	CShell(string.format("del /f /s /q %s*.lik", path))
	CShell(string.format("del /f /s /q %s*.ilk", path))
	CShell(string.format("del /f /s /q %s*.exp", path))
	CShell(string.format("del /f /s /q %s*.pdb", path))
	
	LogonAB_Test()
	CommitToAB(config.abPath, config.binPath, comment)
end

function Distribute(config)
	local fileName = string.format([[%s_%s.%s.%s.%s.zip]], config.projectName, 
	config.version[1], config.version[2], config.version[3], config.version[4])
	
	-- 打包
	CShell(string.format([[rar.exe a -r -ep1 %s%s %s]], config.binPath, fileName, config.binPath))	
	
	LogonAB_Release()
	
	local t = os.date("*t")
	local path = string.format("/engine_release/%d年/%d年%02d月/x52_%d%02d%02d/", t.year, t.year, t.month, t.year, t.month, t.day)
	
	-- 创建ab目录
	EnsureFolderOnAB(path)
	
	-- 上传
	CShell(string.format([[ab import "%s%s" -parent "%s" -dontgetlocal -comment %q]], config.binPath, fileName, path, comment))
	
	-- 删除本地
	path = config.binPath:gsub("/", "\\")
	CShell(string.format([[del /f /q %s%s]], path, fileName))
end
