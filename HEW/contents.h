#pragma once

static constexpr int upperWidth = 5;
static constexpr int upperHeight = 3;
static constexpr int upperBlockState = 4;
static constexpr int upperFixing = 0;
static constexpr int upperMoving = 1;
static constexpr int upperSelecting = 2;
static constexpr int upperRecording = 3;
static constexpr float upperBlockSize = 50;

static constexpr float totalBlockWidth = upperWidth * upperBlockSize;

static constexpr int lowerWidth = 11;
static constexpr int lowerHeight = 7;
static constexpr int lowerBlockState = 2;
static constexpr int lowerUnselected = 0;
static constexpr int lowerSelected = 1;
static constexpr float lowerBlockSize = totalBlockWidth / lowerWidth;

static constexpr int blocklistWidth = 3;
static constexpr int blocklistHeight = 3;