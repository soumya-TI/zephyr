/**
 * @file thermostat_config.h
 *
 * Build-time switches for behaviour that may not survive contact with the
 * target hardware. Anything here can be turned off without touching the UI:
 * the XML is authored so that every one of these features is an enhancement
 * over a working baseline, never a load-bearing part of it.
 */

#ifndef THERMOSTAT_CONFIG_H
#define THERMOSTAT_CONFIG_H

/**
 * Cross-dissolve the Home background and ring colour when heating or cooling
 * starts or stops.
 *
 * At 0 the whole feature compiles out — no animation code, no per-frame
 * blending, zero flash — and the backgrounds swap instantly via the XML
 * bindings, which is correct on its own. A switch because a 320x480
 * cross-dissolve blends two full-screen images every frame, and whether that
 * holds frame rate is a question for the board.
 */
#define THERMOSTAT_USE_TRANSITIONS   0

/** Cross-dissolve duration, milliseconds.
 *  450 rather than the 220 this started at: a background tint is a large, low
 *  contrast change, and at 220ms it registered as a jump rather than as a
 *  dissolve. Long enough to read, short enough not to lag the button press
 *  that caused it. */
#define THERMOSTAT_TRANSITION_MS     450

/** Vertical travel, pixels. The incoming background rises while the outgoing one
 *  is pushed down, giving the change a direction. The two differ because both
 *  images are full-bleed: while both are moving neither covers the top of the
 *  screen, and the exposed strip is as tall as where the travelling edges cross.
 *  Weighting the movement to the incoming image buys visible motion for a
 *  smaller gap. */
#define THERMOSTAT_TRANSITION_SLIDE_IN   80
#define THERMOSTAT_TRANSITION_SLIDE_OUT  100

#endif /*THERMOSTAT_CONFIG_H*/
