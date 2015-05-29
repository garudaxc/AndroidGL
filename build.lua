--cd $(SolutionDir)MyApp1
--$(NDK)\ndk-build
--$(ANT_HOME)\bin\ant debug

cmd = arg[1]

SolutionDir = arg[2]
ndkPath = arg[3]
antPath = arg[4];


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


CShell(string.format([[%s\ndk-build clean]], ndkPath))
CShell(string.format([[%s\ndk-build]], ndkPath))
CShell(string.format([[%s\bin\ant -f %sMyApp1\build.xml debug]], antPath, SolutionDir))