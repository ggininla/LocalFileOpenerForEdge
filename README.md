# Introduction 
The project includes an Edge extension and a standalone Win32 binary that work together to allow the user to open a file://... link in Windows File Explorer.

# Installation
As the extension is not available on Microsoft Edge Add-ons website yet, it needs to be installed manually.
Follow the steps below to install the extension manually:

1. Get a clone of this repository.
2. Start Microsoft Edge browser and navigate to edge://extensions/
3. Enable **Developer mode** by clicking the toggle on the side menu.
4. Click on **Load unpacked** button and choose folder **edge-extension**.
5. Go to folder **native-app-win32** and run **register-host-app.bat** to register the host application. In case the repository folder has been moved to a new location, the registration must be updated by running **register-host-app.bat** again in the new location. 

# Build
The extension does not need to be built. The JS scripts are loaded and executed by the browser.

The standalone application needs to be built with a C++ compiler. In the source folder a solution and a project file can be found for Microsoft Visual Studio IDE. It is a very straighforward project that does not require any third-party libraries. Only Windows standard built-in libraries are used. 

# Copyright
Copyright (c) 2023, Auckland Transport. All right reserves.