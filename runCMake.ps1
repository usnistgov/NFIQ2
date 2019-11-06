
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

function runCMake( $folder, $generator)
{
  # temporary build directories
  new-item "./build/$folder" -ItemType directory -Force
  cd "./build/$folder"
  # run cmake
  & "$global:cmake" -DCMAKE_CONFIGURATION_TYPES="Release" -DCMAKE_SYSTEM_VERSION=8.1 -G"${generator}" ../../
  # cleanup
  cd ../..
}

findCMake
runCMake "Windows32" "Visual Studio 15 2017" 
if ([System.IntPtr]::Size -gt 4) { 
  runCMake "Windows64" "Visual Studio 15 2017 Win64" 
}

pause




