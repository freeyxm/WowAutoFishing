#pragma once
#pragma execution_character_set("utf-8")

#include "Util\MouseUtil.h"
#include "Util\KeyboardUtil.h"
#include "Util\ImageUtil.h"
#include "SoundListener.h"

enum class FishingState
{
	State_None,
	State_Start,
	State_CheckState, // ���״̬
	State_Bait, // �����
	State_ThrowPole, // ˦��
	State_FindFloat, // ��λ��Ư
	State_WaitBite, // �ȴ�ҧ��
	State_Shaduf, // ���
	State_End,
};

const int SEC_PER_MINUTE = 60;
const int MAX_BAIT_TIME = 10 * SEC_PER_MINUTE; // �������ʱ�䣬��λ��
const int MAX_WAIT_TIME = 20; // ��ȴ�ҧ��ʱ�䣬��λ��

const int SWITCH_TIME_MIN = 100; // ״̬�л��������λ����
const int SWITCH_TIME_MAX = 900; // ״̬�л��������λ����

const POINT FLOAT_OFFSET = { 10, 25 }; // ��Ưƫ�ƣ��Ա������У�1024x768����ģʽ��

class Fisher
{
public:
	Fisher(HWND hwnd, int x, int y, int w, int h);
	~Fisher();

	bool Init();

	void StartFishing();

	void NotifyBite();
	bool CheckTimeout();

private:
	void ActiveWindow();

	bool CheckBaitTime();
	bool DoBait();
	bool DoThrowPole();
	bool DoFindFloat();
	bool DoWaitBite();
	bool DoShaduf();

private:
	HWND m_hwnd; // ħ�����細�ھ��
	int m_posX, m_posY;
	int m_width, m_height;
	FishingState m_state;
	long m_waitTime; // �ȴ�ʱ��
	long m_baitTime; // �϶�ʱ��
	long m_throwTime; // ˦��ʱ��
	bool m_hasBite; // �Ƿ���ҧ��
	POINT m_floatPoint;

	int m_throwCount;
	int m_findFloatFailCount;
	int m_timeoutCount;

	char *m_lpBits; // ����λͼ��������
	std::list<POINT> m_points;
	
	MouseUtil m_mouse;
	KeyboardUtil m_keyboard;
	SoundListener m_sound;
};

