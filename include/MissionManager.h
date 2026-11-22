#pragma once

#include <deque>
#include <string>

enum class CommandState {
    PENDING,
    EXECUTING,
    COMPLETED,
    EXPIRED
};

struct PhotoCommand {
    std::string target;
    int startTime;
    int endTime;
};


class MissionManager {
private:
    bool hasPendingCommand = false;
    PhotoCommand pendingCommand;
    CommandState commandState = CommandState::PENDING;
    std::string filename;

public:

    explicit MissionManager(
        const std::string& filename = "mission.dat"
    );
    
    bool schedulePhoto(const PhotoCommand& command);

    bool isPhotoWindowOpen(int currentTime) const;

    bool hasExpired(int currentTime) const;

    bool completeCommand();

    bool expireCommand();

    bool saveCommand() const;

    bool loadCommand();

    CommandState getCommandState() const;

    bool beginExecution();

    bool needsReview() const;
};