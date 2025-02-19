#define ICS20 (20)
#define ICS21 (30)
#define ICS22 (35)
#define ICS_MAX_ID (32)
#define ICS_MAX_POS_VALUE (0x3FFF)
#define ICS_POS_CMD (0x80) // ポジション設定コマンド
#define ICS_POS_BYTE (3)
#define ICS_GET_PARAM_CMD (0xA0) // パラメータ読み書きコマンド
#define ICS_SET_PARAM_CMD (0xC0)
#define ICS_EEPROM_SC (0) // パラメータ読み書きサブコマンド
#define ICS_STRC_SC (1) // STRETCH 値読み書きサブコマンド
#define ICS_SPD_SC (2) // SPEED 値読み書きサブコマンド
#define ICS_CURNT_SC (3) // 電流値・電流制限値読み書きサブコマンド
#define ICS_TMPR_SC (4) // 温度値・温度制限値読み書きサブコマンド
#define ICS_GENE_SC (0x7F) // 汎用データ読み書きサブコマンド
#define ICS_PARAM_BYTE (2) // パラメータ読み書きデータサイズ
#define ICS_EEPROM_BYTE (60)
#define ICS35_EEPROM_BYTE (66)
#define ICS_ID_CMD (0xE0) // ID コマンド
#define ICS_GET_ID_SC (0) // ID 取得サブコマンド
#define ICS_SET_ID_SC (1) // ID 設定サブコマンド

extern int ics_set_pos(int port, unsigned char id, unsigned short pos);
extern int ics_set_param(int port, unsigned char id, unsigned char sc, unsigned char param);
extern bool sio_init(int port, int baudrate);
