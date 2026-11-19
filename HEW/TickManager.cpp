#include "TickManager.h"

void TickManager::Reset()
{
    currentTick = 1;
    tickTimer = 0.0f;
    tickLoopCount = 1;
}

void TickManager::Update(float deltaTime, CountJudgeMock& judgeSystem)
{
    tickTimer += deltaTime;

    if (tickTimer >= tickInterval)
    {
        tickTimer = 0.0f;
        currentTick++;

        if (currentTick > GRID_WIDTH)
        {
            currentTick = 1;
            tickLoopCount++;
        }
    }

    judgeSystem.EvaluateColumn(currentTick);
}

int TickManager::GetCurrentTick() const
{
    return currentTick;
}

int TickManager::GetTickLoopCount() const
{
    return tickLoopCount;
}