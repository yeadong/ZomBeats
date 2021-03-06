#include "AudioController.h"
#include "SongDecomposition.h"

using namespace cocos2d::experimental;

void AudioController::preload(){
	//Force cocos to cache our music because it doesn't let you do it manually
	AudioEngine::stop(AudioEngine::play2d("songs/01 Overdrive.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/01 Looking For.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/01 Pickles.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/BLANCO.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/ChillDeepHouse.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/Galactic_Grass.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/InPizzaWeCrust.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/Its_A_Trap.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/rf1.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/rf2.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/rf3.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/rf4.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/rf5.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/rf6.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/rf7.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/rf8.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/rf9.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/rf10.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/RobotsInTheRain.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/CalibrationLong.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/SimpleBeat.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/Cut Trance 110.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/Dance Monster 120.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/Drums of the Deep 153.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/EDM Detection Mode 128.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/Electro Sketch.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/Entice.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/Huge Room.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/Ouroboros 107.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/Overriding Concern 120.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/Prelude and Action 132.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/RetroFuture Clean 91.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/Secrets of the Schoolyard 100.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/Son of a Rocket 110.mp3", true, 0.0f));
	AudioEngine::stop(AudioEngine::play2d("songs/Variation on Egmont 111.mp3", true, 0.0f));
	//AudioEngine::stopAll();
}

AudioController::AudioController()
{
	currentSong = NULL;
	curSongId = 0;
	total_kept = 0;
	total_beats = 0;
	keepit = 0;
	estimated_song_time = 0;
	prevOnBeat = frameOnBeat = false;
	audioDelay = videoDelay = 0.0f;
	hisses.maxInstances = 3;
	hisses.name = "hisses";
	paused = true;
	//AudioEngine::setMaxAudioInstance(4);
}


AudioController::~AudioController()
{
}

void AudioController::playTrack(SongDecomposition *track, bool loop){
	AudioEngine::stopAll();
	currentSong = track;
	curSongId = AudioEngine::play2d(currentSong->trackName, loop, MUSIC_VOLUME);
	songTime = lastReportedSongTime = 0.0f;
	trackLoops = 0;
}

void AudioController::playEffect(std::string effect, float vol){
	if (effect == "sound_effects/ZombieHiss.mp3" || effect == "sound_effects/ZombieGrowl.mp3" || effect == "sound_effects/ZombieDeath1.mp3"){
		AudioEngine::play2d(effect, false, vol, &hisses);

	}
	else{
		AudioEngine::play2d(effect, false, vol);
	}
}

void AudioController::setFrameOnBeat(float deltaTime){
	//total_kept += elapsedTime - AudioEngine::getCurrentTime(curSongId);
	//total_beats += 1;
	//keepit = total_kept / total_beats;
	//estimated_song_time = elapsedTime - keepit;
	songTime += deltaTime;
	float curTime = AudioEngine::getCurrentTime(curSongId);
	if (curTime != lastReportedSongTime){
		if (curTime < lastReportedSongTime){
			songTime = curTime;
			trackLoops++;
		}
		else{
			songTime = (songTime + curTime) / 2;
		}
		lastReportedSongTime = curTime;
	}
	prevOnBeat = frameOnBeat;
	frameOnBeat = currentSong->isOnBeat(songTime - audioDelay - videoDelay) == 1;
}

int AudioController::wasOnBeat(float timeAgo){
	lastClickTime = songTime - timeAgo;
	return currentSong->isOnBeat(lastClickTime - audioDelay);
}

float AudioController::getBeatStart(){
	return currentSong->getBeatStart(lastClickTime - audioDelay - videoDelay);
}

float AudioController::timeToClosestBeat(){
	return currentSong->timeToClosestBeat(lastClickTime - audioDelay);
}

float AudioController::timeToBeat(int i){
	return currentSong->timeToBeat(lastClickTime - audioDelay, i);
}

bool AudioController::songIsOver(){
	return trackLoops > 0;
}

void AudioController::stop(){
	trackLoops++;
	AudioEngine::stopAll();
}

void AudioController::pauseSounds(){
	AudioEngine::pauseAll();
	paused = true;
}

void AudioController::resumeSounds(){
	AudioEngine::resumeAll();
	paused = false;
}