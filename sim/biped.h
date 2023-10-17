#pragma once

class bodyStr{
public:
	dBodyID	b;		// ボディのID Body ID
	int		e;		// ボディ有効 body valid
	dGeomID	g;		// ジオメトリのID Geometry ID
	int		ge;		// ジオメトリ設定 有/無 Geometry settings presence/absence
	char	k;		// ボディの種類 Body type
	char	c;		// 色の種類 Color type
	double	l;		// 長さ Length
	double	w;		// 幅 Width
	double	h;		// 高さ Height
	double	r;		// 半径 Radius
};

class jointStr{
public:
	dJointID j;		// ジョイントID Joint ID
	char	k;		// ジョイントの種類 Joint type
	int		e;		// ジョイント有効 joint enabled
	double	x;		// X座標 X coordinate
	double	y;		// Y座標 Y coordinate
	double	z;		// Z座標 Z coordinate
	double	dn;		// 下限角度 lower limit angle
	double	up;		// 上限角度 upper limit angle
	double	t;		// 関節角度 Joint angle
	double	t2;		// 関節2角度(ユニバーサル時) Joint 2 angle (when universal)
	double	tm;		// 最大角速度 maximum angular velocity
	double	tm2;	// 最大角速度 maximum angular velocity
	double	tk;		// 関節トルク Joint torque
	double	tk2;	// 関節トルク Joint torque
	int		s;		// 特別制御フラグ(1の場合、歩行制御特別ジョイント) Special control flag (if 1, walking control special joint)
	int		c;		// 汎用カウンタ General purpose counter
	int		mode;	// 駆動モード drive mode
	int		pn;		// 足圧力カウンタ foot pressure counter
	int		sv;		// サーボ識別 servo identification
};

extern double fbRad;// 頭前後角度		前が- The angle of the head back and front    front-
extern double lrRad;// 頭左右角度		右が- The left and right angle of the head    right-
extern double fbAV;	// 頭前後角速度	前が+ head front and back angular velocity    front+
extern double lrAV;	// 頭左右角速度	右が+ Head left and right angular velocity    right+
