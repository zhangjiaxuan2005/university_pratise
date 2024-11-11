'''
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
'''


import os
import shutil
import sys
import subprocess

def pip_check():
    if os.path.exists(r"thirdparty\python38\Scripts\pip.exe"):
        pip = subprocess.run("pip --version", capture_output=True).returncode
        if pip == 0:
            print("pip installed.")
        else:
            print("installing pip...")
            if os.path.exists(r"thirdparty\python38\Lib\site-packages\pip"):
                shutil.rmtree(r"thirdparty\python38\Lib\site-packages\pip")
            if os.path.exists(r"thirdparty\python38\Lib\site-packages\pip-21.1.1.dist-info"):
                shutil.rmtree(r"thirdparty\python38\Lib\site-packages\pip-21.1.1.dist-info")
            os.system(r"thirdparty\python38\python.exe -m ensurepip")
            shutil.copy("thirdparty\python38\Scripts\pip3.exe", "thirdparty\python38\Scripts\pip.exe")
    else:
        print("installing pip...")
        os.system(r"thirdparty\python38\python.exe -m ensurepip")
        shutil.copy("thirdparty\python38\Scripts\pip3.exe", "thirdparty\python38\Scripts\pip.exe")

def hb_check():
    if os.path.exists(r"thirdparty\python38\Scripts\hb.exe"):
        hb = subprocess.run("hb --version", capture_output=True).returncode

        if hb == 0:
            print("ohos-build installed.")
        else:
            print("installing ohos-build...")
            if os.path.exists(r"thirdparty\python38\Lib\site-packages\hb"):
                shutil.rmtree(r"thirdparty\python38\Lib\site-packages\hb")
            if os.path.exists(r"thirdparty\python38\Lib\site-packages\ohos_build-0.4.2.dist-info"):
                shutil.rmtree(r"thirdparty\python38\Lib\site-packages\ohos_build-0.4.2.dist-info")
            
            os.system(r"thirdparty\python38\Scripts\pip.exe install "
                      r"thirdparty\package\ohos_build-0.4.2-py3-none-any.whl")
    else:
        print("installing ohos-build...")
        os.system(r"thirdparty\python38\Scripts\pip.exe install "
                  r"thirdparty\package\six-1.16.0-py2.py3-none-any.whl")
        os.system(r"thirdparty\python38\Scripts\pip.exe install "
                  r"thirdparty\package\wcwidth-0.2.5-py2.py3-none-any.whl")
        os.system(r"thirdparty\python38\Scripts\pip.exe install "
                  r"thirdparty\package\prompt_toolkit-1.0.14-py3-none-any.whl")
        os.system(r"thirdparty\python38\Scripts\pip.exe install "
                  r"thirdparty\package\PyYAML-6.0-cp38-cp38-win_amd64.whl")
        os.system(r"thirdparty\python38\Scripts\pip.exe install "
                  r"thirdparty\package\pycryptodome-3.9.9-cp38-cp38-win_amd64.whl")
        os.system(r"thirdparty\python38\Scripts\pip.exe install "
                  r"thirdparty\package\ecdsa-0.16.1-py2.py3-none-any.whl")
        os.system(r"thirdparty\python38\Scripts\pip.exe install "
                  r"thirdparty\package\ohos_build-0.4.2-py3-none-any.whl")

def scons_check():
    if os.path.exists(r"thirdparty\python38\Scripts\SCons.exe"):
        scons = subprocess.run("scons --version", capture_output=True).returncode
        if scons == 0:
            print("SCons installed.")
        else:
            print("installing scons...")
            if os.path.exists(r"thirdparty\python38\Lib\site-packages\SCons"):
                shutil.rmtree(r"thirdparty\python38\Lib\site-packages\SCons")
            if os.path.exists(r"thirdparty\python38\Lib\site-packages\SCons-4.4.0.dist-info"):
                shutil.rmtree(r"thirdparty\python38\Lib\site-packages\SCons-4.4.0.dist-info")
            os.system(r"thirdparty\python38\Scripts\pip.exe install "
                      r"thirdparty\package\SCons-4.4.0-py3-none-any.whl")
    else:
        os.system(r"thirdparty\python38\Scripts\pip.exe install "
                  r"thirdparty\package\SCons-4.4.0-py3-none-any.whl")

if __name__ == '__main__':
    pip_check()
    hb_check()
    scons_check()