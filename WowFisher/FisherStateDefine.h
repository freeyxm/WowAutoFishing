#pragma once

enum FisherStateType
{
    FisherState_Start,
    FisherState_CheckBait, // 检查状态
    FisherState_Bait, // 上鱼饵
    FisherState_ThrowPole, // 甩竿
    FisherState_FindFloat, // 定位浮标
    FisherState_WaitBite, // 等待咬钩
    FisherState_Shaduf, // 提竿
    FisherState_WaitFloatHide, // 等待浮标消失
    FisherState_Jump, // 跳跃
};
