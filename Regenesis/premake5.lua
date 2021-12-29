solution (_ACTION .. ".regenesis")
  configurations { "Debug", "Release" }
  platforms { "x64" }
  architecture "x64"

  -- -------------------------------------------------------------
  function DefaultOutputDirs()

    objdir      ( "_build/obj/" .. _ACTION .. "/" )
    targetdir   ( "_build/bin/" .. _ACTION .. "/%{cfg.shortname}" )
    debugdir    ( "$(SolutionDir)" )

  end

  -- -------------------------------------------------------------
  function DefaultBuildConfiguration()

    if iscpp then
      cppdialect "C++17"
    end


    -- ----- WINDOWS -----
    filter "system:Windows"
    
    if os.getversion().majorversion == 10 then
        local sRegArch = iif( os.is64bit(), "\\Wow6432Node\\", "\\" )
        local sWin10SDK = os.getWindowsRegistry( "HKLM:SOFTWARE" .. sRegArch .. "Microsoft\\Microsoft SDKs\\Windows\\v10.0\\ProductVersion" )

        -- apparently it needs an extra ".0" to be recognized by VS
        if sWin10SDK ~= nil then systemversion( sWin10SDK .. ".0" ) end
    end

    buildoptions 
    { 
      "/Qpar",
      "/openmp",
    }
    defines 
    {
   --   "RG_QPAR",
      "RG_OPENMP",

      "WIN32", 
      "WIN64",
      "_WIN64",
      "WIN32_LEAN_AND_MEAN",
      "WINVER=0x0600",
      
      "_CRT_SECURE_NO_WARNINGS",
      "_SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING"
    }

    filter {"system:Windows", "configurations:Release"}
      linkoptions { "/ltcg" }
      flags       { "LinkTimeOptimization" }


    -- ----- LINUX -----
    filter "system:linux"
    defines
    {
      "RG_OPENMP"
    }
    buildoptions 
    { 
      "-fPIC -fms-extensions -std=c++17 -fopenmp -fopenmp-version=50"
    }

    -- ----- COMMON -----
    filter {}

    flags
    {
    }

 -- staticruntime       "on"
    rtti                "Off"
    floatingpoint       "fast"
    stringpooling       "On"

    filter "configurations:Debug"
      defines   { "DEBUG" }
      symbols   "On"

    filter "configurations:Release"
      defines   { "NDEBUG" }
      optimize  "Full"
        
  end

  -- -------------------------------------------------------------
  project (_ACTION .. "-core-ext-stb")
    language "C++"
    kind     "StaticLib"

    files
    {
      "regenesis-core/ext-stb/**.cpp",
      "regenesis-core/ext-stb/**.h",
    }

    includedirs
    {
      "regenesis-core/ext-stb/",
    }
    
    DefaultOutputDirs()
    DefaultBuildConfiguration()

  -- -------------------------------------------------------------
  project (_ACTION .. "-core-ext-fastnoise")
    language "C++"
    kind     "StaticLib"

    files
    {
      "regenesis-core/ext-fastnoise/**.cpp",
      "regenesis-core/ext-fastnoise/**.h",
    }
    
    DefaultOutputDirs()
    DefaultBuildConfiguration()

  -- -------------------------------------------------------------
  project (_ACTION .. "-core")
    language "C++"
    kind     "StaticLib"

    files
    {
      "regenesis-core/*.cpp",
      "regenesis-core/*.h",
    }
    
    includedirs
    {
      "regenesis-core/",
      "regenesis-core/ext-stb/",
      "regenesis-core/ext-fastnoise/",
    }

    pchheader "rgCommon.h"
    pchsource "regenesis-core/rgCommon.cpp"

    DefaultOutputDirs()
    DefaultBuildConfiguration()


  -- -------------------------------------------------------------
  function LeafBuildConfiguration()

    links
    {
      _ACTION .. "-core",
      _ACTION .. "-core-ext-stb",
      _ACTION .. "-core-ext-fastnoise",
    }

    filter "system:Linux"
    links
    {
      "omp"
    }

    filter "system:Windows"
    linkoptions { "/OPT:NOREF" }

    filter {}

  end

  -- -------------------------------------------------------------
  project (_ACTION .. "-rg-dll")
    language "C++"
    kind     "SharedLib"

    files
    {
      "regenesis-dll/**.cpp",
      "regenesis-dll/**.h",
    }

    targetname ( "Regenesis64" )
    
    includedirs
    {
      "regenesis-core/",
      "regenesis-dll/",
    }

    LeafBuildConfiguration()

    DefaultOutputDirs()
    DefaultBuildConfiguration()

  -- -------------------------------------------------------------
  project (_ACTION .. "-rg-cli")
    language "C++"
    kind     "ConsoleApp"

    files
    {
      "regenesis-cli/**.cpp",
      "regenesis-cli/**.h",
    }

    targetname ( "regenesis" )

    includedirs
    {
      "regenesis-core/",
      "regenesis-cli/",
    }
    
    LeafBuildConfiguration()

    DefaultOutputDirs()
    DefaultBuildConfiguration()


