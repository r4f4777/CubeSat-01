#include "MissionManager.h"

#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <windows.h>

uint32_t calculateCRC32(const std::string& data) {
    uint32_t crc = 0xFFFFFFFFu;

    for (unsigned char byte : data) {
        crc ^= byte;

        for (int i = 0; i < 8; ++i) {
            if (crc & 1u) {
                crc = (crc >> 1) ^ 0xEDB88320u;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc ^ 0xFFFFFFFFu;
}

uint32_t calculateMissionCRC(const PhotoCommand& command, CommandState state) {
    std::string data = "CUBESAT_MISSION_V1\n" +
        command.target + "\n" +
        std::to_string(command.startTime) + "\n" +
        std::to_string(command.endTime) + "\n" +
        std::to_string(static_cast<int>(state)) + "\n";

    return calculateCRC32(data);
}

MissionManager::MissionManager(const std::string& filename)
    : filename(filename) {
}

bool MissionManager::schedulePhoto(const PhotoCommand& command) {
        if (command.target.empty() ||
            command.target.find('\n') != std::string::npos ||
            command.target.find('\r') != std::string::npos ||
            command.startTime < 0 ||
            command.startTime >= command.endTime) {
            return false;
        }

        if (hasPendingCommand) {
            return false;
        }

        pendingCommand = command;
        hasPendingCommand = true;
        commandState = CommandState::PENDING;

        if (!saveCommand()) {
            hasPendingCommand = false;
            return false;
        }

        return true;
    }

    bool MissionManager::isPhotoWindowOpen(int currentTime) const {
        if(hasPendingCommand){
            if(currentTime >= pendingCommand.startTime && currentTime <= pendingCommand.endTime){
                return true;
            }
        }
        return false;
    }

    bool MissionManager::hasExpired(int currentTime) const {
        return hasPendingCommand && commandState == CommandState::PENDING && currentTime > pendingCommand.endTime;    
    }

    bool MissionManager::completeCommand() {

        if(!hasPendingCommand){
            return false;
        }
        if(commandState != CommandState::EXECUTING){
            return false;
        }

        commandState = CommandState::COMPLETED;
        if (!saveCommand()) {
            commandState = CommandState::EXECUTING;
            return false;
        }

        hasPendingCommand = false;
        return true;

    }


    bool MissionManager::expireCommand() {
        if(!hasPendingCommand){
            return false;
        }
        if(commandState != CommandState::PENDING){
            return false;
        }
        commandState = CommandState::EXPIRED;
        if (!saveCommand()) {
            commandState = CommandState::PENDING;
            return false;
        }
        hasPendingCommand = false;
        return true;
    }


    bool MissionManager::saveCommand() const {
        uint32_t storedChecksum;
        if(!hasPendingCommand){
            return false;
        }
        std::ofstream file(filename + ".tmp");
        if(!file.is_open()){
            return false;
        }

        storedChecksum = calculateMissionCRC(pendingCommand, commandState);

        file << "CUBESAT_MISSION_V1" << '\n';
        file << pendingCommand.target << '\n';
        file << pendingCommand.startTime << '\n';
        file << pendingCommand.endTime << '\n';
        file << static_cast<int>(commandState) << '\n';
        file << storedChecksum << '\n';
        file.close();
        if(!file.good()){
            return false;
        }
        if(!MoveFileExA((filename + ".tmp").c_str(), filename.c_str(), MOVEFILE_REPLACE_EXISTING)){
            return false;
        }
        return true;
    }

    bool MissionManager::loadCommand() {
        PhotoCommand recovered;
        uint32_t storedChecksum = 0;
        int recoveredState = 0;
        std::string version;
        std::ifstream file(filename);

        if (!file.is_open()) {
            return false;
        }

        if (!std::getline(file, version)) {
            return false;
        }

        if (version != "CUBESAT_MISSION_V1") {
            return false;
        }

        if (!std::getline(file, recovered.target)) {
            return false;
        }

        file >> recovered.startTime >> recovered.endTime >> recoveredState >> storedChecksum;

        if (!file) {
            return false;
        }

        file >> std::ws;

        if (file.peek() != std::char_traits<char>::eof()) {
            return false;
        }

        if (recovered.target.empty()) {
            return false;
        }
        if (recovered.startTime >= recovered.endTime) {
            return false;
        }

        if (recovered.startTime < 0) {
            return false;
        }
        
        if (recoveredState < 0 || recoveredState > 3) {
            return false;
        }
       if(storedChecksum != calculateMissionCRC(recovered, static_cast<CommandState>(recoveredState))){
            return false;
       }

        pendingCommand = recovered;
        commandState = static_cast<CommandState>(recoveredState);

        if(recoveredState == 0 || recoveredState == 1){
            hasPendingCommand = true;
        }
        else{
            hasPendingCommand = false;
        }

        return true;
    }

    CommandState MissionManager::getCommandState() const {
        return commandState;
    }

    bool MissionManager::beginExecution() {
        if(!hasPendingCommand){
            return false;
        }
        if (commandState != CommandState::PENDING) {
            return false;
        }
        commandState = CommandState::EXECUTING;
        if(!saveCommand()){
            commandState = CommandState::PENDING;
            return false;
        }
        return true;
    }

    bool MissionManager::needsReview() const {
        return hasPendingCommand &&
            commandState == CommandState::EXECUTING;
    }