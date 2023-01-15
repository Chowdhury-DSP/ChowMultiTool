#!/bin/bash

set -e

version="0.2.0"

# install functions
install_clapval_linux()
{
    curl -L "https://github.com/free-audio/clap-validator/releases/download/${version}/clap-validator-${version}-ubuntu-18.04.zip" -o clap-validator.zip
    unzip clap-validator > /dev/null
    tar -xf clap-validator-${version}-ubuntu-18.04.tar.gz
    echo "./clap-validator"
}

install_clapval_mac()
{
    curl -L "https://github.com/free-audio/clap-validator/releases/download/${version}/clap-validator-${version}-macos-universal.zip" -o clap-validator.zip
    unzip clap-validator > /dev/null
    tar -xf clap-validator-${version}-ubuntu-18.04.tar.gz
    echo "./clap-validator"
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
    $clapval --verbosity info validate ${plugin}
done

# clean up
rm -Rf clap-validator*
