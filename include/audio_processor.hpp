#pragma once
#include "note_tracker.hpp"
#include "pitch_detector.hpp"
#include <deque>
#include <string>
#include <vector>

class AudioProcessor {
public:
  AudioProcessor(int frameSize = 2048, int hopSizeDivider = 4);

  bool processFile(const std::string &filename);
  const std::vector<NoteEvent> &getNoteEvents() const;
  bool saveNoteEventsToFile(const std::string &filename) const;
  void printNoteEvents();

private:
  int frameSize_;
  int hopSize_;
  std::vector<NoteEvent> noteEvents_;
  std::deque<std::string> noteHistory_;
  void processFrame(double time, const std::vector<double> &frame,
                    PitchDetector &detector, NoteTracker &tracker);
  std::string smoothNote(const std::string &currentNote);
};
