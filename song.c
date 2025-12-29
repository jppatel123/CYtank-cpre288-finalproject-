#include "open_interface.h"
#include "timer.h"
#include "song.h"

void tapeDetected()
{
    // ----- Song Notes for "Tape Detected" -----
    unsigned char notes[5] = {
        60, 62, 64, 65, 67  // Short melody with 5 notes
    };

    // ----- Song Durations ----- (quarter note = 32, eighth note = 16, half note = 64)
    unsigned char duration[5] = {
        16, 16, 16, 16, 16  // All eighth notes for quick play
    };

    // Load and play the song
    oi_loadSong(2, 5, notes, duration);  // Using song index 2 for "Tape Detected"
    oi_play_song(2);
}

void beep()
{
    unsigned char notes[3] = {
        60,60,60 // C5 (Middle C) for all 3 beeps
    };

    // ----- Song Durations for Quick Beeps -----
    unsigned char duration[3] = {
        16,16,16  // All eighth notes (quick beeps)
    };
    oi_loadSong(1,2,notes,duration);
    oi_play_song(1);
}
void finish()
{
    // ----- Song Notes for "All Target Destroyed" -----
    unsigned char notes[15] = {
        60, 64, 67, 67, 69, 72, 74, 76, 79, 79, 81, 83, 84, 84, 86
    };

    // ----- Song Durations ----- (quarter note = 32, eighth note = 16, half note = 64)
    unsigned char duration[15] = {
        32, 32, 32, 16, 16, 32, 32, 32, 32, 16, 16, 32, 32, 32, 64
    };

    // Load and play the song
    oi_loadSong(1, 15, notes, duration);
    oi_play_song(1);

}
