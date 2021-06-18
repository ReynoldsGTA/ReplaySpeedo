#include "plugin.h"
#include "CPed.h"
#include "CBike.h"
#include "CVehicle.h"
#include "CReplay.h"
#include "CFont.h"
#include "CWorld.h"
#include "extensions/ScriptCommands.h"
using namespace plugin;

#define MAX_FRAMES 10

class ReplaySpeedo {
public:

    static CPed* m_player;
    static CVehicle* m_vehicle;

    static CVector m_oldPos;

    static unsigned m_currentFrame;
    static float m_speeds[MAX_FRAMES];
      

    static void process() {
        //this will be processed once per frame
        m_player = FindPlayerPed();
        m_vehicle = FindPlayerVehicle(0, false);

        if (m_vehicle) {
            CFont::SetColor(CRGBA(200, 200, 200, 255));
            CFont::SetBackground(false, false);
            CFont::SetWrapx(500.0f);
            CFont::SetFontStyle(FONT_SUBTITLES);

            char text[16];
            CVector newPos = m_vehicle->GetPosition();
            CVector v = newPos - m_oldPos;

            //Compute running average of MAX_FRAMES frames to get a good approximation of speed
            m_speeds[m_currentFrame % MAX_FRAMES] = v.Magnitude();
            float avg = 0.0;
            for (size_t i = 0; i < MAX_FRAMES; ++i) {
                avg += m_speeds[i];
            }
            avg /= float(MAX_FRAMES);

            sprintf(text, "%.2f km/h", avg * 100.0);
            CFont::PrintString(50.0f, 100.0f, text);

            m_currentFrame++;

            m_oldPos = newPos;
        }
    }

    ReplaySpeedo() {
        //add process function to the game so that it will run it every tick
        Events::gameProcessEvent.Add(process);
    }
    
} replaySpeedo;


CPed* ReplaySpeedo::m_player;
CVehicle* ReplaySpeedo::m_vehicle;

CVector ReplaySpeedo::m_oldPos;

unsigned ReplaySpeedo::m_currentFrame;
float ReplaySpeedo::m_speeds[MAX_FRAMES];