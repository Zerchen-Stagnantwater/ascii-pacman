#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <unordered_map>

enum class SoundId {
  Waka,
  Power,
  GhostEat,
  Death,
  ExtraLife,
  LevelClear,
};

class AudioSystem {
public:
  AudioSystem();
  bool load(const std::string &assetsPath);

  void play(SoundId id);
  void startSiren();
  void stopSiren();
  void setSirenPitch(float pitch);
  void setMuted(bool muted);
  bool isMuted() const { return muted; }

private:
  std::unordered_map<SoundId, sf::SoundBuffer> buffers;
  std::unordered_map<SoundId, sf::Sound> sounds;
  sf::Music siren;
  bool muted = false;
  bool sirenLoaded = false;
};
