#pragma once
#include <string>
#include <vector>

struct NoteEvent {
  double startTime;
  double endTime;
  std::string note;
  double confidence; // Porcentaje de frames consistentes
};

class NoteTracker {
public:
  NoteTracker(double minNoteDuration = 0.1, double maxGap = 0.1,
              double silenceThreshold = 0.2);

  void processFrame(double time, const std::string &note);
  void finalize(double totalTime);
  std::vector<NoteEvent> getNoteEvents() const;

private:
  double minNoteDuration_;
  double maxGap_;
  double silenceThreshold_;
  std::string currentNote_;
  double currentStart_;
  double lastNoteTime_;
  int currentNoteFrames_;
  int currentSegmentFrames_;
  std::vector<NoteEvent> events_;

  void finishCurrentNote(double endTime);
};
