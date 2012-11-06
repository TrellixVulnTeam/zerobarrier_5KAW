@echo off
pushd .
cd %~dp0
"../third_party/python/python.exe" "..\third_party\zbd\script\tools\coffee_to_js.py" "../third_party/nodejs/node" "../third_party/coffeescript/bin/coffee" "../../publish/server/js" "./coffeescript" "../shared/coffeescript"
popd
