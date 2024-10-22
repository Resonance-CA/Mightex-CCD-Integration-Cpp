// MightexCcdInitialization.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <iostream>
#include <conio.h>  // Include for _kbhit

#include "CCD_USBCamera_SDK.h"
using namespace std;

int main()
{
    cout << "Trying to Detect Devices ...";
    int numDevices = 0;
    try {
        numDevices = CCDUSB_InitDevice();
        cout << "Initialization result: " << numDevices << " devices found." << endl;
    }
    catch (const exception& e) {
        cerr << "Exception occurred: " << e.what() << endl;
    }

    // Perform operations only if devices are detected
    if (numDevices > 0) {
        // Example operation: Add first device to working set
        int result = CCDUSB_AddDeviceToWorkingSet(1);
        if (result != 1) {
            cerr << "Failed to add device to working set." << endl;
        }
        else {
            cout << "Device added to working set successfully." << endl;

            // Getting module number and serial number
            char moduleNo[16] = { 0 };  // Buffer for module number
            char serialNo[16] = { 0 };  // Buffer for serial number
            int infoResult = CCDUSB_GetModuleNoSerialNo(1, moduleNo, serialNo);
            if (infoResult == 1) {
                cout << "Device 1 Module No: " << moduleNo << ", Serial No: " << serialNo << endl;
            }
            else {
                cerr << "Failed to get module and serial number for Device 1." << endl;
            }

            // Set exposure time
            int exposureTime = 10000;  // 10000 microseconds (10 milliseconds)
            int exposureResult = CCDUSB_SetExposureTime(1, exposureTime, 0);
            if (exposureResult != 1) {
                cerr << "Failed to set exposure time." << endl;
            }
            else {
                cout << "Exposure time set to " << exposureTime << " microseconds successfully." << endl;
            }

            // Assuming 0x8888 is the value for continuous operation
            if (CCDUSB_StartCameraEngine(0) == 1) {
                if (CCDUSB_StartFrameGrab(0x8888) != 1) {
                    cerr << "Failed to start continuous frame grab." << endl;
                }
                else {
                    cout << "Continuous frame grabbing started." << endl;
                }
            }
            else {
                cerr << "Failed to start camera engine." << endl;
            }

            cout << "Continuous frame grabbing started. Press any key to stop..." << endl;

            // Frame fetching and processing loop
            unsigned short* framePtr = nullptr;
            framePtr = CCDUSB_GetCurrentFrame(1, framePtr);
            while (!_kbhit()) { // Loop until keyboard hit
                if (framePtr != NULL) {
                    unsigned short* metaData = framePtr;
                    cout << "New Frame Captured:" << endl;
                    for (int i = 0; i < 8; ++i) { // Assuming 8 words of metadata
                        cout << "MetaData[" << i << "]: " << metaData[i] << endl;
                    }

                    unsigned short* rawData = framePtr + 8; // Skip metadata
                    int rawDataLength = 3648; // for TCX-1304-X
                    unsigned long pixelSum = 0;
                    for (int i = 0; i < rawDataLength; ++i) {
                        pixelSum += rawData[i];
                    }
                    cout << "Average pixel value: " << (pixelSum / rawDataLength) << endl;
                }
                else {
                    cerr << "Failed to capture frame." << endl;
                }
                Sleep(100); // Sleep to slow down the output rate
            }

            CCDUSB_StopFrameGrab();
            CCDUSB_UnInitDevice();


        }
    }

    // Uninitialize the device if any were initialized
    if (numDevices > 0) {
        CCDUSB_UnInitDevice();
    }

    
}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
