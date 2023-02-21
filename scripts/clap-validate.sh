#!/bin/bash

set -e

version="0.3.0"

# install functions
install_clapval_linux()
{
    curl -L "https://github.com/free-audio/clap-validator/releases/download/${version}/clap-validator-${version}-ubuntu-18.04.tar,gz" -o clap-validator.tar.gz
    tar -xf clap-validator.tar.gz
    echo "./clap-validator"
}

install_clapval_mac()
{
    curl -L "https://github.com/free-audio/clap-validator/releases/download/${version}/clap-validator-${version}-macos-universal.tar.gz" -o clap-validator.tar.gz
    tar -xf clap-validator.tar.gz
    echo "./binaries/clap-validator"
}

install_clapval_win()
{
    powershell -Command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest https://github.com/free-audio/clap-validator/releases/download/${version}/clap-validator-${version}-windows.zip -OutFile clap-validator.zip"
    powershell -Command "Expand-Archive clap-validator.zip -DestinationPath ."
    echo "./clap-validator.exe"
}

# install
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    clapval=$(install_clapval_linux)
    declare -a plugins=("build/ChowMultiTool_artefacts/Release/CLAP/ChowMultiTool.clap")
elif [[ "$OSTYPE" == "darwin"* ]]; then
    clapval=$(install_clapval_mac)
    declare -a plugins=("build/ChowMultiTool_artefacts/Release/CLAP/ChowMultiTool.clap")
else
    clapval=$(install_clapval_win)
    declare -a plugins=("build/ChowMultiTool_artefacts/Release/CLAP/ChowMultiTool.clap")
fi

echo "CLAP Validator installed at ${clapval}"
$clapval --version

# run
for plugin in "${plugins[@]}"; do
    echo "Validating ${plugin}"
    $clapval --verbosity error validate --hide-output --in-process ${plugin}
done

# clean up
#rm -Rf clap-validator*
#rm -Rf binaries
