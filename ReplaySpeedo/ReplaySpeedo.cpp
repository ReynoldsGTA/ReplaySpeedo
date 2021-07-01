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
    static float m_tryValue;
    static int m_tryValue2;

    static void process() {
        //this will be processed once per frame
        m_player = FindPlayerPed();
        m_vehicle = FindPlayerVehicle(0, false);

        m_tryValue = 0.0;

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


            m_tryValue = fmin((avg *100.0) / 200.0, 1.0);
            //m_vehicle->m_fMovingSpeed = avg;
            //CVector s = m_vehicle->m_vecMoveSpeed;
            //m_vehicle->m_vecMoveSpeed = v;
            //patch::SetChar(0x460578, 3);
            

            sprintf(text, "%.2f km/h", avg * 100.0);
            CFont::PrintString(50.0f, 100.0f, text);

            m_currentFrame++;

            m_oldPos = newPos;
        }
    }

    ReplaySpeedo() {
        m_tryValue = 0.0;
        m_tryValue2 = 1;
        
        //replace original code with jmp to code cave below
        //jmp 00747C60
        //233 107 75 4 0 144
        patch::SetChar(0x7030F0, 233);
        patch::SetChar(0x7030F1, 107);
        patch::SetChar(0x7030F2, 75);
        patch::SetChar(0x7030F3, 4);
        patch::SetChar(0x7030F4, 0);
        patch::SetChar(0x7030F5, 144);
        //patch::SetPointer(0x747C62, &m_tryValue);
        


        //populate code cave with code that reads our speed value
        //fld dword ptr[008D51E0]
        //fcomp dword ptr[edx]
        //jmp 007030F6
        //217 5 224 81 141 0 216 26 233 137 180 251 255 144
        patch::SetChar(0x747C60, 217);
        patch::SetChar(0x747C61, 5);
        patch::SetChar(0x747C62, 224);
        patch::SetChar(0x747C63, 81);
        patch::SetChar(0x747C64, 141);
        patch::SetChar(0x747C65, 0);
        patch::SetChar(0x747C66, 216);
        patch::SetChar(0x747C67, 26);
        patch::SetChar(0x747C68, 233);
        patch::SetChar(0x747C69, 137);
        patch::SetChar(0x747C6A, 180);
        patch::SetChar(0x747C6B, 251);
        patch::SetChar(0x747C6C, 255);
        patch::SetChar(0x747C6D, 144);
        //217 5 224 81 141 0 216 26 233 137 180 251 255 144
        patch::SetPointer(0x747C62, &m_tryValue);


        //add process function to the game so that it will run it every tick
        Events::gameProcessEvent.Add(process);
    }
    
} replaySpeedo;


CPed* ReplaySpeedo::m_player;
CVehicle* ReplaySpeedo::m_vehicle;

CVector ReplaySpeedo::m_oldPos;


float ReplaySpeedo::m_tryValue;
int ReplaySpeedo::m_tryValue2;

unsigned ReplaySpeedo::m_currentFrame;
float ReplaySpeedo::m_speeds[MAX_FRAMES];

