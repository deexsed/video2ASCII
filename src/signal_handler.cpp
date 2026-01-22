#include "signal_handler.h"
#include <csignal>

namespace video2ascii {

std::atomic<bool> SignalHandler::exitRequested_{false};

void SignalHandler::initialize() {
    signal(SIGINT, SignalHandler::handleSignal);
    signal(SIGTERM, SignalHandler::handleSignal);
    
#ifdef _WIN32
    signal(SIGBREAK, SignalHandler::handleSignal);
#else
    signal(SIGHUP, SignalHandler::handleSignal);
#endif
}

void SignalHandler::handleSignal(int signal) {
    (void)signal; // Подавляем предупреждение о неиспользуемом параметре
    exitRequested_ = true;
}

bool SignalHandler::shouldExit() {
    return exitRequested_.load();
}

void SignalHandler::reset() {
    exitRequested_ = false;
}

void SignalHandler::requestExit() {
    exitRequested_ = true;
}

} // namespace video2ascii
