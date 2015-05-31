--cd $(SolutionDir)MyApp1
--$(NDK)\ndk-build
--$(ANT_HOME)\bin\ant debug

cmd = arg[1]

SolutionDir = arg[2]
ndkPath = arg[3]
antPath = arg[4];

function StringGetPath(s)
	local i = string.find(string.reverse(s), "[/\\]")
	local path = string.sub(s, 1, -i)
	return path
end

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


function ChangeVersionString(fileName, pattern, s)
	local f = OpenFile(fileName, "r")
	local t = f:read("*a")
	f:close()
	
	local n = 0
	t, n = string.gsub(t, pattern, s)
	if n < 1 then		
		print(string.format("ÐÞ¸Ä°æ±¾ºÅÊ§°Ü! %s ÕÒ²»µ½Æ¥Åä%s", fileName, pattern))
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


function ClearReadOnlyAttrib(path)
	CShell(string.format("attrib -r /s /d %s*", path))
end


CShell(string.format([[%s\ndk-build clean]], ndkPath))
CShell(string.format([[%s\ndk-build]], ndkPath))
CShell(string.format([[%s\bin\ant -f %sMyApp1\build.xml debug]], antPath, SolutionDir))