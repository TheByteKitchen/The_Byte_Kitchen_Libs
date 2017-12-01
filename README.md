# The_Byte_Kitchen_Libs

This project is related to The Byte Kitchen Blog (at thebytekitchen.com). It typically deals with Windows 8 apps, DirectX, and the Kinect for Windows.

This project is entirely made up of subprojects, is has no code of its own. Short descriptions of the subprojects are below.

The WinRT DirectX Bus makes it easier to create DirectX applications. It provides basic facilities such as a 3D Device, Context, SwapChain and the like. A developer just creates components, such as a camera, a cube, an avatar, etc. and the code in the library will call its Load, Initialize, Update and Render methods. See the wiki page for more information (the section header is the link).

Other libraries included are DiscreteMedianFilter, which filters jitter out of the Kinect depth data, and rlecodec, a Run Length Encoding (de)compression library.

Version 0.2 has been released. Please find details in three blog posts:
"General information about the release" 
"About the filter for the depth jitter" 
"About very strong data compression for the Kinect" 

Last edited Mar 28, 2014 at 11:22 AM by marcdrossaers, version 19
