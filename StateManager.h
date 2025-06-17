// 状态管理系统
#include "IDebug.h"
namespace IMM
{
    class IMotion
    {
        virtual void enter(MotionManager &sm) = 0;
        virtual void update(MotionManager &sm) = 0;
        virtual void exit(MotionManager &sm) = 0;
    };

    class MotionManager
    {
        IMotion MotionSchedule[3];
        uint8_t currentMotionIndex = 0;
        enum class MotionState : uint8_t { NotStarted, InProgress, Completed };
        MotionState currentMotionState = MotionState::NotStarted;

    public:
        void addNextState(IMotion newMotion)
        {
            if (currentMotionIndex < 3)
            {
                MotionSchedule[currentMotionIndex] = newMotion;
                currentMotionIndex++;
            }
            debugF("Can't add more states.");
        }

        void update()
        {
            IMotion *currentState = &MotionSchedule[currentMotionIndex];
            if (currentState == nullptr)
                return;
            
            switch (currentMotionState)
            {
            case MotionState::NotStarted:
                currentState->enter(*this);
                break;
            case MotionState::InProgress:
                currentState->update(*this);
                break;
            case MotionState::Completed:
                currentState->exit(*this);
                break;
            default:
                break;
            }
        }
    }
}