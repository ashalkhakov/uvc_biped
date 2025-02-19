#define TIMER (0xFF) // ポート番号に指定するため
#define TIMER_INT (0xAA) //ポートを指定せず、一定時間割り込みにつかう
#define TIMER1 (0x00) // PA0
#define TIMER2 (0x01) // PA1
#define TIMER3 (0x06) // PA6
#define TIMER4 (0x07) // PA7
#define TIMER5 (0x10) // PB0
#define TIMER6 (0x11) // PB1

// モード 1
// モード 2
// モード 1 と OR で使用して T 端子から出力、入力を決める
// 不可能な組み合わせは初期化関数でエラーを発生させる
// PWM の INPUT はインプット・キャプチャ(開発中)
typedef enum {
	TIMER_MODE_TIMER16 = 0x0100,
	TIMER_MODE_TIMER32 = 0x0200,
	TIMER_MODE_ONEPULSE = 0x0400,
	TIMER_MODE_PWM = 0x0800,
	TIMER_MODE_OUTPUT = 0x1000,
	TIMER_MODE_INPUT = 0x2000,
	TIMER_MODE_INTERRUPT = 0x4000
} timer_mode;

extern bool timer_init(int port, timer_mode mode, int frequency);
extern bool timer_write(int port, unsigned int data);
extern int timer_start(int port);
extern unsigned int timer_read(int port);
