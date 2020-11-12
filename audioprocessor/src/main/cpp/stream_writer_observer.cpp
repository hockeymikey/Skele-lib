#include "stream_writer_observer.hpp"

void CAP::StreamWriterObserver::streamWriterStopped() {
    /*if (bridge.onHighlightStopped != nullptr) {
        NSString *path = [NSString stringWithUTF8String: filePath.c_str()];
        bridge->audioprocessor->onHighlightStopped(path);
    }*/
}
void CAP::StreamWriterObserver::streamWriterKilled() {
    /*if (bridge.onHighlightKilled != nullptr) {
        NSString *path = [NSString stringWithUTF8String: filePath.c_str()];
        bridge.onHighlightKilled(path);
    }*/
}