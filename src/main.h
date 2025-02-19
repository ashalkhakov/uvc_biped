﻿// representation of angles used by servos
#define HWANGLE_180	(2880)
#define HWANGLE_MULTIPLIER	(16.0f)
#define HWANGLE_RECIPROCAL	(1.0f/16.0f)
#define TO_HWANGLE(x)		((x) * HWANGLE_MULTIPLIER)
#define FROM_HWANGLE(x)		((x) * HWANGLE_RECIPROCAL)
#define HWANGLE_TORAD(x)	((float)x*(M_PI/(180.0*16.0)))

typedef struct state_s {
	int16_t K0W[2];			// 股関節前後方向書込用 For writing in the anteroposterior direction of the hip joint
	int16_t K1W[2];			// 股関節横方向書込用 For hip joint lateral writing
	int16_t K2W[2];			// 股関節横方向書込用 For hip joint lateral writing
	int16_t HW[2];			// 膝関節書込用 For knee joint writing
	int16_t A0W[2];			// 足首上下方向書込用 For writing in the upper and lower direction of the ankle
	int16_t A1W[2];			// 足首横方向書込用 For ankle lateral writing
	int16_t U0W[2];			// 肩前後方向書込用 For writing in shoulder anteroposterior direction
	int16_t U1W[2];			// 肩横後方向書込用 For shoulder lateral and posterior writing
	int16_t U2W[2];			// 肩ヨー向書込用 For shoulder yaw writing
	int16_t EW[2];			// 肘書込用 For elbow writing
	int16_t WESTW;			// 腰回転書込用 For waist rotation writing
	int16_t HEADW;			// 頭回転書込用 For head rotation writing
	int16_t K0R, K0RB, U0R, U0L, U1R, U1L, EWS;
	int16_t K0L, K0LB, U0WB;
	int16_t K0WB;
} state_t;

typedef struct core_s {
	int16_t jikuasi;
	int16_t motCt, motCtBak, motCtBak2, motCtdat;
	int16_t mode, modeNxt, subMode; // keyMode
	int16_t	pitch_gyr, roll_gyr, yaw_gyr;
	int16_t	cycle, tst0;
	int16_t	walkCt, walkCtLim;		// 歩数 number of steps
	int16_t	p_ofs, r_ofs;
	int16_t ir, ip, ira, ipa;
	int16_t irb, ipb, ct;
	// these angles are encoded as degree*16, eg. 180deg = 2880
	int16_t	pitchs, rolls, pitch_ofs, roll_ofs, yaw, yaw_ofs;
	// landB is always 0
	int16_t	landF, landB;

	int32_t	tBak, pitchi;
	uint32_t tNow;

	//uint8_t cmd[2];
	//uint8_t ff[45];
	//uint8_t dsp[110];
	//uint8_t krr[4];
	//int8_t	kn;
	int8_t	LEDct;	// LED点灯カウンタ LED lighting counter

	float fwctEnd;					// DS: 一周期最大カウント数 Maximum count in one cycle
	float fwct;						// DS: 一周期カウンタ One cycle counter
	float fwctUp;
	// these angles are in radians
	float pitch, roll, pitcht, rollt;
	float pitch_gyrg, roll_gyrg;
	float wk, wt;
	// dyi is equivalent to I in PID control, and is for converting the tilt angle in the roll direction into the distance in the left-right direction and integrating it
	float dyi, dyib, dyis;
	// dxi is equivalent to I in PID control, and is for converting the tilt angle in the pitch direction into the distance in the front - back direction and integrating it
	float dxi, dxib, dxis;
	float rollg, fw, fwi, fws, sw, freeBak, tt0;
	float supportingLeg, swingLeg;	// 支持脚、遊脚股関節振出角度 Support leg, swing leg hip joint swing angle
	float footH;					// 脚上げ高さ leg lift height
	float swx, swy, swMax;			// 横振り巾 Horizontal width
	float autoH, fh, fhMax;			// 脚上げ高さ leg lift height
} core_t;

typedef struct input_s {
	uint8_t cmd[2];
	uint8_t ff[45];
	uint8_t krr[4];
	int8_t	kn;
	int16_t keyMode;
} input_t;

typedef struct {
	uint8_t dsp[110];
} globals_t;

extern uint8_t read8(input_t* input, uint8_t reg);
extern bool readLen(input_t* input, uint8_t reg, uint8_t len);
extern bool write8(input_t* input, uint8_t reg, uint8_t dat);

extern void movSv(core_t* core, short* s, int d);
extern void angAdj(core_t* core);
extern void detAng(core_t* core);
extern void uvcSub(core_t* core);
extern void uvcSub2(core_t* core, state_t* state);
extern void uvc(core_t* core);
extern void footUp(core_t* core);
extern void swCont(core_t* core);
extern void armCont(core_t* core, state_t* state);
extern void footCont(core_t* core, state_t* state, float x, float y, float h, int s);
extern void feetCont1(core_t* core, state_t* state, float x0, float y0, float x1, float y1, int s);
extern void feetCont2(core_t* core, state_t* state, int s);
extern void counterCont(core_t* core);
extern void walk(core_t* core, state_t* state);
extern void keyCont(input_t* input, core_t* core, state_t* state);
extern int mainRoutine(state_t* state, core_t* core, input_t* input);

extern int controllerMain();
