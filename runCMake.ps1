
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

function runCMake( $platform, $generator)
{
  # find visual studio string
  if( $generator -match "Visual Studio [1-9]* [0-9]*" ) {
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
  & "$global:cmake" -DCMAKE_SYSTEM_VERSION=8.1 -G "${generator}" ../../../
  # cleanup
  cd ../../../
}

findCMake
runCMake "x86" "Visual Studio 15 2017" 
if ([System.IntPtr]::Size -gt 4) { 
  runCMake "x64" "Visual Studio 15 2017 Win64" 
}

pause




