#pragma once

class bodyStr{
public:
	dBodyID	bodyId;		// ボディのID Body ID
	int		enabled;		// ボディ有効 body valid
	dGeomID	geometry;		// ジオメトリのID Geometry ID
	int		hasGeometry;		// ジオメトリ設定 有/無 Geometry settings presence/absence
	char	bodyType;		// ボディの種類 Body type
	char	color;		// 色の種類 Color type
	double	length;		// 長さ Length
	double	width;		// 幅 Width
	double	height;		// 高さ Height
	double	radius;		// 半径 Radius
};

class jointStr{
public:
	dJointID jointId;		// ジョイントID Joint ID
	char	jointType;		// ジョイントの種類 Joint type
	int		enabled;		// ジョイント有効 joint enabled
	double	origin_x;		// X座標 X coordinate
	double	origin_y;		// Y座標 Y coordinate
	double	origin_z;		// Z座標 Z coordinate
	double	dn;		// 下限角度 lower limit angle
	double	up;		// 上限角度 upper limit angle
	double	t_jointAngle;		// 関節角度 Joint angle
	double	t2_jointAngle;		// 関節2角度(ユニバーサル時) Joint 2 angle (when universal)
	double	tm;		// 最大角速度 maximum angular velocity
	double	tm2;	// 最大角速度 maximum angular velocity
	double	torque_tk;		// 関節トルク Joint torque
	double	torque_tk2;	// 関節トルク Joint torque
	int		s_flag;		// 特別制御フラグ(1の場合、歩行制御特別ジョイント) Special control flag (if 1, walking control special joint)
	int		c_counter;		// 汎用カウンタ General purpose counter
	int		mode;	// 駆動モード drive mode
	int		pn_footPressureCounter;		// 足圧力カウンタ foot pressure counter
	int		sv_servoId;		// サーボ識別 servo identification
};

extern double state_fbRad;// 頭前後角度		前が- The angle of the head back and front    front-
extern double state_lrRad;// 頭左右角度		右が- The left and right angle of the head    right-
extern double state_fbAV;	// 頭前後角速度	前が+ head front and back angular velocity    front+
extern double state_lrAV;	// 頭左右角速度	右が+ Head left and right angular velocity    right+
