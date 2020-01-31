param([Int32]$msvc=2017)

function findCmake()
{
  # find cmake
  $tests = @( ${env:ProgramFiles}, ${env:CommonProgramFiles(x86)})
  $global:cmake = $null
  foreach ($test in $tests ) {
      if( Test-Path $test/CMake/bin/cmake.exe ) {
          $global:cmake = "$test/CMake/bin/cmake.exe"
          break;
      }
  }
  if( $global:cmake -eq $null ) {
      write-host "Missing CMAKE installation\n"
      exit
  }
}


function findCMakeGenerator()
{
    [string[]] $generators = 
        "Visual Studio 10 2010",
        "Visual Studio 11 2012",
        "Visual Studio 12 2013",
        "Visual Studio 14 2015",
        "Visual Studio 15 2017", 
        "Visual Studio 16 2019"
    ForEach ( $generator In $generators) {
        if( $generator -match "$msvc" ) {
            $global:genX86 = $generator
            $global:genX64 = $generator + " Win64"
            return;
        }
    }
    Write-Host "Using default MSVC settings"
}

function runCMake( $platform, $generator)
{
  # find visual studio string
  if( $generator -match "Visual Studio [0-9]* [0-9]*" ) {
    # remove whitespaces to not destroy include pathes
    $msvc = $Matches.Values[0] -replace "\s","_"
  }
  else {
    $msvc = "MSVC"
  }
  # temporary build directories
  new-item "./build/$msvc/$platform" -ItemType directory -Force
  cd "./build/$msvc/$platform"
  # run cmake
  & "$global:cmake" -DCMAKE_SYSTEM_VERSION=8.1 -G "${generator}" -DCMAKE_CONFIGURATION_TYPES:STRING=Release -DCMAKE_TRY_COMPILE_CONFIGURATION:STRING=Release ../../../
  # cleanup
  cd ../../../
}

$global:genX86 = $null
$global:genX64 = $null
findCMake
findCMakeGenerator
Write-Host "Using Generator $genX86"
runCMake "x86" $genX86 
if ([System.IntPtr]::Size -gt 4) { 
  Write-Host "Using Generator $genX64"
  runCMake "x64" $genX64 
}

pause




