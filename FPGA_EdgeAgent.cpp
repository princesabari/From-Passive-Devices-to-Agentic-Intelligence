/*********************************************************
 * File: FPGA_EdgeAgent.cpp
 * Description: Host code for an SoC that offloads
 * inference tasks to an FPGA accelerator.
 *********************************************************/
#include <iostream>
#include <string>
#include "FPGA_Driver.h" // hypothetical driver
#include "CameraCapture.h"

int main() {
    // init camera, FPGA
    initCamera();
    initFPGA();

    while(true) {
        Frame frame = captureFrame();
        // convert frame to FPGA-friendly buffer
        FPGAInput inputBuf = prepareInput(frame);

        runFPGAInference(inputBuf);

        FPGAOutput result = getFPGAOutput();
        if(result.classID == 1 && result.probability > 0.9) {
            // object or anomaly detected
            triggerRejectActuator();
        }
        // possibly log or publish to MQTT
    }
    return 0;
}
