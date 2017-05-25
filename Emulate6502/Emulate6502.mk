##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=Emulate6502
ConfigurationName      :=Debug
WorkspacePath          :=D:/Data/rma10426/Documents/Emulator
ProjectPath            :=D:/Data/rma10426/Documents/Emulator/Emulate6502
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=rma10426
Date                   :=07/06/2017
CodeLitePath           :="D:/Program Files/CodeLite"
LinkerName             :=D:/TDM-GCC-64/bin/g++.exe
SharedObjectLinkerName :=D:/TDM-GCC-64/bin/g++.exe -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="Emulate6502.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=D:/TDM-GCC-64/bin/windres.exe
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := D:/TDM-GCC-64/bin/ar.exe rcu
CXX      := D:/TDM-GCC-64/bin/g++.exe
CC       := D:/TDM-GCC-64/bin/gcc.exe
CXXFLAGS :=  -g -O0 -Wall $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := D:/TDM-GCC-64/bin/as.exe


##
## User defined environment variables
##
CodeLiteDir:=D:\Program Files\CodeLite
Objects0=$(IntermediateDirectory)/main.c$(ObjectSuffix) $(IntermediateDirectory)/cpu.c$(ObjectSuffix) $(IntermediateDirectory)/memory.c$(ObjectSuffix) $(IntermediateDirectory)/cpu_run.c$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@$(MakeDirCommand) "./Debug"


$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./Debug"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main.c$(ObjectSuffix): main.c $(IntermediateDirectory)/main.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/Data/rma10426/Documents/Emulator/Emulate6502/main.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.c$(DependSuffix): main.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.c$(ObjectSuffix) -MF$(IntermediateDirectory)/main.c$(DependSuffix) -MM main.c

$(IntermediateDirectory)/main.c$(PreprocessSuffix): main.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.c$(PreprocessSuffix) main.c

$(IntermediateDirectory)/cpu.c$(ObjectSuffix): cpu.c $(IntermediateDirectory)/cpu.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/Data/rma10426/Documents/Emulator/Emulate6502/cpu.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/cpu.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/cpu.c$(DependSuffix): cpu.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/cpu.c$(ObjectSuffix) -MF$(IntermediateDirectory)/cpu.c$(DependSuffix) -MM cpu.c

$(IntermediateDirectory)/cpu.c$(PreprocessSuffix): cpu.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/cpu.c$(PreprocessSuffix) cpu.c

$(IntermediateDirectory)/memory.c$(ObjectSuffix): memory.c $(IntermediateDirectory)/memory.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/Data/rma10426/Documents/Emulator/Emulate6502/memory.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/memory.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/memory.c$(DependSuffix): memory.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/memory.c$(ObjectSuffix) -MF$(IntermediateDirectory)/memory.c$(DependSuffix) -MM memory.c

$(IntermediateDirectory)/memory.c$(PreprocessSuffix): memory.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/memory.c$(PreprocessSuffix) memory.c

$(IntermediateDirectory)/cpu_run.c$(ObjectSuffix): cpu_run.c $(IntermediateDirectory)/cpu_run.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/Data/rma10426/Documents/Emulator/Emulate6502/cpu_run.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/cpu_run.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/cpu_run.c$(DependSuffix): cpu_run.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/cpu_run.c$(ObjectSuffix) -MF$(IntermediateDirectory)/cpu_run.c$(DependSuffix) -MM cpu_run.c

$(IntermediateDirectory)/cpu_run.c$(PreprocessSuffix): cpu_run.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/cpu_run.c$(PreprocessSuffix) cpu_run.c


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


