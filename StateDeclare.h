#include "StateManager.h"
namespace IMM{
    class M_Idle : public IMotion
    {
    public:
        void enter(MotionManager &sm) override
        {
            
        }

        void update(MotionManager &sm) override
        {
            // 空闲状态的更新逻辑
        }

        void exit(MotionManager &sm) override
        {
            // 退出空闲状态的逻辑
        }
    };

}