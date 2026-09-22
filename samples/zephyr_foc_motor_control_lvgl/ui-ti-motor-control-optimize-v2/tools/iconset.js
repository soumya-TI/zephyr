// Icon set for the AM13 motor-control HMI demo (LP-AM13E230, 320x480 portrait).
// Rasterised by ../../tools/icons.js from ../../tools/lucide-svg — see that file
// for the stroke-scaling rule (Lucide is drawn on a 24px grid with a 2px stroke,
// which renders muddy below ~16px, so stroke widens as the target shrinks).
//
//   node ../tools/icons.js tools/iconset.js images
//
// Everything here is rendered solid black with alpha and recoloured at runtime
// via image_recolor, so one asset serves ink / accent / warn / fault / on-ink.
// That matters more on a dark panel than a light one: the same glyph appears as
// dim grey in a tab bar and as full-white inside a filled button.
//
// [lucide name, output name, px, stroke-width]
module.exports = [
  // ── Tab bar, one per screen ────────────────────────────────────────────────
  // 22px is the ceiling for a 4-up bar on a 320px-wide panel: at 24 the labels
  // beneath them start colliding at the "Config" end.
  ['circle-gauge',        'icon_tab_run',     22, 2.0],
  ['chart-line',          'icon_tab_trend',   22, 2.0],
  ['triangle-alert',      'icon_tab_faults',  22, 2.0],
  ['sliders-horizontal',  'icon_tab_config',  22, 2.0],


  // ── Speed stepper ──────────────────────────────────────────────────────────
  // Sole content of two 56px touch targets, so they carry a heavier stroke than
  // their size alone would suggest — a hairline glyph in a large button reads
  // as disabled.
  ['minus',               'icon_minus',       24, 2.4],
  ['plus',                'icon_plus',        24, 2.4],

  // ── The hardware E-stop marker, left edge ──────────────────────────────────
  // Labels the physical push button on the base board. Never a touch target.
  ['octagon-x',           'icon_estop',       16, 2.4],


  // ── Direction, config screen ───────────────────────────────────────────────
  ['rotate-cw',           'icon_dir_cw',      16, 2.4],
  ['rotate-ccw',          'icon_dir_ccw',     16, 2.4],

  // ── Clear motor faults ─────────────────────────────────────────────────────
  ['rotate-ccw',          'icon_clear',       16, 2.4],

  // ── The checkbox tick ──────────────────────────────────────────────────────
  // Drawn as bg_image_src on the indicator's `checked` state, white on accent.
  // Stroke 3.0 at 14px: this is the one glyph that must survive being the only
  // mark inside a 22px box, and Lucide's default weight disappears there.
  ['check',               'icon_check',       14, 3.0],
];
