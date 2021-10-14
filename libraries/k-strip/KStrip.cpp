
#include <KStrip.h>


//----------------------------------------------------------------------
// NeoPixelTilesTest
// This will display specific colors in specific locations on the led panels
//
// This is useful in confirming the layout of your panels
//
// It does require that you have the actual panels connected
//----------------------------------------------------------------------

#include <NeoPixelAnimator.h>
#include <NeoPixelBus.h>

// uncomment one of these that matches your panel pixel layouts and
// how you want them rotated.  Not all the rotations are listed here
// but you can modifiy the name to include the rotation of 90,180, or 270.

typedef ColumnMajorAlternatingLayout MyPanelLayout;
// typedef ColumnMajorLayout MyPanelLayout;
// typedef RowMajorAlternatingLayout MyPanelLayout;
// typedef RowMajorLayout MyPanelLayout;
// typedef RowMajor90Layout MyPanelLayout; // note rotation 90 was used

// change this to be one of the layouts which will define the layout
// of the panels themselves
typedef ColumnMajorLayout MyTilesLayout;

// make sure to set these panel values to the sizes of yours
const uint8_t PanelWidth = 8;  // 8 pixel x 8 pixel matrix of leds
const uint8_t PanelHeight = 8;
const uint8_t TileWidth = 4;  // laid out in 4 panels x 2 panels mosaic
const uint8_t TileHeight = 2;

const uint16_t PixelCount = 100;
const uint8_t PixelPin = 2;  


//NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
//NeoPixelBus<NeoRgbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
//NeoPixelBus<NeoRgbwFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
// for esp8266 omit the pin
///Brg
NeoPixelBus<NeoBrgFeature, NeoWs2811Method> strip(PixelCount);
//NeoPixelBus<NeoRgbFeature, NeoWs2811Method> strip(PixelCount);

RgbColor red(128, 0, 0);
RgbColor green(0, 128, 0);
RgbColor blue(0, 0, 128);
RgbColor white(128);
// if using NeoRgbwFeature above, use this white instead to use
// the correct white element of the LED
//RgbwColor white(128); 
RgbColor black(0);


void KStrip::init() {
    strip.Begin();
    strip.Show();
}

void KStrip::update() {
    strip.ClearTo(red);
    strip.Show();

    delay(1000);

    strip.ClearTo(green);
    strip.Show();

    delay(500);

    strip.ClearTo(blue);
    strip.Show();

    delay(500);
    strip.ClearTo(black);
    strip.Show();
}

