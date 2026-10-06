#pragma once

#include <Arduino.h>

// Nova v1 reference geometry.
// Initial values match the SpotMicro / Thingiverse 3445283 frame family.
// All dimensions are metres.

constexpr float NOVA_HIP_LINK_M = 0.0550f;
constexpr float NOVA_UPPER_LEG_M = 0.1075f;
constexpr float NOVA_LOWER_LEG_M = 0.1300f;
constexpr float NOVA_BODY_LENGTH_M = 0.1860f;
constexpr float NOVA_BODY_WIDTH_M = 0.0780f;

constexpr float NOVA_STAND_HEIGHT_M = 0.1550f;
constexpr float NOVA_STAND_FRONT_X_OFFSET_M = 0.0150f;
constexpr float NOVA_STAND_REAR_X_OFFSET_M = 0.0000f;

constexpr float NOVA_CRAWL_STEP_LENGTH_M = 0.0400f;
constexpr float NOVA_CRAWL_CLEARANCE_M = 0.0500f;
constexpr float NOVA_CRAWL_FORWARD_SHIFT_M = 0.0350f;
constexpr float NOVA_CRAWL_REAR_SHIFT_M = 0.0050f;
constexpr float NOVA_CRAWL_SIDE_SHIFT_M = 0.0150f;

// Set true only after measuring the physical Nova build.
constexpr bool NOVA_GEOMETRY_CONFIRMED = false;
