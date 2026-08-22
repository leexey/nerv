#pragma once

class IDemoFile
{
public:
    virtual void _Unknown_000(void) = 0;
    virtual void _Unknown_001(void) = 0;

    virtual int GetDemoStartTick(void) = 0; //:002

    virtual int GetDemoTick(void) = 0; //:003

    virtual void _Unknown_004(void) = 0;
    virtual void _Unknown_005(void) = 0;

    virtual void _Unknown_006(void) = 0;
    virtual void _Unknown_007(void) = 0;
    virtual void _Unknown_008(void) = 0;
    virtual void _Unknown_009(void) = 0;
    virtual void _Unknown_010(void) = 0;
    virtual bool IsPlayingDemo(void) = 0; //:011
    virtual bool IsDemoPaused(void) = 0; //:012

    // ...
};