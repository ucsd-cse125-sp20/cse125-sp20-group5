#include <inih/INIReader.h>
#include "ClientParams.h"

ClientParams::ClientParams() {
    INIReader config("ClientConfig.ini");

    if (config.ParseError() != 0) {
        throw std::exception("Cannot Load file ClientConfig.ini ");
    }

    // Sound
    BGMVolume = config.GetFloat("Sound", "BGMVolume", 0.02f);

    // Scaler
    seedSourceScaler = config.GetFloat("Scaler", "SeedSourceScaler", 0.2f);
    dirtScaler = config.GetFloat("Scaler", "DirtScaler", 0.05f);;
    treeScaler = config.GetFloat("Scaler", "TreeScaler", 0.3f);;
    stoneScaler = config.GetFloat("Scaler", "StoneScaler", 0.8f);;
    playerScaler = config.GetFloat("Scaler", "PlayerScaler", 0.27f);

    // Camera
    FOV = config.GetFloat("Camera", "FOV", 45.0f);
    aspect = config.GetFloat("Camera", "Aspect", 1.00f);;
    nearClip = config.GetFloat("Camera", "NearClip", 0.1f);
    farClip = config.GetFloat("Camera", "FarClip", 200.0f);
    distance = config.GetFloat("Camera", "Distance", 19.5f);
    azimuth = config.GetFloat("Camera", "Azimuth", 0.0f);
    incline = config.GetFloat("Camera", "Incline", 54.0f);

    // Others
    timeDifference = config.GetFloat("Others", "TimeDifference", 0.1f);
}