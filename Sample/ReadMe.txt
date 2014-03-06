Kinect for Windows SDK v2 Developer Preview Sample Program ReadMe.txt


■Structure
Structure of this Sample Program is as follows.

    Kinect2Sample
    │
    │  // Sample Program
    ├─Sample
    │  │
    │  ├─Sample.sln
    │  │
    │  ├─Color
    │  │  ├─Color.vcxproj
    │  │  └─Color.cpp
    │  │
    │  ├─Depth
    │  │  ├─Depth.vcxproj
    │  │  └─Depth.cpp
    │  │
    │  ├─BodyIndex
    │  │  ├─BodyIndex.vcxproj
    │  │  └─BodyIndex.cpp
    │  │
    │  ├─Body
    │  │  ├─Body.vcxproj
    │  │  └─Body.cpp
    │  │
    │  ├─Infrared
    │  │  ├─Infrared.vcxproj
    │  │  └─Infrared.cpp
    │  │
    │  ├─Audio
    │  │  ├─Audio.vcxproj
    │  │  └─Audio.cpp
    │  │
    │  │  // Property Sheet
    │  ├─Sample.props
    │  │
    │  │  // License
    │  ├─License.txt
    │  │
    │  │  // Read Me
    │  └─ReadMe.txt
    │
    └─README.md


■About Sample Program
This Sample Program is setting to be able to build in the following environments. 
Please rewrite the appropriate setting depending on the environment.

    Visual Studio Express 2013 for Windows Desktop
    Kinect for Windows SDK v2 Developer Preview 1311
    OpenCV 2.4.8


■Setting for OpenCV
Please UnZip to the following Location.

    Location:
    C:\Program Files\opencv

Please set the following Environment Variables.

    Variable Name:	Variable Value:
    OPENCV_DIR		C:\Program Files\opencv\
    OPENCV_VER		248
    Path		;%OPENCV_DIR%\build\x64\vc12\bin


■About License
License of this Sample Program suppose MIT License.
Please refer to License.txt.


■Disclaimer
"This is preliminary software and/or hardware and APIs are preliminary and subject to change."

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIM
ED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DI
RECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLU
DING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF 
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEO
RY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGL
IGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN I
F ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


■Change History
2014/02/01 Created Sample Program
2014/02/01 Add Color
2014/02/14 Add Depth
2014/02/28 Add BodyIndex
2014/03/07 Add Body