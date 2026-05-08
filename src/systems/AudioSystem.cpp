#include "AudioSystem.hpp"
#include <iostream>

AudioSystem::AudioSystem() {}

bool AudioSystem::load(const std::string &assetsPath) {
  std::string soundsPath = assetsPath + "/sounds/";

  struct SoundFile {
    SoundId id;
    std::string file;
  };
  std::vector<SoundFile> files = {
      {SoundId::Waka, "waka.wav"},
      {SoundId::Power, "power.wav"},
      {SoundId::GhostEat, "ghost_eat.wav"},
      {SoundId::Death, "death.wav"},
      {SoundId::ExtraLife, "extra_life.wav"},
      {SoundId::LevelClear, "level_clear.wav"},
  };

  bool allLoaded = true;
  for (auto &f : files) {
    if (!buffers[f.id].loadFromFile(soundsPath + f.file)) {
      std::cerr << "Failed to load sound: " << f.file << "\n";
      allLoaded = false;
    } else {
      sounds.emplace(f.id, sf::Sound(buffers[f.id]));
    }
  }

  // siren is streaming music
  if (!siren.openFromFile(soundsPath + "siren.wav")) {
    std::cerr << "Failed to load siren.wav\n";
  } else {
    siren.setLooping(true);
    siren.setVolume(40.f);
    sirenLoaded = true;
  }

  return allLoaded;
}

void AudioSystem::play(SoundId id) {
  if (muted)
    return;
  auto it = sounds.find(id);
  if (it != sounds.end())
    it->second.play();
}

void AudioSystem::startSiren() {
  if (muted || !sirenLoaded)
    return;
  if (siren.getStatus() != sf::Music::Status::Playing)
    siren.play();
}

void AudioSystem::stopSiren() { siren.stop(); }

void AudioSystem::setSirenPitch(float pitch) { siren.setPitch(pitch); }

void AudioSystem::setMuted(bool m) {
  muted = m;
  if (muted)
    siren.stop();
}
