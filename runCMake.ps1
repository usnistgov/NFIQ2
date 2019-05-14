
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

function removeDebugConfig()
{
  $lines = Get-Content "NFIQ2.sln"
  Set-Content "NFIQ2.sln" -value ""
  foreach ($line in $lines) {
    if (-not($line -match "Debug")) {
      Add-Content "NFIQ2.sln" -value $line
    }
  }
}

function runCMake( $folder, $generator)
{
  # temporary build directories
  new-item "./build/$folder" -ItemType directory -Force
  cd "./build/$folder"
  # run cmake
  & "$global:cmake" -DCMAKE_SYSTEM_VERSION=8.1 -G"${generator}" ../../
  # unable to generate only release with cmake, will modify the solution manually
  removeDebugConfig
  # cleanup
  cd ../..
}

findCMake
runCMake "Windows32" "Visual Studio 14" 
if ([System.IntPtr]::Size -gt 4) { 
  runCMake "Windows64" "Visual Studio 14 Win64" 
}

pause




