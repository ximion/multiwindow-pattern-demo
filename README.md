# Multiwindow Pattern Demo

This is a small application that was quickly thrown together to demonstrate some multiwindow
SDI UX patterns all at once.

It highlights some X11 deficiencies (including ugly workarounds) and serves as a playground
to check if an implementation of `ext-positioning` in Wayland works correctly (provided Qt implements
the necessary protocol).
It has originally been used to mock protocol interaction and test Wayland protocol viability
using a small, hackable application instead of relying on actual applications.

In reality, no application will likely use all of these multiwindow paradigms all at once.


## Implemented Paradigms

### Initial Window Placement

<a href="graphics/demo/initial-layout.avif">
  <img src="graphics/demo/initial-layout.avif" style="max-height:600px;"
    alt="Multiple windows with correct initial placement" />
</a>

Upon launching the application, windows should appear correctly placed in a certain arrangement,
occupying about 90% of the space designated for the application.

### Layout Profiles

<a href="graphics/demo/layout-profiles.avif">
  <img src="graphics/demo/layout-profiles.avif" style="max-height:600px;"
    alt="Window layouts can be created and restored" />
</a>

Window layouts can be created in arbitrary amounts and restored at any time.
The layouts may be made by the user, or defined by the developer of the application
in a fixed list.
This is a very common pattern in many scientific applications.

### Floating Windows

<a href="graphics/demo/float-on-top.avif">
  <img src="graphics/demo/float-on-top.avif" style="max-height:600px;"
    alt="Window can permanently float on top" />
</a>

A window can be set to float permanently on top of all other windows of the same application.
This is usually done for toolboxes and palettes and heavily used in DAW applications.

### Window creation at coordinates

<a href="graphics/demo/window-at-cursor.avif">
  <img src="graphics/demo/window-at-cursor.avif" style="max-height:600px;"
    alt="Window is created at cursor position" />
</a>

A window is created at the current cursor position, or at the position of any other
UI element of the application, with a defined offset.


### Window snapping / attachment

<a href="graphics/demo/snap-attach.avif">
  <img src="graphics/demo/snap-attach.avif" style="max-height:600px;"
    alt="Permanently attach / snap windows to other windows" />
</a>

This pattern has been popular in older mediaplayer applications and has fallen a bit
out of favour in many modern applications.
It is a useful testcase though. A window permanently attaches to the side of another window
and can be dragged around with it. The windows also minimize together and orient
each other in relation to each other when their states change.
The secondary window is usually some kind of toolbox with content that is strongly related
to the primary window.

## Compiling the demo application

Ensure Qt6 and Meson are installed, then use the following commands:
```bash
# build
mkdir build && cd build
meson setup ..
ninja
# run
./multiwindow-pattern-demo
```
