@echo off
pushd .
cd %~dp0
"./source/third_party/python/python.exe" "./source/third_party/zbd/muffinman/muffinman.py" --asset_dir "./assets/" --output_dir "./publish/client/" --temp_dir "./temp/muffinman/" %*
popd
